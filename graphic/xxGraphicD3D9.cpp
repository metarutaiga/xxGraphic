#include "xxGraphicD3D9.h"
#include "xxGraphicInternal.h"

#include "dxsdk/d3d9.h"
typedef LPDIRECT3D9 (WINAPI *PFN_DIRECT3D_CREATE9)(UINT);

static const wchar_t* const g_dummy = L"xxGraphicDummyWindow";
static HMODULE              g_d3dLibrary = nullptr;
static HWND                 g_hWnd = nullptr;
static LPDIRECT3DSURFACE9   g_depthStencil = nullptr;

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D9()
{
    if (g_d3dLibrary == nullptr)
        g_d3dLibrary = LoadLibraryW(L"d3d9.dll");
    if (g_d3dLibrary == nullptr)
        return 0;

    PFN_DIRECT3D_CREATE9 Direct3DCreate9;
    (void*&)Direct3DCreate9 = GetProcAddress(g_d3dLibrary, "Direct3DCreate9");
    if (Direct3DCreate9 == nullptr)
        return 0;

    LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (d3d == nullptr)
        return 0;

    xxRegisterFunction(D3D9);

    return reinterpret_cast<uint64_t>(d3d);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceD3D9(uint64_t instance)
{
    LPDIRECT3D9 d3d = reinterpret_cast<LPDIRECT3D9>(instance);
    if (d3d == nullptr)
        return;
    d3d->Release();

    if (g_d3dLibrary)
    {
        FreeLibrary(g_d3dLibrary);
        g_d3dLibrary = nullptr;
    }
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceD3D9(uint64_t instance)
{
    LPDIRECT3D9 d3d = reinterpret_cast<LPDIRECT3D9>(instance);
    if (d3d == nullptr)
        return 0;

    if (g_hWnd == nullptr)
    {
        WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_CLASSDC, DefWindowProcW, 0, 0, GetModuleHandleW(nullptr), nullptr, nullptr, nullptr, nullptr, g_dummy, nullptr };
        RegisterClassExW(&wc);
        g_hWnd = CreateWindowW(g_dummy, g_dummy, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
    }

    D3DPRESENT_PARAMETERS d3dPresentParameters = {};
    d3dPresentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPresentParameters.hDeviceWindow = g_hWnd;
    d3dPresentParameters.Windowed = TRUE;
    d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    LPDIRECT3DDEVICE9 d3dDevice = nullptr;
    HRESULT hResult = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, nullptr, D3DCREATE_MULTITHREADED | D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dPresentParameters, &d3dDevice);
    if (hResult != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(d3dDevice);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceD3D9(uint64_t device)
{
    if (g_depthStencil)
    {
        g_depthStencil->Release();
        g_depthStencil = nullptr;
    }

    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return;
    d3dDevice->Release();

    if (g_hWnd)
    {
        DestroyWindow(g_hWnd);
        UnregisterClassW(g_dummy, GetModuleHandleW(nullptr));
        g_hWnd = nullptr;
    }
}
//------------------------------------------------------------------------------
xxGL_API void xxResetDeviceD3D9(uint64_t device)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return;

    if (g_depthStencil)
        g_depthStencil->Release();
    g_depthStencil = nullptr;

    D3DPRESENT_PARAMETERS d3dPresentParameters = {};
    d3dPresentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPresentParameters.hDeviceWindow = g_hWnd;
    d3dPresentParameters.Windowed = TRUE;
    d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    d3dDevice->Reset(&d3dPresentParameters);
}
//------------------------------------------------------------------------------
xxGL_API bool xxTestDeviceD3D9(uint64_t device)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return false;

    HRESULT hResult = d3dDevice->TestCooperativeLevel();
    return hResult == S_OK;
}
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t xxCreateSwapchainD3D9(uint64_t device, void* view, unsigned int width, unsigned int height)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
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
    d3dPresentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPresentParameters.hDeviceWindow = hWnd;
    d3dPresentParameters.Windowed = TRUE;
    d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    LPDIRECT3DSWAPCHAIN9 d3dSwapchain = nullptr;
    HRESULT hResult = d3dDevice->CreateAdditionalSwapChain(&d3dPresentParameters, &d3dSwapchain);
    if (hResult != S_OK)
        return 0;

    D3DSURFACE_DESC desc = {};
    if (g_depthStencil)
        g_depthStencil->GetDesc(&desc);
    if (desc.Width < width || desc.Height < height)
    {
        if (desc.Width < width)
            desc.Width = width;
        if (desc.Height < height)
            desc.Height = height;
        if (g_depthStencil)
            g_depthStencil->Release();
        d3dDevice->CreateDepthStencilSurface(desc.Width, desc.Height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &g_depthStencil, nullptr);
    }

    return reinterpret_cast<uint64_t>(d3dSwapchain);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainD3D9(uint64_t swapchain)
{
    LPDIRECT3DSWAPCHAIN9 d3dSwapchain = reinterpret_cast<LPDIRECT3DSWAPCHAIN9>(swapchain);
    if (d3dSwapchain == nullptr)
        return;
    d3dSwapchain->Release();
}
//------------------------------------------------------------------------------
void xxPresentSwapchainD3D9(uint64_t swapchain, void* view)
{
    LPDIRECT3DSWAPCHAIN9 d3dSwapchain = reinterpret_cast<LPDIRECT3DSWAPCHAIN9>(swapchain);
    if (d3dSwapchain == nullptr)
        return;
    d3dSwapchain->Present(nullptr, nullptr, (HWND)view, nullptr, 0);
}
//==============================================================================
//  Command Buffer
//==============================================================================
uint64_t xxGetCommandBufferD3D9(uint64_t device, uint64_t swapchain)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;

    LPDIRECT3DSWAPCHAIN9 d3dSwapchain = reinterpret_cast<LPDIRECT3DSWAPCHAIN9>(swapchain);
    if (d3dSwapchain == nullptr)
        return 0;

    LPDIRECT3DSURFACE9 surface = nullptr;
    d3dSwapchain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surface);
    d3dDevice->SetRenderTarget(0, surface);
    d3dDevice->SetDepthStencilSurface(g_depthStencil);
    if (surface)
        surface->Release();

    return device;
}
//------------------------------------------------------------------------------
bool xxBeginCommandBufferD3D9(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return false;
    HRESULT hResult = d3dDevice->BeginScene();
    if (hResult != S_OK)
        return false;

    d3dDevice->SetPixelShader(nullptr);
    d3dDevice->SetVertexShader(nullptr);
    d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    d3dDevice->SetRenderState(D3DRS_LIGHTING, false);
    d3dDevice->SetRenderState(D3DRS_ZENABLE, false);
    d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
    d3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    d3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
    d3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    d3dDevice->SetRenderState(D3DRS_FOGENABLE, false);
    d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    return true;
}
//------------------------------------------------------------------------------
void xxEndCommandBufferD3D9(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;
    d3dDevice->EndScene();
}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferD3D9(uint64_t commandBuffer)
{
}
//==============================================================================
//  Render Pass
//==============================================================================
union D3DRENDERPASS9
{
    uint64_t value;
    struct
    {
        D3DCOLOR color;
        uint16_t depth;
        uint8_t stencil;
        uint8_t flags;
    };
};
//------------------------------------------------------------------------------
uint64_t xxCreateRenderPassD3D9(uint64_t device, float r, float g, float b, float a, float depth, unsigned char stencil)
{
    D3DRENDERPASS9 d3dRenderPass;

    d3dRenderPass.color = D3DCOLOR_COLORVALUE(r, g, b, a);
    d3dRenderPass.depth = (uint16_t)(depth * UINT16_MAX);
    d3dRenderPass.stencil = stencil;
    d3dRenderPass.flags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

    return d3dRenderPass.value;
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassD3D9(uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
bool xxBeginRenderPassD3D9(uint64_t commandBuffer, uint64_t renderPass)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return false;
    D3DRENDERPASS9 d3dRenderPass = { renderPass };

    float depth = d3dRenderPass.depth / (float)UINT16_MAX;
    HRESULT hResult = d3dDevice->Clear(0, nullptr, d3dRenderPass.flags, d3dRenderPass.color, depth, d3dRenderPass.stencil);
    return hResult == S_OK;
}
//------------------------------------------------------------------------------
void xxEndRenderPassD3D9(uint64_t commandBuffer, uint64_t renderPass)
{

}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateBufferD3D9(uint64_t device, unsigned int size, bool indexBuffer)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (indexBuffer)
    {
        LPDIRECT3DINDEXBUFFER9 d3dIndexBuffer = nullptr;
        HRESULT hResult = d3dDevice->CreateIndexBuffer(size, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &d3dIndexBuffer, nullptr);
        if (hResult != S_OK)
            return 0;
        return reinterpret_cast<uint64_t>(d3dIndexBuffer);
    }

    LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer = nullptr;
    HRESULT hResult = d3dDevice->CreateVertexBuffer(size, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &d3dVertexBuffer, nullptr);
    if (hResult != S_OK)
        return 0;
    return reinterpret_cast<uint64_t>(d3dVertexBuffer);
}
//------------------------------------------------------------------------------
void xxDestroyBufferD3D9(uint64_t buffer)
{
    LPDIRECT3DRESOURCE9 d3dResource = reinterpret_cast<LPDIRECT3DRESOURCE9>(buffer);
    if (d3dResource == nullptr)
        return;
    d3dResource->Release();
}
//------------------------------------------------------------------------------
void* xxMapBufferD3D9(uint64_t buffer)
{
    LPDIRECT3DRESOURCE9 d3dResource = reinterpret_cast<LPDIRECT3DRESOURCE9>(buffer);
    if (d3dResource == nullptr)
        return nullptr;

    LPDIRECT3DINDEXBUFFER9 d3dIndexBuffer = nullptr;
    if (d3dResource->QueryInterface(IID_PPV_ARGS(&d3dIndexBuffer)) == S_OK)
    {
        d3dIndexBuffer->Release();

        void* ptr = nullptr;
        HRESULT hResult = d3dIndexBuffer->Lock(0, 0, &ptr, D3DLOCK_NOOVERWRITE);
        if (hResult != S_OK)
            return nullptr;
        return ptr;
    }

    LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer = nullptr;
    if (d3dResource->QueryInterface(IID_PPV_ARGS(&d3dVertexBuffer)) == S_OK)
    {
        d3dVertexBuffer->Release();

        void* ptr = nullptr;
        HRESULT hResult = d3dVertexBuffer->Lock(0, 0, &ptr, D3DLOCK_NOOVERWRITE);
        if (hResult != S_OK)
            return nullptr;
        return ptr;
    }

    return nullptr;
}
//------------------------------------------------------------------------------
void xxUnmapBufferD3D9(uint64_t buffer)
{
    LPDIRECT3DRESOURCE9 d3dResource = reinterpret_cast<LPDIRECT3DRESOURCE9>(buffer);
    if (d3dResource == nullptr)
        return;

    LPDIRECT3DINDEXBUFFER9 d3dIndexBuffer = nullptr;
    if (d3dResource->QueryInterface(IID_PPV_ARGS(&d3dIndexBuffer)) == S_OK)
    {
        d3dIndexBuffer->Release();

        d3dIndexBuffer->Unlock();
    }

    LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer = nullptr;
    if (d3dResource->QueryInterface(IID_PPV_ARGS(&d3dVertexBuffer)) == S_OK)
    {
        d3dVertexBuffer->Release();

        d3dVertexBuffer->Unlock();
    }
}
//==============================================================================
//  Texture
//==============================================================================
uint64_t xxCreateTextureD3D9(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (width == 0 || height == 0 || depth == 0 || mipmap == 0 || array == 0)
        return 0;

    if (depth != 1 && array == 1)
    {
        LPDIRECT3DVOLUMETEXTURE9 d3dVolumeTexture = nullptr;
        HRESULT hResult = d3dDevice->CreateVolumeTexture(width, height, depth, mipmap, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3dVolumeTexture, nullptr);
        if (hResult != S_OK)
            return 0;
        return reinterpret_cast<uint64_t>(d3dVolumeTexture);
    }

    if (width == height && depth == 1 && array == 6)
    {
        LPDIRECT3DCUBETEXTURE9 d3dCubeTexture = nullptr;
        HRESULT hResult = d3dDevice->CreateCubeTexture(width, mipmap, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3dCubeTexture, nullptr);
        if (hResult != S_OK)
            return 0;
        return reinterpret_cast<uint64_t>(d3dCubeTexture);
    }

    if (depth == 1 && array == 1)
    {
        LPDIRECT3DTEXTURE9 d3dTexture = nullptr;
        HRESULT hResult = d3dDevice->CreateTexture(width, height, mipmap, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3dTexture, nullptr);
        if (hResult != S_OK)
            return 0;
        return reinterpret_cast<uint64_t>(d3dTexture);
    }

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyTextureD3D9(uint64_t texture)
{
    LPDIRECT3DBASETEXTURE9 d3dBaseTexture = reinterpret_cast<LPDIRECT3DBASETEXTURE9>(texture);
    if (d3dBaseTexture == nullptr)
        return;
    d3dBaseTexture->Release();
}
//------------------------------------------------------------------------------
void* xxMapTextureD3D9(uint64_t texture, unsigned int& stride, unsigned int mipmap, unsigned int array)
{
    LPDIRECT3DBASETEXTURE9 d3dBaseTexture = reinterpret_cast<LPDIRECT3DBASETEXTURE9>(texture);
    if (d3dBaseTexture == nullptr)
        return nullptr;

    LPDIRECT3DVOLUMETEXTURE9 d3dVolumeTexture = nullptr;
    if (d3dBaseTexture->QueryInterface(IID_PPV_ARGS(&d3dVolumeTexture)) == S_OK)
    {
        d3dVolumeTexture->Release();

        D3DLOCKED_BOX box = {};
        d3dVolumeTexture->LockBox(mipmap, &box, nullptr, D3DLOCK_NOSYSLOCK);
        stride = box.RowPitch;
        return box.pBits;
    }

    LPDIRECT3DCUBETEXTURE9 d3dCubeTexture = nullptr;
    if (d3dBaseTexture->QueryInterface(IID_PPV_ARGS(&d3dCubeTexture)) == S_OK)
    {
        d3dCubeTexture->Release();

        D3DLOCKED_RECT rect = {};
        d3dCubeTexture->LockRect((D3DCUBEMAP_FACES)array, mipmap, &rect, nullptr, D3DLOCK_NOSYSLOCK);
        stride = rect.Pitch;
        return rect.pBits;
    }

    LPDIRECT3DTEXTURE9 d3dTexture = nullptr;
    if (d3dBaseTexture->QueryInterface(IID_PPV_ARGS(&d3dTexture)) == S_OK)
    {
        d3dTexture->Release();

        D3DLOCKED_RECT rect = {};
        d3dTexture->LockRect(mipmap, &rect, nullptr, D3DLOCK_NOSYSLOCK);
        stride = rect.Pitch;
        return rect.pBits;
    }

    return nullptr;
}
//------------------------------------------------------------------------------
void xxUnmapTextureD3D9(uint64_t texture, unsigned int mipmap, unsigned int array)
{
    LPDIRECT3DBASETEXTURE9 d3dBaseTexture = reinterpret_cast<LPDIRECT3DBASETEXTURE9>(texture);
    if (d3dBaseTexture == nullptr)
        return;

    LPDIRECT3DVOLUMETEXTURE9 d3dVolumeTexture = nullptr;
    if (d3dBaseTexture->QueryInterface(IID_PPV_ARGS(&d3dVolumeTexture)) == S_OK)
    {
        d3dVolumeTexture->Release();

        d3dVolumeTexture->UnlockBox(mipmap);
        return;
    }

    LPDIRECT3DCUBETEXTURE9 d3dCubeTexture = nullptr;
    if (d3dBaseTexture->QueryInterface(IID_PPV_ARGS(&d3dCubeTexture)) == S_OK)
    {
        d3dCubeTexture->Release();

        d3dCubeTexture->UnlockRect((D3DCUBEMAP_FACES)array, mipmap);
        return;
    }

    LPDIRECT3DTEXTURE9 d3dTexture = nullptr;
    if (d3dBaseTexture->QueryInterface(IID_PPV_ARGS(&d3dTexture)) == S_OK)
    {
        d3dTexture->Release();

        d3dTexture->UnlockRect(mipmap);
        return;
    }
}
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t xxCreateVertexAttributeD3D9(uint64_t device, int count, ...)
{
    DWORD fvf = 0;

    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; ++i)
    {
        size_t offset = va_arg(args, size_t);
        int element = va_arg(args, int);
        size_t size = va_arg(args, size_t);

        if (offset == 0 && element == 3 && size == sizeof(float) * 3)
            fvf |= D3DFVF_XYZ;
        if (offset != 0 && element == 3 && size == sizeof(float) * 3)
            fvf |= D3DFVF_NORMAL;
        if (offset != 0 && element == 4 && size == sizeof(char) * 4)
            fvf |= D3DFVF_DIFFUSE;
        if (offset != 0 && element == 2 && size == sizeof(float) * 2)
            fvf += D3DFVF_TEX1;
    }
    va_end(args);

    return fvf;
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeD3D9(uint64_t vertexAttribute)
{

}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportD3D9(uint64_t commandBuffer, int x, int y, int width, int height, float minZ, float maxZ)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

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
void xxSetScissorD3D9(uint64_t commandBuffer, int x, int y, int width, int height)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    RECT rect;
    rect.left = x;
    rect.top = y;
    rect.right = x + width;
    rect.bottom = y + height;
    d3dDevice->SetScissorRect(&rect);
}
//------------------------------------------------------------------------------
void xxSetIndexBufferD3D9(uint64_t commandBuffer, uint64_t buffer)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;
    LPDIRECT3DINDEXBUFFER9 d3dIndexBuffer = reinterpret_cast<LPDIRECT3DINDEXBUFFER9>(buffer);
    if (d3dIndexBuffer == nullptr)
        return;

    d3dDevice->SetIndices(d3dIndexBuffer);
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersD3D9(uint64_t commandBuffer, const uint64_t* buffers, const int* offsets, const int* strides, int count)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    for (int i = 0; i < count; ++i)
    {
        LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER9>(buffers[i]);
        d3dDevice->SetStreamSource(i, d3dVertexBuffer, offsets[i], strides[i]);
    }
}
//------------------------------------------------------------------------------
void xxSetFragmentBuffersD3D9(uint64_t commandBuffer, const uint64_t* buffers, const int* offsets, const int* strides, int count)
{

}
//------------------------------------------------------------------------------
void xxSetVertexTexturesD3D9(uint64_t commandBuffer, const uint64_t* textures, int count)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    for (int i = 0; i < count; ++i)
    {
        LPDIRECT3DBASETEXTURE9 d3dBaseTexture = reinterpret_cast<LPDIRECT3DBASETEXTURE9>(textures[i]);
        d3dDevice->SetTexture(i + D3DVERTEXTEXTURESAMPLER0, d3dBaseTexture);
    }
}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesD3D9(uint64_t commandBuffer, const uint64_t* textures, int count)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    for (int i = 0; i < count; ++i)
    {
        LPDIRECT3DBASETEXTURE9 d3dBaseTexture = reinterpret_cast<LPDIRECT3DBASETEXTURE9>(textures[i]);
        d3dDevice->SetTexture(i, d3dBaseTexture);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexAttributeD3D9(uint64_t commandBuffer, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;
    DWORD fvf = (DWORD)vertexAttribute;

    d3dDevice->SetFVF(fvf);
}
//------------------------------------------------------------------------------
void xxDrawIndexedD3D9(uint64_t commandBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vertexOffset, 0, 0, firstIndex, indexCount / 3);
}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetOrthographicTransformD3D9(uint64_t commandBuffer, float left, float right, float top, float bottom)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    float L = left;
    float R = right;
    float T = top;
    float B = bottom;
    D3DMATRIX mat_identity = { { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } } };
    D3DMATRIX mat_projection =
    { { {
        2.0f/(R-L),   0.0f,         0.0f,  0.0f,
        0.0f,         2.0f/(T-B),   0.0f,  0.0f,
        0.0f,         0.0f,         0.5f,  0.0f,
        (L+R)/(L-R),  (T+B)/(B-T),  0.5f,  1.0f
    } } };
    d3dDevice->SetTransform(D3DTS_WORLD, &mat_identity);
    d3dDevice->SetTransform(D3DTS_VIEW, &mat_identity);
    d3dDevice->SetTransform(D3DTS_PROJECTION, &mat_projection);
}
//==============================================================================
