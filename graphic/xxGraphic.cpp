#include "xxGraphic.h"

#include <d3d9.h>
typedef LPDIRECT3D9 (WINAPI *PFN_DIRECT3D_CREATE9)(UINT);

static const wchar_t        g_dummy[] = L"xxGraphicDummyWindow";
static HMODULE              g_d3dLibrary = nullptr;
static HWND                 g_hWnd = nullptr;
static LPDIRECT3DSURFACE9   g_depthStencil = nullptr;

//------------------------------------------------------------------------------
//  Instance
//------------------------------------------------------------------------------
uint64_t xxCreateInstance()
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

    return reinterpret_cast<uint64_t>(d3d);
}
//------------------------------------------------------------------------------
void xxDestroyInstance(uint64_t instance)
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
//------------------------------------------------------------------------------
//  Device
//------------------------------------------------------------------------------
uint64_t xxCreateDevice(uint64_t instance)
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
    d3dPresentParameters.Windowed = TRUE;
    d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPresentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dPresentParameters.hDeviceWindow = g_hWnd;
    d3dPresentParameters.EnableAutoDepthStencil = FALSE;
    d3dPresentParameters.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
    d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    LPDIRECT3DDEVICE9 d3dDevice = nullptr;
    HRESULT hResult = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, nullptr, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dPresentParameters, &d3dDevice);
    if (hResult != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(d3dDevice);
}
//------------------------------------------------------------------------------
void xxDestroyDevice(uint64_t device)
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
//  Swapchain
//------------------------------------------------------------------------------
uint64_t xxCreateSwapchain(uint64_t device, void* view, unsigned int width, unsigned int height)
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
    d3dPresentParameters.Windowed = TRUE;
    d3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPresentParameters.BackBufferWidth = width;
    d3dPresentParameters.BackBufferHeight = height;
    d3dPresentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dPresentParameters.hDeviceWindow = hWnd;
    d3dPresentParameters.EnableAutoDepthStencil = FALSE;
    d3dPresentParameters.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
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
void xxDestroySwapchain(uint64_t swapchain)
{
    LPDIRECT3DSWAPCHAIN9 d3dSwapchain = reinterpret_cast<LPDIRECT3DSWAPCHAIN9>(swapchain);
    if (d3dSwapchain == nullptr)
        return;
    d3dSwapchain->Release();
}
//------------------------------------------------------------------------------
void xxPresentSwapchain(uint64_t swapchain, void* view)
{
    LPDIRECT3DSWAPCHAIN9 d3dSwapchain = reinterpret_cast<LPDIRECT3DSWAPCHAIN9>(swapchain);
    if (d3dSwapchain == nullptr)
        return;
    d3dSwapchain->Present(nullptr, nullptr, (HWND)view, nullptr, 0);
}
//------------------------------------------------------------------------------
//  CommandBuffer
//------------------------------------------------------------------------------
uint64_t xxGetCommandBuffer(uint64_t device, uint64_t swapchain)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;

    LPDIRECT3DSWAPCHAIN9 d3dSwapchain = reinterpret_cast<LPDIRECT3DSWAPCHAIN9>(swapchain);
    if (d3dSwapchain == nullptr)
        return 0;

    LPDIRECT3DSURFACE9 surface = NULL;
    d3dSwapchain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surface);
    d3dDevice->SetRenderTarget(0, surface);
    if (surface)
        surface->Release();
    d3dDevice->SetDepthStencilSurface(g_depthStencil);

    return device;
}
//------------------------------------------------------------------------------
bool xxBeginCommandBuffer(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return false;
    HRESULT hResult = d3dDevice->BeginScene();
    if (hResult != S_OK)
        return false;

    d3dDevice->SetPixelShader(NULL);
    d3dDevice->SetVertexShader(NULL);
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
void xxEndCommandBuffer(uint64_t commandBuffer)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;
    d3dDevice->EndScene();
}
//------------------------------------------------------------------------------
void xxSubmitCommandBuffer(uint64_t commandBuffer)
{
}
//------------------------------------------------------------------------------
//  RenderPass
//------------------------------------------------------------------------------
struct Direct3DRenderPass9
{
    DWORD flags;
    D3DCOLOR color;
    float depth;
    DWORD stencil;
};
//------------------------------------------------------------------------------
uint64_t xxCreateRenderPass(uint64_t device, float r, float g, float b, float a, float depth, unsigned int stencil)
{
    Direct3DRenderPass9* d3dRenderPass = new Direct3DRenderPass9;
    if (d3dRenderPass == nullptr)
        return 0;

    d3dRenderPass->flags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
    d3dRenderPass->color = D3DCOLOR_COLORVALUE(r, g, b, a);
    d3dRenderPass->depth = depth;
    d3dRenderPass->stencil = stencil;

    return reinterpret_cast<uint64_t>(d3dRenderPass);
}
//------------------------------------------------------------------------------
void xxDestroyRenderPass(uint64_t renderPass)
{
    Direct3DRenderPass9* d3dRenderPass = reinterpret_cast<Direct3DRenderPass9*>(renderPass);
    if (d3dRenderPass == nullptr)
        return;

    delete d3dRenderPass;
}
//------------------------------------------------------------------------------
bool xxBeginRenderPass(uint64_t commandBuffer, uint64_t renderPass)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return false;
    Direct3DRenderPass9* d3dRenderPass = reinterpret_cast<Direct3DRenderPass9*>(renderPass);
    if (d3dRenderPass == nullptr)
        return false;

    HRESULT hResult = d3dDevice->Clear(0, nullptr, d3dRenderPass->flags, d3dRenderPass->color, d3dRenderPass->depth, d3dRenderPass->stencil);
    return hResult == S_OK;
}
//------------------------------------------------------------------------------
void xxEndRenderPass(uint64_t commandBuffer, uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
//  Buffer
//------------------------------------------------------------------------------
uint64_t xxCreateBuffer(uint64_t device, unsigned int size, bool indexBuffer)
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
    HRESULT hResult = d3dDevice->CreateVertexBuffer(size, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_DEFAULT, &d3dVertexBuffer, nullptr);
    if (hResult != S_OK)
        return 0;
    return reinterpret_cast<uint64_t>(d3dVertexBuffer);
}
//------------------------------------------------------------------------------
void xxDestroyBuffer(uint64_t buffer)
{
    LPDIRECT3DRESOURCE9 d3dResource = reinterpret_cast<LPDIRECT3DRESOURCE9>(buffer);
    if (d3dResource == nullptr)
        return;
    d3dResource->Release();
}
//------------------------------------------------------------------------------
void* xxMapBuffer(uint64_t buffer)
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
void xxUnmapBuffer(uint64_t buffer)
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
//------------------------------------------------------------------------------
//  Texture
//------------------------------------------------------------------------------
uint64_t xxCreateTexture(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array)
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
void xxDestroyTexture(uint64_t texture)
{
    LPDIRECT3DBASETEXTURE9 d3dBaseTexture = reinterpret_cast<LPDIRECT3DBASETEXTURE9>(texture);
    if (d3dBaseTexture == nullptr)
        return;
    d3dBaseTexture->Release();
}
//------------------------------------------------------------------------------
void* xxMapTexture(uint64_t texture, unsigned int& stride, unsigned int mipmap, unsigned int array)
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
void xxUnmapTexture(uint64_t texture, unsigned int mipmap, unsigned int array)
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
//------------------------------------------------------------------------------
//  Command
//------------------------------------------------------------------------------
void xxSetViewport(uint64_t commandBuffer, int x, int y, int width, int height, float minZ, float maxZ)
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
void xxSetScissor(uint64_t commandBuffer, int x, int y, int width, int height)
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
void xxSetIndexBuffer(uint64_t commandBuffer, uint64_t buffer)
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
void xxSetVertexBuffers(uint64_t commandBuffer, const uint64_t* buffers, const int* offsets, const int* strides, int count)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    for (int i = 0; i < count; ++i)
    {
        LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER9>(buffers[i]);
        d3dDevice->SetStreamSource(i, d3dVertexBuffer, offsets[i], strides[i]);
    }

    d3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
}
//------------------------------------------------------------------------------
void xxSetFragmentBuffers(uint64_t commandBuffer, const uint64_t* buffers, const int* offsets, const int* strides, int count)
{

}
//------------------------------------------------------------------------------
void xxSetVertexTextures(uint64_t commandBuffer, const uint64_t* textures, int count)
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
void xxSetFragmentTextures(uint64_t commandBuffer, const uint64_t* textures, int count)
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
void xxDrawIndexed(uint64_t commandBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vertexOffset, 0, 0, firstIndex, indexCount / 3);
}
//------------------------------------------------------------------------------
//  Legacy
//------------------------------------------------------------------------------
void xxSetOrthographicTransform(uint64_t commandBuffer, float left, float right, float top, float bottom)
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
//------------------------------------------------------------------------------
