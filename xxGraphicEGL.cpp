//==============================================================================
// xxGraphic : OpenGL EGL Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalGL.h"
#include "xxGraphicGL.h"
#include "xxGraphicEGL.h"

#define GL_GLEXT_PROTOTYPES 0
#define EGL_EGL_PROTOTYPES 0
#define EGL_EGLEXT_PROTOTYPES 0
#include "gl/gl2ext.h"
#include "gl/egl.h"
#include "gl/eglext.h"
static void*                                    g_eglLibrary = nullptr;
static void*                                    g_glLibrary = nullptr;
static EGLConfig                                g_eglConfig = nullptr;
static EGLDisplay                               g_eglDisplay = EGL_NO_DISPLAY;
static PFNEGLCHOOSECONFIGPROC                   eglChooseConfig;
static PFNEGLCREATECONTEXTPROC                  eglCreateContext;
static PFNEGLCREATEPBUFFERSURFACEPROC           eglCreatePbufferSurface;
static PFNEGLCREATEWINDOWSURFACEPROC            eglCreateWindowSurface;
static PFNEGLDESTROYCONTEXTPROC                 eglDestroyContext;
static PFNEGLDESTROYSURFACEPROC                 eglDestroySurface;
static PFNEGLGETCURRENTCONTEXTPROC              eglGetCurrentContext;
static PFNEGLGETDISPLAYPROC                     eglGetDisplay;
static PFNEGLGETPROCADDRESSPROC                 eglGetProcAddress;
static PFNEGLINITIALIZEPROC                     eglInitialize;
static PFNEGLMAKECURRENTPROC                    eglMakeCurrent;
static PFNEGLSWAPINTERVALPROC                   eglSwapInterval;
static PFNEGLSWAPBUFFERSPROC                    eglSwapBuffers;

#if defined(xxANDROID)
static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC      glEGLImageTargetTexture2DOES;
static PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC   eglGetNativeClientBufferANDROID;
static PFNEGLCREATEIMAGEKHRPROC                 eglCreateImageKHR;
static PFNEGLDESTROYIMAGEKHRPROC                eglDestroyImageKHR;
#endif

