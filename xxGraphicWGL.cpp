//==============================================================================
// xxGraphic : OpenGL WGL Source
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalGL.h"
#include <d3d11.h>
#include <d3d10.h>
#include <d3d9.h>
#include "xxGraphicGL.h"
#include "xxGraphicWGL.h"

#include "gl/wgl.h"
static void*                                g_gdiLibrary = nullptr;
static void*                                g_glLibrary = nullptr;
static HWND                                 g_dummyWindow = nullptr;
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
static PFNWGLDXOPENDEVICENVPROC             wglDXOpenDeviceNV;
static PFNWGLDXCLOSEDEVICENVPROC            wglDXCloseDeviceNV;
static PFNWGLDXREGISTEROBJECTNVPROC         wglDXRegisterObjectNV;
static PFNWGLDXUNREGISTEROBJECTNVPROC       wglDXUnregisterObjectNV;
static PFNWGLDXLOCKOBJECTSNVPROC            wglDXLockObjectsNV;
static PFNWGLDXUNLOCKOBJECTSNVPROC          wglDXUnlockObjectsNV;

//==============================================================================
//  Initialize - WGL
//==============================================================================
static void GL_APIENTRY wglBlendBarrier(void)
{
    
}
static void GL_APIENTRY wglGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision)
{
    
}
//------------------------------------------------------------------------------
static void GL_APIENTRY wglPrimitiveBoundingBox(GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW)
{

}
//------------------------------------------------------------------------------
static void GL_APIENTRY wglReleaseShaderCompiler(void)
{
    
}
//------------------------------------------------------------------------------
static void GL_APIENTRY wglShaderBinary(GLsizei count, const GLuint *shaders, GLenum binaryformat, const void *binary, GLsizei length)
{
    
}
//------------------------------------------------------------------------------
static PFNGLSHADERSOURCEPROC glShaderSource_;
//------------------------------------------------------------------------------
static void GL_APIENTRY wglShaderSourceLegacy(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length)
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
static void GL_APIENTRY wglShaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length)
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
        if (strcmp(var, "#define __FRAGMENT__ 1") == 0)
        {
            var =   "#define __FRAGMENT__ 1\n"
                    "#define gl_FragColor fragColor\n"
                    "#define texture2D texture\n"
                    "out vec4 fragColor;";
            fragmentShader = true;
        }
        if (strncmp(var, "#define attribute", sizeof("#define attribute") - 1) == 0)
            var = fragmentShader ? "#define attribute" : "#define attribute in";
        if (strncmp(var, "#define varying", sizeof("#define varying") - 1) == 0)
            var = fragmentShader ? "#define varying in" : "#define varying out";
        replaceString[i] = var;
    }

    glShaderSource_(shader, count, replaceString, length);

    xxFree(replaceString);
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
        static bool internal;
        if (failed == &internal)
            return nullptr;

        ptr = getSymbolExtension(wglSymbol, name, &internal);
    }

    if (ptr == nullptr)
    {
        bool failed = wglSymbolFailed;
        if (strcmp(name, "glClearDepthf") == 0)
            ptr = wglSymbol("glClearDepth", nullptr);
        if (strcmp(name, "glDepthRangef") == 0)
            ptr = wglSymbol("glDepthRange", nullptr);
        wglSymbolFailed = failed;
    }

    if (ptr == nullptr)
    {
        if (strcmp(name, "glBlendBarrier") == 0)
            ptr = (void*)wglBlendBarrier;
        if (strcmp(name, "glGetShaderPrecisionFormat") == 0)
            ptr = (void*)wglGetShaderPrecisionFormat;
        if (strcmp(name, "glPrimitiveBoundingBox") == 0)
            ptr = (void*)wglPrimitiveBoundingBox;
        if (strcmp(name, "glReleaseShaderCompiler") == 0)
            ptr = (void*)wglReleaseShaderCompiler;
        if (strcmp(name, "glShaderBinary") == 0)
            ptr = (void*)wglShaderBinary;
    }

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
float glGetScaleContextWGL(uint64_t context, void* view)
{
    return 1.0f;
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

    WNDCLASSEXA wc = { sizeof(WNDCLASSEXA), CS_OWNDC, ::DefWindowProcA, 0, 0, ::GetModuleHandleA(nullptr), nullptr, nullptr, nullptr, nullptr, "xxGraphic", nullptr };
    ::RegisterClassExA(&wc);
    g_dummyWindow = ::CreateWindowA(wc.lpszClassName, "xxGraphic", WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
    if (g_dummyWindow == nullptr)
        return 0;

    uint64_t context = glCreateContextWGL(0, g_dummyWindow, nullptr);
    if (context == 0)
        return 0;

    wglSymbolFailed = false;
    wglSymbol(wglCreateContextAttribsARB);
    if (wglSymbolFailed == false)
    {
        glDestroyContextWGL(context, g_dummyWindow, nullptr);
        context = glCreateContextWGL(0, g_dummyWindow, nullptr);
    }
    if (context == 0)
    {
        wglCreateContextAttribsARB = nullptr;
        context = glCreateContextWGL(0, g_dummyWindow, nullptr);
    }

    wglSymbol(wglDXOpenDeviceNV);
    wglSymbol(wglDXCloseDeviceNV);
    wglSymbol(wglDXRegisterObjectNV);
    wglSymbol(wglDXUnregisterObjectNV);
    wglSymbol(wglDXLockObjectsNV);
    wglSymbol(wglDXUnlockObjectsNV);

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
    glGetScaleContext = glGetScaleContextWGL;
    glMakeCurrentContext = glMakeCurrentContextWGL;
    glPresentContext = glPresentContextWGL;

    glShaderSource_ = glShaderSource;
    glShaderSource = version <= 200 ? wglShaderSourceLegacy : wglShaderSource;

    return context;
}
//------------------------------------------------------------------------------
void xxGraphicDestroyWGL(uint64_t context)
{
    glDestroyContextWGL(context, g_dummyWindow, nullptr);

    if (g_dummyWindow)
    {
        ::DestroyWindow(g_dummyWindow);
        ::UnregisterClassA("xxGraphic", ::GetModuleHandleA(nullptr));
        g_dummyWindow = nullptr;
    }

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
//  Extension
//==============================================================================
const void* xxGetDeviceFromDirect3DTexture(const void* texture)
{
    if (wglDXOpenDeviceNV == nullptr)
        return nullptr;

    IUnknown* unknown = (IUnknown*)texture;
    if (unknown == nullptr)
        return nullptr;

    ID3D11Texture2D* d3d11Texture = nullptr;
    ID3D10Texture2D* d3d10Texture = nullptr;
    IDirect3DTexture9* d3d9Texture = nullptr;

    if (SUCCEEDED(unknown->QueryInterface(IID_PPV_ARGS(&d3d11Texture))))
    {
        ID3D11Device* d3d11Device = nullptr;
        d3d11Texture->GetDevice(&d3d11Device);
        d3d11Texture->Release();
        if (d3d11Device)
            d3d11Device->Release();
        return wglDXOpenDeviceNV(d3d11Device);
    }
    else if (SUCCEEDED(unknown->QueryInterface(IID_PPV_ARGS(&d3d10Texture))))
    {
        ID3D10Device* d3d10Device = nullptr;
        d3d10Texture->GetDevice(&d3d10Device);
        d3d10Texture->Release();
        if (d3d10Device)
            d3d10Device->Release();
        return wglDXOpenDeviceNV(d3d10Device);
    }
    else if (SUCCEEDED(unknown->QueryInterface(IID_PPV_ARGS(&d3d9Texture))))
    {
        IDirect3DDevice9* d3d9Device = nullptr;
        d3d9Texture->GetDevice(&d3d9Device);
        d3d9Texture->Release();
        if (d3d9Device)
            d3d9Device->Release();
        return wglDXOpenDeviceNV(d3d9Device);
    }

    return nullptr;
}
//------------------------------------------------------------------------------
const void* xxCreateImageFromDirect3DTexture(const void* device, const void* texture, int id)
{
    if (wglDXRegisterObjectNV == nullptr)
        return nullptr;

    return wglDXRegisterObjectNV((HANDLE)device, (void*)texture, id, GL_TEXTURE_2D, WGL_ACCESS_READ_ONLY_NV);
}
//------------------------------------------------------------------------------
void xxDestroyImage(const void* device, const void* image)
{
    if (wglDXUnregisterObjectNV == nullptr || wglDXCloseDeviceNV == nullptr)
        return;

    wglDXUnregisterObjectNV((HANDLE)device, (HANDLE)image);
    wglDXCloseDeviceNV((HANDLE)device);
}
//==============================================================================
