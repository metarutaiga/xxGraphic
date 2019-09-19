//==============================================================================
// xxGraphic : OpenGL WGL Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicInternal.h"
#include "xxGraphicGL.h"
#include "xxGraphicWGL.h"

#include "gl/wgl.h"
static const wchar_t* const                 g_dummy = L"xxGraphicDummyWindow";
static HMODULE                              g_glLibrary = nullptr;
static HWND                                 g_hWnd = nullptr;
static PFNWGLCREATECONTEXTPROC              wglCreateContext;
static PFNWGLDELETECONTEXTPROC              wglDeleteContext;
static PFNWGLGETCURRENTCONTEXTPROC          wglGetCurrentContext;
static PFNWGLGETCURRENTDCPROC               wglGetCurrentDC;
static PFNWGLGETPROCADDRESSPROC             wglGetProcAddress;
static PFNWGLMAKECURRENTPROC                wglMakeCurrent;
static PFNWGLSHARELISTSPROC                 wglShareLists;
static PFNWGLCREATECONTEXTATTRIBSARBPROC    wglCreateContextAttribsARB;
static PFNGLGETINTEGERVPROC                 glGetIntegerv;
static PFNGLGENVERTEXARRAYSOESPROC          glGenVertexArrays;
static PFNGLDELETEVERTEXARRAYSOESPROC       glDeleteVertexArrays;
static PFNGLBINDVERTEXARRAYOESPROC          glBindVertexArray;

//==============================================================================
//  Initialize - WGL
//==============================================================================
static bool wglSymbolFailed = false;
static void* wglSymbol(const char* name, bool* failed)
{
    void* ptr = nullptr;

    if (ptr == nullptr && wglGetProcAddress)
        ptr = wglGetProcAddress(name);

    if (ptr == nullptr && g_glLibrary)
        ptr = GetProcAddress(g_glLibrary, name);

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

    if (glGetIntegerv && glDeleteVertexArrays)
    {
        GLuint vao = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING_OES, (GLint*)&vao);
        glDeleteVertexArrays(1, &vao);
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
void glGetViewSizeWGL(void* view, unsigned int* width, unsigned int* height)
{
    HWND hWnd = reinterpret_cast<HWND>(view);

    RECT rect = {};
    if (GetClientRect(hWnd, &rect) == TRUE)
    {
        (*width) = rect.right - rect.left;
        (*height) = rect.bottom - rect.top;
    }
}
//------------------------------------------------------------------------------
uint64_t xxGraphicCreateWGL()
{
    if (g_glLibrary == nullptr)
        g_glLibrary = LoadLibraryW(L"opengl32.dll");
    if (g_glLibrary == nullptr)
        return 0;

    if (g_hWnd == nullptr)
    {
        WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_OWNDC, DefWindowProcW, 0, 0, GetModuleHandleW(nullptr), nullptr, nullptr, nullptr, nullptr, g_dummy, nullptr };
        RegisterClassExW(&wc);
        g_hWnd = CreateWindowW(g_dummy, g_dummy, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
    }

    wglSymbolFailed = false;
    wglSymbol(wglCreateContext);
    wglSymbol(wglDeleteContext);
    wglSymbol(wglGetCurrentContext);
    wglSymbol(wglGetCurrentDC);
    wglSymbol(wglGetProcAddress);
    wglSymbol(wglMakeCurrent);
    wglSymbol(wglShareLists);
    if (wglSymbolFailed)
        return 0;

    uint64_t context = glCreateContextWGL(0, g_hWnd, nullptr);
    if (context == 0)
        return 0;

    wglSymbolFailed = false;
    wglSymbol(wglCreateContextAttribsARB);
    if (wglSymbolFailed == false)
    {
        glDestroyContextWGL(context, g_hWnd, nullptr);
        context = glCreateContextWGL(0, g_hWnd, nullptr);
    }

    if (xxGraphicCreateGL(wglSymbol) == false)
    {
        xxGraphicDestroyWGL(context);
        return 0;
    }

    glCreateContext = glCreateContextWGL;
    glDestroyContext = glDestroyContextWGL;
    glMakeCurrentContext = glMakeCurrentContextWGL;
    glPresentContext = glPresentContextWGL;
    glGetViewSize = glGetViewSizeWGL;

    return context;
}
//------------------------------------------------------------------------------
void xxGraphicDestroyWGL(uint64_t context)
{
    glDestroyContextWGL(context, g_hWnd, nullptr);

    if (g_hWnd)
    {
        DestroyWindow(g_hWnd);
        UnregisterClassW(g_dummy, GetModuleHandleW(nullptr));
        g_hWnd = nullptr;
    }

    xxGraphicDestroyGL();
}
//==============================================================================
