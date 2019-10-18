//==============================================================================
// xxGraphic : OpenGL CGL Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "xxGraphicGL.h"
#include "xxGraphicCGL.h"

#define GL_SILENCE_DEPRECATION
#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <AppKit/NSWindow.h>
#import <AppKit/NSOpenGL.h>
#import <AppKit/NSOpenGLView.h>
static void*                            g_glLibrary = nullptr;
static NSOpenGLView*                    g_rootView = nil;

//==============================================================================
//  Initialize - CGL
//==============================================================================
static void GL_APIENTRY cglInvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments)
{

}
//------------------------------------------------------------------------------
static void GL_APIENTRY cglInvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height)
{

}
//------------------------------------------------------------------------------
static bool cglSymbolFailed = false;
static void* GL_APIENTRY cglSymbol(const char* name, bool* failed)
{
    void* ptr = nullptr;

    if (ptr == nullptr && g_glLibrary)
        ptr = xxGetProcAddress(g_glLibrary, name);

    if (ptr == nullptr)
    {
        static const char* const tags[] = { "ARB", "OES", "EXT", "KHR", "AMD", "ARM", "IMG", "INTEL", "NV", "QCOM" };

        static bool internal;
        if (failed == &internal)
            return nullptr;

        char extensionName[64];
        for (int i = 0; i < xxCountOf(tags); ++i)
        {
            snprintf(extensionName, 64, "%s%s", name, tags[i]);
            ptr = cglSymbol(extensionName, &internal);
            if (ptr != nullptr)
                break;
        }
    }

    if (ptr == nullptr && strcmp(name, "glInvalidateFramebuffer") == 0)
        ptr = (void*)cglInvalidateFramebuffer;

    if (ptr == nullptr && strcmp(name, "glInvalidateSubFramebuffer") == 0)
        ptr = (void*)cglInvalidateSubFramebuffer;

    if (ptr == nullptr)
        xxLog("CGL", "%s is not found", name);

    cglSymbolFailed |= (ptr == nullptr);
    if (failed)
        (*failed) |= (ptr == nullptr);

    return ptr;
}
#define cglSymbol(var) (void*&)var = cglSymbol(#var, nullptr);
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
    [nsContext setValues:&swapInterval forParameter:NSOpenGLContextParameterSwapInterval];

    if (display)
    {
        CGLDISPLAY* cglDisplay = new CGLDISPLAY;
        if (cglDisplay == nullptr)
            return 0;

        cglDisplay->vao = 0;
        (*display) = cglDisplay;

        cglSymbol(glGenVertexArrays);
        cglSymbol(glDeleteVertexArrays);
        cglSymbol(glBindVertexArray);
        if (glGenVertexArrays && glBindVertexArray)
        {
            GLuint vao = 0;
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            cglDisplay->vao = vao;
        }
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
        if (glDeleteVertexArrays)
        {
            glDeleteVertexArrays(1, &cglDisplay->vao);
        }
        delete cglDisplay;
    }

    [NSOpenGLContext clearCurrentContext];
}
//------------------------------------------------------------------------------
void glMakeCurrentContextCGL(uint64_t context, void* display)
{
    NSOpenGLContext* __unsafe_unretained nsContext = (__bridge NSOpenGLContext*)reinterpret_cast<void*>(context);
    CGLDISPLAY* cglDisplay = reinterpret_cast<CGLDISPLAY*>(display);

    [nsContext makeCurrentContext];

    if (glBindVertexArray)
    {
        glBindVertexArray(cglDisplay->vao);
    }
}
//------------------------------------------------------------------------------
void glPresentContextCGL(uint64_t context, void* display)
{
    NSOpenGLContext* __unsafe_unretained nsContext = (__bridge NSOpenGLContext*)reinterpret_cast<void*>(context);

    [nsContext flushBuffer];
}
//------------------------------------------------------------------------------
PFNGLSHADERSOURCEPROC glShaderSource_;
//------------------------------------------------------------------------------
void cglShaderSourceLegacy(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length)
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

    glShaderSource_(shader, count, replaceString, length);

    xxFree(replaceString);
}
//------------------------------------------------------------------------------
void cglShaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length)
{
    const GLchar** replaceString = xxAlloc(const GLchar*, count);

    bool fragmentShader = false;
    for (GLsizei i = 0; i < count; ++i)
    {
        const GLchar* var = string[i];
        if (strcmp(var, "#version 100") == 0)
            var = "#version 140";
        if (strncmp(var, "precision", sizeof("precision") - 1) == 0)
        {
            var =   "out vec4 fragColor;\n"
                    "#define gl_FragColor fragColor\n"
                    "#define texture2D texture";
            fragmentShader = true;
        }
        if (strcmp(var, "#define attribute attribute") == 0)
            var = "#define attribute in";
        if (strcmp(var, "#define varying varying") == 0)
            var = fragmentShader ? "#define varying in" : "#define varying out";

        replaceString[i] = var;
    }

    glShaderSource_(shader, count, replaceString, length);

    xxFree(replaceString);
}
//------------------------------------------------------------------------------
uint64_t xxGraphicCreateCGL(int version)
{
    if (g_glLibrary == nullptr)
        g_glLibrary = xxLoadLibrary("/System/Library/Frameworks/OpenGL.framework/OpenGL");
    if (g_glLibrary == nullptr)
        return 0;

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

    bool success = false;
    switch (version)
    {
    case 320:
        success = xxGraphicCreateGLES32(cglSymbol);
        break;
    case 310:
        success = xxGraphicCreateGLES31(cglSymbol);
        break;
    case 300:
        success = xxGraphicCreateGLES3(cglSymbol);
        break;
    case 200:
    default:
        success = xxGraphicCreateGLES2(cglSymbol);
        break;
    }
    if (success == false)
    {
        xxGraphicDestroyCGL(reinterpret_cast<uint64_t>(rootContext));
        return 0;
    }

    glCreateContext = glCreateContextCGL;
    glDestroyContext = glDestroyContextCGL;
    glMakeCurrentContext = glMakeCurrentContextCGL;
    glPresentContext = glPresentContextCGL;

    glShaderSource_ = glShaderSource;
    glShaderSource = version <= 200 ? cglShaderSourceLegacy : cglShaderSource;

    return reinterpret_cast<uint64_t>((__bridge_retained void*)rootContext);
}
//------------------------------------------------------------------------------
void xxGraphicDestroyCGL(uint64_t context)
{
    glDestroyContextCGL(context, 0, nullptr);

    g_rootView = nil;

    if (g_glLibrary)
    {
        xxFreeLibrary(g_glLibrary);
        g_glLibrary = nullptr;
    }

    xxGraphicDestroyGL();
}
//==============================================================================
