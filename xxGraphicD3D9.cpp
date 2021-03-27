//==============================================================================
// xxGraphic : Direct3D 9.0 Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalD3D.h"
#include "xxGraphicD3DAsm.h"
#include "xxGraphicD3D9.h"

#if defined(_DEBUG)
#define D3D_DEBUG_INFO 1
#endif
#include "dxsdk/d3d9.h"
typedef LPDIRECT3D9 (WINAPI *PFN_DIRECT3D_CREATE9)(UINT);
#define D3DRTYPE_CONSTANTBUFFER     0

static void*                        g_d3dLibrary = nullptr;

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D9()
{
    if (g_d3dLibrary == nullptr)
        g_d3dLibrary = xxLoadLibrary("d3d9.dll");
    if (g_d3dLibrary == nullptr)
        return 0;

    PFN_DIRECT3D_CREATE9 Direct3DCreate9;
    (void*&)Direct3DCreate9 = xxGetProcAddress(g_d3dLibrary, "Direct3DCreate9");
    if (Direct3DCreate9 == nullptr)
        return 0;

    LPDIRECT3D9 d3d = nullptr;
    if (d3d == nullptr)
        d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (d3d == nullptr)
        d3d = Direct3DCreate9(D3D9b_SDK_VERSION);
    if (d3d == nullptr)
        d3d = Direct3DCreate9(30/*D3D_SDK_VERSION*/);
    if (d3d == nullptr)
        return 0;

    xxRegisterFunction(D3D9);

    return reinterpret_cast<uint64_t>(d3d);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceD3D9(uint64_t instance)
{
    LPDIRECT3D9 d3d = reinterpret_cast<LPDIRECT3D9>(instance);

    SafeRelease(d3d);

    if (g_d3dLibrary)
    {
        xxFreeLibrary(g_d3dLibrary);
        g_d3dLibrary = nullptr;
    }

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceD3D9(uint64_t instance)
{
    LPDIRECT3D9 d3d = reinterpret_cast<LPDIRECT3D9>(instance);
    if (d3d == nullptr)
        return 0;

    D3DPRESENT_PARAMETERS d3dPresentParameters = {};
    d3dPresentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPresentParameters.hDeviceWindow = GetDesktopWindow();
    d3dPresentParameters.Windowed = TRUE;
    d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    LPDIRECT3DDEVICE9 d3dDevice = nullptr;
    HRESULT hResult = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, nullptr, D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dPresentParameters, &d3dDevice);
    if (hResult != S_OK)
        return 0;

    IUnknown* unknown = nullptr;
    xxLocalBreak()
    {
        if (d3dDevice->QueryInterface(__uuidof(IDirect3DDevice9On12), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "9On12", xxGetDeviceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(IDirect3DDevice9Ex), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "9Ex", xxGetDeviceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(IDirect3DDevice9), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "9.0", xxGetDeviceName());
            break;
        }
    }
    SafeRelease(unknown);

    return reinterpret_cast<uint64_t>(d3dDevice);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceD3D9(uint64_t device)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);

    SafeRelease(d3dDevice);
}
//------------------------------------------------------------------------------
bool xxResetDeviceD3D9(uint64_t device)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return true;

    D3DPRESENT_PARAMETERS d3dPresentParameters = {};
    d3dPresentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPresentParameters.hDeviceWindow = GetDesktopWindow();
    d3dPresentParameters.Windowed = TRUE;
    d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    d3dDevice->Reset(&d3dPresentParameters);
    return true;
}
//------------------------------------------------------------------------------
bool xxTestDeviceD3D9(uint64_t device)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return false;

    HRESULT hResult = d3dDevice->TestCooperativeLevel();
    return hResult == S_OK;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameD3D9()
{
    return "Direct3D 9.0 Fixed Function";
}
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t xxCreateSwapchainD3D9(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DSWAPCHAIN9* d3dOldSwapchain = reinterpret_cast<D3DSWAPCHAIN9*>(oldSwapchain);
    if (d3dOldSwapchain && d3dOldSwapchain->hWnd == view && d3dOldSwapchain->width == width && d3dOldSwapchain->height == height)
        return oldSwapchain;
    D3DSWAPCHAIN9* swapchain = xxAlloc(D3DSWAPCHAIN9);
    if (swapchain == nullptr)
        return 0;

    xxDestroySwapchainD3D9(oldSwapchain);

    HWND hWnd = (HWND)view;

    D3DPRESENT_PARAMETERS d3dPresentParameters = {};
    d3dPresentParameters.BackBufferWidth = width;
    d3dPresentParameters.BackBufferHeight = height;
    d3dPresentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dPresentParameters.BackBufferCount = 3;
    d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_FLIPEX;
    d3dPresentParameters.hDeviceWindow = hWnd;
    d3dPresentParameters.Windowed = TRUE;
    d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    LPDIRECT3DSWAPCHAIN9 d3dSwapchain = nullptr;
    if (d3dSwapchain == nullptr)
    {
        HRESULT hResult = d3dDevice->CreateAdditionalSwapChain(&d3dPresentParameters, &d3dSwapchain);
        if (hResult != S_OK)
        {
            d3dPresentParameters.BackBufferCount = 0;
            d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
            HRESULT hResult = d3dDevice->CreateAdditionalSwapChain(&d3dPresentParameters, &d3dSwapchain);
            if (hResult != S_OK)
            {
                xxFree(swapchain);
                return 0;
            }
        }
    }

    LPDIRECT3DSURFACE9 d3dDepthStencil = nullptr;
    if (d3dDepthStencil == nullptr)
    {
        HRESULT hResult = d3dDevice->CreateDepthStencilSurface(width, height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &d3dDepthStencil, nullptr);
        if (hResult != S_OK)
        {
        }
    }

    swapchain->backBuffer = nullptr;
    swapchain->depthStencil = d3dDepthStencil;
    swapchain->swapchain = d3dSwapchain;
    swapchain->hWnd = hWnd;
    swapchain->width = width;
    swapchain->height = height;

    if (d3dPresentParameters.SwapEffect == D3DSWAPEFFECT_FLIPEX)
    {
        swapchain->hWnd = nullptr;
    }

    return reinterpret_cast<uint64_t>(swapchain);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainD3D9(uint64_t swapchain)
{
    D3DSWAPCHAIN9* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN9*>(swapchain);
    if (d3dSwapchain == nullptr)
        return;

    SafeRelease(d3dSwapchain->backBuffer);
    SafeRelease(d3dSwapchain->depthStencil);
    SafeRelease(d3dSwapchain->swapchain);
    xxFree(d3dSwapchain);
}
//------------------------------------------------------------------------------
void xxPresentSwapchainD3D9(uint64_t swapchain)
{
    D3DSWAPCHAIN9* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN9*>(swapchain);
    if (d3dSwapchain == nullptr)
        return;

    d3dSwapchain->swapchain->Present(nullptr, nullptr, d3dSwapchain->hWnd, nullptr, 0);
}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferD3D9(uint64_t device, uint64_t swapchain)
{
    return device;
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferD3D9(uint64_t device, uint64_t swapchain, float* scale)
{
    if (scale)
    {
        (*scale) = 1.0f;
    }

    D3DSWAPCHAIN9* d3dSwapchain = reinterpret_cast<D3DSWAPCHAIN9*>(swapchain);
    if (d3dSwapchain == nullptr)
        return 0;

    LPDIRECT3DSURFACE9 surface = nullptr;
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
bool xxBeginCommandBufferD3D9(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return false;

    HRESULT hResult = d3dDevice->BeginScene();
    return (hResult == S_OK);
}
//------------------------------------------------------------------------------
void xxEndCommandBufferD3D9(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    HRESULT hResult = d3dDevice->EndScene();
}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferD3D9(uint64_t commandBuffer, uint64_t swapchain)
{

}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxCreateRenderPassD3D9(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
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
void xxDestroyRenderPassD3D9(uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassD3D9(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return 0;
    D3DFRAMEBUFFER9* d3dFramebuffer = reinterpret_cast<D3DFRAMEBUFFER9*>(framebuffer);
    if (d3dFramebuffer == nullptr)
        return 0;
    DWORD d3dFlags = static_cast<DWORD>(renderPass);

    d3dDevice->SetRenderTarget(0, d3dFramebuffer->backBuffer);
    d3dDevice->SetDepthStencilSurface(d3dFramebuffer->depthStencil);

    D3DVIEWPORT9 vp;
    vp.X = 0;
    vp.Y = 0;
    vp.Width = width;
    vp.Height = height;
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;
    d3dDevice->SetViewport(&vp);

    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    d3dDevice->SetScissorRect(&rect);

    if (d3dFlags)
    {
        d3dDevice->Clear(0, nullptr, d3dFlags, D3DCOLOR_COLORVALUE(color[0], color[1], color[2], color[3]), depth, stencil);
    }

    return commandBuffer;
}
//------------------------------------------------------------------------------
void xxEndRenderPassD3D9(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{

}
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t xxCreateVertexAttributeD3D9(uint64_t device, int count, int* attribute)
{
    D3DVERTEXATTRIBUTE9 d3dVertexAttribute = {};
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
void xxDestroyVertexAttributeD3D9(uint64_t vertexAttribute)
{

}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateConstantBufferD3D9(uint64_t device, int size)
{
    char* d3dBuffer = xxAlloc(char, size);

    return reinterpret_cast<uint64_t>(d3dBuffer) | D3DRTYPE_CONSTANTBUFFER;
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferD3D9(uint64_t device, int size)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;

    D3DFORMAT format = (INDEX_BUFFER_WIDTH == 2) ? D3DFMT_INDEX16 : D3DFMT_INDEX32;
    LPDIRECT3DINDEXBUFFER9 d3dIndexBuffer = nullptr;
    HRESULT hResult = d3dDevice->CreateIndexBuffer(size, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, format, D3DPOOL_DEFAULT, &d3dIndexBuffer, nullptr);
    if (hResult != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(d3dIndexBuffer) | D3DRTYPE_INDEXBUFFER;
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferD3D9(uint64_t device, int size, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;

    LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer = nullptr;
    HRESULT hResult = d3dDevice->CreateVertexBuffer(size, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &d3dVertexBuffer, nullptr);
    if (hResult != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(d3dVertexBuffer) | D3DRTYPE_VERTEXBUFFER;
}
//------------------------------------------------------------------------------
void xxDestroyBufferD3D9(uint64_t device, uint64_t buffer)
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
        LPDIRECT3DRESOURCE9 d3dResource = reinterpret_cast<LPDIRECT3DRESOURCE9>(getResourceData(buffer));
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
void* xxMapBufferD3D9(uint64_t device, uint64_t buffer)
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
        LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER9>(getResourceData(buffer));
        if (d3dVertexBuffer == nullptr)
            break;

        void* ptr = nullptr;
        HRESULT hResult = d3dVertexBuffer->Lock(0, 0, &ptr, D3DLOCK_NOOVERWRITE);
        if (hResult != S_OK)
            break;

        return ptr;
    }
    case D3DRTYPE_INDEXBUFFER:
    {
        LPDIRECT3DINDEXBUFFER9 d3dIndexBuffer = reinterpret_cast<LPDIRECT3DINDEXBUFFER9>(getResourceData(buffer));
        if (d3dIndexBuffer == nullptr)
            break;

        void* ptr = nullptr;
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
void xxUnmapBufferD3D9(uint64_t device, uint64_t buffer)
{
    switch (getResourceType(buffer))
    {
    case D3DRTYPE_CONSTANTBUFFER:
    {
        return;
    }
    case D3DRTYPE_VERTEXBUFFER:
    {
        LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER9>(getResourceData(buffer));
        if (d3dVertexBuffer == nullptr)
            break;

        d3dVertexBuffer->Unlock();
        return;
    }
    case D3DRTYPE_INDEXBUFFER:
    {
        LPDIRECT3DINDEXBUFFER9 d3dIndexBuffer = reinterpret_cast<LPDIRECT3DINDEXBUFFER9>(getResourceData(buffer));
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
uint64_t xxCreateTextureD3D9(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, const void* external)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (width == 0 || height == 0 || depth == 0 || mipmap == 0 || array == 0)
        return 0;

    if (external)
    {
        IUnknown* unknown = (IUnknown*)external;
        LPDIRECT3DTEXTURE9 d3dTexture = nullptr;
        HRESULT hResult = unknown->QueryInterface(__uuidof(LPDIRECT3DTEXTURE9), (void**)&d3dTexture);
        if (hResult != S_OK)
            return 0;

        return reinterpret_cast<uint64_t>(d3dTexture) | D3DRTYPE_TEXTURE;
    }

    if (depth == 1 && array == 1)
    {
        LPDIRECT3DTEXTURE9 d3dTexture = nullptr;
        HRESULT hResult = d3dDevice->CreateTexture(width, height, mipmap, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3dTexture, nullptr);
        if (hResult != S_OK)
            return 0;

        return reinterpret_cast<uint64_t>(d3dTexture) | D3DRTYPE_TEXTURE;
    }

    if (width == height && depth == 1 && array == 6)
    {
        LPDIRECT3DCUBETEXTURE9 d3dCubeTexture = nullptr;
        HRESULT hResult = d3dDevice->CreateCubeTexture(width, mipmap, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3dCubeTexture, nullptr);
        if (hResult != S_OK)
            return 0;

        return reinterpret_cast<uint64_t>(d3dCubeTexture) | D3DRTYPE_CUBETEXTURE;
    }

    if (depth != 1 && array == 1)
    {
        LPDIRECT3DVOLUMETEXTURE9 d3dVolumeTexture = nullptr;
        HRESULT hResult = d3dDevice->CreateVolumeTexture(width, height, depth, mipmap, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3dVolumeTexture, nullptr);
        if (hResult != S_OK)
            return 0;

        return reinterpret_cast<uint64_t>(d3dVolumeTexture) | D3DRTYPE_VOLUME;
    }

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyTextureD3D9(uint64_t texture)
{
    LPDIRECT3DBASETEXTURE9 d3dBaseTexture = reinterpret_cast<LPDIRECT3DBASETEXTURE9>(getResourceData(texture));

    SafeRelease(d3dBaseTexture);
}
//------------------------------------------------------------------------------
void* xxMapTextureD3D9(uint64_t device, uint64_t texture, int* stride, int level, int array)
{
    switch (getResourceType(texture))
    {
    case D3DRTYPE_TEXTURE:
    {
        LPDIRECT3DTEXTURE9 d3dTexture = reinterpret_cast<LPDIRECT3DTEXTURE9>(getResourceData(texture));
        if (d3dTexture == nullptr)
            break;

        D3DLOCKED_RECT rect = {};
        HRESULT hResult = d3dTexture->LockRect(level, &rect, nullptr, D3DLOCK_DISCARD);
        if (hResult != S_OK)
            break;

        (*stride) = rect.Pitch;
        return rect.pBits;
    }
    case D3DRTYPE_CUBETEXTURE:
    {
        LPDIRECT3DCUBETEXTURE9 d3dCubeTexture = reinterpret_cast<LPDIRECT3DCUBETEXTURE9>(getResourceData(texture));
        if (d3dCubeTexture == nullptr)
            break;

        D3DLOCKED_RECT rect = {};
        HRESULT hResult = d3dCubeTexture->LockRect((D3DCUBEMAP_FACES)array, level, &rect, nullptr, D3DLOCK_DISCARD);
        if (hResult != S_OK)
            break;

        (*stride) = rect.Pitch;
        return rect.pBits;
    }
    case D3DRTYPE_VOLUME:
    {
        LPDIRECT3DVOLUMETEXTURE9 d3dVolumeTexture = reinterpret_cast<LPDIRECT3DVOLUMETEXTURE9>(getResourceData(texture));
        if (d3dVolumeTexture == nullptr)
            break;

        D3DLOCKED_BOX box = {};
        HRESULT hResult = d3dVolumeTexture->LockBox(level, &box, nullptr, D3DLOCK_DISCARD);
        if (hResult != S_OK)
            break;

        (*stride) = box.RowPitch;
        return box.pBits;
    }
    default:
        break;
    }

    return nullptr;
}
//------------------------------------------------------------------------------
void xxUnmapTextureD3D9(uint64_t device, uint64_t texture, int level, int array)
{
    switch (getResourceType(texture))
    {
    case D3DRTYPE_TEXTURE:
    {
        LPDIRECT3DTEXTURE9 d3dTexture = reinterpret_cast<LPDIRECT3DTEXTURE9>(getResourceData(texture));
        if (d3dTexture == nullptr)
            break;

        HRESULT hResult = d3dTexture->UnlockRect(level);
        if (hResult != S_OK)
            break;

        return;
    }
    case D3DRTYPE_CUBETEXTURE:
    {
        LPDIRECT3DCUBETEXTURE9 d3dCubeTexture = reinterpret_cast<LPDIRECT3DCUBETEXTURE9>(getResourceData(texture));
        if (d3dCubeTexture == nullptr)
            break;

        HRESULT hResult = d3dCubeTexture->UnlockRect((D3DCUBEMAP_FACES)array, level);
        if (hResult != S_OK)
            break;

        return;
    }
    case D3DRTYPE_VOLUME:
    {
        LPDIRECT3DVOLUMETEXTURE9 d3dVolumeTexture = reinterpret_cast<LPDIRECT3DVOLUMETEXTURE9>(getResourceData(texture));
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
uint64_t xxCreateSamplerD3D9(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    D3DSAMPLER9 d3dSampler = {};

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
void xxDestroySamplerD3D9(uint64_t sampler)
{

}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderD3D9(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderD3D9(uint64_t device, const char* shader)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderD3D9(uint64_t device, uint64_t shader)
{

}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreateBlendStateD3D9(uint64_t device, bool blending)
{
    D3DRENDERSTATE9 d3dRenderState = {};
    d3dRenderState.alphaBlending = blending;
    return d3dRenderState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateD3D9(uint64_t device, bool depthTest, bool depthWrite)
{
    D3DRENDERSTATE9 d3dRenderState = {};
    d3dRenderState.depthTest = depthTest;
    d3dRenderState.depthWrite = depthWrite;
    return d3dRenderState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateD3D9(uint64_t device, bool cull, bool scissor)
{
    D3DRENDERSTATE9 d3dRenderState = {};
    d3dRenderState.cull = cull;
    d3dRenderState.scissor = scissor;
    return d3dRenderState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineD3D9(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    D3DPIPELINE9* d3dPipeline = xxAlloc(D3DPIPELINE9);
    if (d3dPipeline == nullptr)
        return 0;

    D3DVERTEXATTRIBUTE9 d3dVertexAttribute  = { vertexAttribute };
    LPDIRECT3DVERTEXSHADER9 d3dVertexShader = reinterpret_cast<LPDIRECT3DVERTEXSHADER9>(vertexShader);
    LPDIRECT3DPIXELSHADER9 d3dPixelShader   = reinterpret_cast<LPDIRECT3DPIXELSHADER9>(fragmentShader);
    D3DRENDERSTATE9 d3dBlendState           = { blendState };
    D3DRENDERSTATE9 d3dDepthStencilState    = { depthStencilState };
    D3DRENDERSTATE9 d3dRasterizerState      = { rasterizerState };
    d3dPipeline->vertexDeclaration          = nullptr;
    d3dPipeline->fvf                        = d3dVertexAttribute.fvf;
    d3dPipeline->vertexShader               = d3dVertexShader;
    d3dPipeline->pixelShader                = d3dPixelShader;
    d3dPipeline->renderState.alphaBlending  = d3dBlendState.alphaBlending;
    d3dPipeline->renderState.depthTest      = d3dDepthStencilState.depthTest;
    d3dPipeline->renderState.depthWrite     = d3dDepthStencilState.depthWrite;
    d3dPipeline->renderState.cull           = d3dRasterizerState.cull;
    d3dPipeline->renderState.scissor        = d3dRasterizerState.scissor;

    return reinterpret_cast<uint64_t>(d3dPipeline);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateD3D9(uint64_t blendState)
{

}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateD3D9(uint64_t depthStencilState)
{

}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateD3D9(uint64_t rasterizerState)
{

}
//------------------------------------------------------------------------------
void xxDestroyPipelineD3D9(uint64_t pipeline)
{
    D3DPIPELINE9* d3dPipeline = reinterpret_cast<D3DPIPELINE9*>(pipeline);

    xxFree(d3dPipeline);
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportD3D9(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);

    D3DVIEWPORT9 vp;
    vp.X = x;
    vp.Y = y;
    vp.Width = width;
    vp.Height = height;
    vp.MinZ = minZ;
    vp.MaxZ = maxZ;
    d3dDevice->SetViewport(&vp);
}
//------------------------------------------------------------------------------
void xxSetScissorD3D9(uint64_t commandEncoder, int x, int y, int width, int height)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);

    RECT rect;
    rect.left = x;
    rect.top = y;
    rect.right = x + width;
    rect.bottom = y + height;
    d3dDevice->SetScissorRect(&rect);
}
//------------------------------------------------------------------------------
void xxSetPipelineD3D9(uint64_t commandEncoder, uint64_t pipeline)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);
    D3DPIPELINE9* d3dPipeline = reinterpret_cast<D3DPIPELINE9*>(pipeline);

    if (d3dPipeline->vertexDeclaration == nullptr)
    {
        d3dDevice->SetFVF(d3dPipeline->fvf);
        d3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
        d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    }
    else
    {
        d3dDevice->SetVertexDeclaration(d3dPipeline->vertexDeclaration);
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
    d3dDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    d3dDevice->SetRenderState(D3DRS_ZENABLE, d3dPipeline->renderState.depthWrite);
    d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, d3dPipeline->renderState.alphaBlending);
    d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, d3dPipeline->renderState.alphaTesting);
    d3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    d3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, d3dPipeline->renderState.scissor);
}
//------------------------------------------------------------------------------
void xxSetIndexBufferD3D9(uint64_t commandEncoder, uint64_t buffer)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);
    LPDIRECT3DINDEXBUFFER9 d3dIndexBuffer = reinterpret_cast<LPDIRECT3DINDEXBUFFER9>(getResourceData(buffer));

    d3dDevice->SetIndices(d3dIndexBuffer);
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersD3D9(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);
    D3DVERTEXATTRIBUTE9 d3dVertexAttribute = { vertexAttribute };

    for (int i = 0; i < count; ++i)
    {
        LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER9>(getResourceData(buffers[i]));
        d3dDevice->SetStreamSource(i, d3dVertexBuffer, 0, d3dVertexAttribute.stride);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesD3D9(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        LPDIRECT3DBASETEXTURE9 d3dBaseTexture = reinterpret_cast<LPDIRECT3DBASETEXTURE9>(getResourceData(textures[i]));
        d3dDevice->SetTexture(i + D3DVERTEXTEXTURESAMPLER0, d3dBaseTexture);
    }
}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesD3D9(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        LPDIRECT3DBASETEXTURE9 d3dBaseTexture = reinterpret_cast<LPDIRECT3DBASETEXTURE9>(getResourceData(textures[i]));
        d3dDevice->SetTexture(i, d3dBaseTexture);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersD3D9(uint64_t commandEncoder, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersD3D9(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        D3DSAMPLER9 d3dSampler = { samplers[i] };
        d3dDevice->SetSamplerState(i, D3DSAMP_ADDRESSU, d3dSampler.addressU);
        d3dDevice->SetSamplerState(i, D3DSAMP_ADDRESSV, d3dSampler.addressV);
        d3dDevice->SetSamplerState(i, D3DSAMP_ADDRESSW, d3dSampler.addressW);
        d3dDevice->SetSamplerState(i, D3DSAMP_MAGFILTER, d3dSampler.magFilter);
        d3dDevice->SetSamplerState(i, D3DSAMP_MINFILTER, d3dSampler.minFilter);
        d3dDevice->SetSamplerState(i, D3DSAMP_MIPFILTER, d3dSampler.mipFilter);
        d3dDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, d3dSampler.anisotropy);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferD3D9(uint64_t commandEncoder, uint64_t buffer, int size)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);
    D3DMATRIX* d3dMatrix = reinterpret_cast<D3DMATRIX*>(getResourceData(buffer));

    if (size >= sizeof(D3DMATRIX) * 1)
        d3dDevice->SetTransform(D3DTS_WORLD, &d3dMatrix[0]);
    if (size >= sizeof(D3DMATRIX) * 2)
        d3dDevice->SetTransform(D3DTS_VIEW, &d3dMatrix[1]);
    if (size >= sizeof(D3DMATRIX) * 3)
        d3dDevice->SetTransform(D3DTS_PROJECTION, &d3dMatrix[2]);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferD3D9(uint64_t commandEncoder, uint64_t buffer, int size)
{

}
//------------------------------------------------------------------------------
void xxDrawIndexedD3D9(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);

    d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vertexOffset, 0, 0, firstIndex, indexCount / 3);
}
//==============================================================================