//==============================================================================
//  Initialize - EGL
//==============================================================================
static bool eglSymbolFailed = false;
static void* GL_APIENTRY eglSymbol(const char* name, bool* failed)
{
    void* ptr = nullptr;

    if (ptr == nullptr && eglGetProcAddress)
        ptr = (void*)eglGetProcAddress(name);

    if (ptr == nullptr && g_glLibrary)
        ptr = xxGetProcAddress(g_glLibrary, name);

    if (ptr == nullptr && g_eglLibrary)
        ptr = xxGetProcAddress(g_eglLibrary, name);

    if (ptr == nullptr)
    {
        static bool internal;
        if (failed == &internal)
            return nullptr;

        ptr = getSymbolExtension(eglSymbol, name, &internal);
    }

    if (ptr == nullptr)
        xxLog("EGL", "%s is not found", name);

    eglSymbolFailed |= (ptr == nullptr);
    if (failed)
        (*failed) |= (ptr == nullptr);

    return ptr;
}
#define eglSymbol(var) (void*&)var = eglSymbol(#var, nullptr);
//------------------------------------------------------------------------------
uint64_t glCreateContextEGL(uint64_t instance, void* view, void** surface)
{
    EGLContext eglShareContext = reinterpret_cast<EGLContext>(instance);
    EGLDisplay eglDisplay = g_eglDisplay;
    EGLConfig eglConfig = g_eglConfig;

    EGLSurface eglSurface = EGL_NO_SURFACE;
    if (view)
    {
        eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, (EGLNativeWindowType)view, nullptr);
    }
    else
    {
        EGLint surfaceAttributes[] =
        {
            EGL_WIDTH,  1,
            EGL_HEIGHT, 1,
            EGL_NONE
        };

        eglSurface = eglCreatePbufferSurface(eglDisplay, eglConfig, surfaceAttributes);
    }
    if (eglSurface == EGL_NO_SURFACE)
        return 0;

    EGLint contextAttibutes[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    EGLContext eglContext = eglCreateContext(eglDisplay, eglConfig, eglShareContext, contextAttibutes);
    if (eglContext == EGL_NO_CONTEXT)
        return 0;
 
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    eglSwapInterval(eglDisplay, 0);

    eglSymbol(glGetIntegerv);
    eglSymbol(glGenVertexArrays);
    eglSymbol(glDeleteVertexArrays);
    eglSymbol(glBindVertexArray);
    if (glGenVertexArrays && glBindVertexArray)
    {
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    if (surface)
    {
        (*surface) = eglSurface;
    }
    else
    {
        eglDestroySurface(eglDisplay, eglSurface);
    }

    return reinterpret_cast<uint64_t>(eglContext);
}
//------------------------------------------------------------------------------
void glDestroyContextEGL(uint64_t context, void* view, void* surface)
{
    EGLContext eglContext = reinterpret_cast<EGLContext>(context);
    if (eglContext == EGL_NO_CONTEXT)
        return;
    EGLDisplay eglDisplay = g_eglDisplay;
    EGLSurface eglSurface = reinterpret_cast<EGLSurface>(surface);

    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

    if (glGetIntegerv && glDeleteVertexArrays && glBindVertexArray)
    {
        GLuint vao = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&vao);
        glDeleteVertexArrays(1, &vao);
        glBindVertexArray(0);
    }

    eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDisplay, eglContext);
    eglDestroySurface(eglDisplay, eglSurface);
}
//------------------------------------------------------------------------------
void glMakeCurrentContextEGL(uint64_t context, void* surface)
{
    EGLContext eglContext = reinterpret_cast<EGLContext>(context);
    EGLDisplay eglDisplay = g_eglDisplay;
    EGLSurface eglSurface = reinterpret_cast<EGLSurface>(surface);

    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
}
//------------------------------------------------------------------------------
void glPresentContextEGL(uint64_t context, void* surface)
{
    EGLDisplay eglDisplay = g_eglDisplay;
    EGLSurface eglSurface = reinterpret_cast<EGLSurface>(surface);

    eglSwapBuffers(eglDisplay, eglSurface);
}
//------------------------------------------------------------------------------
uint64_t xxGraphicCreateEGL(int version)
{
#if defined(xxANDROID)
    if (g_eglLibrary == nullptr)
        g_eglLibrary = xxLoadLibrary("libEGL.so");
#elif defined(xxWINDOWS)
    if (g_eglLibrary == nullptr)
        g_eglLibrary = xxLoadLibrary("libEGL.dll");
#endif
    if (g_eglLibrary == nullptr)
        return 0;

#if defined(xxANDROID)
    if (g_glLibrary == nullptr)
        g_glLibrary = xxLoadLibrary("libGLESv2.so");
#elif defined(xxWINDOWS)
    if (g_glLibrary == nullptr)
        g_glLibrary = xxLoadLibrary("libGLESv2.dll");
#endif
    if (g_glLibrary == nullptr)
        return 0;

    eglSymbolFailed = false;
    eglSymbol(eglChooseConfig);
    eglSymbol(eglCreateContext);
    eglSymbol(eglCreatePbufferSurface);
    eglSymbol(eglCreateWindowSurface);
    eglSymbol(eglDestroyContext);
    eglSymbol(eglDestroySurface);
    eglSymbol(eglGetCurrentContext);
    eglSymbol(eglGetDisplay);
    eglSymbol(eglGetProcAddress);
    eglSymbol(eglInitialize);
    eglSymbol(eglMakeCurrent);
    eglSymbol(eglSwapInterval);
    eglSymbol(eglSwapBuffers);
    if (eglSymbolFailed)
        return 0;

#if defined(xxANDROID)
    eglSymbol(glEGLImageTargetTexture2DOES);
    eglSymbol(eglGetNativeClientBufferANDROID);
    eglSymbol(eglCreateImageKHR);
    eglSymbol(eglDestroyImageKHR);
#endif

    EGLDisplay eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDisplay == EGL_NO_DISPLAY)
        return 0;
    g_eglDisplay = eglDisplay;

    EGLint majorVersion = 0;
    EGLint minorVersion = 0;
    if (eglInitialize(eglDisplay, &majorVersion, &minorVersion) == EGL_FALSE)
        return 0;

    EGLint configs = 0;
    EGLint configAttributes[] =
    {
        EGL_SURFACE_TYPE,       EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES2_BIT,
        EGL_DEPTH_SIZE,         24,
        EGL_RED_SIZE,           8,
        EGL_GREEN_SIZE,         8,
        EGL_BLUE_SIZE,          8,
        EGL_NONE
    };

    EGLConfig eglConfig = nullptr;
    if (eglChooseConfig(eglDisplay, configAttributes, &eglConfig, 1, &configs) == EGL_FALSE)
        return 0;
    g_eglConfig = eglConfig;

    uint64_t context = glCreateContextEGL(0, nullptr, nullptr);
    if (context == 0)
        return 0;

    bool success = false;
    switch (version)
    {
    case 320:
        success = xxGraphicCreateGLES32(eglSymbol);
        break;
    case 310:
        success = xxGraphicCreateGLES31(eglSymbol);
        break;
    case 300:
        success = xxGraphicCreateGLES3(eglSymbol);
        break;
    case 200:
    default:
        success = xxGraphicCreateGLES2(eglSymbol);
        break;
    }
    if (success == false)
    {
        xxGraphicDestroyEGL(context);
        return 0;
    }

    glCreateContext = glCreateContextEGL;
    glDestroyContext = glDestroyContextEGL;
    glMakeCurrentContext = glMakeCurrentContextEGL;
    glPresentContext = glPresentContextEGL;

    return context;
}
//------------------------------------------------------------------------------
void xxGraphicDestroyEGL(uint64_t context)
{
    glDestroyContextEGL(context, nullptr, nullptr);

    if (g_glLibrary)
    {
        xxFreeLibrary(g_glLibrary);
        g_glLibrary = nullptr;
    }

    if (g_eglLibrary)
    {
        xxFreeLibrary(g_eglLibrary);
        g_eglLibrary = nullptr;
    }

    g_eglConfig = nullptr;
    g_eglDisplay = EGL_NO_DISPLAY;

    xxGraphicDestroyGL();
}
//==============================================================================
//  Extension
//==============================================================================
const void* xxCreateImageFromHardwareBuffer(const void* hardwareBuffer)
{
    if (hardwareBuffer == nullptr)
        return 0;

#if defined(xxANDROID)
    EGLint eglImageAttributes[] = { EGL_NONE };
    EGLClientBuffer clientBuffer = eglGetNativeClientBufferANDROID((AHardwareBuffer*)hardwareBuffer);
    EGLImageKHR image = eglCreateImageKHR(g_eglDisplay, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, clientBuffer, eglImageAttributes);
    return image;
#endif

    return 0;
}
//------------------------------------------------------------------------------
void xxBindTextureWithImage(const void* image)
{
    if (image == nullptr)
        return;

#if defined(xxANDROID)
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, (EGLImageKHR)image);
#endif
}
//------------------------------------------------------------------------------
void xxDestroyImage(const void* image)
{
    if (image == nullptr)
        return;

#if defined(xxANDROID)
    eglDestroyImageKHR(g_eglDisplay, (EGLImageKHR)image);
#endif
}
//==============================================================================
