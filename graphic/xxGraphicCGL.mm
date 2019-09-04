#include "xxGraphicGL.h"
#include "xxGraphicCGL.h"

#include <dlfcn.h>

#define GL_SILENCE_DEPRECATION
#define CGLCreateContext CGLCreateContext_unused
#define CGLDestroyContext CGLDestroyContext_unused
#define CGLGetPixelFormat CGLGetPixelFormat_unused
#define CGLChoosePixelFormat CGLChoosePixelFormat_unused
#define CGLDestroyPixelFormat CGLDestroyPixelFormat_unused
#define CGLDescribePixelFormat CGLDescribePixelFormat_unused
#define CGLSetCurrentContext CGLSetCurrentContext_unused
#define CGLGetCurrentContext CGLGetCurrentContext_unused
#define CGLFlushDrawable CGLFlushDrawable_unused
#define CGLDisable CGLDisable_unused
#define CGLEnable CGLEnable_unused
#define CGLSetParameter CGLSetParameter_unused
#define CGLGetParameter CGLGetParameter_unused
#include <OpenGL/OpenGL.h>
#undef CGLCreateContext
#undef CGLDestroyContext
#undef CGLGetPixelFormat
#undef CGLChoosePixelFormat
#undef CGLDestroyPixelFormat
#undef CGLDescribePixelFormat
#undef CGLSetCurrentContext
#undef CGLGetCurrentContext
#undef CGLFlushDrawable
#undef CGLDisable
#undef CGLEnable
#undef CGLSetParameter
#undef CGLGetParameter
#include <AppKit/NSWindow.h>
#include <AppKit/NSOpenGL.h>
#include <AppKit/NSOpenGLView.h>
static void*                                g_glLibrary = nullptr;
static NSOpenGLView*                        g_rootView = nil;
static CGLError                             (*CGLCreateContext)(CGLPixelFormatObj, CGLContextObj, CGLContextObj*);
static CGLError                             (*CGLDestroyContext)(CGLContextObj);
static CGLPixelFormatObj                    (*CGLGetPixelFormat)(CGLContextObj);
static CGLError                             (*CGLChoosePixelFormat)(const CGLPixelFormatAttribute*, CGLPixelFormatObj*, GLint*);
static CGLError                             (*CGLDestroyPixelFormat)(CGLPixelFormatObj);
static CGLError                             (*CGLDescribePixelFormat)(CGLPixelFormatObj, GLint, CGLPixelFormatAttribute, GLint*);
static CGLError                             (*CGLSetCurrentContext)(CGLContextObj);
static CGLContextObj                        (*CGLGetCurrentContext)();
static CGLError                             (*CGLFlushDrawable)(CGLContextObj);
static CGLError                             (*CGLDisable)(CGLContextObj, CGLContextEnable);
static CGLError                             (*CGLEnable)(CGLContextObj, CGLContextEnable);
static CGLError                             (*CGLSetParameter)(CGLContextObj, CGLContextParameter, const GLint*);
static CGLError                             (*CGLGetParameter)(CGLContextObj, CGLContextParameter, GLint*);
static PFNGLGENVERTEXARRAYSOESPROC          glGenVertexArrays;
static PFNGLDELETEVERTEXARRAYSOESPROC       glDeleteVertexArrays;
static PFNGLBINDVERTEXARRAYOESPROC          glBindVertexArray;

