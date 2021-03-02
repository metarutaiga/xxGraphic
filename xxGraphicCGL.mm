//==============================================================================
// xxGraphic : OpenGL CGL Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalGL.h"
#include "xxGraphicCGL.h"

#define IOSurfaceGetWidth IOSurfaceGetWidth_unused
#define IOSurfaceGetHeight IOSurfaceGetHeight_unused
#define IOSurfaceLock IOSurfaceLock_unused
#define IOSurfaceUnlock IOSurfaceUnlock_unused
#define CGLTexImageIOSurface2D CGLTexImageIOSurface2D_unused
#define GL_SILENCE_DEPRECATION
#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <AppKit/NSWindow.h>
#import <AppKit/NSOpenGL.h>
#import <AppKit/NSOpenGLView.h>
#undef IOSurfaceGetWidth
#undef IOSurfaceGetHeight
#undef IOSurfaceLock
#undef IOSurfaceUnlock
#undef CGLTexImageIOSurface2D
static void*                            g_glLibrary = nullptr;
static NSOpenGLView*                    g_rootView = nil;
static size_t                           (*IOSurfaceGetWidth)(IOSurfaceRef);
static size_t                           (*IOSurfaceGetHeight)(IOSurfaceRef);
static kern_return_t                    (*IOSurfaceLock)(IOSurfaceRef, IOSurfaceLockOptions, uint32_t*);
static kern_return_t                    (*IOSurfaceUnlock)(IOSurfaceRef, IOSurfaceLockOptions, uint32_t*);
static CGLError                         (*CGLTexImageIOSurface2D)(CGLContextObj, GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, IOSurfaceRef, GLuint);
static struct { GLint x; GLint y; }     rectanglePrograms[16];
static GLint                            rectangleProgramTexture = 0;
static float                            rectangleProgramMatrix[16 * 3];

//==============================================================================
//  Initialize - CGL
//==============================================================================
extern PFNGLSHADERSOURCEPROC glShaderSourceEntry;
static PFNGLSHADERSOURCEPROC glShaderSourceInternal;
//------------------------------------------------------------------------------
static void GL_APIENTRY cglShaderSourceLegacy(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length)
{
    const GLchar** replaceString = xxAlloc(const GLchar*, count);

    for (GLsizei i = 0; i < count; ++i)
    {
        const GLchar* var = string[i];
        if (strcmp(var, "#version 100") == 0)
            var = "#version 120";
        if (strncmp(var, "precision", sizeof("precision") - 1) == 0)
            var = "";
        replaceString[i] = var;
    }

    glShaderSourceInternal(shader, count, replaceString, length);

    xxFree(replaceString);
}
//------------------------------------------------------------------------------
static void GL_APIENTRY cglShaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length)
{
    const GLchar** replaceString = xxAlloc(const GLchar*, count);

    bool fragmentShader = false;
    for (GLsizei i = 0; i < count; ++i)
    {
        const GLchar* var = string[i];
        if (strcmp(var, "#version 100") == 0)
            var = "#version 140";
        if (strncmp(var, "precision", sizeof("precision") - 1) == 0)
            var = "";
        if (strcmp(var, "#define _FRAGMENT_ 1") == 0)
        {
            var =   "#define _FRAGMENT_ 1\n"
                    "#ifndef texture2D\n"
                    "#define texture2D texture\n"
                    "#endif\n"
                    "#ifndef texture2DRect\n"
                    "#define texture2DRect texture\n"
                    "#endif\n"
                    "#ifndef textureSize2DRect\n"
                    "#define textureSize2DRect(t, l) textureSize(t)\n"
                    "#endif\n"
                    "#ifndef gl_FragColor\n"
                    "#define gl_FragColor fragColor\n"
                    "out vec4 fragColor;\n"
                    "#endif";
            fragmentShader = true;
        }
        if (strncmp(var, "#define attribute", sizeof("#define attribute") - 1) == 0)
            var = fragmentShader ? "#define attribute" : "#define attribute in";
        if (strncmp(var, "#define varying", sizeof("#define varying") - 1) == 0)
            var = fragmentShader ? "#define varying in" : "#define varying out";
        if (strcmp(var, "#extension GL_EXT_gpu_shader4 : enable") == 0)
            var = "";
        replaceString[i] = var;
    }

    glShaderSourceInternal(shader, count, replaceString, length);

    xxFree(replaceString);
}
//------------------------------------------------------------------------------
extern PFNGLBINDTEXTUREPROC glBindTextureEntry;
static PFNGLBINDTEXTUREPROC glBindTextureInternal;
//------------------------------------------------------------------------------
static void GL_APIENTRY cglBindTexture(GLenum target, GLuint texture)
{
    glBindTextureInternal(target, texture);
    if (target == GL_TEXTURE_RECTANGLE_ARB)
    {
        xxBindRectangleProgram();
    }
}
//------------------------------------------------------------------------------
extern PFNGLUNIFORM4FVPROC glUniform4fvEntry;
static PFNGLUNIFORM4FVPROC glUniform4fvInternal;
//------------------------------------------------------------------------------
static void GL_APIENTRY cglUniform4fv(GLint location, GLsizei count, const GLfloat *value)
{
    glUniform4fvInternal(location, count, value);
    if (location == 0 && count == 4 * 3)
    {
        memcpy(rectangleProgramMatrix, value, sizeof(rectangleProgramMatrix));
    }
}
//------------------------------------------------------------------------------
static bool cglSymbolFailed = false;
static void* GL_APIENTRY cglSymbolImpl(const char* name, bool* failed = nullptr)
{
    void* ptr = nullptr;

    if (ptr == nullptr && g_glLibrary)
        ptr = xxGetProcAddress(g_glLibrary, name);

    if (ptr == nullptr)
    {
        static bool internal;
        if (failed == &internal)
            return nullptr;

        ptr = getSymbolExtension(cglSymbolImpl, name, &internal);
    }

    if (ptr == nullptr)
        xxLog("CGL", "%s is not found", name);

    cglSymbolFailed |= (ptr == nullptr);
    if (failed)
        (*failed) |= (ptr == nullptr);

    return ptr;
}
#define cglSymbol(var) (void*&)var = cglSymbolImpl(#var);
//------------------------------------------------------------------------------
struct CGLDISPLAY
{
    GLuint  vao;
};
//------------------------------------------------------------------------------
uint64_t glCreateContextCGL(uint64_t instance, void* view, void** display)
{
    if (g_rootView == nil)
        return 0;
    NSWindow* nsWindow = (__bridge NSWindow*)view;
    if (nsWindow == nil)
        return 0;
    NSView* nsView = [[nsWindow contentViewController] view];
    if (nsView == nil)
        return 0;
#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1070
    if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6)
        [nsView setWantsBestResolutionOpenGLSurface:YES];
