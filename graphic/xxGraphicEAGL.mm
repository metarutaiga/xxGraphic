#include "xxGraphicGL.h"
#include "xxGraphicEAGL.h"

#include <dlfcn.h>

#define GLES_SILENCE_DEPRECATION
#import <UIKit/UIKit.h>
#import <QuartzCore/CAEAGLLayer.h>
static void*                            g_glLibrary = nullptr;
static Class                            g_classEAGLContext = nullptr;
static PFNGLGENVERTEXARRAYSOESPROC      glGenVertexArrays;
static PFNGLDELETEVERTEXARRAYSOESPROC   glDeleteVertexArrays;
static PFNGLBINDVERTEXARRAYOESPROC      glBindVertexArray;

//==============================================================================
//  Initialize - EAGL
//==============================================================================
static bool eaglSymbolFailed = false;
static void* eaglSymbol(const char* name, bool* failed)
{
    void* ptr = nullptr;

    if (ptr == nullptr && g_glLibrary)
        ptr = dlsym(g_glLibrary, name);

    if (ptr == nullptr)
        xxLog("CGL", "%s is not found", name);

    eaglSymbolFailed |= (ptr == nullptr);
    if (failed)
        (*failed) |= (ptr == nullptr);

    return ptr;
}
#define eaglSymbol(var) (void*&)var = eaglSymbol(#var, nullptr);
//------------------------------------------------------------------------------
union GLDISPLAY
{
    uint64_t    value;
    struct
    {
        GLuint  framebuffer;
        GLuint  colorRenderbuffer;
    };
};
//------------------------------------------------------------------------------
uint64_t glCreateContextEAGL(uint64_t instance, void* view, void** display)
{
    EAGLContext* rootContext = (__bridge EAGLContext*)reinterpret_cast<void*>(instance);
    if (rootContext == nil)
        return 0;
    UIWindow* nsWindow = (__bridge UIWindow*)view;
    if (nsWindow == nil)
        return 0;
    UIView* nsView = [[nsWindow rootViewController] view];
    if (nsView == nil)
        return 0;
    CAEAGLLayer* layer = (CAEAGLLayer*)[nsView layer];

    EAGLContext* eaglContext = [[g_classEAGLContext alloc] initWithAPI:[rootContext API] sharegroup:[rootContext sharegroup]];
    if (eaglContext == nil)
        return 0;
    [g_classEAGLContext setCurrentContext:eaglContext];

    GLint framebufferWidth = 1;
    GLint framebufferHeight = 1;
    GLuint colorRenderbuffer = 0;
    glGenRenderbuffers(1, &colorRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    GLuint depthRenderbuffer = 0;
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, framebufferWidth, framebufferHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    eaglSymbol(glGetIntegerv);
    eaglSymbol(glGenVertexArrays);
    eaglSymbol(glDeleteVertexArrays);
    eaglSymbol(glBindVertexArray);
    if (glGenVertexArrays && glBindVertexArray)
    {
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    if (display)
    {
        GLDISPLAY glDisplay;
        glDisplay.framebuffer = framebuffer;
        glDisplay.colorRenderbuffer = colorRenderbuffer;
        (*display) = reinterpret_cast<void*>(glDisplay.value);
    }

    return reinterpret_cast<uint64_t>((__bridge_retained void*)eaglContext);
}
//------------------------------------------------------------------------------
void glDestroyContextEAGL(uint64_t context, void* view, void* display)
{
    EAGLContext* eaglContext = (__bridge_transfer EAGLContext*)reinterpret_cast<void*>(context);
    if (eaglContext == nil)
        return;
    GLDISPLAY glDisplay = { reinterpret_cast<uint64_t>(display) };

    [g_classEAGLContext setCurrentContext:eaglContext];

    if (glGetIntegerv && glDeleteVertexArrays)
    {
        GLuint vao = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING_OES, (GLint*)&vao);
        glDeleteVertexArrays(1, &vao);
    }

    GLuint colorRenderbuffer = 0;
    GLuint depthRenderbuffer = 0;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, glDisplay.framebuffer);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, (GLint*)&colorRenderbuffer);
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, (GLint*)&depthRenderbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &glDisplay.framebuffer);
    glDeleteRenderbuffers(1, &colorRenderbuffer);
    glDeleteRenderbuffers(1, &depthRenderbuffer);

    [g_classEAGLContext setCurrentContext:nil];
}
//------------------------------------------------------------------------------
void glMakeCurrentContextEAGL(uint64_t context, void* display)
{
    EAGLContext* eaglContext = (__bridge EAGLContext*)reinterpret_cast<void*>(context);
    GLDISPLAY glDisplay = { reinterpret_cast<uint64_t>(display) };

    [g_classEAGLContext setCurrentContext:eaglContext];

    glBindFramebuffer(GL_FRAMEBUFFER, glDisplay.framebuffer);
}
//------------------------------------------------------------------------------
void glPresentContextEAGL(uint64_t context, void* display)
{
    EAGLContext* eaglContext = (__bridge EAGLContext*)reinterpret_cast<void*>(context);
    GLDISPLAY glDisplay = { reinterpret_cast<uint64_t>(display) };

    glBindRenderbuffer(GL_RENDERBUFFER, glDisplay.colorRenderbuffer);

    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}
//------------------------------------------------------------------------------
void glGetViewSizeEAGL(void* view, unsigned int* width, unsigned int* height)
{
    UIWindow* nsWindow = (__bridge UIWindow*)view;
    UIView* nsView = [[nsWindow rootViewController] view];

    CGSize size = [nsView bounds].size;
    (*width) = size.width;
    (*height) = size.height;
}
//------------------------------------------------------------------------------
uint64_t xxGraphicCreateEAGL()
{
    if (g_glLibrary == nullptr)
        g_glLibrary = dlopen("/System/Library/Frameworks/OpenGLES.framework/OpenGLES", RTLD_LAZY);
    if (g_glLibrary == nullptr)
        return 0;

    g_classEAGLContext = (__bridge Class)dlsym(g_glLibrary, "OBJC_CLASS_$_EAGLContext");

    EAGLContext* rootContext = [[g_classEAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (rootContext == nil)
    {
        rootContext = [[g_classEAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    }

    if (xxGraphicCreateGL(eaglSymbol) == false)
    {
        xxGraphicDestroyEAGL(reinterpret_cast<uint64_t>(rootContext));
        return 0;
    }

    glCreateContext = glCreateContextEAGL;
    glDestroyContext = glDestroyContextEAGL;
    glMakeCurrentContext = glMakeCurrentContextEAGL;
    glPresentContext = glPresentContextEAGL;
    glGetViewSize = glGetViewSizeEAGL;

    return reinterpret_cast<uint64_t>((__bridge_retained void*)rootContext);
}
//------------------------------------------------------------------------------
void xxGraphicDestroyEAGL(uint64_t context)
{
    glDestroyContextEAGL(context, 0, nullptr);

    g_classEAGLContext = nullptr;

    if (g_glLibrary)
    {
        dlclose(g_glLibrary);
        g_glLibrary = nullptr;
    }

    xxGraphicDestroyGL();
}
//==============================================================================
