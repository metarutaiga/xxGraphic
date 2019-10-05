//==============================================================================
// xxGraphic : OpenGL EAGL Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicInternal.h"
#include "xxGraphicGL.h"
#include "xxGraphicEAGL.h"

#define GLES_SILENCE_DEPRECATION
#import <UIKit/UIKit.h>
#import <QuartzCore/CAEAGLLayer.h>
static void*                            g_glLibrary = nullptr;
static Class                            classEAGLContext = nil;
static PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
static PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
static PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;

//==============================================================================
//  Initialize - EAGL
//==============================================================================
static bool eaglSymbolFailed = false;
static void* GL_APIENTRY eaglSymbol(const char* name, bool* failed)
{
    void* ptr = nullptr;

    if (ptr == nullptr && g_glLibrary)
        ptr = xxGetProcAddress(g_glLibrary, name);

    if (ptr == nullptr)
        xxLog("CGL", "%s is not found", name);

    eaglSymbolFailed |= (ptr == nullptr);
    if (failed)
        (*failed) |= (ptr == nullptr);

    return ptr;
}
#define eaglSymbol(var) (void*&)var = eaglSymbol(#var, nullptr);
//------------------------------------------------------------------------------
struct EAGLDISPLAY
{
    GLuint  framebuffer;
    GLuint  colorRenderbuffer;
    GLuint  depthRenderbuffer;
    GLuint  vao;
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

    EAGLContext* eaglContext = [[classEAGLContext alloc] initWithAPI:[rootContext API] sharegroup:[rootContext sharegroup]];
    if (eaglContext == nil)
        return 0;
    [classEAGLContext setCurrentContext:eaglContext];

    if (display)
    {
        EAGLDISPLAY* eaglDisplay = new EAGLDISPLAY;
        if (eaglDisplay == nullptr)
            return 0;

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
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, framebufferWidth, framebufferHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        GLuint framebuffer = 0;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        eaglDisplay->framebuffer = framebuffer;
        eaglDisplay->colorRenderbuffer = colorRenderbuffer;
        eaglDisplay->depthRenderbuffer = depthRenderbuffer;
        eaglDisplay->vao = 0;
        (*display) = eaglDisplay;

        eaglSymbol(glGenVertexArrays);
        eaglSymbol(glDeleteVertexArrays);
        eaglSymbol(glBindVertexArray);
        if (glGenVertexArrays && glBindVertexArray)
        {
            GLuint vao = 0;
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
            eaglDisplay->vao = vao;
        }
    }

    return reinterpret_cast<uint64_t>((__bridge_retained void*)eaglContext);
}
//------------------------------------------------------------------------------
void glDestroyContextEAGL(uint64_t context, void* view, void* display)
{
    EAGLContext* eaglContext = (__bridge_transfer EAGLContext*)reinterpret_cast<void*>(context);
    if (eaglContext == nil)
        return;
    EAGLDISPLAY* eaglDisplay = reinterpret_cast<EAGLDISPLAY*>(display);

    [classEAGLContext setCurrentContext:eaglContext];

    if (eaglDisplay)
    {
        glDeleteFramebuffers(1, &eaglDisplay->framebuffer);
        glDeleteRenderbuffers(1, &eaglDisplay->colorRenderbuffer);
        glDeleteRenderbuffers(1, &eaglDisplay->depthRenderbuffer);
        if (glDeleteVertexArrays)
        {
            glDeleteVertexArrays(1, &eaglDisplay->vao);
        }
        delete eaglDisplay;
    }

    [classEAGLContext setCurrentContext:nil];
}
//------------------------------------------------------------------------------
void glMakeCurrentContextEAGL(uint64_t context, void* display)
{
    EAGLContext* eaglContext = (__bridge EAGLContext*)reinterpret_cast<void*>(context);
    EAGLDISPLAY* eaglDisplay = reinterpret_cast<EAGLDISPLAY*>(display);

    [classEAGLContext setCurrentContext:eaglContext];

    glBindFramebuffer(GL_FRAMEBUFFER, eaglDisplay->framebuffer);

    if (glBindVertexArray)
    {
        glBindVertexArray(eaglDisplay->vao);
    }
}
//------------------------------------------------------------------------------
void glPresentContextEAGL(uint64_t context, void* display)
{
    EAGLContext* eaglContext = (__bridge EAGLContext*)reinterpret_cast<void*>(context);
    EAGLDISPLAY* eaglDisplay = reinterpret_cast<EAGLDISPLAY*>(display);

    glBindRenderbuffer(GL_RENDERBUFFER, eaglDisplay->colorRenderbuffer);

    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
}
//------------------------------------------------------------------------------
uint64_t xxGraphicCreateEAGL()
{
    if (g_glLibrary == nullptr)
        g_glLibrary = xxLoadLibrary("/System/Library/Frameworks/OpenGLES.framework/OpenGLES");
    if (g_glLibrary == nullptr)
        return 0;

    if (classEAGLContext == nil)
        classEAGLContext = (__bridge Class)xxGetProcAddress(g_glLibrary, "OBJC_CLASS_$_EAGLContext");
    if (classEAGLContext == nil)
        return 0;

    EAGLContext* rootContext = [[classEAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (rootContext == nil)
    {
        rootContext = [[classEAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
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

    return reinterpret_cast<uint64_t>((__bridge_retained void*)rootContext);
}
//------------------------------------------------------------------------------
void xxGraphicDestroyEAGL(uint64_t context)
{
    glDestroyContextEAGL(context, 0, nullptr);

    classEAGLContext = nullptr;

    if (g_glLibrary)
    {
        xxFreeLibrary(g_glLibrary);
        g_glLibrary = nullptr;
    }

    xxGraphicDestroyGL();
}
//==============================================================================
