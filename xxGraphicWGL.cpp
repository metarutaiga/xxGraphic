//==============================================================================
// xxGraphic : OpenGL WGL Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "xxGraphicGL.h"
#include "xxGraphicWGL.h"

#include "gl/wgl.h"
static void*                                g_gdiLibrary = nullptr;
static void*                                g_glLibrary = nullptr;
static int                                  (WINAPI *ChoosePixelFormat)(HDC, CONST PIXELFORMATDESCRIPTOR*);
static BOOL                                 (WINAPI *SetPixelFormat)(HDC, int, CONST PIXELFORMATDESCRIPTOR*);
static BOOL                                 (WINAPI *SwapBuffers)(HDC);
static PFNWGLCREATECONTEXTPROC              wglCreateContext;
static PFNWGLDELETECONTEXTPROC              wglDeleteContext;
static PFNWGLGETCURRENTCONTEXTPROC          wglGetCurrentContext;
static PFNWGLGETCURRENTDCPROC               wglGetCurrentDC;
static PFNWGLGETPROCADDRESSPROC             wglGetProcAddress;
static PFNWGLMAKECURRENTPROC                wglMakeCurrent;
static PFNWGLSHARELISTSPROC                 wglShareLists;
static PFNWGLCREATECONTEXTATTRIBSARBPROC    wglCreateContextAttribsARB;

