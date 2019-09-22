//==============================================================================
// xxGraphic : Direct3D 8.0 Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicInternal.h"
#include "xxGraphicD3D.h"
#include "xxGraphicD3DAsm.h"
#include "xxGraphicD3D8.h"

#include "dxsdk/d3d8.h"
typedef LPDIRECT3D8 (WINAPI *PFN_DIRECT3D_CREATE8)(UINT);
#define D3DRTYPE_CONSTANTBUFFER 0

static const wchar_t* const g_dummy = L"xxGraphicDummyWindow";
static HMODULE              g_d3dLibrary = nullptr;
static HWND                 g_hWnd = nullptr;

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
uint64_t xxCreateInstanceD3D8()
{
    if (g_d3dLibrary == nullptr)
        g_d3dLibrary = LoadLibraryW(L"d3d8.dll");
    if (g_d3dLibrary == nullptr)
        return 0;

    PFN_DIRECT3D_CREATE8 Direct3DCreate8;
    (void*&)Direct3DCreate8 = GetProcAddress(g_d3dLibrary, "Direct3DCreate8");
    if (Direct3DCreate8 == nullptr)
        return 0;

    LPDIRECT3D8 d3d = nullptr;
    if (d3d == nullptr)
        d3d = Direct3DCreate8(D3D_SDK_VERSION);
    if (d3d == nullptr)
        d3d = Direct3DCreate8(120/*D3D_SDK_VERSION*/);
    if (d3d == nullptr)
        return 0;

    xxRegisterFunction(D3D8);

    return reinterpret_cast<uint64_t>(d3d);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceD3D8(uint64_t instance)
{
    LPDIRECT3D8 d3d = reinterpret_cast<LPDIRECT3D8>(instance);

    SafeRelease(d3d);

    if (g_d3dLibrary)
    {
        FreeLibrary(g_d3dLibrary);
        g_d3dLibrary = nullptr;
    }

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceD3D8(uint64_t instance)
{
    LPDIRECT3D8 d3d = reinterpret_cast<LPDIRECT3D8>(instance);
    if (d3d == nullptr)
        return 0;

    if (g_hWnd == nullptr)
    {
        WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_OWNDC, DefWindowProcW, 0, 0, GetModuleHandleW(nullptr), nullptr, nullptr, nullptr, nullptr, g_dummy, nullptr };
        RegisterClassExW(&wc);
        g_hWnd = CreateWindowW(g_dummy, g_dummy, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
    }

    D3DPRESENT_PARAMETERS d3dPresentParameters = {};
    d3dPresentParameters.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPresentParameters.hDeviceWindow = g_hWnd;
    d3dPresentParameters.Windowed = TRUE;

    LPDIRECT3DDEVICE8 d3dDevice = nullptr;
    HRESULT hResult = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, nullptr, D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dPresentParameters, &d3dDevice);
    if (hResult != S_OK)
        return 0;

    IUnknown* unknown = nullptr;
    xxLocalBreak()
    {
        if (d3dDevice->QueryInterface(__uuidof(IDirect3DDevice8), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "8.0", xxGetDeviceName());
            break;
        }
    }
    SafeRelease(unknown);

    return reinterpret_cast<uint64_t>(d3dDevice);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceD3D8(uint64_t device)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);

    SafeRelease(d3dDevice);

    if (g_hWnd)
    {
        DestroyWindow(g_hWnd);
        UnregisterClassW(g_dummy, GetModuleHandleW(nullptr));
        g_hWnd = nullptr;
    }
}
//------------------------------------------------------------------------------
void xxResetDeviceD3D8(uint64_t device)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);
    if (d3dDevice == nullptr)
        return;

    D3DPRESENT_PARAMETERS d3dPresentParameters = {};
    d3dPresentParameters.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPresentParameters.hDeviceWindow = g_hWnd;
    d3dPresentParameters.Windowed = TRUE;

    d3dDevice->Reset(&d3dPresentParameters);
}
//------------------------------------------------------------------------------
bool xxTestDeviceD3D8(uint64_t device)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);
    if (d3dDevice == nullptr)
        return false;

    HRESULT hResult = d3dDevice->TestCooperativeLevel();
    return hResult == S_OK;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameD3D8()
{
    return "Direct3D 8.0 Fixed Function";
}
//==============================================================================
//  Framebuffer
//==============================================================================
struct D3DFRAMEBUFFER8
{
    LPDIRECT3DSURFACE8      backBuffer;
    LPDIRECT3DSURFACE8      depthStencil;
};
//==============================================================================
//  Swapchain
//==============================================================================
struct D3DSWAPCHAIN8 : public D3DFRAMEBUFFER8
{
    LPDIRECT3DSWAPCHAIN8    swapchain;
    HWND                    hWnd;
};
//------------------------------------------------------------------------------
uint64_t xxCreateSwapchainD3D8(uint64_t device, uint64_t renderPass, void* view, unsigned int width, unsigned int height)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DSWAPCHAIN8* swapchain = new D3DSWAPCHAIN8;
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

    D3DPRESENT_PARAMETERS d3dPresentParameters = {};
    d3dPresentParameters.BackBufferWidth = width;
    d3dPresentParameters.BackBufferHeight = height;
    d3dPresentParameters.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPresentParameters.hDeviceWindow = hWnd;
    d3dPresentParameters.Windowed = TRUE;

    LPDIRECT3DSWAPCHAIN8 d3dSwapchain = nullptr;
    if (d3dSwapchain == nullptr)
    {
        HRESULT hResult = d3dDevice->CreateAdditionalSwapChain(&d3dPresentParameters, &d3dSwapchain);
        if (hResult != S_OK)
        {
            delete swapchain;
            return 0;
        }
    }

    LPDIRECT3DSURFACE8 d3dDepthStencil = nullptr;
    if (d3dDepthStencil == nullptr)
    {
        HRESULT hResult = d3dDevice->CreateDepthStencilSurface(width, height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, &d3dDepthStencil);
        if (hResult != S_OK)
        {
        }
    }

    swapchain->backBuffer = nullptr;
    swapchain->depthStencil = d3dDepthStencil;
    swapchain->swapchain = d3dSwapchain;
    swapchain->hWnd = hWnd;

    return reinterpret_cast<uint64_t>(swapchain);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainD3D8(uint64_t swapchain)
{
    D3DSWAPCHAIN8* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN8*>(swapchain);
    if (d3dSwapchain == nullptr)
        return;

    SafeRelease(d3dSwapchain->backBuffer);
    SafeRelease(d3dSwapchain->depthStencil);
    SafeRelease(d3dSwapchain->swapchain);
    delete d3dSwapchain;
}
//------------------------------------------------------------------------------
void xxPresentSwapchainD3D8(uint64_t swapchain)
{
    D3DSWAPCHAIN8* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN8*>(swapchain);
    if (d3dSwapchain == nullptr)
        return;

    d3dSwapchain->swapchain->Present(nullptr, nullptr, d3dSwapchain->hWnd, nullptr);
}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferD3D8(uint64_t device, uint64_t swapchain)
{
    return device;
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferD3D8(uint64_t device, uint64_t swapchain)
{
    D3DSWAPCHAIN8* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN8*>(swapchain);
    if (d3dSwapchain == nullptr)
        return 0;

    LPDIRECT3DSURFACE8 surface = nullptr;
    HRESULT hResult = d3dSwapchain->swapchain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surface);
    if (hResult != S_OK)
        return 0;

    SafeRelease(d3dSwapchain->backBuffer);
    d3dSwapchain->backBuffer = surface;

    return swapchain;
}
//==============================================================================
//  Command Buffer
//==============================================================================
bool xxBeginCommandBufferD3D8(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandBuffer);
    if (d3dDevice == nullptr)
        return false;

    HRESULT hResult = d3dDevice->BeginScene();
    return (hResult == S_OK);
}
//------------------------------------------------------------------------------
void xxEndCommandBufferD3D8(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    HRESULT hResult = d3dDevice->EndScene();
}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferD3D8(uint64_t commandBuffer, uint64_t swapchain)
{

}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxCreateRenderPassD3D8(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
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
void xxDestroyRenderPassD3D8(uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassD3D8(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float r, float g, float b, float a, float depth, unsigned char stencil)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandBuffer);
    if (d3dDevice == nullptr)
        return 0;
    D3DFRAMEBUFFER8* d3dFramebuffer = reinterpret_cast<D3DFRAMEBUFFER8*>(framebuffer);
    if (d3dFramebuffer == nullptr)
        return 0;
    DWORD d3dFlags = static_cast<DWORD>(renderPass);

    d3dDevice->SetRenderTarget(d3dFramebuffer->backBuffer, d3dFramebuffer->depthStencil);

    if (d3dFlags)
    {
        HRESULT hResult = d3dDevice->Clear(0, nullptr, d3dFlags, D3DCOLOR_COLORVALUE(r, g, b, a), depth, stencil);
        if (hResult != S_OK)
            return 0;
    }

    return commandBuffer;
}
//------------------------------------------------------------------------------
void xxEndRenderPassD3D8(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{

}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateConstantBufferD3D8(uint64_t device, unsigned int size)
{
    char* d3dBuffer = xxAlloc(char, size);

    return reinterpret_cast<uint64_t>(d3dBuffer) | D3DRTYPE_CONSTANTBUFFER;
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferD3D8(uint64_t device, unsigned int size)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);
    if (d3dDevice == nullptr)
        return 0;

    LPDIRECT3DINDEXBUFFER8 d3dIndexBuffer = nullptr;
    HRESULT hResult = d3dDevice->CreateIndexBuffer(size, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &d3dIndexBuffer);
    if (hResult != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(d3dIndexBuffer) | D3DRTYPE_INDEXBUFFER;
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferD3D8(uint64_t device, unsigned int size)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);
    if (d3dDevice == nullptr)
        return 0;

    LPDIRECT3DVERTEXBUFFER8 d3dVertexBuffer = nullptr;
    HRESULT hResult = d3dDevice->CreateVertexBuffer(size, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &d3dVertexBuffer);
    if (hResult != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(d3dVertexBuffer) | D3DRTYPE_VERTEXBUFFER;
}
//------------------------------------------------------------------------------
void xxDestroyBufferD3D8(uint64_t device, uint64_t buffer)
{
    switch (getResourceType(buffer))
    {
    case D3DRTYPE_CONSTANTBUFFER:
    {
        char* d3dBuffer = reinterpret_cast<char*>(getResourceData(buffer));

        xxFree(d3dBuffer);
        break;
    }
    case D3DRTYPE_VERTEXBUFFER:
    case D3DRTYPE_INDEXBUFFER:
    {
        LPDIRECT3DRESOURCE8 d3dResource = reinterpret_cast<LPDIRECT3DRESOURCE8>(getResourceData(buffer));
        if (d3dResource == nullptr)
            return;

        d3dResource->Release();
        break;
    }
    default:
        break;
    }
}
//------------------------------------------------------------------------------
void* xxMapBufferD3D8(uint64_t device, uint64_t buffer)
{
    switch (getResourceType(buffer))
    {
    case D3DRTYPE_CONSTANTBUFFER:
    {
        char* ptr = reinterpret_cast<char*>(getResourceData(buffer));
        if (ptr == nullptr)
            break;

        return ptr;
    }
    case D3DRTYPE_VERTEXBUFFER:
    {
        LPDIRECT3DVERTEXBUFFER8 d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER8>(getResourceData(buffer));
        if (d3dVertexBuffer == nullptr)
            break;

        BYTE* ptr = nullptr;
        HRESULT hResult = d3dVertexBuffer->Lock(0, 0, &ptr, D3DLOCK_NOOVERWRITE);
        if (hResult != S_OK)
            break;

        return ptr;
    }
    case D3DRTYPE_INDEXBUFFER:
    {
        LPDIRECT3DINDEXBUFFER8 d3dIndexBuffer = reinterpret_cast<LPDIRECT3DINDEXBUFFER8>(getResourceData(buffer));
        if (d3dIndexBuffer == nullptr)
            break;

        BYTE* ptr = nullptr;
        HRESULT hResult = d3dIndexBuffer->Lock(0, 0, &ptr, D3DLOCK_NOOVERWRITE);
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
void xxUnmapBufferD3D8(uint64_t device, uint64_t buffer)
{
    switch (getResourceType(buffer))
    {
    case D3DRTYPE_CONSTANTBUFFER:
    {
        return;
    }
    case D3DRTYPE_VERTEXBUFFER:
    {
        LPDIRECT3DVERTEXBUFFER8 d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER8>(getResourceData(buffer));
        if (d3dVertexBuffer == nullptr)
            break;

        d3dVertexBuffer->Unlock();
        return;
    }
    case D3DRTYPE_INDEXBUFFER:
    {
        LPDIRECT3DINDEXBUFFER8 d3dIndexBuffer = reinterpret_cast<LPDIRECT3DINDEXBUFFER8>(getResourceData(buffer));
        if (d3dIndexBuffer == nullptr)
            break;

        d3dIndexBuffer->Unlock();
        return;
    }
    default:
        break;
    }
}
//==============================================================================
//  Texture
//==============================================================================
uint64_t xxCreateTextureD3D8(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (width == 0 || height == 0 || depth == 0 || mipmap == 0 || array == 0)
        return 0;

    if (depth == 1 && array == 1)
    {
        LPDIRECT3DTEXTURE8 d3dTexture = nullptr;
        HRESULT hResult = d3dDevice->CreateTexture(width, height, mipmap, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3dTexture);
        if (hResult != S_OK)
            return 0;

        return reinterpret_cast<uint64_t>(d3dTexture) | D3DRTYPE_TEXTURE;
    }

    if (width == height && depth == 1 && array == 6)
    {
        LPDIRECT3DCUBETEXTURE8 d3dCubeTexture = nullptr;
        HRESULT hResult = d3dDevice->CreateCubeTexture(width, mipmap, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3dCubeTexture);
        if (hResult != S_OK)
            return 0;

        return reinterpret_cast<uint64_t>(d3dCubeTexture) | D3DRTYPE_CUBETEXTURE;
    }

    if (depth != 1 && array == 1)
    {
        LPDIRECT3DVOLUMETEXTURE8 d3dVolumeTexture = nullptr;
        HRESULT hResult = d3dDevice->CreateVolumeTexture(width, height, depth, mipmap, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3dVolumeTexture);
        if (hResult != S_OK)
            return 0;

        return reinterpret_cast<uint64_t>(d3dVolumeTexture) | D3DRTYPE_VOLUME;
    }

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyTextureD3D8(uint64_t texture)
{
    LPDIRECT3DBASETEXTURE8 d3dBaseTexture = reinterpret_cast<LPDIRECT3DBASETEXTURE8>(getResourceData(texture));

    SafeRelease(d3dBaseTexture);
}
//------------------------------------------------------------------------------
void* xxMapTextureD3D8(uint64_t device, uint64_t texture, unsigned int& stride, unsigned int level, unsigned int array, unsigned int mipmap)
{
    switch (getResourceType(texture))
    {
    case D3DRTYPE_TEXTURE:
    {
        LPDIRECT3DTEXTURE8 d3dTexture = reinterpret_cast<LPDIRECT3DTEXTURE8>(getResourceData(texture));
        if (d3dTexture == nullptr)
            break;

        D3DLOCKED_RECT rect = {};
        HRESULT hResult = d3dTexture->LockRect(level, &rect, nullptr, D3DLOCK_DISCARD);
        if (hResult != S_OK)
            break;

        stride = rect.Pitch;
        return rect.pBits;
    }
    case D3DRTYPE_CUBETEXTURE:
    {
        LPDIRECT3DCUBETEXTURE8 d3dCubeTexture = reinterpret_cast<LPDIRECT3DCUBETEXTURE8>(getResourceData(texture));
        if (d3dCubeTexture == nullptr)
            break;

        D3DLOCKED_RECT rect = {};
        HRESULT hResult = d3dCubeTexture->LockRect((D3DCUBEMAP_FACES)array, level, &rect, nullptr, D3DLOCK_DISCARD);
        if (hResult != S_OK)
            break;

        stride = rect.Pitch;
        return rect.pBits;
    }
    case D3DRTYPE_VOLUME:
    {
        LPDIRECT3DVOLUMETEXTURE8 d3dVolumeTexture = reinterpret_cast<LPDIRECT3DVOLUMETEXTURE8>(getResourceData(texture));
        if (d3dVolumeTexture == nullptr)
            break;

        D3DLOCKED_BOX box = {};
        HRESULT hResult = d3dVolumeTexture->LockBox(level, &box, nullptr, D3DLOCK_DISCARD);
        if (hResult != S_OK)
            break;

        stride = box.RowPitch;
        return box.pBits;
    }
    default:
        break;
    }

    return nullptr;
}
//------------------------------------------------------------------------------
void xxUnmapTextureD3D8(uint64_t device, uint64_t texture, unsigned int level, unsigned int array, unsigned int mipmap)
{
    switch (getResourceType(texture))
    {
    case D3DRTYPE_TEXTURE:
    {
        LPDIRECT3DTEXTURE8 d3dTexture = reinterpret_cast<LPDIRECT3DTEXTURE8>(getResourceData(texture));
        if (d3dTexture == nullptr)
            break;

        HRESULT hResult = d3dTexture->UnlockRect(level);
        if (hResult != S_OK)
            break;

        return;
    }
    case D3DRTYPE_CUBETEXTURE:
    {
        LPDIRECT3DCUBETEXTURE8 d3dCubeTexture = reinterpret_cast<LPDIRECT3DCUBETEXTURE8>(getResourceData(texture));
        if (d3dCubeTexture == nullptr)
            break;

        HRESULT hResult = d3dCubeTexture->UnlockRect((D3DCUBEMAP_FACES)array, level);
        if (hResult != S_OK)
            break;

        return;
    }
    case D3DRTYPE_VOLUME:
    {
        LPDIRECT3DVOLUMETEXTURE8 d3dVolumeTexture = reinterpret_cast<LPDIRECT3DVOLUMETEXTURE8>(getResourceData(texture));
        if (d3dVolumeTexture == nullptr)
            break;

        HRESULT hResult = d3dVolumeTexture->UnlockBox(level);
        if (hResult != S_OK)
            break;

        return;
    }
    default:
        break;
    }
}
//==============================================================================
//  Sampler
//==============================================================================
union D3DSAMPLER8
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
uint64_t xxCreateSamplerD3D8(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    D3DSAMPLER8 d3dSampler = {};

    d3dSampler.addressU = clampU ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
    d3dSampler.addressV = clampV ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
    d3dSampler.addressW = clampW ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;
    d3dSampler.magFilter = linearMag ? D3DTEXF_LINEAR : D3DTEXF_POINT;
    d3dSampler.minFilter = linearMin ? D3DTEXF_LINEAR : D3DTEXF_POINT;
    d3dSampler.mipFilter = linearMip ? D3DTEXF_LINEAR : D3DTEXF_POINT;
    d3dSampler.anisotropy = anisotropy;
    if (anisotropy > 1)
    {
        d3dSampler.magFilter = linearMag ? D3DTEXF_ANISOTROPIC : D3DTEXF_POINT;
        d3dSampler.minFilter = linearMin ? D3DTEXF_ANISOTROPIC : D3DTEXF_POINT;
        d3dSampler.mipFilter = linearMip ? D3DTEXF_ANISOTROPIC : D3DTEXF_POINT;
    }

    return d3dSampler.value;
}
//------------------------------------------------------------------------------
void xxDestroySamplerD3D8(uint64_t sampler)
{

}
//==============================================================================
//  Vertex Attribute
//==============================================================================
union D3DVERTEXATTRIBUTE8
{
    uint64_t    value;
    struct
    {
        DWORD   fvf;
        int     stride;
    };
};
//------------------------------------------------------------------------------
uint64_t xxCreateVertexAttributeD3D8(uint64_t device, int count, ...)
{
    D3DVERTEXATTRIBUTE8 d3dVertexAttribute = {};
    int stride = 0;

    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; ++i)
    {
        int stream = va_arg(args, int);
        int offset = va_arg(args, int);
        int element = va_arg(args, int);
        int size = va_arg(args, int);

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
    va_end(args);

    d3dVertexAttribute.stride = stride;

    return d3dVertexAttribute.value;
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeD3D8(uint64_t vertexAttribute)
{

}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderD3D8(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderD3D8(uint64_t device, const char* shader)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderD3D8(uint64_t device, uint64_t shader)
{

}
//==============================================================================
//  Pipeline
//==============================================================================
union D3DRENDERSTATE8
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
struct D3DPIPELINE8
{
    DWORD           vertexShader;
    DWORD           pixelShader;
    D3DRENDERSTATE8 renderState;
};
//------------------------------------------------------------------------------
uint64_t xxCreateBlendStateD3D8(uint64_t device, bool blending)
{
    D3DRENDERSTATE8 d3dRenderState = {};
    d3dRenderState.alphaBlending = blending;
    return d3dRenderState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateD3D8(uint64_t device, bool depthTest, bool depthWrite)
{
    D3DRENDERSTATE8 d3dRenderState = {};
    d3dRenderState.depthTest = depthTest;
    d3dRenderState.depthWrite = depthWrite;
    return d3dRenderState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateD3D8(uint64_t device, bool cull, bool scissor)
{
    D3DRENDERSTATE8 d3dRenderState = {};
    d3dRenderState.cull = cull;
    d3dRenderState.scissor = scissor;
    return d3dRenderState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineD3D8(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    D3DPIPELINE8* d3dPipeline = new D3DPIPELINE8;
    if (d3dPipeline == nullptr)
        return 0;

    D3DVERTEXATTRIBUTE8 d3dVertexAttribute  = { vertexAttribute };
    DWORD d3dVertexShader                   = static_cast<DWORD>(vertexShader);
    DWORD d3dPixelShader                    = static_cast<DWORD>(fragmentShader);
    D3DRENDERSTATE8 d3dBlendState           = { blendState };
    D3DRENDERSTATE8 d3dDepthStencilState    = { depthStencilState };
    D3DRENDERSTATE8 d3dRasterizerState      = { rasterizerState };
    d3dPipeline->vertexShader               = d3dVertexShader ? d3dVertexShader : d3dVertexAttribute.fvf;
    d3dPipeline->pixelShader                = d3dPixelShader;
    d3dPipeline->renderState.alphaBlending  = d3dBlendState.alphaBlending;
    d3dPipeline->renderState.depthTest      = d3dDepthStencilState.depthTest;
    d3dPipeline->renderState.depthWrite     = d3dDepthStencilState.depthWrite;
    d3dPipeline->renderState.cull           = d3dRasterizerState.cull;
    d3dPipeline->renderState.scissor        = d3dRasterizerState.scissor;

    return reinterpret_cast<uint64_t>(d3dPipeline);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateD3D8(uint64_t blendState)
{

}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateD3D8(uint64_t depthStencilState)
{

}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateD3D8(uint64_t rasterizerState)
{

}
//------------------------------------------------------------------------------
void xxDestroyPipelineD3D8(uint64_t pipeline)
{
    D3DPIPELINE8* d3dPipeline = reinterpret_cast<D3DPIPELINE8*>(pipeline);

    delete d3dPipeline;
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportD3D8(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandEncoder);

    D3DVIEWPORT8 vp;
    vp.X = x;
    vp.Y = y;
    vp.Width = width;
    vp.Height = height;
    vp.MinZ = minZ;
    vp.MaxZ = maxZ;
    d3dDevice->SetViewport(&vp);
}
//------------------------------------------------------------------------------
void xxSetScissorD3D8(uint64_t commandEncoder, int x, int y, int width, int height)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandEncoder);

    D3DVIEWPORT8 vp;
    d3dDevice->GetViewport(&vp);

    float invWidth = 1.0f / width;
    float invHeight = 1.0f / height;

    D3DMATRIX scissor;
    scissor._11 = vp.Width * invWidth;
    scissor._22 = vp.Height * invHeight;
    scissor._41 = scissor._11 + 2.0f * (vp.X - (x + width * 0.5f)) * invWidth;
    scissor._42 = scissor._22 + 2.0f * (vp.Y - (y + height * 0.5f)) * invHeight;

    D3DMATRIX projection;
    d3dDevice->GetTransform(D3DTS_PROJECTION, &projection);
    projection._11 = projection._11 * scissor._11;
    projection._22 = projection._22 * scissor._22;
    projection._41 = projection._41 * scissor._11 + scissor._41;
    projection._42 = projection._42 * scissor._22 - scissor._42;
    d3dDevice->SetTransform(D3DTS_PROJECTION, &projection);

    vp.X = x;
    vp.Y = y;
    vp.Width = width;
    vp.Height = height;
    d3dDevice->SetViewport(&vp);
}
//------------------------------------------------------------------------------
void xxSetPipelineD3D8(uint64_t commandEncoder, uint64_t pipeline)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandEncoder);
    D3DPIPELINE8* d3dPipeline = reinterpret_cast<D3DPIPELINE8*>(pipeline);

    if ((d3dPipeline->vertexShader & D3DFVF_RESERVED0) == 0)
    {
        d3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
        d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    }
    if (d3dPipeline->pixelShader == 0)
    {
        d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    }
    d3dDevice->SetVertexShader(d3dPipeline->vertexShader);
    d3dDevice->SetPixelShader(d3dPipeline->pixelShader);
    d3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    d3dDevice->SetRenderState(D3DRS_ZENABLE, d3dPipeline->renderState.depthWrite);
    d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, d3dPipeline->renderState.alphaBlending);
    d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, d3dPipeline->renderState.alphaTesting);
    d3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
//------------------------------------------------------------------------------
void xxSetIndexBufferD3D8(uint64_t commandEncoder, uint64_t buffer)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandEncoder);
    LPDIRECT3DINDEXBUFFER8 d3dIndexBuffer = reinterpret_cast<LPDIRECT3DINDEXBUFFER8>(getResourceData(buffer));

    d3dDevice->SetIndices(d3dIndexBuffer, 0);
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersD3D8(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandEncoder);
    D3DVERTEXATTRIBUTE8 d3dVertexAttribute = { vertexAttribute };

    for (int i = 0; i < count; ++i)
    {
        LPDIRECT3DVERTEXBUFFER8 d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER8>(getResourceData(buffers[i]));
        d3dDevice->SetStreamSource(i, d3dVertexBuffer, d3dVertexAttribute.stride);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesD3D8(uint64_t commandEncoder, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesD3D8(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        LPDIRECT3DBASETEXTURE8 d3dBaseTexture = reinterpret_cast<LPDIRECT3DBASETEXTURE8>(getResourceData(textures[i]));
        d3dDevice->SetTexture(i, d3dBaseTexture);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersD3D8(uint64_t commandEncoder, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersD3D8(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        D3DSAMPLER8 d3dSampler = { samplers[i] };
        d3dDevice->SetTextureStageState(i, D3DTSS_ADDRESSU, d3dSampler.addressU);
        d3dDevice->SetTextureStageState(i, D3DTSS_ADDRESSV, d3dSampler.addressV);
        d3dDevice->SetTextureStageState(i, D3DTSS_ADDRESSW, d3dSampler.addressW);
        d3dDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, d3dSampler.magFilter);
        d3dDevice->SetTextureStageState(i, D3DTSS_MINFILTER, d3dSampler.minFilter);
        d3dDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, d3dSampler.mipFilter);
        d3dDevice->SetTextureStageState(i, D3DTSS_MAXANISOTROPY, d3dSampler.anisotropy);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferD3D8(uint64_t commandEncoder, uint64_t buffer, unsigned int size)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferD3D8(uint64_t commandEncoder, uint64_t buffer, unsigned int size)
{

}
//------------------------------------------------------------------------------
void xxDrawIndexedD3D8(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandEncoder);

    LPDIRECT3DINDEXBUFFER8 d3dIndexBuffer = nullptr;
    UINT d3dBaseVertexIndex = 0;
    d3dDevice->GetIndices(&d3dIndexBuffer, &d3dBaseVertexIndex);
    d3dDevice->SetIndices(d3dIndexBuffer, vertexOffset);
    SafeRelease(d3dIndexBuffer);

    d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, firstIndex, indexCount / 3);
}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetTransformD3D8(uint64_t commandEncoder, const float* world, const float* view, const float* projection)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandEncoder);

    if (world)
        d3dDevice->SetTransform(D3DTS_WORLD, (const D3DMATRIX*)world);
    if (view)
        d3dDevice->SetTransform(D3DTS_VIEW, (const D3DMATRIX*)view);
    if (projection)
        d3dDevice->SetTransform(D3DTS_PROJECTION, (const D3DMATRIX*)projection);
}
//==============================================================================