//==============================================================================
//  Initialize - CGL
//==============================================================================
static bool cglSymbolFailed = false;
static void* cglSymbol(const char* name, bool* failed)
{
    void* ptr = nullptr;

    if (ptr == nullptr && g_glLibrary)
        ptr = dlsym(g_glLibrary, name);

    if (ptr == nullptr)
        xxLog("CGL", "%s is not found", name);

    cglSymbolFailed |= (ptr == nullptr);
    if (failed)
        (*failed) |= (ptr == nullptr);

    return ptr;
}
#define cglSymbol(var) (void*&)var = cglSymbol(#var, nullptr);
//------------------------------------------------------------------------------
uint64_t glCreateContextCGL(uint64_t instance, void* view, void** display)
{
    if (g_rootView == nil)
        return 0;

    NSWindow* __unsafe_unretained nsWindow = (__bridge NSWindow*)view;
    NSOpenGLView* nsView = (NSOpenGLView*)[nsWindow contentView];
    if (nsView == nil)
        return 0;
    [nsView setWantsBestResolutionOpenGLSurface:YES];
    NSOpenGLContext* nsContext = [[NSOpenGLContext alloc] initWithFormat:[g_rootView pixelFormat]
                                                            shareContext:[g_rootView openGLContext]];
    if (nsContext == nil)
        return 0;
    [nsContext makeCurrentContext];
    [nsView setOpenGLContext:nsContext];

    cglSymbol(glGetIntegerv);
    cglSymbol(glGenVertexArrays);
    cglSymbol(glDeleteVertexArrays);
    cglSymbol(glBindVertexArray);
    if (glGenVertexArrays && glBindVertexArray)
    {
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    if (display)
    {
        (*display) = (__bridge void*)nsView;
    }

    return reinterpret_cast<uint64_t>(nsContext);
}
//------------------------------------------------------------------------------
void glDestroyContextCGL(uint64_t context, void* view, void* display)
{
    NSOpenGLContext* __unsafe_unretained nsContext = (__bridge NSOpenGLContext*)reinterpret_cast<void*>(context);
    NSOpenGLView* __unsafe_unretained nsView = (__bridge NSOpenGLView*)display;

    [nsContext makeCurrentContext];

    if (glGetIntegerv && glDeleteVertexArrays)
    {
        GLuint vao = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING_OES, (GLint*)&vao);
        glDeleteVertexArrays(1, &vao);
    }

    [nsView setOpenGLContext:nil];
}
//------------------------------------------------------------------------------
void glMakeCurrentContextCGL(uint64_t context, void* display)
{
    NSOpenGLContext* __unsafe_unretained nsContext = (__bridge NSOpenGLContext*)reinterpret_cast<void*>(context);

    [nsContext makeCurrentContext];
}
//------------------------------------------------------------------------------
void glPresentContextCGL(uint64_t context, void* display)
{
    NSOpenGLContext* __unsafe_unretained nsContext = (__bridge NSOpenGLContext*)reinterpret_cast<void*>(context);

    [nsContext flushBuffer];
}
//------------------------------------------------------------------------------
void glGetViewSizeCGL(void* view, unsigned int* width, unsigned int* height)
{
    NSWindow* __unsafe_unretained nsWindow = (__bridge NSWindow*)view;
    NSView* __unsafe_unretained nsView = [nsWindow contentView];

    NSSize size = [nsView convertRectToBacking:[nsView bounds]].size;
    (*width) = size.width;
    (*height) = size.height;
}
//------------------------------------------------------------------------------
PFNGLSHADERSOURCEPROC glShaderSource_;
void glShaderSourceCGL(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length)
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
uint64_t xxGraphicCreateCGL()
{
    if (g_glLibrary == nullptr)
        g_glLibrary = dlopen("/System/Library/Frameworks/OpenGL.framework/OpenGL", RTLD_LAZY);
    if (g_glLibrary == nullptr)
        return 0;

    cglSymbolFailed = false;
    cglSymbol(CGLCreateContext);
    cglSymbol(CGLDestroyContext);
    cglSymbol(CGLGetPixelFormat);
    cglSymbol(CGLChoosePixelFormat);
    cglSymbol(CGLDestroyPixelFormat);
    cglSymbol(CGLDescribePixelFormat);
    cglSymbol(CGLSetCurrentContext);
    cglSymbol(CGLGetCurrentContext);
    cglSymbol(CGLFlushDrawable);
    cglSymbol(CGLDisable);
    cglSymbol(CGLEnable);
    cglSymbol(CGLSetParameter);
    cglSymbol(CGLGetParameter);
    if (cglSymbolFailed)
        return 0;

    NSOpenGLPixelFormatAttribute attributes[] =
    {
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
    [g_rootView setWantsBestResolutionOpenGLSurface:YES];
    [[g_rootView openGLContext] makeCurrentContext];
    NSOpenGLContext* rootContext = [g_rootView openGLContext];

    if (xxGraphicCreateGL(cglSymbol) == false)
    {
        xxGraphicDestroyCGL(reinterpret_cast<uint64_t>(rootContext));
        return 0;
    }

    glCreateContext = glCreateContextCGL;
    glDestroyContext = glDestroyContextCGL;
    glMakeCurrentContext = glMakeCurrentContextCGL;
    glPresentContext = glPresentContextCGL;
    glGetViewSize = glGetViewSizeCGL;

    glShaderSource_ = glShaderSource;
    glShaderSource = glShaderSourceCGL;

    return reinterpret_cast<uint64_t>(rootContext);
}
//------------------------------------------------------------------------------
void xxGraphicDestroyCGL(uint64_t context)
{
    glDestroyContextCGL(context, 0, nullptr);

    g_rootView = nil;

    if (g_glLibrary)
    {
        dlclose(g_glLibrary);
        g_glLibrary = nullptr;
    }

    xxGraphicDestroyGL();
}
//==============================================================================
