//==============================================================================
// xxGraphic : Direct3D 6.0 Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#ifndef _M_IX86
#define xxAPI xxEXTERN
#endif
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalD3D.h"
#include "xxGraphicD3D6.h"

#define DIRECTDRAW_VERSION          0x600
#define DIRECT3D_VERSION            0x600
#define D3D_OVERLOADS
#include "dxsdk/ddraw.h"
#include "dxsdk/d3d.h"
typedef HRESULT (WINAPI *PFN_DIRECT_DRAW_CREATE)(GUID*, LPDIRECTDRAW*, IUnknown*);
#define D3DRTYPE_CONSTANTBUFFER     0
#define D3DRTYPE_INDEXBUFFER        1
#define D3DRTYPE_VERTEXBUFFER       2

static void*                        g_ddrawLibrary = nullptr;
static LPDIRECTDRAW4                g_ddraw = nullptr;
static LPDIRECTDRAWSURFACE4         g_primarySurface = nullptr;

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D6()
{
    if (g_ddrawLibrary == nullptr)
        g_ddrawLibrary = xxLoadLibrary("ddraw.dll");
    if (g_ddrawLibrary == nullptr)
        return 0;

    PFN_DIRECT_DRAW_CREATE DirectDrawCreate;
    (void*&)DirectDrawCreate = xxGetProcAddress(g_ddrawLibrary, "DirectDrawCreate");
    if (DirectDrawCreate == nullptr)
        return 0;

    LPDIRECTDRAW ddraw = nullptr;
    if (ddraw == nullptr)
    {
        HRESULT result = DirectDrawCreate(nullptr, &ddraw, nullptr);
        if (result != S_OK)
            return 0;
        ddraw->SetCooperativeLevel(nullptr, DDSCL_FPUSETUP | DDSCL_NORMAL);
    }

    LPDIRECTDRAW4 ddraw4 = nullptr;
    if (ddraw4 == nullptr)
    {
        HRESULT result = ddraw->QueryInterface(IID_PPV_ARGS(&ddraw4));
        if (result != S_OK)
            return 0;
        ddraw->Release();
    }
    g_ddraw = ddraw4;

    LPDIRECT3D3 d3d = nullptr;
    HRESULT result = ddraw4->QueryInterface(IID_PPV_ARGS(&d3d));
    if (result != S_OK)
        return 0;

    PatchD3DIM("d3dim.dll");

    xxRegisterFunction(D3D6);

    return reinterpret_cast<uint64_t>(d3d);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceD3D6(uint64_t instance)
{
    LPDIRECT3D3 d3d = reinterpret_cast<LPDIRECT3D3>(instance);

    SafeRelease(d3d);
    SafeRelease(g_ddraw);

    if (g_ddrawLibrary)
    {
        xxFreeLibrary(g_ddrawLibrary);
        g_ddrawLibrary = nullptr;
    }

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceD3D6(uint64_t instance)
{
    LPDIRECT3D3 d3d = reinterpret_cast<LPDIRECT3D3>(instance);
    if (d3d == nullptr)
        return 0;

    LPDIRECTDRAWSURFACE4 primarySurface = nullptr;
    if (primarySurface == nullptr)
    {
        DDSURFACEDESC2 desc = {};
        desc.dwSize = sizeof(DDSURFACEDESC2);
        desc.dwFlags = DDSD_CAPS;
        desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

        HRESULT result = g_ddraw->CreateSurface(&desc, &primarySurface, nullptr);
        if (result != S_OK)
            return 0;

        g_primarySurface = primarySurface;
    }

    LPDIRECTDRAWSURFACE4 backSurface = nullptr;
    if (backSurface == nullptr)
    {
        DDSURFACEDESC2 desc = {};
        desc.dwSize = sizeof(DDSURFACEDESC2);
        desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
        desc.dwWidth = 1;
        desc.dwHeight = 1;

        HRESULT result = g_ddraw->CreateSurface(&desc, &backSurface, nullptr);
        if (result != S_OK)
            return 0;
    }

    const IID* iid = nullptr;
    LPDIRECT3DDEVICE3 d3dDevice = nullptr;
    if (d3dDevice == nullptr)
    {
        iid = &__uuidof(IDirect3DTnLHalDevice);
        d3d->CreateDevice(*iid, backSurface, &d3dDevice, nullptr);
    }
    if (d3dDevice == nullptr)
    {
        iid = &__uuidof(IDirect3DHALDevice);
        d3d->CreateDevice(*iid, backSurface, &d3dDevice, nullptr);
    }
    if (d3dDevice == nullptr)
    {
        backSurface->Release();
        return 0;
    }
    backSurface->Release();

    LPDIRECT3DVIEWPORT3 viewport = nullptr;
    d3d->CreateViewport(&viewport, nullptr);
    d3dDevice->AddViewport(viewport);
    d3dDevice->SetCurrentViewport(viewport);
    viewport->Release();

    IUnknown* unknown = nullptr;
    xxLocalBreak()
    {
        if ((*iid) == __uuidof(IDirect3DTnLHalDevice))
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "6.0 Hardware Transform and Lighting", xxGetDeviceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(IDirect3DDevice3), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "6.0", xxGetDeviceName());
            break;
        }
    }
    SafeRelease(unknown);

    return reinterpret_cast<uint64_t>(d3dDevice);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceD3D6(uint64_t device)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(device);

    SafeRelease(d3dDevice);
    SafeRelease(g_primarySurface);
}
//------------------------------------------------------------------------------
bool xxResetDeviceD3D6(uint64_t device)
{
    return false;
}
//------------------------------------------------------------------------------
bool xxTestDeviceD3D6(uint64_t device)
{
    HRESULT hResult = g_ddraw->TestCooperativeLevel();
    while (hResult == DDERR_NOEXCLUSIVEMODE || hResult == DDERR_EXCLUSIVEMODEALREADYSET)
    {
        Sleep(100);
        hResult = g_ddraw->TestCooperativeLevel();
    }
    return hResult == DD_OK;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameD3D6()
{
    return "Direct3D 6.0";
}
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t xxCreateSwapchainD3D6(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DSWAPCHAIN3* d3dOldSwapchain = reinterpret_cast<D3DSWAPCHAIN3*>(oldSwapchain);
    if (d3dOldSwapchain && d3dOldSwapchain->hWnd == view && d3dOldSwapchain->width == width && d3dOldSwapchain->height == height)
        return oldSwapchain;
    D3DSWAPCHAIN3* swapchain = xxAlloc(D3DSWAPCHAIN3);
    if (swapchain == nullptr)
        return 0;

    xxDestroySwapchainD3D6(oldSwapchain);

    HWND hWnd = (HWND)view;

    LPDIRECTDRAWSURFACE4 backSurface = nullptr;
    if (backSurface == nullptr)
    {
        DDSURFACEDESC2 desc = {};
        desc.dwSize = sizeof(DDSURFACEDESC2);
        desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        desc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE;
        desc.dwWidth = width;
        desc.dwHeight = height;

        HRESULT result = g_ddraw->CreateSurface(&desc, &backSurface, nullptr);
        if (result != S_OK)
            return 0;
    }

    LPDIRECTDRAWSURFACE4 depthSurface = nullptr;
    if (depthSurface == nullptr)
    {
        DDSURFACEDESC2 desc = {};
        desc.dwSize = sizeof(DDSURFACEDESC2);
        desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
        desc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
        desc.dwWidth = width;
        desc.dwHeight = height;
        desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
        desc.ddpfPixelFormat.dwFlags = DDPF_ZBUFFER | DDPF_STENCILBUFFER;
        desc.ddpfPixelFormat.dwZBufferBitDepth = 32;
        desc.ddpfPixelFormat.dwStencilBitDepth = 8;
        desc.ddpfPixelFormat.dwZBitMask = 0x00FFFFFF;
        desc.ddpfPixelFormat.dwStencilBitMask = 0xFF000000;

        HRESULT result = g_ddraw->CreateSurface(&desc, &depthSurface, nullptr);
        if (result != S_OK)
        {
            backSurface->Release();
            return 0;
        }

        backSurface->AddAttachedSurface(depthSurface);
    }

    LPDIRECTDRAWCLIPPER clipper = nullptr;
    if (clipper == nullptr)
    {
        HRESULT result = g_ddraw->CreateClipper(0, &clipper, nullptr);
        if (result != S_OK)
        {
            backSurface->Release();
            depthSurface->Release();
            return 0;
        }

        clipper->SetHWnd(0, hWnd);
    }

    swapchain->backSurface = backSurface;
    swapchain->depthSurface = depthSurface;
    swapchain->clipper = clipper;
    swapchain->hWnd = hWnd;
    swapchain->width = width;
    swapchain->height = height;

    return reinterpret_cast<uint64_t>(swapchain);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainD3D6(uint64_t swapchain)
{
    D3DSWAPCHAIN3* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN3*>(swapchain);
    if (d3dSwapchain == nullptr)
        return;

    SafeRelease(d3dSwapchain->backSurface);
    SafeRelease(d3dSwapchain->depthSurface);
    SafeRelease(d3dSwapchain->clipper);
    xxFree(d3dSwapchain);
}
//------------------------------------------------------------------------------
void xxPresentSwapchainD3D6(uint64_t swapchain)
{
    D3DSWAPCHAIN3* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN3*>(swapchain);
    if (d3dSwapchain == nullptr)
        return;

    RECT rect = {};
    GetClientRect(d3dSwapchain->hWnd, &rect);
    ClientToScreen(d3dSwapchain->hWnd, (POINT*)&rect.left);
    ClientToScreen(d3dSwapchain->hWnd, (POINT*)&rect.right);

    g_primarySurface->SetClipper(d3dSwapchain->clipper);
    g_primarySurface->Blt(&rect, d3dSwapchain->backSurface, nullptr, DDBLT_WAIT, nullptr);
}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferD3D6(uint64_t device, uint64_t swapchain)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DSWAPCHAIN3* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN3*>(swapchain);
    if (d3dSwapchain == nullptr)
        return 0;

    HRESULT hResult = d3dDevice->SetRenderTarget(d3dSwapchain->backSurface, 0);
    if (hResult != S_OK)
        return 0;

    return device;
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferD3D6(uint64_t device, uint64_t swapchain, float* scale)
{
    if (scale)
    {
        (*scale) = 1.0f;
    }

    return swapchain;
}
//==============================================================================
//  Command Buffer
//==============================================================================
bool xxBeginCommandBufferD3D6(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(commandBuffer);
    if (d3dDevice == nullptr)
        return false;

    HRESULT hResult = d3dDevice->BeginScene();
    return (hResult == S_OK);
}
//------------------------------------------------------------------------------
void xxEndCommandBufferD3D6(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    HRESULT hResult = d3dDevice->EndScene();
}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferD3D6(uint64_t commandBuffer, uint64_t swapchain)
{
}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxCreateRenderPassD3D6(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
{
    DWORD flags = 0;

    if (clearColor)
        flags |= D3DCLEAR_TARGET;
    if (clearDepth)
        flags |= D3DCLEAR_ZBUFFER;
    if (clearStencil)
        flags |= D3DCLEAR_STENCIL;

    return flags;
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassD3D6(uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassD3D6(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(commandBuffer);
    if (d3dDevice == nullptr)
        return 0;
    D3DFRAMEBUFFER3* d3dFramebuffer = reinterpret_cast<D3DFRAMEBUFFER3*>(framebuffer);
    if (d3dFramebuffer == nullptr)
        return 0;
    DWORD d3dFlags = static_cast<DWORD>(renderPass);

    if (d3dFlags)
    {
        LPDIRECT3DVIEWPORT3 viewport = nullptr;
        d3dDevice->GetCurrentViewport(&viewport);
        viewport->Release();

        D3DVIEWPORT2 vp = {};
        vp.dwSize = sizeof(D3DVIEWPORT2);
        vp.dwX = 0;
        vp.dwY = 0;
        vp.dwWidth = width;
        vp.dwHeight = height;
        vp.dvClipX = -1.0f;
        vp.dvClipY = 1.0f;
        vp.dvClipWidth = 2.0f;
        vp.dvClipHeight = 2.0f;
        vp.dvMinZ = 0.0f;
        vp.dvMaxZ = 1.0f;
        viewport->SetViewport2(&vp);

        D3DRECT rect = {};
        rect.lX2 = width;
        rect.lY2 = height;

        viewport->Clear2(1, &rect, d3dFlags, D3DRGBA(color[0], color[1], color[2], color[3]), depth, stencil);
    }

    return commandBuffer;
}
//------------------------------------------------------------------------------
void xxEndRenderPassD3D6(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{

}
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t xxCreateVertexAttributeD3D6(uint64_t device, int count, int* attribute)
{
    D3DVERTEXATTRIBUTE3 d3dVertexAttribute = {};
    int stride = 0;

    for (int i = 0; i < count; ++i)
    {
        int stream = (*attribute++);
        int offset = (*attribute++);
        int element = (*attribute++);
        int size = (*attribute++);

        stride += size;

        if (offset == 0 && element == 3 && size == sizeof(float) * 3)
            d3dVertexAttribute.fvf |= D3DFVF_XYZ;
        if (offset != 0 && element == 3 && size == sizeof(float) * 3)
            d3dVertexAttribute.fvf |= D3DFVF_NORMAL;
        if (offset != 0 && element == 4 && size == sizeof(char) * 4)
            d3dVertexAttribute.fvf |= D3DFVF_DIFFUSE;
        if (offset != 0 && element == 2 && size == sizeof(float) * 2)
            d3dVertexAttribute.fvf += D3DFVF_TEX1;
    }

    d3dVertexAttribute.stride = stride;

    return d3dVertexAttribute.value;
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeD3D6(uint64_t vertexAttribute)
{

}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateConstantBufferD3D6(uint64_t device, int size)
{
    char* d3dBuffer = xxAlloc(char, size);

    return reinterpret_cast<uint64_t>(d3dBuffer) | D3DRTYPE_CONSTANTBUFFER;
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferD3D6(uint64_t device, int size)
{
    char* d3dBuffer = xxAlloc(char, size);

    return reinterpret_cast<uint64_t>(d3dBuffer) | D3DRTYPE_INDEXBUFFER;
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferD3D6(uint64_t device, int size, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(device);
    if (d3dDevice == nullptr)
        return 0;
    LPDIRECT3D3 d3d = nullptr;
    if (d3d == nullptr)
    {
        HRESULT result = d3dDevice->GetDirect3D(&d3d);
        if (result != S_OK)
            return 0;
        d3d->Release();
    }
    D3DVERTEXATTRIBUTE3 d3dVertexAttribute = { vertexAttribute };

    D3DVERTEXBUFFERDESC desc = {};
    desc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
    desc.dwCaps = D3DVBCAPS_WRITEONLY;
    desc.dwFVF = d3dVertexAttribute.fvf;
    desc.dwNumVertices = (size / d3dVertexAttribute.stride);

    LPDIRECT3DVERTEXBUFFER buffer = nullptr;
    HRESULT result = d3d->CreateVertexBuffer(&desc, &buffer, 0, nullptr);
    if (result != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(buffer) | D3DRTYPE_VERTEXBUFFER;
}
//------------------------------------------------------------------------------
void xxDestroyBufferD3D6(uint64_t device, uint64_t buffer)
{
    switch (getResourceType(buffer))
    {
    case D3DRTYPE_CONSTANTBUFFER:
    case D3DRTYPE_INDEXBUFFER:
    {
        char* d3dBuffer = reinterpret_cast<char*>(getResourceData(buffer));

        xxFree(d3dBuffer);
        break;
    }
    case D3DRTYPE_VERTEXBUFFER:
    {
        LPDIRECT3DVERTEXBUFFER d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER>(getResourceData(buffer));
        if (d3dVertexBuffer == nullptr)
            return;

        d3dVertexBuffer->Release();
        break;
    }
    default:
        break;
    }
}
//------------------------------------------------------------------------------
void* xxMapBufferD3D6(uint64_t device, uint64_t buffer)
{
    switch (getResourceType(buffer))
    {
    case D3DRTYPE_CONSTANTBUFFER:
    case D3DRTYPE_INDEXBUFFER:
    {
        char* ptr = reinterpret_cast<char*>(getResourceData(buffer));
        if (ptr == nullptr)
            break;

        return ptr;
    }
    case D3DRTYPE_VERTEXBUFFER:
    {
        LPDIRECT3DVERTEXBUFFER d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER>(getResourceData(buffer));
        if (d3dVertexBuffer == nullptr)
            return nullptr;

        void* ptr = nullptr;
        HRESULT hResult = d3dVertexBuffer->Lock(DDLOCK_DISCARDCONTENTS, &ptr, nullptr);
        if (hResult != S_OK)
            break;

        return ptr;
    }
    default:
        break;
    }

    return nullptr;
}
//------------------------------------------------------------------------------
void xxUnmapBufferD3D6(uint64_t device, uint64_t buffer)
{
    switch (getResourceType(buffer))
    {
    case D3DRTYPE_CONSTANTBUFFER:
    case D3DRTYPE_INDEXBUFFER:
    {
        return;
    }
    case D3DRTYPE_VERTEXBUFFER:
    {
        LPDIRECT3DVERTEXBUFFER d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER>(getResourceData(buffer));
        if (d3dVertexBuffer == nullptr)
            return;

        d3dVertexBuffer->Unlock();
        return;
    }
    default:
        break;
    }
}
//==============================================================================
//  Texture
//==============================================================================
uint64_t xxCreateTextureD3D6(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, const void* external)
{
    DDSURFACEDESC2 desc = {};
    desc.dwSize = sizeof(DDSURFACEDESC2);
    desc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
    desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
    desc.ddsCaps.dwCaps2 = DDSCAPS2_HINTDYNAMIC;
    desc.dwWidth = width;
    desc.dwHeight = height;
    desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    desc.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
    desc.ddpfPixelFormat.dwRGBBitCount = 32;
    desc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
    desc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
    desc.ddpfPixelFormat.dwBBitMask = 0x000000FF;
    desc.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;

    LPDIRECTDRAWSURFACE4 surface = nullptr;
    HRESULT result = g_ddraw->CreateSurface(&desc, &surface, nullptr);
    if (result != S_OK)
        return 0;
 
    LPDIRECT3DTEXTURE2 texture = nullptr;
    surface->QueryInterface(IID_PPV_ARGS(&texture));
    surface->Release();

    return reinterpret_cast<uint64_t>(texture);
}
//------------------------------------------------------------------------------
void xxDestroyTextureD3D6(uint64_t texture)
{
    LPDIRECT3DTEXTURE2 d3dTexture = reinterpret_cast<LPDIRECT3DTEXTURE2>(texture);

    SafeRelease(d3dTexture);
}
//------------------------------------------------------------------------------
void* xxMapTextureD3D6(uint64_t device, uint64_t texture, int* stride, int level, int array)
{
    LPDIRECT3DTEXTURE2 d3dTexture = reinterpret_cast<LPDIRECT3DTEXTURE2>(texture);
    if (d3dTexture == nullptr)
        return nullptr;

    LPDIRECTDRAWSURFACE4 surface = nullptr;
    d3dTexture->QueryInterface(IID_PPV_ARGS(&surface));
    if (surface == nullptr)
        return nullptr;
    surface->Release();

    DDSURFACEDESC2 desc = {};
    if (desc.dwSize == 0)
    {
        desc.dwSize = sizeof(DDSURFACEDESC2);
        HRESULT result = surface->GetSurfaceDesc(&desc);
        if (result != S_OK)
            return nullptr;
    }

    HRESULT result = surface->Lock(nullptr, &desc, DDLOCK_WAIT, nullptr);
    if (result != S_OK)
        return nullptr;

    (*stride) = desc.lPitch;
    return desc.lpSurface;
}
//------------------------------------------------------------------------------
void xxUnmapTextureD3D6(uint64_t device, uint64_t texture, int level, int array)
{
    LPDIRECT3DTEXTURE2 d3dTexture = reinterpret_cast<LPDIRECT3DTEXTURE2>(texture);
    if (d3dTexture == nullptr)
        return;

    LPDIRECTDRAWSURFACE4 surface = nullptr;
    d3dTexture->QueryInterface(IID_PPV_ARGS(&surface));
    if (surface == nullptr)
        return;
    surface->Release();
 
    surface->Unlock(nullptr);
}
//==============================================================================
//  Sampler
//==============================================================================
uint64_t xxCreateSamplerD3D6(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    D3DSAMPLER3 d3dSampler = {};

    d3dSampler.addressU = clampU ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
    d3dSampler.addressV = clampV ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
    d3dSampler.addressW = clampW ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
    d3dSampler.magFilter = linearMag ? D3DTFG_LINEAR : D3DTFG_POINT;
    d3dSampler.minFilter = linearMin ? D3DTFN_LINEAR : D3DTFN_POINT;
    d3dSampler.mipFilter = linearMip ? D3DTFP_LINEAR : D3DTFP_POINT;
    d3dSampler.anisotropy = anisotropy;
    if (anisotropy > 1)
    {
        d3dSampler.magFilter = linearMag ? D3DTFG_ANISOTROPIC : D3DTFG_POINT;
        d3dSampler.minFilter = linearMin ? D3DTFN_ANISOTROPIC : D3DTFN_POINT;
    }

    return d3dSampler.value;
}
//------------------------------------------------------------------------------
void xxDestroySamplerD3D6(uint64_t sampler)
{

}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderD3D6(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderD3D6(uint64_t device, const char* shader)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderD3D6(uint64_t device, uint64_t shader)
{

}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreateBlendStateD3D6(uint64_t device, bool blending)
{
    D3DRENDERSTATE3 d3dRenderState = {};
    d3dRenderState.alphaBlending = blending;
    return d3dRenderState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateD3D6(uint64_t device, bool depthTest, bool depthWrite)
{
    D3DRENDERSTATE3 d3dRenderState = {};
    d3dRenderState.depthTest = depthTest;
    d3dRenderState.depthWrite = depthWrite;
    return d3dRenderState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateD3D6(uint64_t device, bool cull, bool scissor)
{
    D3DRENDERSTATE3 d3dRenderState = {};
    d3dRenderState.cull = cull;
    d3dRenderState.scissor = scissor;
    return d3dRenderState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineD3D6(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    D3DPIPELINE3* d3dPipeline = xxAlloc(D3DPIPELINE3);
    if (d3dPipeline == nullptr)
        return 0;

    DWORD d3dVertexShader                   = static_cast<DWORD>(vertexShader);
    DWORD d3dPixelShader                    = static_cast<DWORD>(fragmentShader);
    D3DRENDERSTATE3 d3dBlendState           = { blendState };
    D3DRENDERSTATE3 d3dDepthStencilState    = { depthStencilState };
    D3DRENDERSTATE3 d3dRasterizerState      = { rasterizerState };
    d3dPipeline->renderState.alphaBlending  = d3dBlendState.alphaBlending;
    d3dPipeline->renderState.depthTest      = d3dDepthStencilState.depthTest;
    d3dPipeline->renderState.depthWrite     = d3dDepthStencilState.depthWrite;
    d3dPipeline->renderState.cull           = d3dRasterizerState.cull;
    d3dPipeline->renderState.scissor        = d3dRasterizerState.scissor;

    return reinterpret_cast<uint64_t>(d3dPipeline);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateD3D6(uint64_t blendState)
{

}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateD3D6(uint64_t depthStencilState)
{

}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateD3D6(uint64_t rasterizerState)
{

}
//------------------------------------------------------------------------------
void xxDestroyPipelineD3D6(uint64_t pipeline)
{
    D3DPIPELINE3* d3dPipeline = reinterpret_cast<D3DPIPELINE3*>(pipeline);

    xxFree(d3dPipeline);
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportD3D6(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(commandEncoder);

    LPDIRECT3DVIEWPORT3 viewport = nullptr;
    d3dDevice->GetCurrentViewport(&viewport);
    viewport->Release();

    D3DVIEWPORT2 vp = {};
    vp.dwSize = sizeof(D3DVIEWPORT2);
    vp.dwX = x;
    vp.dwY = y;
    vp.dwWidth = width;
    vp.dwHeight = height;
    vp.dvClipX = -1.0f;
    vp.dvClipY = 1.0f;
    vp.dvClipWidth = 2.0f;
    vp.dvClipHeight = 2.0f;
    vp.dvMinZ = minZ;
    vp.dvMaxZ = maxZ;
    viewport->SetViewport2(&vp);
}
//------------------------------------------------------------------------------
void xxSetScissorD3D6(uint64_t commandEncoder, int x, int y, int width, int height)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(commandEncoder);

    LPDIRECT3DVIEWPORT3 viewport = nullptr;
    d3dDevice->GetCurrentViewport(&viewport);
    viewport->Release();

    D3DVIEWPORT2 vp;
    vp.dwSize = sizeof(D3DVIEWPORT2);
    viewport->GetViewport2(&vp);

    D3DMATRIX projection;
    d3dDevice->GetTransform(D3DTRANSFORMSTATE_PROJECTION, &projection);
    ViewportFromScissor(projection.m, vp.dwX, vp.dwY, vp.dwWidth, vp.dwHeight, x, y, width, height);
    d3dDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &projection);

    vp.dwX = x;
    vp.dwY = y;
    vp.dwWidth = width;
    vp.dwHeight = height;
    viewport->SetViewport2(&vp);
}
//------------------------------------------------------------------------------
void xxSetPipelineD3D6(uint64_t commandEncoder, uint64_t pipeline)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(commandEncoder);
    D3DPIPELINE3* d3dPipeline = reinterpret_cast<D3DPIPELINE3*>(pipeline);

    d3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
    d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    d3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
    d3dDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE);
    d3dDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
    d3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, d3dPipeline->renderState.depthWrite);
    d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, d3dPipeline->renderState.alphaBlending);
    d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, d3dPipeline->renderState.alphaTesting);
    d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersD3D6(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(commandEncoder);

    d3dDevice->SetRenderState(D3DRENDERSTATE_LINEPATTERN, (DWORD)(getResourceData(buffers[0])));
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesD3D6(uint64_t commandEncoder, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesD3D6(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        LPDIRECT3DTEXTURE2 texture = reinterpret_cast<LPDIRECT3DTEXTURE2>(textures[i]);
        d3dDevice->SetTexture(i, texture);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersD3D6(uint64_t commandEncoder, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersD3D6(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        D3DSAMPLER3 d3dSampler = { samplers[i] };
        d3dDevice->SetTextureStageState(i, D3DTSS_ADDRESSU, d3dSampler.addressU);
        d3dDevice->SetTextureStageState(i, D3DTSS_ADDRESSV, d3dSampler.addressV);
        d3dDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, d3dSampler.magFilter);
        d3dDevice->SetTextureStageState(i, D3DTSS_MINFILTER, d3dSampler.minFilter);
        d3dDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, d3dSampler.mipFilter);
        d3dDevice->SetTextureStageState(i, D3DTSS_MAXANISOTROPY, d3dSampler.anisotropy);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferD3D6(uint64_t commandEncoder, uint64_t buffer, int size)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(commandEncoder);
    D3DMATRIX* d3dMatrix = reinterpret_cast<D3DMATRIX*>(getResourceData(buffer));

    if (size >= sizeof(D3DMATRIX) * 1)
        d3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &d3dMatrix[0]);
    if (size >= sizeof(D3DMATRIX) * 2)
        d3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &d3dMatrix[1]);
    if (size >= sizeof(D3DMATRIX) * 3)
        d3dDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &d3dMatrix[2]);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferD3D6(uint64_t commandEncoder, uint64_t buffer, int size)
{

}
//------------------------------------------------------------------------------
void xxDrawIndexedD3D6(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    LPDIRECT3DDEVICE3 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE3>(commandEncoder);

    static WORD wordIndexBuffer[65536];
    WORD* indexArray = nullptr;
    if (INDEX_BUFFER_WIDTH == 2 && vertexOffset == 0)
    {
        WORD* d3dIndexBuffer = reinterpret_cast<WORD*>(getResourceData(indexBuffer));
        indexArray = d3dIndexBuffer + firstIndex;
    }
    else if (INDEX_BUFFER_WIDTH == 2)
    {
        WORD* d3dIndexBuffer = reinterpret_cast<WORD*>(getResourceData(indexBuffer));
        WORD* p = d3dIndexBuffer + firstIndex;
        WORD* q = wordIndexBuffer;
        #pragma loop(ivdep)
        for (int i = 0; i < indexCount; ++i)
        {
            (*q++) = (*p++) + vertexOffset;
        }
        indexArray = wordIndexBuffer;
    }
    else
    {
        DWORD* d3dIndexBuffer = reinterpret_cast<DWORD*>(getResourceData(indexBuffer));
        DWORD* p = d3dIndexBuffer + firstIndex;
        WORD* q = wordIndexBuffer;
        #pragma loop(ivdep)
        for (int i = 0; i < indexCount; ++i)
        {
            (*q++) = (WORD)((*p++) + vertexOffset);
        }
        indexArray = wordIndexBuffer;
    }

    LPDIRECT3DVERTEXBUFFER vertexBuffer = nullptr;
    d3dDevice->GetRenderState(D3DRENDERSTATE_LINEPATTERN, (DWORD*)&vertexBuffer);
    d3dDevice->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST, vertexBuffer, indexArray, indexCount, 0);
}
//==============================================================================
