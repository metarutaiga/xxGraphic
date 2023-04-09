//==============================================================================
// xxGraphic : OpenGL EAGL Source
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternalGL.h"
#include "xxGraphicEAGL.h"

#if defined(xxIOS) && !defined(xxMACCATALYST)

#define GLES_SILENCE_DEPRECATION
#import <UIKit/UIKit.h>
#import <QuartzCore/CAEAGLLayer.h>
static void*                            g_glLibrary = nullptr;
static Class                            classEAGLContext = nil;

//==============================================================================
//  Initialize - EAGL
//==============================================================================
static bool eaglSymbolFailed = false;
static void* GL_APIENTRY eaglSymbolImpl(const char* name, bool* failed = nullptr)
{
    void* ptr = nullptr;

    if (ptr == nullptr && g_glLibrary)
        ptr = xxGetProcAddress(g_glLibrary, name);

    if (ptr == nullptr)
    {
        static bool internal;
        if (failed == &internal)
            return nullptr;

        ptr = getSymbolExtension(eaglSymbolImpl, name, &internal);
    }

    if (ptr == nullptr)
        xxLog("CGL", "%s is not found", name);

    eaglSymbolFailed |= (ptr == nullptr);
    if (failed)
        (*failed) |= (ptr == nullptr);

    return ptr;
}
#define eaglSymbol(var) (void*&)var = eaglSymbolImpl(#var);
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
    float contentsScale = [[nsWindow screen] nativeScale];

    CAEAGLLayer* layer = (CAEAGLLayer*)[nsView layer];
    layer.contentsScale = contentsScale;

    EAGLContext* eaglContext = [[classEAGLContext alloc] initWithAPI:[rootContext API]
                                                          sharegroup:[rootContext sharegroup]];
    if (eaglContext == nil)
        return 0;
    [classEAGLContext setCurrentContext:eaglContext];

    if (display)
    {
        EAGLDISPLAY* eaglDisplay = xxAlloc(EAGLDISPLAY);
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

        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        eaglDisplay->vao = vao;
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
        glDeleteVertexArrays(1, &eaglDisplay->vao);
        xxFree(eaglDisplay);
    }

    [classEAGLContext setCurrentContext:nil];
}
//------------------------------------------------------------------------------
void* glGetProcAddressEAGL(const char* name)
{
    return eaglSymbolImpl(name);
}
//------------------------------------------------------------------------------
float glGetScaleContextEAGL(uint64_t context, void* view)
{
    UIWindow* nsWindow = (__bridge UIWindow*)view;
    if (nsWindow == nil)
        return 1.0f;

    return [nsWindow contentScaleFactor];
}
//------------------------------------------------------------------------------
void glMakeCurrentContextEAGL(uint64_t context, void* display)
{
    EAGLContext* eaglContext = (__bridge EAGLContext*)reinterpret_cast<void*>(context);
    EAGLDISPLAY* eaglDisplay = reinterpret_cast<EAGLDISPLAY*>(display);

    [classEAGLContext setCurrentContext:eaglContext];

    glBindFramebuffer(GL_FRAMEBUFFER, eaglDisplay->framebuffer);

    glBindVertexArray(eaglDisplay->vao);
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
uint64_t xxGraphicCreateEAGL(int version)
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

    glCreateContext = glCreateContextEAGL;
    glDestroyContext = glDestroyContextEAGL;
    glGetProcAddress = glGetProcAddressEAGL;
    glGetScaleContext = glGetScaleContextEAGL;
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
}
//==============================================================================
//  Extension
//==============================================================================
@interface EAGLContext()
- (BOOL)texImageIOSurface:(IOSurfaceRef)ioSurface
                   target:(NSUInteger)target
           internalFormat:(NSUInteger)internalFormat
                    width:(uint32_t)width
                   height:(uint32_t)height
                   format:(NSUInteger)format
                     type:(NSUInteger)type
                    plane:(uint32_t)plane
                   invert:(BOOL)invert NS_AVAILABLE_IOS(4_0);
@end
//------------------------------------------------------------------------------
void xxBindTextureWithSurface(const void* surface)
{
    if (@available(iOS 11.0, *))
    {
        IOSurfaceRef ioSurface = (IOSurfaceRef)surface;

        GLsizei width = (GLsizei)IOSurfaceGetWidth(ioSurface);
        GLsizei height = (GLsizei)IOSurfaceGetHeight(ioSurface);

        [[classEAGLContext currentContext] texImageIOSurface:ioSurface
                                                      target:GL_TEXTURE_2D
                                              internalFormat:GL_RGBA
                                                       width:width
                                                      height:height
                                                      format:GL_BGRA_EXT
                                                        type:GL_UNSIGNED_BYTE
                                                       plane:0
                                                      invert:NO];
    }
}
//------------------------------------------------------------------------------

#endif
