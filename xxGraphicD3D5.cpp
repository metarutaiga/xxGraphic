//==============================================================================
// xxGraphic : Direct3D 5.0 Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#ifndef _M_IX86
#define xxAPI xxEXTERN
#endif
#define DIRECTDRAW_VERSION          0x500
#define DIRECT3D_VERSION            0x500
#define D3D_OVERLOADS
#include "xxSystem.h"
#include "dxsdk/ddraw.h"
#include "dxsdk/d3d.h"
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalD3D.h"
#include "xxGraphicD3D5.h"

typedef HRESULT (WINAPI *PFN_DIRECT_DRAW_CREATE)(GUID*, LPDIRECTDRAW*, IUnknown*);
#define D3DRTYPE_CONSTANTBUFFER     0
#define D3DRTYPE_INDEXBUFFER        1
#define D3DRTYPE_VERTEXBUFFER       2
#define D3DFVF_XYZ                  0x002
#define D3DFVF_DIFFUSE              0x040
#define D3DFVF_TEX1                 0x100

static void*                        g_ddrawLibrary = nullptr;
static LPDIRECTDRAW                 g_ddraw = nullptr;
static LPDIRECTDRAWSURFACE          g_primarySurface = nullptr;
static LPDIRECT3DMATERIAL2          g_clearColorMaterial = nullptr;

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D5()
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
    g_ddraw = ddraw;

    LPDIRECT3D2 d3d = nullptr;
    HRESULT result = ddraw->QueryInterface(IID_PPV_ARGS(&d3d));
    if (result != S_OK)
        return 0;
    d3d->CreateMaterial(&g_clearColorMaterial, nullptr);
    if (g_clearColorMaterial == nullptr)
    {
        d3d->Release();
        return 0;
    }

    PatchD3DIM("d3dim.dll");

    xxRegisterFunction(D3D5);

    return reinterpret_cast<uint64_t>(d3d);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceD3D5(uint64_t instance)
{
    LPDIRECT3D2 d3d = reinterpret_cast<LPDIRECT3D2>(instance);

    SafeRelease(g_clearColorMaterial);
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
uint64_t xxCreateDeviceD3D5(uint64_t instance)
{
    LPDIRECT3D2 d3d = reinterpret_cast<LPDIRECT3D2>(instance);
    if (d3d == nullptr)
        return 0;

    LPDIRECTDRAWSURFACE primarySurface = nullptr;
    if (primarySurface == nullptr)
    {
        DDSURFACEDESC desc = {};
        desc.dwSize = sizeof(DDSURFACEDESC);
        desc.dwFlags = DDSD_CAPS;
        desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

        HRESULT result = g_ddraw->CreateSurface(&desc, &primarySurface, nullptr);
        if (result != S_OK)
            return 0;

        g_primarySurface = primarySurface;
    }

    LPDIRECTDRAWSURFACE backSurface = nullptr;
    if (backSurface == nullptr)
    {
        DDSURFACEDESC desc = {};
        desc.dwSize = sizeof(DDSURFACEDESC);
        desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
        desc.dwWidth = 1;
        desc.dwHeight = 1;

        HRESULT result = g_ddraw->CreateSurface(&desc, &backSurface, nullptr);
        if (result != S_OK)
            return 0;
    }

    LPDIRECT3DDEVICE2 d3dDevice = nullptr;
    d3d->CreateDevice(__uuidof(IDirect3DHALDevice), backSurface, &d3dDevice);
    if (d3dDevice == nullptr)
    {
        backSurface->Release();
        return 0;
    }
    backSurface->Release();

    LPDIRECT3DVIEWPORT2 viewport = nullptr;
    d3d->CreateViewport(&viewport, nullptr);
    d3dDevice->AddViewport(viewport);
    d3dDevice->SetCurrentViewport(viewport);
    viewport->Release();

    IUnknown* unknown = nullptr;
    xxLocalBreak()
    {
        if (d3dDevice->QueryInterface(__uuidof(IDirect3DDevice2), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "5.0", xxGetDeviceName());
            break;
        }
    }
    SafeRelease(unknown);

    return reinterpret_cast<uint64_t>(d3dDevice);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceD3D5(uint64_t device)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(device);

    SafeRelease(d3dDevice);
    SafeRelease(g_primarySurface);
}
//------------------------------------------------------------------------------
bool xxResetDeviceD3D5(uint64_t device)
{
    return false;
}
//------------------------------------------------------------------------------
bool xxTestDeviceD3D5(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameD3D5()
{
    return "Direct3D 5.0";
}
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t xxCreateSwapchainD3D5(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DSWAPCHAIN2* d3dOldSwapchain = reinterpret_cast<D3DSWAPCHAIN2*>(oldSwapchain);
    if (d3dOldSwapchain && d3dOldSwapchain->hWnd == view && d3dOldSwapchain->width == width && d3dOldSwapchain->height == height)
        return oldSwapchain;
    D3DSWAPCHAIN2* swapchain = xxAlloc(D3DSWAPCHAIN2);
    if (swapchain == nullptr)
        return 0;

    xxDestroySwapchainD3D5(oldSwapchain);

    HWND hWnd = (HWND)view;

    LPDIRECTDRAWSURFACE backSurface = nullptr;
    if (backSurface == nullptr)
    {
        DDSURFACEDESC desc = {};
        desc.dwSize = sizeof(DDSURFACEDESC);
        desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        desc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE;
        desc.dwWidth = width;
        desc.dwHeight = height;

        HRESULT result = g_ddraw->CreateSurface(&desc, &backSurface, nullptr);
        if (result != S_OK)
            return 0;
    }

    LPDIRECTDRAWSURFACE depthSurface = nullptr;
    if (depthSurface == nullptr)
    {
        DDSURFACEDESC desc = {};
        desc.dwSize = sizeof(DDSURFACEDESC);
        desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_ZBUFFERBITDEPTH;
        desc.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
        desc.dwWidth = width;
        desc.dwHeight = height;
        desc.dwZBufferBitDepth = 24;

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
void xxDestroySwapchainD3D5(uint64_t swapchain)
{
    D3DSWAPCHAIN2* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN2*>(swapchain);
    if (d3dSwapchain == nullptr)
        return;

    SafeRelease(d3dSwapchain->backSurface);
    SafeRelease(d3dSwapchain->depthSurface);
    SafeRelease(d3dSwapchain->clipper);
    xxFree(d3dSwapchain);
}
//------------------------------------------------------------------------------
void xxPresentSwapchainD3D5(uint64_t swapchain)
{
    D3DSWAPCHAIN2* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN2*>(swapchain);
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
uint64_t xxGetCommandBufferD3D5(uint64_t device, uint64_t swapchain)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DSWAPCHAIN2* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN2*>(swapchain);
    if (d3dSwapchain == nullptr)
        return 0;

    HRESULT hResult = d3dDevice->SetRenderTarget(d3dSwapchain->backSurface, 0);
    if (hResult != S_OK)
        return 0;

    return device;
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferD3D5(uint64_t device, uint64_t swapchain, float* scale)
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
bool xxBeginCommandBufferD3D5(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(commandBuffer);
    if (d3dDevice == nullptr)
        return false;

    HRESULT hResult = d3dDevice->BeginScene();
    return (hResult == S_OK);
}
//------------------------------------------------------------------------------
void xxEndCommandBufferD3D5(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    HRESULT hResult = d3dDevice->EndScene();
}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferD3D5(uint64_t commandBuffer, uint64_t swapchain)
{
}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxCreateRenderPassD3D5(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
{
    DWORD flags = 0;

    if (clearColor)
        flags |= D3DCLEAR_TARGET;
    if (clearDepth)
        flags |= D3DCLEAR_ZBUFFER;

    return flags;
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassD3D5(uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassD3D5(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(commandBuffer);
    if (d3dDevice == nullptr)
        return 0;
    D3DFRAMEBUFFER3* d3dFramebuffer = reinterpret_cast<D3DFRAMEBUFFER3*>(framebuffer);
    if (d3dFramebuffer == nullptr)
        return 0;
    DWORD d3dFlags = static_cast<DWORD>(renderPass);

    if (d3dFlags)
    {
        LPDIRECT3DVIEWPORT2 viewport = nullptr;
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

        D3DMATERIAL clearColor = {};
        clearColor.dwSize = sizeof(D3DMATERIAL);
        clearColor.diffuse.r = color[0];
        clearColor.diffuse.g = color[1];
        clearColor.diffuse.b = color[2];
        clearColor.diffuse.a = color[3];
        g_clearColorMaterial->SetMaterial(&clearColor);

        D3DMATERIALHANDLE materialHandle = 0;
        g_clearColorMaterial->GetHandle(d3dDevice, &materialHandle);
        viewport->SetBackground(materialHandle);

        viewport->Clear(1, &rect, d3dFlags);
    }

    return commandBuffer;
}
//------------------------------------------------------------------------------
void xxEndRenderPassD3D5(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{

}
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t xxCreateVertexAttributeD3D5(uint64_t device, int count, int* attribute)
{
    D3DVERTEXATTRIBUTE2 d3dVertexAttribute = {};
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
        if (offset != 0 && element == 4 && size == sizeof(char) * 4)
            d3dVertexAttribute.fvf |= D3DFVF_DIFFUSE;
        if (offset != 0 && element == 2 && size == sizeof(float) * 2)
            d3dVertexAttribute.fvf += D3DFVF_TEX1;
    }

    d3dVertexAttribute.stride = stride;

    return static_cast<uint64_t>(d3dVertexAttribute.value);
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeD3D5(uint64_t vertexAttribute)
{

}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateConstantBufferD3D5(uint64_t device, int size)
{
    char* d3dBuffer = xxAlloc(char, size);

    return reinterpret_cast<uint64_t>(d3dBuffer) | D3DRTYPE_CONSTANTBUFFER;
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferD3D5(uint64_t device, int size)
{
    char* d3dBuffer = xxAlloc(char, size);

    return reinterpret_cast<uint64_t>(d3dBuffer) | D3DRTYPE_INDEXBUFFER;
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferD3D5(uint64_t device, int size, uint64_t vertexAttribute)
{
    D3DVERTEXATTRIBUTE3 d3dVertexAttribute = { vertexAttribute };
    if (d3dVertexAttribute.stride == 0)
        return 0;
    D3DVERTEXBUFFER2* d3dVertexBuffer = xxAlloc(D3DVERTEXBUFFER2);
    if (d3dVertexBuffer == nullptr)
        return 0;

    d3dVertexBuffer->buffer = xxAlloc(char, size);
    d3dVertexBuffer->size = size;
    d3dVertexBuffer->count = size / d3dVertexAttribute.stride;
    d3dVertexBuffer->dirty = false;
    d3dVertexBuffer->address = xxAlloc(char, d3dVertexBuffer->count * sizeof(D3DLVERTEX));

    return reinterpret_cast<uint64_t>(d3dVertexBuffer) | D3DRTYPE_VERTEXBUFFER;
}
//------------------------------------------------------------------------------
void xxDestroyBufferD3D5(uint64_t device, uint64_t buffer)
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
        D3DVERTEXBUFFER2* d3dVertexBuffer = reinterpret_cast<D3DVERTEXBUFFER2*>(getResourceData(buffer));
        if (d3dVertexBuffer == nullptr)
            break;

        xxFree(d3dVertexBuffer->buffer);
        xxFree(d3dVertexBuffer->address);
        xxFree(d3dVertexBuffer);
        break;
    }
    default:
        break;
    }
}
//------------------------------------------------------------------------------
void* xxMapBufferD3D5(uint64_t device, uint64_t buffer)
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
        D3DVERTEXBUFFER2* d3dVertexBuffer = reinterpret_cast<D3DVERTEXBUFFER2*>(getResourceData(buffer));
        if (d3dVertexBuffer == nullptr)
            break;
        d3dVertexBuffer->dirty = true;

        return d3dVertexBuffer->buffer;
    }
    default:
        break;
    }

    return nullptr;
}
//------------------------------------------------------------------------------
void xxUnmapBufferD3D5(uint64_t device, uint64_t buffer)
{
    switch (getResourceType(buffer))
    {
    case D3DRTYPE_CONSTANTBUFFER:
    case D3DRTYPE_INDEXBUFFER:
    case D3DRTYPE_VERTEXBUFFER:
    {
        return;
    }
    default:
        break;
    }
}
//==============================================================================
//  Texture
//==============================================================================
uint64_t xxCreateTextureD3D5(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, const void* external)
{
    DDSURFACEDESC desc = {};
    desc.dwSize = sizeof(DDSURFACEDESC);
    desc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
    desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
    desc.dwWidth = width;
    desc.dwHeight = height;
    desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    desc.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
    desc.ddpfPixelFormat.dwRGBBitCount = 32;
    desc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
    desc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
    desc.ddpfPixelFormat.dwBBitMask = 0x000000FF;
    desc.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;

    LPDIRECTDRAWSURFACE surface = nullptr;
    HRESULT result = g_ddraw->CreateSurface(&desc, &surface, nullptr);
    if (result != S_OK)
        return 0;
 
    LPDIRECT3DTEXTURE2 texture = nullptr;
    surface->QueryInterface(IID_PPV_ARGS(&texture));
    surface->Release();

    return reinterpret_cast<uint64_t>(texture);
}
//------------------------------------------------------------------------------
void xxDestroyTextureD3D5(uint64_t texture)
{
    LPDIRECT3DTEXTURE2 d3dTexture = reinterpret_cast<LPDIRECT3DTEXTURE2>(texture);

    SafeRelease(d3dTexture);
}
//------------------------------------------------------------------------------
void* xxMapTextureD3D5(uint64_t device, uint64_t texture, int* stride, int level, int array)
{
    LPDIRECT3DTEXTURE2 d3dTexture = reinterpret_cast<LPDIRECT3DTEXTURE2>(texture);
    if (d3dTexture == nullptr)
        return nullptr;

    LPDIRECTDRAWSURFACE surface = nullptr;
    d3dTexture->QueryInterface(IID_PPV_ARGS(&surface));
    if (surface == nullptr)
        return nullptr;
    surface->Release();

    DDSURFACEDESC desc = {};
    if (desc.dwSize == 0)
    {
        desc.dwSize = sizeof(DDSURFACEDESC);
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
void xxUnmapTextureD3D5(uint64_t device, uint64_t texture, int level, int array)
{
    LPDIRECT3DTEXTURE2 d3dTexture = reinterpret_cast<LPDIRECT3DTEXTURE2>(texture);
    if (d3dTexture == nullptr)
        return;

    LPDIRECTDRAWSURFACE surface = nullptr;
    d3dTexture->QueryInterface(IID_PPV_ARGS(&surface));
    if (surface == nullptr)
        return;
    surface->Release();
 
    surface->Unlock(nullptr);
}
//==============================================================================
//  Sampler
//==============================================================================
uint64_t xxCreateSamplerD3D5(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    D3DSAMPLER2 d3dSampler = {};

    d3dSampler.addressU = clampU ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
    d3dSampler.addressV = clampV ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
    d3dSampler.addressW = clampW ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
    d3dSampler.magFilter = linearMag ? D3DFILTER_LINEAR : D3DFILTER_NEAREST;
    d3dSampler.minFilter = linearMin ? D3DFILTER_LINEAR : D3DFILTER_NEAREST;
    d3dSampler.mipFilter = linearMip ? D3DFILTER_LINEARMIPLINEAR : D3DFILTER_MIPNEAREST;

    return static_cast<uint64_t>(d3dSampler.value);
}
//------------------------------------------------------------------------------
void xxDestroySamplerD3D5(uint64_t sampler)
{

}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderD3D5(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderD3D5(uint64_t device, const char* shader)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderD3D5(uint64_t device, uint64_t shader)
{

}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreateBlendStateD3D5(uint64_t device, xxGraphicBlendFactor sourceColor, xxGraphicBlendFactor destinationColor)
{
    D3DRENDERSTATE3 d3dRenderState = {};
    d3dRenderState.blendSourceColor = d3dBlendFactor(sourceColor);
    d3dRenderState.blendDestinationColor = d3dBlendFactor(destinationColor);
    return static_cast<uint64_t>(d3dRenderState.value);
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateD3D5(uint64_t device, bool depthTest, bool depthWrite)
{
    D3DRENDERSTATE3 d3dRenderState = {};
    d3dRenderState.depthTest = depthTest;
    d3dRenderState.depthWrite = depthWrite;
    return static_cast<uint64_t>(d3dRenderState.value);
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateD3D5(uint64_t device, bool cull, bool scissor)
{
    D3DRENDERSTATE3 d3dRenderState = {};
    d3dRenderState.cull = cull;
    d3dRenderState.scissor = scissor;
    return static_cast<uint64_t>(d3dRenderState.value);
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineD3D5(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    D3DPIPELINE3* d3dPipeline = xxAlloc(D3DPIPELINE3);
    if (d3dPipeline == nullptr)
        return 0;

    DWORD d3dVertexShader                           = static_cast<DWORD>(vertexShader);
    DWORD d3dPixelShader                            = static_cast<DWORD>(fragmentShader);
    D3DRENDERSTATE3 d3dBlendState                   = { blendState };
    D3DRENDERSTATE3 d3dDepthStencilState            = { depthStencilState };
    D3DRENDERSTATE3 d3dRasterizerState              = { rasterizerState };
    d3dPipeline->renderState.blendSourceColor       = d3dBlendState.blendSourceColor;
    d3dPipeline->renderState.blendDestinationColor  = d3dBlendState.blendDestinationColor;
    d3dPipeline->renderState.depthTest              = d3dDepthStencilState.depthTest;
    d3dPipeline->renderState.depthWrite             = d3dDepthStencilState.depthWrite;
    d3dPipeline->renderState.cull                   = d3dRasterizerState.cull;
    d3dPipeline->renderState.scissor                = d3dRasterizerState.scissor;

    return reinterpret_cast<uint64_t>(d3dPipeline);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateD3D5(uint64_t blendState)
{

}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateD3D5(uint64_t depthStencilState)
{

}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateD3D5(uint64_t rasterizerState)
{

}
//------------------------------------------------------------------------------
void xxDestroyPipelineD3D5(uint64_t pipeline)
{
    D3DPIPELINE3* d3dPipeline = reinterpret_cast<D3DPIPELINE3*>(pipeline);

    xxFree(d3dPipeline);
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportD3D5(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(commandEncoder);

    LPDIRECT3DVIEWPORT2 viewport = nullptr;
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
void xxSetScissorD3D5(uint64_t commandEncoder, int x, int y, int width, int height)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(commandEncoder);

    LPDIRECT3DVIEWPORT2 viewport = nullptr;
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
void xxSetPipelineD3D5(uint64_t commandEncoder, uint64_t pipeline)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(commandEncoder);
    D3DPIPELINE3* d3dPipeline = reinterpret_cast<D3DPIPELINE3*>(pipeline);

    d3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
    d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
    d3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
    d3dDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE);
    d3dDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
    d3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, d3dPipeline->renderState.depthWrite);
    d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (d3dPipeline->renderState.blendSourceColor != D3DBLEND_ONE || d3dPipeline->renderState.blendDestinationColor != D3DBLEND_ZERO));
    d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, d3dPipeline->renderState.blendSourceColor);
    d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, d3dPipeline->renderState.blendDestinationColor);
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersD3D5(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(commandEncoder);
    D3DVERTEXBUFFER2* d3dVertexBuffer = reinterpret_cast<D3DVERTEXBUFFER2*>(getResourceData(buffers[0]));
    D3DVERTEXATTRIBUTE3 d3dVertexAttribute = { vertexAttribute };

    if (d3dVertexBuffer && d3dVertexBuffer->dirty)
    {
        d3dVertexBuffer->dirty = false;

        float* source = (float*)d3dVertexBuffer->buffer;
        D3DLVERTEX* target = (D3DLVERTEX*)d3dVertexBuffer->address;
        for (UINT i = 0; i < d3dVertexBuffer->count; ++i)
        {
            int offset = 0;
            if (d3dVertexAttribute.fvf & D3DFVF_XYZ)
            {
                target->dvX = source[offset++];
                target->dvY = source[offset++];
                target->dvZ = source[offset++];
            }
            if (d3dVertexAttribute.fvf & D3DFVF_DIFFUSE)
            {
                target->dcColor = (D3DCOLOR&)source[offset++];
            }
            if (d3dVertexAttribute.fvf & D3DFVF_TEX1)
            {
                target->dvTU = source[offset++];
                target->dvTV = source[offset++];
            }

            source = (float*)((char*)source + d3dVertexAttribute.stride);
            target++;
        }
    }

    d3dDevice->SetRenderState(D3DRENDERSTATE_LINEPATTERN, (DWORD)getResourceData(buffers[0]));
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesD3D5(uint64_t commandEncoder, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesD3D5(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        LPDIRECT3DTEXTURE2 texture = reinterpret_cast<LPDIRECT3DTEXTURE2>(textures[i]);
        D3DTEXTUREHANDLE handle = 0;
        texture->GetHandle(d3dDevice, &handle);
        d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE, handle);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersD3D5(uint64_t commandEncoder, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersD3D5(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        D3DSAMPLER2 d3dSampler = { samplers[i] };
        d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSU, d3dSampler.addressU);
        d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREADDRESSV, d3dSampler.addressV);
        d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, d3dSampler.magFilter);
        d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, d3dSampler.minFilter);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferD3D5(uint64_t commandEncoder, uint64_t buffer, int size)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(commandEncoder);
    D3DMATRIX* d3dMatrix = reinterpret_cast<D3DMATRIX*>(getResourceData(buffer));

    if (size >= sizeof(D3DMATRIX) * 1)
        d3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &d3dMatrix[0]);
    if (size >= sizeof(D3DMATRIX) * 2)
        d3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, &d3dMatrix[1]);
    if (size >= sizeof(D3DMATRIX) * 3)
        d3dDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &d3dMatrix[2]);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferD3D5(uint64_t commandEncoder, uint64_t buffer, int size)
{

}
//------------------------------------------------------------------------------
void xxDrawIndexedD3D5(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    LPDIRECT3DDEVICE2 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE2>(commandEncoder);

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

    D3DVERTEXBUFFER2* vertexBuffer = nullptr;
    d3dDevice->GetRenderState(D3DRENDERSTATE_LINEPATTERN, (DWORD*)&vertexBuffer);
    d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DVT_LVERTEX, vertexBuffer->address, vertexBuffer->count, indexArray, indexCount, 0);
}
//==============================================================================