#endif // MAC_OS_X_VERSION_MAX_ALLOWED >= 1070
    NSOpenGLContext* nsContext = [[NSOpenGLContext alloc] initWithFormat:[g_rootView pixelFormat]
                                                            shareContext:[g_rootView openGLContext]];
    if (nsContext == nil)
        return 0;
    [nsContext setView:nsView];
    [nsContext makeCurrentContext];

    int swapInterval = 0;
    [nsContext setValues:&swapInterval
            forParameter:NSOpenGLContextParameterSwapInterval];

    if (display)
    {
        CGLDISPLAY* cglDisplay = xxAlloc(CGLDISPLAY);
        if (cglDisplay == nullptr)
            return 0;

        cglDisplay->vao = 0;
        (*display) = cglDisplay;

        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        cglDisplay->vao = vao;
    }

    return reinterpret_cast<uint64_t>((__bridge_retained void*)nsContext);
}
//------------------------------------------------------------------------------
void glDestroyContextCGL(uint64_t context, void* view, void* display)
{
    NSOpenGLContext* nsContext = (__bridge_transfer NSOpenGLContext*)reinterpret_cast<void*>(context);
    if (nsContext == nil)
        return;
    CGLDISPLAY* cglDisplay = reinterpret_cast<CGLDISPLAY*>(display);

    [nsContext makeCurrentContext];
    [nsContext clearDrawable];

    if (cglDisplay)
    {
        glDeleteVertexArrays(1, &cglDisplay->vao);
        xxFree(cglDisplay);
    }

    [NSOpenGLContext clearCurrentContext];
}
//------------------------------------------------------------------------------
void* glGetProcAddressCGL(const char* name)
{
    return cglSymbolImpl(name);
}
//------------------------------------------------------------------------------
float glGetScaleContextCGL(uint64_t context, void* view)
{
    NSWindow* nsWindow = (__bridge NSWindow*)view;
    if (nsWindow == nil)
        return 1.0f;

    return [nsWindow backingScaleFactor];
}
//------------------------------------------------------------------------------
void glMakeCurrentContextCGL(uint64_t context, void* display)
{
    NSOpenGLContext* __unsafe_unretained nsContext = (__bridge NSOpenGLContext*)reinterpret_cast<void*>(context);
    CGLDISPLAY* cglDisplay = reinterpret_cast<CGLDISPLAY*>(display);

    [nsContext makeCurrentContext];

    glBindVertexArray(cglDisplay->vao);
}
//------------------------------------------------------------------------------
void glPresentContextCGL(uint64_t context, void* display)
{
    NSOpenGLContext* __unsafe_unretained nsContext = (__bridge NSOpenGLContext*)reinterpret_cast<void*>(context);

    [nsContext flushBuffer];
}
//------------------------------------------------------------------------------
uint64_t xxGraphicCreateCGL(int version)
{
    if (g_glLibrary == nullptr)
        g_glLibrary = xxLoadLibrary("/System/Library/Frameworks/OpenGL.framework/OpenGL");
    if (g_glLibrary == nullptr)
        return 0;

    cglSymbol(IOSurfaceGetWidth);
    cglSymbol(IOSurfaceGetHeight);
    cglSymbol(IOSurfaceLock);
    cglSymbol(IOSurfaceUnlock);
    cglSymbol(CGLTexImageIOSurface2D);

    NSOpenGLPixelFormatAttribute attributes[] =
    {
        NSOpenGLPFAOpenGLProfile, version <= 200 ? NSOpenGLProfileVersionLegacy : NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAStencilSize, 8,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFANoRecovery,
        0
    };

    NSRect rect = CGRectMake(0, 0, 1, 1);
    NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
    g_rootView = [[NSOpenGLView alloc] initWithFrame:rect
                                         pixelFormat:pixelFormat];
    if (g_rootView == nil)
        return 0;
    [[g_rootView openGLContext] makeCurrentContext];
    NSOpenGLContext* rootContext = [g_rootView openGLContext];

    glCreateContext = glCreateContextCGL;
    glDestroyContext = glDestroyContextCGL;
    glGetProcAddress = glGetProcAddressCGL;
    glGetScaleContext = glGetScaleContextCGL;
    glMakeCurrentContext = glMakeCurrentContextCGL;
    glPresentContext = glPresentContextCGL;

    (void*&)glShaderSourceInternal = glGetProcAddress("glShaderSource");
    (void*&)glBindTextureInternal = glGetProcAddress("glBindTexture");
    (void*&)glUniform4fvInternal = glGetProcAddress("glUniform4fv");

    glShaderSourceEntry = version <= 200 ? cglShaderSourceLegacy : cglShaderSource;
    glBindTextureEntry = cglBindTexture;
    glUniform4fvEntry = cglUniform4fv;

    return reinterpret_cast<uint64_t>((__bridge_retained void*)rootContext);
}
//------------------------------------------------------------------------------
void xxGraphicDestroyCGL(uint64_t context)
{
    for (int i = 0; i < xxCountOf(rectanglePrograms); ++i)
    {
        if (rectanglePrograms[i].x)
        {
            glDeleteProgram(rectanglePrograms[i].x);
            rectanglePrograms[i].x = 0;
        }
        if (rectanglePrograms[i].y)
        {
            glDeleteProgram(rectanglePrograms[i].y);
            rectanglePrograms[i].y = 0;
        }
    }

    glDestroyContextCGL(context, 0, nullptr);

    g_rootView = nil;

    if (g_glLibrary)
    {
        xxFreeLibrary(g_glLibrary);
        g_glLibrary = nullptr;
    }
}
//==============================================================================
//  Extension
//==============================================================================
void xxBindTextureWithSurface(const void* surface)
{
    CGLContextObj contextObj = [[NSOpenGLContext currentContext] CGLContextObj];
    IOSurfaceRef ioSurface = (IOSurfaceRef)surface;

    GLsizei width = (GLsizei)IOSurfaceGetWidth(ioSurface);
    GLsizei height = (GLsizei)IOSurfaceGetHeight(ioSurface);

    CGLTexImageIOSurface2D(contextObj, GL_TEXTURE_RECTANGLE_ARB, GL_RGBA, width, height, GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8_REV, ioSurface, 0);
}
//------------------------------------------------------------------------------
void xxBindRectangleProgram()
{
    GLint program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    for (int i = 0; i < xxCountOf(rectanglePrograms); ++i)
    {
        if (rectanglePrograms[i].x == program)
        {
            glUseProgram(rectanglePrograms[i].y);
            glUniform4fv(0, 4 * 3, rectangleProgramMatrix);
            glUniform1i(rectangleProgramTexture, 0);
            return;
        }
    }

    GLuint shaders[2];
    GLsizei shaderCount = 2;
    glGetAttachedShaders(program, 2, &shaderCount, shaders);

    GLchar* source = xxAlloc(char, 4096);
    GLsizei length = 4096;
    glGetShaderSource(shaders[1], 4096, &length, source);

    const GLchar* fragmentShaderCodes[] =
    {
        "#version 100", "\n",
        "#define _FRAGMENT_ 1", "\n",
        "#define sampler2D sampler2DRect", "\n",
        "#undef texture2D", "\n",
        "#define texture2D(t,c) texture2DRect(t, (c) * vec2(textureSize2DRect(t, 0)))", "\n",
        "#extension GL_EXT_gpu_shader4 : enable", "\n",
        "//", source
    };

    GLuint glFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(glFragmentShader, xxCountOf(fragmentShaderCodes), fragmentShaderCodes, nullptr);
    glCompileShader(glFragmentShader);

    xxFree(source);

    GLuint glProgram = glCreateProgram();
    glBindAttribLocation(glProgram, 0, "position");
    glBindAttribLocation(glProgram, 1, "color");
    glBindAttribLocation(glProgram, 2, "uv");
    glAttachShader(glProgram, shaders[0]);
    glAttachShader(glProgram, glFragmentShader);
    glLinkProgram(glProgram);

    glDeleteShader(glFragmentShader);

    for (int i = 0; i < xxCountOf(rectanglePrograms); ++i)
    {
        if (rectanglePrograms[i].x == 0)
        {
            rectanglePrograms[i].x = program;
            rectanglePrograms[i].y = glProgram;
            break;
        }
    }

    glUseProgram(glProgram);
    glUniform4fv(0, 4 * 3, rectangleProgramMatrix);
    glUniform1i(rectangleProgramTexture, 0);
}
//==============================================================================
