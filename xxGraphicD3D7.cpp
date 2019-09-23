//==============================================================================
// xxGraphic : Direct3D 7.0 Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicInternal.h"
#include "xxGraphicD3D.h"
#include "xxGraphicD3D7.h"

#include "dxsdk/ddraw.h"
#include "dxsdk/d3d.h"
interface DECLSPEC_UUID("15e65ec0-3b9c-11d2-b92f-00609797ea5b") IDirectDraw7;
interface DECLSPEC_UUID("f5049e77-4861-11d2-a407-00a0c90629a8") IDirect3D7;
interface DECLSPEC_UUID("f5049e78-4861-11d2-a407-00a0c90629a8") IDirect3DTnLHalDevice;
typedef HRESULT (WINAPI *PFN_DIRECT_DRAW_CREATE_EX)(GUID*, LPVOID*, REFIID, IUnknown*);
#define D3DRTYPE_CONSTANTBUFFER 0
#define D3DRTYPE_INDEXBUFFER    1
#define D3DRTYPE_VERTEXBUFFER   2

static HMODULE                  g_ddrawLibrary = nullptr;
static LPDIRECTDRAW7            g_ddraw = nullptr;
static LPDIRECTDRAWSURFACE7     g_primarySurface = nullptr;

//------------------------------------------------------------------------------
static void PatchD3DIM700()
{
    HMODULE d3dim700 = LoadLibraryW(L"d3dim700.dll");
    if (d3dim700 == nullptr)
        return;

    // https://github.com/UCyborg/LegacyD3DResolutionHack
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)d3dim700;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((char*)dosHeader + dosHeader->e_lfanew);
    char* codeBase = (char*)d3dim700 + ntHeader->OptionalHeader.BaseOfCode;
    DWORD codeSize = ntHeader->OptionalHeader.SizeOfCode;

    const BYTE wantedBytes[] = { 0xB8, 0x00, 0x08, 0x00, 0x00, 0x39 };
    for (DWORD i = 0; i < codeSize - sizeof(wantedBytes); ++i)
    {
        if (memcmp(codeBase + i, wantedBytes, sizeof(wantedBytes)) == 0)
        {
            DWORD oldProtect;
            VirtualProtect(&codeBase[i + 1], 4, PAGE_EXECUTE_READWRITE, &oldProtect);
            codeBase[i + 1] = -1;
            codeBase[i + 2] = -1;
            codeBase[i + 3] = -1;
            codeBase[i + 4] = -1;
            VirtualProtect(&codeBase[i + 1], 4, oldProtect, &oldProtect);
        }
    }
}
//==============================================================================
//  Resource Type
//==============================================================================
static uint64_t getResourceType(uint64_t resource)
{
    return resource & 7ull;
}
//------------------------------------------------------------------------------
static uint64_t getResourceData(uint64_t resource)
{
    return resource & ~7ull;
}
//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D7()
{
    if (g_ddrawLibrary == nullptr)
        g_ddrawLibrary = LoadLibraryW(L"ddraw.dll");
    if (g_ddrawLibrary == nullptr)
        return 0;

    PFN_DIRECT_DRAW_CREATE_EX DirectDrawCreateEx;
    (void*&)DirectDrawCreateEx = GetProcAddress(g_ddrawLibrary, "DirectDrawCreateEx");
    if (DirectDrawCreateEx == nullptr)
        return 0;

    LPDIRECTDRAW7 ddraw = nullptr;
    if (ddraw == nullptr)
    {
        HRESULT result = DirectDrawCreateEx(nullptr, (void**)&ddraw, __uuidof(IDirectDraw7), nullptr);
        if (result != S_OK)
            return 0;
        ddraw->SetCooperativeLevel(nullptr, DDSCL_FPUSETUP | DDSCL_NORMAL);
    }
    g_ddraw = ddraw;

    LPDIRECT3D7 d3d = nullptr;
    HRESULT result = ddraw->QueryInterface(__uuidof(IDirect3D7), (void**)&d3d);
    if (result != S_OK)
        return 0;

    PatchD3DIM700();

    xxRegisterFunction(D3D7);

    return reinterpret_cast<uint64_t>(d3d);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceD3D7(uint64_t instance)
{
    LPDIRECT3D7 d3d = reinterpret_cast<LPDIRECT3D7>(instance);

    SafeRelease(g_ddraw);
    SafeRelease(d3d);

    if (g_ddrawLibrary)
    {
        FreeLibrary(g_ddrawLibrary);
        g_ddrawLibrary = nullptr;
    }

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceD3D7(uint64_t instance)
{
    LPDIRECT3D7 d3d = reinterpret_cast<LPDIRECT3D7>(instance);
    if (d3d == nullptr)
        return 0;

    LPDIRECTDRAWSURFACE7 primarySurface = nullptr;
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

    LPDIRECTDRAWSURFACE7 backSurface = nullptr;
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

    LPDIRECT3DDEVICE7 d3dDevice = nullptr;
    HRESULT result = d3d->CreateDevice(__uuidof(IDirect3DTnLHalDevice), backSurface, &d3dDevice);
    if (result != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(d3dDevice);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceD3D7(uint64_t device)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(device);

    SafeRelease(d3dDevice);
    SafeRelease(g_primarySurface);
}
//------------------------------------------------------------------------------
void xxResetDeviceD3D7(uint64_t device)
{

}
//------------------------------------------------------------------------------
bool xxTestDeviceD3D7(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameD3D7()
{
    return "Direct3D 7.0";
}
//==============================================================================
//  Framebuffer
//==============================================================================
struct D3DFRAMEBUFFER7
{
    LPDIRECTDRAWSURFACE7    backSurface;
    LPDIRECTDRAWSURFACE7    depthSurface;
};
//==============================================================================
//  Swapchain
//==============================================================================
struct D3DSWAPCHAIN7 : public D3DFRAMEBUFFER7
{
    LPDIRECTDRAWCLIPPER     clipper;
    HWND                    hWnd;
};
//------------------------------------------------------------------------------
uint64_t xxCreateSwapchainD3D7(uint64_t device, uint64_t renderPass, void* view, unsigned int width, unsigned int height)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DSWAPCHAIN7* swapchain = new D3DSWAPCHAIN7;
    if (swapchain == nullptr)
        return 0;

    HWND hWnd = (HWND)view;
    if (width == 0 || height == 0)
    {
        RECT rect = {};
        if (GetClientRect(hWnd, &rect) == TRUE)
        {
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
        }
    }

    LPDIRECTDRAWSURFACE7 backSurface = nullptr;
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

    LPDIRECTDRAWSURFACE7 depthSurface = nullptr;
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
            return 0;

        backSurface->AddAttachedSurface(depthSurface);
    }

    LPDIRECTDRAWCLIPPER clipper = nullptr;
    if (clipper == nullptr)
    {
        HRESULT result = g_ddraw->CreateClipper(0, &clipper, nullptr);
        if (result != S_OK)
            return 0;

        clipper->SetHWnd(0, hWnd);
    }

    swapchain->backSurface = backSurface;
    swapchain->depthSurface = depthSurface;
    swapchain->clipper = clipper;
    swapchain->hWnd = hWnd;

    return reinterpret_cast<uint64_t>(swapchain);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainD3D7(uint64_t swapchain)
{
    D3DSWAPCHAIN7* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN7*>(swapchain);
    if (d3dSwapchain == nullptr)
        return;

    SafeRelease(d3dSwapchain->backSurface);
    SafeRelease(d3dSwapchain->depthSurface);
    SafeRelease(d3dSwapchain->clipper);
    delete d3dSwapchain;
}
//------------------------------------------------------------------------------
void xxPresentSwapchainD3D7(uint64_t swapchain)
{
    D3DSWAPCHAIN7* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN7*>(swapchain);
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
uint64_t xxGetCommandBufferD3D7(uint64_t device, uint64_t swapchain)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DSWAPCHAIN7* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN7*>(swapchain);
    if (d3dSwapchain == nullptr)
        return 0;

    HRESULT hResult = d3dDevice->SetRenderTarget(d3dSwapchain->backSurface, 0);
    if (hResult != S_OK)
        return 0;

    return device;
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferD3D7(uint64_t device, uint64_t swapchain)
{
    return swapchain;
}
//==============================================================================
//  Command Buffer
//==============================================================================
bool xxBeginCommandBufferD3D7(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(commandBuffer);
    if (d3dDevice == nullptr)
        return false;

    HRESULT hResult = d3dDevice->BeginScene();
    return (hResult == S_OK);
}
//------------------------------------------------------------------------------
void xxEndCommandBufferD3D7(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    HRESULT hResult = d3dDevice->EndScene();
}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferD3D7(uint64_t commandBuffer, uint64_t swapchain)
{
}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxCreateRenderPassD3D7(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
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
void xxDestroyRenderPassD3D7(uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassD3D7(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float r, float g, float b, float a, float depth, unsigned char stencil)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(commandBuffer);
    if (d3dDevice == nullptr)
        return 0;
    D3DFRAMEBUFFER7* d3dFramebuffer = reinterpret_cast<D3DFRAMEBUFFER7*>(framebuffer);
    if (d3dFramebuffer == nullptr)
        return 0;
    DWORD d3dFlags = static_cast<DWORD>(renderPass);

    if (d3dFlags)
    {
        D3DVIEWPORT7 vp;
        vp.dwX = 0;
        vp.dwY = 0;
        vp.dwWidth = width;
        vp.dwHeight = height;
        vp.dvMinZ = 0.0f;
        vp.dvMaxZ = 1.0f;
        d3dDevice->SetViewport(&vp);

        HRESULT hResult = d3dDevice->Clear(0, nullptr, d3dFlags, D3DRGBA(r, g, b, a), depth, stencil);
        if (hResult != S_OK)
            return 0;
    }

    return commandBuffer;
}
//------------------------------------------------------------------------------
void xxEndRenderPassD3D7(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{

}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateConstantBufferD3D7(uint64_t device, unsigned int size)
{
    char* d3dBuffer = xxAlloc(char, size);

    return reinterpret_cast<uint64_t>(d3dBuffer) | D3DRTYPE_CONSTANTBUFFER;
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferD3D7(uint64_t device, unsigned int size)
{
    char* d3dBuffer = xxAlloc(char, size);

    return reinterpret_cast<uint64_t>(d3dBuffer) | D3DRTYPE_INDEXBUFFER;
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferD3D7(uint64_t device, unsigned int size)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(device);
    if (d3dDevice == nullptr)
        return 0;
    LPDIRECT3D7 d3d = nullptr;
    if (d3d == nullptr)
    {
        HRESULT result = d3dDevice->GetDirect3D(&d3d);
        if (result != S_OK)
            return 0;
        d3d->Release();
    }

    D3DVERTEXBUFFERDESC desc = {};
    desc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
    desc.dwCaps = D3DVBCAPS_WRITEONLY;
    desc.dwFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
    desc.dwNumVertices = (size / (sizeof(float) * (3 + 1 + 2)));

    LPDIRECT3DVERTEXBUFFER7 buffer = nullptr;
    HRESULT result = d3d->CreateVertexBuffer(&desc, &buffer, 0);
    if (result != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(buffer) | D3DRTYPE_VERTEXBUFFER;
}
//------------------------------------------------------------------------------
void xxDestroyBufferD3D7(uint64_t device, uint64_t buffer)
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
        LPDIRECT3DVERTEXBUFFER7 d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER7>(getResourceData(buffer));
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
void* xxMapBufferD3D7(uint64_t device, uint64_t buffer)
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
        LPDIRECT3DVERTEXBUFFER7 d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER7>(getResourceData(buffer));
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
void xxUnmapBufferD3D7(uint64_t device, uint64_t buffer)
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
        LPDIRECT3DVERTEXBUFFER7 d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER7>(getResourceData(buffer));
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
uint64_t xxCreateTextureD3D7(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array)
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

    LPDIRECTDRAWSURFACE7 surface = nullptr;
    HRESULT result = g_ddraw->CreateSurface(&desc, &surface, nullptr);
    if (result != S_OK)
        return 0;
 
    return reinterpret_cast<uint64_t>(surface);
}
//------------------------------------------------------------------------------
void xxDestroyTextureD3D7(uint64_t texture)
{
    LPDIRECTDRAWSURFACE7 surface = reinterpret_cast<LPDIRECTDRAWSURFACE7>(texture);

    SafeRelease(surface);
}
//------------------------------------------------------------------------------
void* xxMapTextureD3D7(uint64_t device, uint64_t texture, unsigned int& stride, unsigned int level, unsigned int array, unsigned int mipmap)
{
    LPDIRECTDRAWSURFACE7 surface = reinterpret_cast<LPDIRECTDRAWSURFACE7>(texture);
    if (surface == nullptr)
        return nullptr;

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

    stride = desc.lPitch;
    return desc.lpSurface;
}
//------------------------------------------------------------------------------
void xxUnmapTextureD3D7(uint64_t device, uint64_t texture, unsigned int level, unsigned int array, unsigned int mipmap)
{
    LPDIRECTDRAWSURFACE7 surface = reinterpret_cast<LPDIRECTDRAWSURFACE7>(texture);
    if (surface == nullptr)
        return;
 
    surface->Unlock(nullptr);
}
//==============================================================================
//  Sampler
//==============================================================================
union D3DSAMPLER7
{
    uint64_t    value;
    struct
    {
        uint8_t addressU;
        uint8_t addressV;
        uint8_t addressW;
        uint8_t magFilter;
        uint8_t minFilter;
        uint8_t mipFilter;
        uint8_t anisotropy;
    };
};
//------------------------------------------------------------------------------
uint64_t xxCreateSamplerD3D7(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    D3DSAMPLER7 d3dSampler = {};

    d3dSampler.addressU = clampU ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
    d3dSampler.addressV = clampV ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
    d3dSampler.addressW = clampW ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
    d3dSampler.magFilter = linearMag ? D3DTFG_LINEAR : D3DTFG_POINT;
    d3dSampler.minFilter = linearMin ? D3DTFG_LINEAR : D3DTFG_POINT;
    d3dSampler.mipFilter = linearMip ? D3DTFG_LINEAR : D3DTFG_POINT;
    d3dSampler.anisotropy = anisotropy;
    if (anisotropy > 1)
    {
        d3dSampler.magFilter = linearMag ? D3DTFG_ANISOTROPIC : D3DTFG_POINT;
        d3dSampler.minFilter = linearMin ? D3DTFG_ANISOTROPIC : D3DTFG_POINT;
        d3dSampler.mipFilter = linearMip ? D3DTFG_ANISOTROPIC : D3DTFG_POINT;
    }

    return d3dSampler.value;
}
//------------------------------------------------------------------------------
void xxDestroySamplerD3D7(uint64_t sampler)
{

}
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t xxCreateVertexAttributeD3D7(uint64_t device, int count, ...)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeD3D7(uint64_t vertexAttribute)
{

}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderD3D7(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderD3D7(uint64_t device, const char* shader)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderD3D7(uint64_t device, uint64_t shader)
{

}
//==============================================================================
//  Pipeline
//==============================================================================
union D3DRENDERSTATE7
{
    uint64_t        value;
    struct
    {
        uint64_t    alphaBlending:1;
        uint64_t    alphaTesting:1;
        uint64_t    depthTest:1;
        uint64_t    depthWrite:1;
        uint64_t    cull:1;
        uint64_t    scissor:1;
    };
};
//------------------------------------------------------------------------------
struct D3DPIPELINE7
{
    D3DRENDERSTATE7 renderState;
};
//------------------------------------------------------------------------------
uint64_t xxCreateBlendStateD3D7(uint64_t device, bool blending)
{
    D3DRENDERSTATE7 d3dRenderState = {};
    d3dRenderState.alphaBlending = blending;
    return d3dRenderState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateD3D7(uint64_t device, bool depthTest, bool depthWrite)
{
    D3DRENDERSTATE7 d3dRenderState = {};
    d3dRenderState.depthTest = depthTest;
    d3dRenderState.depthWrite = depthWrite;
    return d3dRenderState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateD3D7(uint64_t device, bool cull, bool scissor)
{
    D3DRENDERSTATE7 d3dRenderState = {};
    d3dRenderState.cull = cull;
    d3dRenderState.scissor = scissor;
    return d3dRenderState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineD3D7(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    D3DPIPELINE7* d3dPipeline = new D3DPIPELINE7;
    if (d3dPipeline == nullptr)
        return 0;

    DWORD d3dVertexShader                   = static_cast<DWORD>(vertexShader);
    DWORD d3dPixelShader                    = static_cast<DWORD>(fragmentShader);
    D3DRENDERSTATE7 d3dBlendState           = { blendState };
    D3DRENDERSTATE7 d3dDepthStencilState    = { depthStencilState };
    D3DRENDERSTATE7 d3dRasterizerState      = { rasterizerState };
    d3dPipeline->renderState.alphaBlending  = d3dBlendState.alphaBlending;
    d3dPipeline->renderState.depthTest      = d3dDepthStencilState.depthTest;
    d3dPipeline->renderState.depthWrite     = d3dDepthStencilState.depthWrite;
    d3dPipeline->renderState.cull           = d3dRasterizerState.cull;
    d3dPipeline->renderState.scissor        = d3dRasterizerState.scissor;

    return reinterpret_cast<uint64_t>(d3dPipeline);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateD3D7(uint64_t blendState)
{

}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateD3D7(uint64_t depthStencilState)
{

}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateD3D7(uint64_t rasterizerState)
{

}
//------------------------------------------------------------------------------
void xxDestroyPipelineD3D7(uint64_t pipeline)
{
    D3DPIPELINE7* d3dPipeline = reinterpret_cast<D3DPIPELINE7*>(pipeline);

    delete d3dPipeline;
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportD3D7(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(commandEncoder);

    D3DVIEWPORT7 vp;
    vp.dwX = x;
    vp.dwY = y;
    vp.dwWidth = width;
    vp.dwHeight = height;
    vp.dvMinZ = minZ;
    vp.dvMaxZ = maxZ;
    d3dDevice->SetViewport(&vp);
}
//------------------------------------------------------------------------------
void xxSetScissorD3D7(uint64_t commandEncoder, int x, int y, int width, int height)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(commandEncoder);

    D3DVIEWPORT7 vp;
    d3dDevice->GetViewport(&vp);

    float invWidth = 1.0f / width;
    float invHeight = 1.0f / height;

    D3DMATRIX scissor;
    scissor._11 = vp.dwWidth * invWidth;
    scissor._22 = vp.dwHeight * invHeight;
    scissor._41 = scissor._11 + 2.0f * (vp.dwX - (x + width * 0.5f)) * invWidth;
    scissor._42 = scissor._22 + 2.0f * (vp.dwY - (y + height * 0.5f)) * invHeight;

    D3DMATRIX projection;
    d3dDevice->GetTransform(D3DTRANSFORMSTATE_PROJECTION, &projection);
    projection._11 = projection._11 * scissor._11;
    projection._22 = projection._22 * scissor._22;
    projection._41 = projection._41 * scissor._11 + scissor._41;
    projection._42 = projection._42 * scissor._22 - scissor._42;
    d3dDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &projection);

    vp.dwX = x;
    vp.dwY = y;
    vp.dwWidth = width;
    vp.dwHeight = height;
    d3dDevice->SetViewport(&vp);
}
//------------------------------------------------------------------------------
void xxSetPipelineD3D7(uint64_t commandEncoder, uint64_t pipeline)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(commandEncoder);
    D3DPIPELINE7* d3dPipeline = reinterpret_cast<D3DPIPELINE7*>(pipeline);

    d3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
    d3dDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE);
    d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    d3dDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
    d3dDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
    d3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, d3dPipeline->renderState.depthWrite);
    d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, d3dPipeline->renderState.alphaBlending);
    d3dDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, d3dPipeline->renderState.alphaTesting);
    d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
//------------------------------------------------------------------------------
void xxSetIndexBufferD3D7(uint64_t commandEncoder, uint64_t buffer)
{

}
//------------------------------------------------------------------------------
void xxSetVertexBuffersD3D7(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(commandEncoder);

    d3dDevice->SetRenderState(D3DRENDERSTATETYPE(0), (DWORD)(getResourceData(buffers[0])));
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesD3D7(uint64_t commandEncoder, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesD3D7(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        LPDIRECTDRAWSURFACE7 ddSurface = reinterpret_cast<LPDIRECTDRAWSURFACE7>(textures[i]);
        d3dDevice->SetTexture(i, ddSurface);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersD3D7(uint64_t commandEncoder, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersD3D7(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        D3DSAMPLER7 d3dSampler = { samplers[i] };
        d3dDevice->SetTextureStageState(i, D3DTSS_ADDRESSU, d3dSampler.addressU);
        d3dDevice->SetTextureStageState(i, D3DTSS_ADDRESSV, d3dSampler.addressV);
        d3dDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, d3dSampler.magFilter);
        d3dDevice->SetTextureStageState(i, D3DTSS_MINFILTER, d3dSampler.minFilter);
        d3dDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, d3dSampler.mipFilter);
        d3dDevice->SetTextureStageState(i, D3DTSS_MAXANISOTROPY, d3dSampler.anisotropy);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferD3D7(uint64_t commandEncoder, uint64_t buffer, unsigned int size)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferD3D7(uint64_t commandEncoder, uint64_t buffer, unsigned int size)
{

}
//------------------------------------------------------------------------------
void xxDrawIndexedD3D7(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(commandEncoder);
    if (d3dDevice == nullptr)
        return;
    DWORD* d3dIndexBuffer = reinterpret_cast<DWORD*>(getResourceData(indexBuffer));

    LPDIRECT3DVERTEXBUFFER7 vertexBuffer = nullptr;
    d3dDevice->GetRenderState(D3DRENDERSTATETYPE(0), (DWORD*)&vertexBuffer);

    static WORD indexBuffer16[65536];
    for (int i = 0; i < indexCount; ++i)
    {
        indexBuffer16[i] = (WORD)d3dIndexBuffer[firstIndex + i];
    }

    d3dDevice->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST, vertexBuffer, vertexOffset, 0, indexBuffer16, indexCount, 0);
}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetTransformD3D7(uint64_t commandEncoder, const float* world, const float* view, const float* projection)
{
    LPDIRECT3DDEVICE7 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE7>(commandEncoder);

    if (world)
        d3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX*)world);
    if (view)
        d3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, (D3DMATRIX*)view);
    if (projection)
        d3dDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, (D3DMATRIX*)projection);
}
//==============================================================================