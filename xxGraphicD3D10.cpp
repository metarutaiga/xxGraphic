//==============================================================================
// xxGraphic : Direct3D 10.0 Source
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxSystem.h"
#include "dxsdk/d3d10.h"
#include "internal/xxGraphicInternalD3D.h"
#include "xxGraphicD3DAsm.h"
#include "xxGraphicD3D10.h"

typedef HRESULT (WINAPI *PFN_D3D10_CREATE_DEVICE)(IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, ID3D10Device**);
#ifndef _DEBUG
#define PERSISTENT_BUFFER           1
#endif
#define DXGI_FORMAT_B8G8R8A8_UNORM  g_unsupportedBGRA ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM

static void*                        g_d3dLibrary = nullptr;
static IDXGIFactory*                g_dxgiFactory = nullptr;
static bool                         g_unsupportedBGRA = false;
static bool                         g_unsupportedPersistentBuffer = false;

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D10()
{
    if (g_d3dLibrary == nullptr)
        g_d3dLibrary = xxLoadLibrary("d3d10.dll");
    if (g_d3dLibrary == nullptr)
        return 0;

    xxRegisterFunction(D3D10);

    return reinterpret_cast<uint64_t>(g_d3dLibrary);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceD3D10(uint64_t instance)
{
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
uint64_t xxCreateDeviceD3D10(uint64_t instance)
{
    HMODULE d3d = reinterpret_cast<HMODULE>(instance);
    if (d3d == nullptr)
        return 0;

    PFN_D3D10_CREATE_DEVICE D3D10CreateDevice;
    (void*&)D3D10CreateDevice = GetProcAddress(d3d, "D3D10CreateDevice");
    if (D3D10CreateDevice == nullptr)
        return 0;

    UINT flags = D3D10_CREATE_DEVICE_ALLOW_NULL_FROM_MAP;
#if defined(_DEBUG)
    flags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
    ID3D10Device* d3dDevice = nullptr;
    HRESULT hResult = D3D10CreateDevice(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr, flags, D3D10_SDK_VERSION, &d3dDevice);
    if (hResult != S_OK)
        return 0;

    IUnknown* unknown = nullptr;
    xxLocalBreak()
    {
        if (d3dDevice->QueryInterface(__uuidof(ID3D10Device1*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "10.1", xxGetInstanceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(ID3D10Device*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "10.0", xxGetInstanceName());
            break;
        }
    }
    SafeRelease(unknown);

    g_unsupportedBGRA = true;
    g_unsupportedPersistentBuffer = GetFileAttributesA("C:\\.windows-serial") != INVALID_FILE_ATTRIBUTES;

    return reinterpret_cast<uint64_t>(d3dDevice);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceD3D10(uint64_t device)
{
    g_unsupportedBGRA = false;

    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);

    SafeRelease(g_dxgiFactory);
    SafeRelease(d3dDevice);
}
//------------------------------------------------------------------------------
bool xxResetDeviceD3D10(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
bool xxTestDeviceD3D10(uint64_t device)
{
    return true;
}
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t xxCreateSwapchainD3D10(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3D10SWAPCHAIN* d3dOldSwapchain = reinterpret_cast<D3D10SWAPCHAIN*>(oldSwapchain);
    if (d3dOldSwapchain && d3dOldSwapchain->hWnd == view && d3dOldSwapchain->width == width && d3dOldSwapchain->height == height)
        return oldSwapchain;
    D3D10SWAPCHAIN* d3dSwapchain = xxAlloc(D3D10SWAPCHAIN);
    if (d3dSwapchain == nullptr)
        return 0;

    xxDestroySwapchainD3D10(oldSwapchain);

    if (g_dxgiFactory == nullptr)
    {
        IDXGIDevice* dxgiDevice = nullptr;
        if (d3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice)) == S_OK)
        {
            IDXGIAdapter* dxgiAdapter = nullptr;
            if (dxgiDevice->GetParent(IID_PPV_ARGS(&dxgiAdapter)) == S_OK)
            {
                IDXGIFactory* dxgiFactory = nullptr;
                if (dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)) == S_OK)
                {
                    g_dxgiFactory = dxgiFactory;
                }
                dxgiAdapter->Release();
            }
            dxgiDevice->Release();
        }
        if (g_dxgiFactory == nullptr)
        {
            xxFree(d3dSwapchain);
            return 0;
        }
    }

    HWND hWnd = (HWND)view;

    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;
    desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = NUM_BACK_BUFFERS;
    desc.OutputWindow = hWnd;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    IDXGISwapChain* dxgiSwapchain = nullptr;
    if (dxgiSwapchain == nullptr)
    {
        HRESULT hResult = g_dxgiFactory->CreateSwapChain(d3dDevice, &desc, &dxgiSwapchain);
        if (hResult != S_OK)
        {
            desc.BufferCount = 1;
            desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            HRESULT hResult = g_dxgiFactory->CreateSwapChain(d3dDevice, &desc, &dxgiSwapchain);
            if (hResult != S_OK)
            {
                xxFree(d3dSwapchain);
                return 0;
            }
        }
    }

    ID3D10RenderTargetView* renderTargetView = nullptr;
    if (renderTargetView == nullptr)
    {
        ID3D10Texture2D* d3dTexture = nullptr;
        dxgiSwapchain->GetBuffer(0, IID_PPV_ARGS(&d3dTexture));
        if (d3dTexture)
        {
            d3dTexture->Release();

            HRESULT hResult = d3dDevice->CreateRenderTargetView(d3dTexture, nullptr, &renderTargetView);
            if (hResult != S_OK)
            {
            }
        }
    }

    ID3D10Texture2D* depthStencilTexture = nullptr;
    if (depthStencilTexture == nullptr)
    {
        D3D10_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;

        HRESULT hResult = d3dDevice->CreateTexture2D(&textureDesc, nullptr, &depthStencilTexture);
        if (hResult != S_OK)
        {
        }
    }

    ID3D10DepthStencilView* depthStencilView = nullptr;
    if (depthStencilView == nullptr)
    {
        D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
        depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;

        HRESULT hResult = d3dDevice->CreateDepthStencilView(depthStencilTexture, &depthStencilViewDesc, &depthStencilView);
        if (hResult != S_OK)
        {
        }
    }

    d3dSwapchain->renderTargetView = renderTargetView;
    d3dSwapchain->depthStencilView = depthStencilView;
    d3dSwapchain->dxgiSwapchain = dxgiSwapchain;
    d3dSwapchain->depthStencilTexture = depthStencilTexture;
    d3dSwapchain->hWnd = hWnd;
    d3dSwapchain->width = width;
    d3dSwapchain->height = height;

    return reinterpret_cast<uint64_t>(d3dSwapchain);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainD3D10(uint64_t swapchain)
{
    D3D10SWAPCHAIN* d3dSwapchain = reinterpret_cast<D3D10SWAPCHAIN*>(swapchain);
    if (d3dSwapchain == nullptr)
        return;

    SafeRelease(d3dSwapchain->renderTargetView);
    SafeRelease(d3dSwapchain->depthStencilView);
    SafeRelease(d3dSwapchain->dxgiSwapchain);
    SafeRelease(d3dSwapchain->depthStencilTexture);
    xxFree(d3dSwapchain);
}
//------------------------------------------------------------------------------
void xxPresentSwapchainD3D10(uint64_t swapchain)
{
    D3D10SWAPCHAIN* d3dSwapchain = reinterpret_cast<D3D10SWAPCHAIN*>(swapchain);
    if (d3dSwapchain == nullptr)
        return;

    d3dSwapchain->dxgiSwapchain->Present(0, 0);
}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferD3D10(uint64_t device, uint64_t swapchain)
{
    return device;
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferD3D10(uint64_t device, uint64_t swapchain, float* scale)
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
bool xxBeginCommandBufferD3D10(uint64_t commandBuffer)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandBuffer);
    if (d3dDevice == nullptr)
        return false;

    return true;
}
//------------------------------------------------------------------------------
void xxEndCommandBufferD3D10(uint64_t commandBuffer)
{

}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferD3D10(uint64_t commandBuffer, uint64_t swapchain)
{
}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxCreateRenderPassD3D10(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
{
    D3D10RENDERPASS d3dRenderPass = {};

    if (clearColor)
        d3dRenderPass.clearColor = true;
    if (clearDepth)
        d3dRenderPass.clearDepthStencil |= D3D10_CLEAR_DEPTH;
    if (clearStencil)
        d3dRenderPass.clearDepthStencil |= D3D10_CLEAR_STENCIL;

    return static_cast<uint64_t>(d3dRenderPass.value);
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassD3D10(uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassD3D10(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandBuffer);
    if (d3dDevice == nullptr)
        return 0;
    D3D10FRAMEBUFFER* d3dFramebuffer = reinterpret_cast<D3D10FRAMEBUFFER*>(framebuffer);
    if (d3dFramebuffer == nullptr)
        return 0;
    D3D10RENDERPASS d3dRenderPass = { renderPass };

    d3dDevice->OMSetRenderTargets(1, &d3dFramebuffer->renderTargetView, d3dFramebuffer->depthStencilView);

    D3D10_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = width;
    vp.Height = height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    d3dDevice->RSSetViewports(1, &vp);

    D3D10_RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    d3dDevice->RSSetScissorRects(1, &rect);

    if (d3dRenderPass.clearColor)
    {
        d3dDevice->ClearRenderTargetView(d3dFramebuffer->renderTargetView, color);
    }
    if (d3dRenderPass.clearDepthStencil)
    {
        d3dDevice->ClearDepthStencilView(d3dFramebuffer->depthStencilView, d3dRenderPass.clearDepthStencil, depth, stencil);
    }

    return commandBuffer;
}
//------------------------------------------------------------------------------
void xxEndRenderPassD3D10(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{

}
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t xxCreateVertexAttributeD3D10(uint64_t device, int count, int* attribute)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3D10VERTEXATTRIBUTE* d3dVertexAttribute = xxAlloc(D3D10VERTEXATTRIBUTE);
    if (d3dVertexAttribute == nullptr)
        return 0;

    D3D10_INPUT_ELEMENT_DESC inputElements[16];
    BYTE xyzIndex = 0;
    BYTE textureIndex = 0;
    int stride = 0;

    for (int i = 0; i < count; ++i)
    {
        int stream = (*attribute++);
        int offset = (*attribute++);
        int element = (*attribute++);
        int size = (*attribute++);

        stride += size;

        D3D10_INPUT_ELEMENT_DESC& inputElement = inputElements[i];
        inputElement.SemanticName;
        inputElement.SemanticIndex;
        inputElement.Format;
        inputElement.InputSlot = stream;
        inputElement.AlignedByteOffset = offset;
        inputElement.InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
        inputElement.InstanceDataStepRate = 0;

        if (element == 3 && size == sizeof(float) * 3)
        {
            inputElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            switch (xyzIndex)
            {
            case 0:
                inputElement.SemanticName = "POSITION";
                inputElement.SemanticIndex = 0;
                break;
            case 1:
                inputElement.SemanticName = "NORMAL";
                inputElement.SemanticIndex = 0;
                break;
            case 2:
                inputElement.SemanticName = "BINORMAL";
                inputElement.SemanticIndex = 0;
                break;
            case 3:
                inputElement.SemanticName = "TANGENT";
                inputElement.SemanticIndex = 0;
                break;
            default:
                break;
            }
            xyzIndex++;
            continue;
        }

        if (element == 4 && size == sizeof(char) * 4)
        {
            inputElement.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            inputElement.SemanticName = "COLOR";
            inputElement.SemanticIndex = 0;
            continue;
        }

        if (element == 2 && size == sizeof(float) * 2)
        {
            inputElement.Format = DXGI_FORMAT_R32G32_FLOAT;
            inputElement.SemanticName = "TEXCOORD";
            inputElement.SemanticIndex = textureIndex;
            textureIndex++;
            continue;
        }
    }

    DWORD dxbc[256] = {};
    DWORD dxbcChunkCount = 1;
    DWORD dxbcCount = 8 + dxbcChunkCount;
    DWORD inputSignatureCount = 4 + (6 * count) + (4 * count);
    DWORD dxbcSize = (dxbcCount + inputSignatureCount) * sizeof(DWORD);
    DWORD* dxbcChunk = dxbc + 8;
    DWORD* inputSignatureHeader = dxbc + dxbcCount;
    DWORD* inputSignature = inputSignatureHeader + 4;
    char* inputSignatureName = (char*)(inputSignature + (6 * count));

    dxbc[0] = *(DWORD*)"DXBC";
    dxbc[5] = 1;
    dxbc[6] = dxbcSize;
    dxbc[7] = dxbcChunkCount;

    dxbcChunk[0] = (uint32_t)(inputSignatureHeader - dxbc) * sizeof(DWORD);

    inputSignatureHeader[0] = *(DWORD*)"ISGN";
    inputSignatureHeader[1] = (inputSignatureCount - 2) * sizeof(DWORD);
    inputSignatureHeader[2] = count;
    inputSignatureHeader[3] = 8;
    for (int i = 0; i < count; ++i)
    {
        const D3D10_INPUT_ELEMENT_DESC& inputElement = inputElements[i];
        inputSignature[0] = (DWORD)(inputSignatureName - (char*)inputSignatureHeader - 8);
        inputSignature[1] = inputElement.SemanticIndex;
        inputSignature[2] = 0;
        inputSignature[3] = 3;
        inputSignature[4] = i;
        switch (inputElement.Format)
        {
        case DXGI_FORMAT_R32_FLOAT:
            inputSignature[5] = 0x0101;
            break;
        case DXGI_FORMAT_R32G32_FLOAT:
            inputSignature[5] = 0x0303;
            break;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            inputSignature[5] = 0x0707;
            break;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        default:
            inputSignature[5] = 0x0F0F;
            break;
        }
        strcpy(inputSignatureName, inputElement.SemanticName);
        inputSignature += 6;
        inputSignatureName += strlen(inputSignatureName) + 1;
    }
    xxDXBCChecksum(dxbc, dxbcSize, (uint8_t*)&dxbc[1]);

    ID3D10InputLayout* inputLayout = nullptr;
    HRESULT hResult = d3dDevice->CreateInputLayout(inputElements, count, dxbc, dxbcSize, &inputLayout);
    if (hResult != S_OK)
    {
        xxFree(d3dVertexAttribute);
        return 0;
    }
    d3dVertexAttribute->inputLayout = inputLayout;
    d3dVertexAttribute->stride = stride;

    return reinterpret_cast<uint64_t>(d3dVertexAttribute);
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeD3D10(uint64_t vertexAttribute)
{
    D3D10VERTEXATTRIBUTE* d3dVertexAttribute = reinterpret_cast<D3D10VERTEXATTRIBUTE*>(vertexAttribute);
    if (d3dVertexAttribute == nullptr)
        return;

    d3dVertexAttribute->inputLayout->Release();
    xxFree(d3dVertexAttribute);
}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateConstantBufferD3D10(uint64_t device, int size)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3D10BUFFER* d3dBuffer = xxAlloc(D3D10BUFFER);
    if (d3dBuffer == nullptr)
        return 0;

    D3D10_BUFFER_DESC desc = {};
    desc.ByteWidth = size;
    desc.Usage = D3D10_USAGE_DYNAMIC;
    desc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

    ID3D10Buffer* buffer = nullptr;
    HRESULT hResult = d3dDevice->CreateBuffer(&desc, nullptr, &buffer);
    if (hResult != S_OK)
        return 0;

    d3dBuffer->buffer = buffer;
    d3dBuffer->size = size;
    d3dBuffer->map = D3D10_MAP_WRITE_NO_OVERWRITE;
    d3dBuffer->address = nullptr;

    return reinterpret_cast<uint64_t>(d3dBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferD3D10(uint64_t device, int size)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3D10BUFFER* d3dBuffer = xxAlloc(D3D10BUFFER);
    if (d3dBuffer == nullptr)
        return 0;

    D3D10_BUFFER_DESC desc = {};
    desc.ByteWidth = size;
    desc.Usage = D3D10_USAGE_DYNAMIC;
    desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

    ID3D10Buffer* buffer = nullptr;
    HRESULT hResult = d3dDevice->CreateBuffer(&desc, nullptr, &buffer);
    if (hResult != S_OK)
        return 0;

    d3dBuffer->buffer = buffer;
    d3dBuffer->size = size;
    d3dBuffer->map = D3D10_MAP_WRITE_NO_OVERWRITE;
    d3dBuffer->address = nullptr;

    return reinterpret_cast<uint64_t>(d3dBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferD3D10(uint64_t device, int size, uint64_t vertexAttribute)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3D10BUFFER* d3dBuffer = xxAlloc(D3D10BUFFER);
    if (d3dBuffer == nullptr)
        return 0;

    D3D10_BUFFER_DESC desc = {};
    desc.ByteWidth = size;
    desc.Usage = D3D10_USAGE_DYNAMIC;
    desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

    ID3D10Buffer* buffer = nullptr;
    HRESULT hResult = d3dDevice->CreateBuffer(&desc, nullptr, &buffer);
    if (hResult != S_OK)
        return 0;

    d3dBuffer->buffer = buffer;
    d3dBuffer->size = size;
    d3dBuffer->map = D3D10_MAP_WRITE_NO_OVERWRITE;
    d3dBuffer->address = nullptr;

    return reinterpret_cast<uint64_t>(d3dBuffer);
}
//------------------------------------------------------------------------------
void xxDestroyBufferD3D10(uint64_t device, uint64_t buffer)
{
    D3D10BUFFER* d3dBuffer = reinterpret_cast<D3D10BUFFER*>(buffer);
    if (d3dBuffer == nullptr)
        return;

    if (d3dBuffer->buffer)
    {
        if (d3dBuffer->address)
        {
            d3dBuffer->buffer->Unmap();
        }
        d3dBuffer->buffer->Release();
    }
    xxFree(d3dBuffer);
}
//------------------------------------------------------------------------------
void* xxMapBufferD3D10(uint64_t device, uint64_t buffer)
{
    D3D10BUFFER* d3dBuffer = reinterpret_cast<D3D10BUFFER*>(buffer);
    if (d3dBuffer == nullptr)
        return nullptr;
    if (d3dBuffer->address)
        return d3dBuffer->address;

    void* ptr = nullptr;
    HRESULT hResult = d3dBuffer->buffer->Map(d3dBuffer->map, 0, &ptr);
    if (hResult != S_OK && d3dBuffer->map == D3D10_MAP_WRITE_NO_OVERWRITE)
    {
        d3dBuffer->map = D3D10_MAP_WRITE_DISCARD;
        hResult = d3dBuffer->buffer->Map(d3dBuffer->map, 0, &ptr);
    }
    if (hResult != S_OK)
    {
        return nullptr;
    }
#if PERSISTENT_BUFFER
    if (d3dBuffer->map == D3D10_MAP_WRITE_NO_OVERWRITE && g_unsupportedPersistentBuffer == false)
    {
        d3dBuffer->address = ptr;
    }
#endif

    return ptr;
}
//------------------------------------------------------------------------------
void xxUnmapBufferD3D10(uint64_t device, uint64_t buffer)
{
    D3D10BUFFER* d3dBuffer = reinterpret_cast<D3D10BUFFER*>(buffer);
    if (d3dBuffer == nullptr)
        return;
    if (d3dBuffer->address)
        return;

    d3dBuffer->buffer->Unmap();
}
//==============================================================================
//  Texture
//==============================================================================
uint64_t xxCreateTextureD3D10(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, void const* external)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (width == 0 || height == 0 || depth == 0 || mipmap == 0 || array == 0)
        return 0;

    D3D10TEXTURE* d3dTexture = xxAlloc(D3D10TEXTURE);
    if (d3dTexture == nullptr)
        return 0;

    D3D10_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
    ID3D10Resource* d3dTextureGPU = nullptr;
    ID3D10Resource* d3dTextureCPU = nullptr;

    if (external)
    {
        ID3D10Texture2D* d3dTexture2D = nullptr;

        IUnknown* unknown = (IUnknown*)external;
        HRESULT hResult = unknown->QueryInterface(__uuidof(ID3D10Texture2D), (void**)&d3dTexture2D);
        if (hResult != S_OK)
        {
            xxFree(d3dTexture);
            return 0;
        }
        d3dTextureGPU = d3dTexture2D;

        D3D10_TEXTURE2D_DESC texture2DDesc = {};
        d3dTexture2D->GetDesc(&texture2DDesc);

        viewDesc.Format = texture2DDesc.Format;
        viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
        viewDesc.Texture2D.MipLevels = mipmap;
    }
    else if (depth == 1 && array == 1)
    {
        D3D10_TEXTURE2D_DESC texture2DDesc = {};
        texture2DDesc.Width = width;
        texture2DDesc.Height = height;
        texture2DDesc.MipLevels = mipmap;
        texture2DDesc.ArraySize = array;
        texture2DDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        texture2DDesc.SampleDesc.Count = 1;
        texture2DDesc.Usage = mipmap > 1 ? D3D10_USAGE_DEFAULT : D3D10_USAGE_DYNAMIC;
        texture2DDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
        texture2DDesc.CPUAccessFlags = mipmap > 1 ? 0 : D3D10_CPU_ACCESS_WRITE;

        HRESULT hResult = d3dDevice->CreateTexture2D(&texture2DDesc, nullptr, (ID3D10Texture2D**)&d3dTextureGPU);
        if (hResult != S_OK)
        {
            xxFree(d3dTexture);
            return 0;
        }
        if (mipmap > 1)
        {
            texture2DDesc.Usage = D3D10_USAGE_STAGING;
            texture2DDesc.BindFlags = 0;
            texture2DDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
            hResult = d3dDevice->CreateTexture2D(&texture2DDesc, nullptr, (ID3D10Texture2D**)&d3dTextureCPU);
            if (hResult != S_OK)
            {
                SafeRelease(d3dTextureGPU);
                xxFree(d3dTexture);
                return 0;
            }
        }
        else
        {
            d3dTextureCPU = d3dTextureGPU;
            d3dTextureCPU->AddRef();
        }

        viewDesc.Format = texture2DDesc.Format;
        viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
        viewDesc.Texture2D.MipLevels = mipmap;
    }
    else if (width == height && depth == 1 && array == 6)
    {
        D3D10_TEXTURE2D_DESC texture2DDesc = {};
        texture2DDesc.Width = width;
        texture2DDesc.Height = height;
        texture2DDesc.MipLevels = mipmap;
        texture2DDesc.ArraySize = array;
        texture2DDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        texture2DDesc.SampleDesc.Count = 1;
        texture2DDesc.Usage = mipmap > 1 ? D3D10_USAGE_DEFAULT : D3D10_USAGE_DYNAMIC;
        texture2DDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
        texture2DDesc.CPUAccessFlags = mipmap > 1 ? 0 : D3D10_CPU_ACCESS_WRITE;
        texture2DDesc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;

        HRESULT hResult = d3dDevice->CreateTexture2D(&texture2DDesc, nullptr, (ID3D10Texture2D**)&d3dTextureGPU);
        if (hResult != S_OK)
        {
            xxFree(d3dTexture);
            return 0;
        }
        if (mipmap > 1)
        {
            texture2DDesc.Usage = D3D10_USAGE_STAGING;
            texture2DDesc.BindFlags = 0;
            texture2DDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
            hResult = d3dDevice->CreateTexture2D(&texture2DDesc, nullptr, (ID3D10Texture2D**)&d3dTextureCPU);
            if (hResult != S_OK)
            {
                SafeRelease(d3dTextureGPU);
                xxFree(d3dTexture);
                return 0;
            }
        }
        else
        {
            d3dTextureCPU = d3dTextureGPU;
            d3dTextureCPU->AddRef();
        }

        viewDesc.Format = texture2DDesc.Format;
        viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
        viewDesc.Texture2D.MipLevels = mipmap;
    }
    else if (depth != 1 && array == 1)
    {
        D3D10_TEXTURE3D_DESC texture3DDesc = {};
        texture3DDesc.Width = width;
        texture3DDesc.Height = height;
        texture3DDesc.Depth = depth;
        texture3DDesc.MipLevels = mipmap;
        texture3DDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        texture3DDesc.Usage = mipmap > 1 ? D3D10_USAGE_DEFAULT : D3D10_USAGE_DYNAMIC;
        texture3DDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
        texture3DDesc.CPUAccessFlags = mipmap > 1 ? 0 : D3D10_CPU_ACCESS_WRITE;

        HRESULT hResult = d3dDevice->CreateTexture3D(&texture3DDesc, nullptr, (ID3D10Texture3D**)&d3dTextureGPU);
        if (hResult != S_OK)
        {
            xxFree(d3dTexture);
            return 0;
        }
        if (mipmap > 1)
        {
            texture3DDesc.Usage = D3D10_USAGE_STAGING;
            texture3DDesc.BindFlags = 0;
            texture3DDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
            hResult = d3dDevice->CreateTexture3D(&texture3DDesc, nullptr, (ID3D10Texture3D**)&d3dTextureCPU);
            if (hResult != S_OK)
            {
                SafeRelease(d3dTextureGPU);
                xxFree(d3dTexture);
                return 0;
            }
        }
        else
        {
            d3dTextureCPU = d3dTextureGPU;
            d3dTextureCPU->AddRef();
        }

        viewDesc.Format = texture3DDesc.Format;
        viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE3D;
        viewDesc.Texture3D.MipLevels = mipmap;
    }

    if (d3dTextureGPU == nullptr)
    {
        xxFree(d3dTexture);
        return 0;
    }

    ID3D10ShaderResourceView* d3dView = nullptr;
    HRESULT hResult = d3dDevice->CreateShaderResourceView(d3dTextureGPU, &viewDesc, &d3dView);
    if (hResult != S_OK)
    {
        SafeRelease(d3dTextureGPU);
        SafeRelease(d3dTextureCPU);
        xxFree(d3dTexture);
        return 0;
    }

    d3dTexture->resourceView = d3dView;
    d3dTexture->textureGPU = d3dTextureGPU;
    d3dTexture->textureCPU = d3dTextureCPU;
    d3dTexture->mipmap = mipmap;

    return reinterpret_cast<uint64_t>(d3dTexture);
}
//------------------------------------------------------------------------------
void xxDestroyTextureD3D10(uint64_t texture)
{
    D3D10TEXTURE* d3dTexture = reinterpret_cast<D3D10TEXTURE*>(texture);
    if (d3dTexture == nullptr)
        return;

    SafeRelease(d3dTexture->resourceView);
    SafeRelease(d3dTexture->textureGPU);
    SafeRelease(d3dTexture->textureCPU);
    xxFree(d3dTexture);
}
//------------------------------------------------------------------------------
void* xxMapTextureD3D10(uint64_t device, uint64_t texture, int* stride, int level, int array)
{
    D3D10TEXTURE* d3dTexture = reinterpret_cast<D3D10TEXTURE*>(texture);
    if (d3dTexture == nullptr)
        return nullptr;

    ID3D10Texture3D* texture3D = (ID3D10Texture3D*)d3dTexture->textureCPU;
    if (texture3D)
    {
        UINT subresource = D3D10CalcSubresource(level, array, d3dTexture->mipmap);
        D3D10_MAP mapType = (d3dTexture->textureCPU == d3dTexture->textureGPU) ? D3D10_MAP_WRITE_DISCARD : D3D10_MAP_WRITE;
        D3D10_MAPPED_TEXTURE3D mappedTexture3D = {};
        HRESULT hResult = texture3D->Map(subresource, mapType, 0, &mappedTexture3D);
        if (hResult != S_OK)
            return nullptr;

        (*stride) = mappedTexture3D.RowPitch;
        return mappedTexture3D.pData;
    }

    return nullptr;
}
//------------------------------------------------------------------------------
void xxUnmapTextureD3D10(uint64_t device, uint64_t texture, int level, int array)
{
    D3D10TEXTURE* d3dTexture = reinterpret_cast<D3D10TEXTURE*>(texture);
    if (d3dTexture == nullptr)
        return;

    ID3D10Texture3D* texture3D = (ID3D10Texture3D*)d3dTexture->textureCPU;
    if (texture3D)
    {
        UINT subresource = D3D10CalcSubresource(level, array, d3dTexture->mipmap);
        texture3D->Unmap(subresource);
        if (d3dTexture->textureCPU != d3dTexture->textureGPU)
        {
            ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);
            if (d3dDevice == nullptr)
                return;
            d3dDevice->CopySubresourceRegion(d3dTexture->textureGPU, subresource, 0, 0, 0, d3dTexture->textureCPU, subresource, nullptr);
        }
    }
}
//==============================================================================
//  Sampler
//==============================================================================
uint64_t xxCreateSamplerD3D10(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);
    if (d3dDevice == nullptr)
        return 0;

    D3D10_SAMPLER_DESC desc = {};
    desc.Filter = D3D10_ENCODE_BASIC_FILTER(linearMin ? D3D10_FILTER_TYPE_LINEAR : D3D10_FILTER_TYPE_POINT,
                                            linearMag ? D3D10_FILTER_TYPE_LINEAR : D3D10_FILTER_TYPE_POINT,
                                            linearMip ? D3D10_FILTER_TYPE_LINEAR : D3D10_FILTER_TYPE_POINT, FALSE);
    desc.AddressU = clampU ? D3D10_TEXTURE_ADDRESS_CLAMP : D3D10_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = clampV ? D3D10_TEXTURE_ADDRESS_CLAMP : D3D10_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = clampW ? D3D10_TEXTURE_ADDRESS_CLAMP : D3D10_TEXTURE_ADDRESS_WRAP;
    desc.ComparisonFunc = D3D10_COMPARISON_NEVER;
    desc.MaxLOD = D3D10_FLOAT32_MAX;
    if (anisotropy > 1)
    {
        desc.Filter = D3D10_ENCODE_ANISOTROPIC_FILTER(FALSE);
        desc.MaxAnisotropy = anisotropy;
    }

    ID3D10SamplerState* d3dSamplerState = nullptr;
    HRESULT hResult = d3dDevice->CreateSamplerState(&desc, &d3dSamplerState);
    if (hResult != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(d3dSamplerState);
}
//------------------------------------------------------------------------------
void xxDestroySamplerD3D10(uint64_t sampler)
{
    ID3D10SamplerState* d3dSamplerState = reinterpret_cast<ID3D10SamplerState*>(sampler);

    SafeRelease(d3dSamplerState);
}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderD3D10(uint64_t device, char const* shader, uint64_t vertexAttribute)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);
    if (d3dDevice == nullptr)
        return 0;

    ID3D10VertexShader* d3dShader = nullptr;
    if (strcmp(shader, "default") == 0)
    {
        HRESULT hResult = d3dDevice->CreateVertexShader(vertexShaderCode40, vertexShaderCode40[6], &d3dShader);
        if (hResult != S_OK)
            return 0;

        return reinterpret_cast<uint64_t>(d3dShader);
    }
    else
    {
        ID3D10Blob* blob = CreateD3D10Shader(shader, "VSMain", "vs_4_0");
        if (blob == nullptr)
            return 0;

        HRESULT hResult = d3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), &d3dShader);
        blob->Release();
        if (hResult != S_OK)
            return 0;

        return reinterpret_cast<uint64_t>(d3dShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderD3D10(uint64_t device, char const* shader)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);
    if (d3dDevice == nullptr)
        return 0;

    ID3D10PixelShader* d3dShader = nullptr;
    if (strcmp(shader, "default") == 0)
    {
        HRESULT hResult = d3dDevice->CreatePixelShader(pixelShaderCode40, pixelShaderCode40[6], &d3dShader);
        if (hResult != S_OK)
            return 0;

        return reinterpret_cast<uint64_t>(d3dShader);
    }
    else
    {
        ID3D10Blob* blob = CreateD3D10Shader(shader, "FSMain", "ps_4_0");
        if (blob == nullptr)
            return 0;

        HRESULT hResult = d3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), &d3dShader);
        blob->Release();
        if (hResult != S_OK)
            return 0;

        return reinterpret_cast<uint64_t>(d3dShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderD3D10(uint64_t device, uint64_t shader)
{
    IUnknown* d3dShader = reinterpret_cast<IUnknown*>(shader);

    SafeRelease(d3dShader);
}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreateBlendStateD3D10(uint64_t device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);
    if (d3dDevice == nullptr)
        return 0;

    D3D10_BLEND_DESC desc = {};
    desc.SrcBlend = d3d10BlendFactor(sourceColor);
    desc.DestBlend = d3d10BlendFactor(destinationColor);
    desc.BlendOp = d3d10BlendOp(operationColor);
    desc.SrcBlendAlpha = d3d10BlendFactor(sourceAlpha);
    desc.DestBlendAlpha = d3d10BlendFactor(destinationAlpha);
    desc.BlendOpAlpha = d3d10BlendOp(operationAlpha);
    desc.BlendEnable[0] = (desc.SrcBlend != D3D10_BLEND_ONE ||
                           desc.DestBlend != D3D10_BLEND_ZERO ||
                           desc.BlendOp != D3D10_BLEND_OP_ADD ||
                           desc.SrcBlendAlpha != D3D10_BLEND_ONE ||
                           desc.DestBlendAlpha != D3D10_BLEND_ZERO ||
                           desc.BlendOpAlpha != D3D10_BLEND_OP_ADD) ? TRUE : FALSE;
    desc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

    ID3D10BlendState* d3dBlendState = nullptr;
    HRESULT hResult = d3dDevice->CreateBlendState(&desc, &d3dBlendState);
    if (hResult != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(d3dBlendState);
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateD3D10(uint64_t device, char const* depthTest, bool depthWrite)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);
    if (d3dDevice == nullptr)
        return 0;

    D3D10_DEPTH_STENCIL_DESC desc = {};
    desc.DepthFunc = d3d10CompareOp(depthTest);
    desc.DepthEnable = (desc.DepthFunc != D3D10_COMPARISON_ALWAYS) ? TRUE : FALSE;
    desc.DepthWriteMask = depthWrite ? D3D10_DEPTH_WRITE_MASK_ALL : D3D10_DEPTH_WRITE_MASK_ZERO;
    desc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
    desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    desc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
    desc.BackFace = desc.FrontFace;

    ID3D10DepthStencilState* d3dDepthStencilState = nullptr;
    HRESULT hResult = d3dDevice->CreateDepthStencilState(&desc, &d3dDepthStencilState);
    if (hResult != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(d3dDepthStencilState);
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateD3D10(uint64_t device, bool cull, bool scissor)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(device);
    if (d3dDevice == nullptr)
        return 0;

    D3D10_RASTERIZER_DESC desc = {};
    desc.FillMode = D3D10_FILL_SOLID;
    desc.CullMode = cull ? D3D10_CULL_BACK : D3D10_CULL_NONE;
    desc.ScissorEnable = scissor;
    desc.DepthClipEnable = true;

    ID3D10RasterizerState* d3dRasterizerState = nullptr;
    HRESULT hResult = d3dDevice->CreateRasterizerState(&desc, &d3dRasterizerState);
    if (hResult != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(d3dRasterizerState);
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineD3D10(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    D3D10PIPELINE* d3dPipeline = xxAlloc(D3D10PIPELINE);
    if (d3dPipeline == nullptr)
        return 0;
    D3D10VERTEXATTRIBUTE* d3dVertexAttribute = reinterpret_cast<D3D10VERTEXATTRIBUTE*>(vertexAttribute);
    if (d3dVertexAttribute == nullptr)
        return 0;
    ID3D10VertexShader* d3dVertexShader = reinterpret_cast<ID3D10VertexShader*>(vertexShader);
    ID3D10PixelShader* d3dPixelShader = reinterpret_cast<ID3D10PixelShader*>(fragmentShader);
    ID3D10BlendState* d3dBlendState = reinterpret_cast<ID3D10BlendState*>(blendState);
    ID3D10DepthStencilState* d3dDepthStencilState = reinterpret_cast<ID3D10DepthStencilState*>(depthStencilState);
    ID3D10RasterizerState* d3dRasterizerState = reinterpret_cast<ID3D10RasterizerState*>(rasterizerState);

    d3dPipeline->vertexShader = d3dVertexShader;
    d3dPipeline->pixelShader = d3dPixelShader;
    d3dPipeline->blendState = d3dBlendState;
    d3dPipeline->depthStencilState = d3dDepthStencilState;
    d3dPipeline->rasterizerState = d3dRasterizerState;
    d3dPipeline->inputLayout = d3dVertexAttribute->inputLayout;

    return reinterpret_cast<uint64_t>(d3dPipeline);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateD3D10(uint64_t blendState)
{
    IUnknown* d3dBlendState = reinterpret_cast<IUnknown*>(blendState);

    SafeRelease(d3dBlendState);
}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateD3D10(uint64_t depthStencilState)
{
    IUnknown* d3dDepthStencilState = reinterpret_cast<IUnknown*>(depthStencilState);

    SafeRelease(d3dDepthStencilState);
}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateD3D10(uint64_t rasterizerState)
{
    IUnknown* d3dRasterizerState = reinterpret_cast<IUnknown*>(rasterizerState);

    SafeRelease(d3dRasterizerState);
}
//------------------------------------------------------------------------------
void xxDestroyPipelineD3D10(uint64_t pipeline)
{
    D3D10PIPELINE* d3dPipeline = reinterpret_cast<D3D10PIPELINE*>(pipeline);

    xxFree(d3dPipeline);
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportD3D10(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandEncoder);

    D3D10_VIEWPORT vp;
    vp.TopLeftX = x;
    vp.TopLeftY = y;
    vp.Width = width;
    vp.Height = height;
    vp.MinDepth = minZ;
    vp.MaxDepth = maxZ;
    d3dDevice->RSSetViewports(1, &vp);
}
//------------------------------------------------------------------------------
void xxSetScissorD3D10(uint64_t commandEncoder, int x, int y, int width, int height)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandEncoder);

    D3D10_RECT rect;
    rect.left = x;
    rect.top = y;
    rect.right = x + width;
    rect.bottom = y + height;
    d3dDevice->RSSetScissorRects(1, &rect);
}
//------------------------------------------------------------------------------
void xxSetPipelineD3D10(uint64_t commandEncoder, uint64_t pipeline)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandEncoder);
    D3D10PIPELINE* d3dPipeline = reinterpret_cast<D3D10PIPELINE*>(pipeline);
    static const float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    d3dDevice->VSSetShader(d3dPipeline->vertexShader);
    d3dDevice->PSSetShader(d3dPipeline->pixelShader);
    d3dDevice->OMSetBlendState(d3dPipeline->blendState, blendFactor, 0xFFFFFFFF);
    d3dDevice->OMSetDepthStencilState(d3dPipeline->depthStencilState, 0);
    d3dDevice->RSSetState(d3dPipeline->rasterizerState);
    d3dDevice->IASetInputLayout(d3dPipeline->inputLayout);
    d3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersD3D10(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandEncoder);
    D3D10VERTEXATTRIBUTE* d3dVertexAttribute = reinterpret_cast<D3D10VERTEXATTRIBUTE*>(vertexAttribute);
    ID3D10Buffer* d3dBuffers[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    UINT offsets[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
    UINT strides[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

    for (int i = 0; i < count; ++i)
    {
        D3D10BUFFER* d3dBuffer = reinterpret_cast<D3D10BUFFER*>(buffers[i]);
        d3dBuffers[i] = d3dBuffer->buffer;
        offsets[i] = 0;
        strides[i] = d3dVertexAttribute->stride;
    }

    d3dDevice->IASetVertexBuffers(0, count, d3dBuffers, strides, offsets);
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesD3D10(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandEncoder);
    ID3D10ShaderResourceView* d3dTextures[D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];

    for (int i = 0; i < count; ++i)
    {
        D3D10TEXTURE* d3dTexture = reinterpret_cast<D3D10TEXTURE*>(textures[i]);
        d3dTextures[i] = d3dTexture->resourceView;
    }

    d3dDevice->VSSetShaderResources(0, count, d3dTextures);
}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesD3D10(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandEncoder);
    ID3D10ShaderResourceView* d3dTextures[D3D10_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];

    for (int i = 0; i < count; ++i)
    {
        D3D10TEXTURE* d3dTexture = reinterpret_cast<D3D10TEXTURE*>(textures[i]);
        d3dTextures[i] = d3dTexture->resourceView;
    }

    d3dDevice->PSSetShaderResources(0, count, d3dTextures);
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersD3D10(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandEncoder);
    ID3D10SamplerState* d3dSamplerStates[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];

    for (int i = 0; i < count; ++i)
    {
        d3dSamplerStates[i] = reinterpret_cast<ID3D10SamplerState*>(samplers[i]);
    }

    d3dDevice->VSSetSamplers(0, count, d3dSamplerStates);
}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersD3D10(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandEncoder);
    ID3D10SamplerState* d3dSamplerStates[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];

    for (int i = 0; i < count; ++i)
    {
        d3dSamplerStates[i] = reinterpret_cast<ID3D10SamplerState*>(samplers[i]);
    }

    d3dDevice->PSSetSamplers(0, count, d3dSamplerStates);
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferD3D10(uint64_t commandEncoder, uint64_t buffer, int size)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandEncoder);
    D3D10BUFFER* d3dBuffer = reinterpret_cast<D3D10BUFFER*>(buffer);

    d3dDevice->VSSetConstantBuffers(0, 1, &d3dBuffer->buffer);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferD3D10(uint64_t commandEncoder, uint64_t buffer, int size)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandEncoder);
    D3D10BUFFER* d3dBuffer = reinterpret_cast<D3D10BUFFER*>(buffer);

    d3dDevice->PSSetConstantBuffers(0, 1, &d3dBuffer->buffer);
}
//------------------------------------------------------------------------------
void xxDrawD3D10(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandEncoder);

    d3dDevice->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
}
//------------------------------------------------------------------------------
void xxDrawIndexedD3D10(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    ID3D10Device* d3dDevice = reinterpret_cast<ID3D10Device*>(commandEncoder);
    D3D10BUFFER* d3dIndexBuffer = reinterpret_cast<D3D10BUFFER*>(indexBuffer);

    DXGI_FORMAT format = (INDEX_BUFFER_WIDTH == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    d3dDevice->IASetIndexBuffer(d3dIndexBuffer->buffer, format, 0);
    d3dDevice->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
//==============================================================================