//==============================================================================
//  Initialize - WGL
//==============================================================================
static void GL_APIENTRY wglBlendBarrier(void)
{

}
//------------------------------------------------------------------------------
static void GL_APIENTRY wglPrimitiveBoundingBox(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW)
{

}
//------------------------------------------------------------------------------
static bool wglSymbolFailed = false;
static void* GL_APIENTRY wglSymbol(const char* name, bool* failed)
{
    void* ptr = nullptr;

    if (ptr == nullptr && wglGetProcAddress)
        ptr = wglGetProcAddress(name);

    if (ptr == nullptr && g_glLibrary)
        ptr = xxGetProcAddress(g_glLibrary, name);

    if (ptr == nullptr && g_gdiLibrary)
        ptr = xxGetProcAddress(g_gdiLibrary, name);

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
            ptr = wglSymbol(extensionName, &internal);
            if (ptr != nullptr)
                break;
        }
    }

    if (ptr == nullptr && strcmp(name, "glBlendBarrier") == 0)
        ptr = (void*)wglBlendBarrier;

    if (ptr == nullptr && strcmp(name, "glPrimitiveBoundingBox") == 0)
        ptr = (void*)wglPrimitiveBoundingBox;

    if (ptr == nullptr)
        xxLog("WGL", "%s is not found", name);

    wglSymbolFailed |= (ptr == nullptr);
    if (failed)
        (*failed) |= (ptr == nullptr);

    return ptr;
}
#define wglSymbol(var) (void*&)var = wglSymbol(#var, nullptr);
//------------------------------------------------------------------------------
uint64_t glCreateContextWGL(uint64_t instance, void* view, void** display)
{
    HGLRC hInstance = reinterpret_cast<HGLRC>(instance);
    HWND hWnd = reinterpret_cast<HWND>(view);
    HDC hDC = GetDC(hWnd);

    PIXELFORMATDESCRIPTOR desc = {};
    desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    desc.nVersion = 1;
    desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    desc.iPixelType = PFD_TYPE_RGBA;
    desc.cColorBits = 32;
    desc.cDepthBits = 24;
    desc.cStencilBits = 8;
    desc.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(hDC, &desc);
    SetPixelFormat(hDC, pixelFormat, &desc);

    HGLRC hGLRC;
    if (wglCreateContextAttribsARB)
    {
        int attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 2,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0,
        };

        hGLRC = wglCreateContextAttribsARB(hDC, hInstance, attribs);
        if (hGLRC == nullptr)
        {
            ReleaseDC(hWnd, hDC);
            return 0;
        }
    }
    else
    {
        hGLRC = wglCreateContext(hDC);
        if (hGLRC == nullptr)
        {
            ReleaseDC(hWnd, hDC);
            return 0;
        }

        if (hInstance)
        {
            wglShareLists(hInstance, hGLRC);
        }
    }
    wglMakeCurrent(hDC, hGLRC);

    wglSymbol(glGetIntegerv);
    wglSymbol(glGenVertexArrays);
    wglSymbol(glDeleteVertexArrays);
    wglSymbol(glBindVertexArray);
    if (glGenVertexArrays && glBindVertexArray)
    {
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    if (display)
    {
        (*display) = hDC;
    }
    else
    {
        ReleaseDC(hWnd, hDC);
    }

    return reinterpret_cast<uint64_t>(hGLRC);
}
//------------------------------------------------------------------------------
void glDestroyContextWGL(uint64_t context, void* view, void* display)
{
    HGLRC hGLRC = reinterpret_cast<HGLRC>(context);
    if (hGLRC == nullptr)
        return;
    HWND hWnd = reinterpret_cast<HWND>(view);
    HDC hDC = reinterpret_cast<HDC>(display);
    if (hDC == nullptr)
        hDC = GetDC(hWnd);

    wglMakeCurrent(hDC, hGLRC);

    if (glGetIntegerv && glDeleteVertexArrays && glBindVertexArray)
    {
        GLuint vao = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&vao);
        glDeleteVertexArrays(1, &vao);
        glBindVertexArray(0);
    }

    wglMakeCurrent(hDC, nullptr);
    wglDeleteContext(hGLRC);
    ReleaseDC(hWnd, hDC);
}
//------------------------------------------------------------------------------
void glMakeCurrentContextWGL(uint64_t context, void* display)
{
    HGLRC hGLRC = reinterpret_cast<HGLRC>(context);
    HDC hDC = reinterpret_cast<HDC>(display);

    wglMakeCurrent(hDC, hGLRC);
}
//------------------------------------------------------------------------------
void glPresentContextWGL(uint64_t context, void* display)
{
    HDC hDC = reinterpret_cast<HDC>(display);

    SwapBuffers(hDC);
}
//------------------------------------------------------------------------------
uint64_t xxGraphicCreateWGL(int version)
{
    if (g_gdiLibrary == nullptr)
        g_gdiLibrary = xxLoadLibrary("gdi32.dll");
    if (g_gdiLibrary == nullptr)
        return 0;

    if (g_glLibrary == nullptr)
        g_glLibrary = xxLoadLibrary("opengl32.dll");
    if (g_glLibrary == nullptr)
        return 0;

    wglSymbolFailed = false;
    wglSymbol(ChoosePixelFormat);
    wglSymbol(SetPixelFormat);
    wglSymbol(SwapBuffers);
    wglSymbol(wglCreateContext);
    wglSymbol(wglDeleteContext);
    wglSymbol(wglGetCurrentContext);
    wglSymbol(wglGetCurrentDC);
    wglSymbol(wglGetProcAddress);
    wglSymbol(wglMakeCurrent);
    wglSymbol(wglShareLists);
    if (wglSymbolFailed)
        return 0;

    uint64_t context = glCreateContextWGL(0, GetDesktopWindow(), nullptr);
    if (context == 0)
        return 0;

    wglSymbolFailed = false;
    wglSymbol(wglCreateContextAttribsARB);
    if (wglSymbolFailed == false)
    {
        glDestroyContextWGL(context, GetDesktopWindow(), nullptr);
        context = glCreateContextWGL(0, GetDesktopWindow(), nullptr);
    }

    bool success = false;
    switch (version)
    {
    case 320:
        success = xxGraphicCreateGLES32(wglSymbol);
        break;
    case 310:
        success = xxGraphicCreateGLES31(wglSymbol);
        break;
    case 300:
        success = xxGraphicCreateGLES3(wglSymbol);
        break;
    case 200:
    default:
        success = xxGraphicCreateGLES2(wglSymbol);
        break;
    }
    if (success == false)
    {
        xxGraphicDestroyWGL(context);
        return 0;
    }

    glCreateContext = glCreateContextWGL;
    glDestroyContext = glDestroyContextWGL;
    glMakeCurrentContext = glMakeCurrentContextWGL;
    glPresentContext = glPresentContextWGL;

    return context;
}
//------------------------------------------------------------------------------
void xxGraphicDestroyWGL(uint64_t context)
{
    glDestroyContextWGL(context, GetDesktopWindow(), nullptr);

    if (g_glLibrary)
    {
        xxFreeLibrary(g_glLibrary);
        g_glLibrary = nullptr;
    }

    if (g_gdiLibrary)
    {
        xxFreeLibrary(g_gdiLibrary);
        g_gdiLibrary = nullptr;
    }

    xxGraphicDestroyGL();
}
//==============================================================================
