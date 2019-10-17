//==============================================================================
// xxGraphic : Direct3D 11On12 Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalD3D.h"
#include "xxGraphicD3D11.h"
#include "xxGraphicD3D11On12.h"

#include <d3d12.h>
#include <d3d11on12.h>
#include <dxgi1_4.h>
#define NUM_BACK_BUFFERS            3

static void*                        g_d3d11Library = nullptr;
static void*                        g_d3d12Library = nullptr;
static ID3D12Device*                g_d3d12Device = nullptr;
static ID3D12CommandQueue*          g_d3d12CommandQueue = nullptr;
static ID3D12Fence*                 g_d3d12Fence = nullptr;
static HANDLE                       g_d3d12FenceEvent = nullptr;
static UINT64                       g_d3d12FenceValue = 0;
static UINT64                       g_d3d12FenceValues[4] = {};

//==============================================================================
//  Fence
//==============================================================================
static void signalFence(bool wait)
{
    g_d3d12CommandQueue->Signal(g_d3d12Fence, g_d3d12FenceValue);
    if (wait)
    {
        g_d3d12Fence->SetEventOnCompletion(g_d3d12FenceValue, g_d3d12FenceEvent);
        WaitForSingleObjectEx(g_d3d12FenceEvent, 1000, FALSE);
    }
    g_d3d12FenceValue++;
}
//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D11On12()
{
    if (g_d3d11Library == nullptr)
        g_d3d11Library = xxLoadLibrary("d3d11.dll");
    if (g_d3d11Library == nullptr)
        return 0;

    if (g_d3d12Library == nullptr)
        g_d3d12Library = xxLoadLibrary("d3d12.dll");
    if (g_d3d12Library == nullptr)
        return 0;

    xxRegisterFunction(D3D11);

    xxDestroyInstance = xxDestroyInstanceD3D11On12;
    xxCreateDevice = xxCreateDeviceD3D11On12;
    xxDestroyDevice = xxDestroyDeviceD3D11On12;
    xxGetDeviceName = xxGetDeviceNameD3D11On12;
    xxPresentSwapchain = xxPresentSwapchainD3D11On12;
    xxGetCommandBuffer = xxGetCommandBufferD3D11On12;

    return reinterpret_cast<uint64_t>(g_d3d11Library);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceD3D11On12(uint64_t instance)
{
    if (g_d3d12Library)
    {
        xxFreeLibrary(g_d3d12Library);
        g_d3d12Library = nullptr;
    }

    if (g_d3d11Library)
    {
        xxFreeLibrary(g_d3d11Library);
        g_d3d11Library = nullptr;
    }

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceD3D11On12(uint64_t instance)
{
    PFN_D3D12_CREATE_DEVICE D3D12CreateDevice;
    (void*&)D3D12CreateDevice = xxGetProcAddress(g_d3d12Library, "D3D12CreateDevice");
    if (D3D12CreateDevice == nullptr)
        return 0;

    PFN_D3D11ON12_CREATE_DEVICE D3D11On12CreateDevice;
    (void*&)D3D11On12CreateDevice = xxGetProcAddress(g_d3d11Library, "D3D11On12CreateDevice");
    if (D3D11On12CreateDevice == nullptr)
        return 0;

    if (g_d3d12Device == nullptr)
    {
        HRESULT hResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_d3d12Device));
        if (hResult != S_OK)
            return 0;
    }

    if (g_d3d12CommandQueue == nullptr)
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        HRESULT hResult = g_d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_d3d12CommandQueue));
        if (hResult != S_OK)
            return 0;
    }

    if (g_d3d12Fence == nullptr)
    {
        HRESULT hResult = g_d3d12Device->CreateFence(g_d3d12FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_d3d12Fence));
        if (hResult != S_OK)
            return 0;

        g_d3d12FenceValue++;
        memset(g_d3d12FenceValues, 0, sizeof(g_d3d12FenceValues));
    }

    if (g_d3d12FenceEvent == nullptr)
    {
        g_d3d12FenceEvent = CreateEvent(nullptr, FALSE, TRUE, nullptr);
        if (g_d3d12FenceEvent == nullptr)
            return 0;
    }

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    ID3D11Device* d3dDevice = nullptr;
    HRESULT hResult = D3D11On12CreateDevice(g_d3d12Device, flags, nullptr, 0, (IUnknown**)&g_d3d12CommandQueue, 1, 0, &d3dDevice, nullptr, nullptr);
    if (hResult != S_OK)
        return 0;

    IUnknown* unknown = nullptr;
    xxLocalBreak()
    {
        if (d3dDevice->QueryInterface(__uuidof(ID3D11On12Device2*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "11On12.2", xxGetDeviceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(ID3D11On12Device1*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "11On12.1", xxGetDeviceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(ID3D11On12Device*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "11On12", xxGetDeviceName());
            break;
        }
    }
    SafeRelease(unknown);

    return reinterpret_cast<uint64_t>(d3dDevice);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceD3D11On12(uint64_t device)
{
    xxDestroyDeviceD3D11(device);

    if (g_d3d12Fence)
    {
        for (int i = 0; i < NUM_BACK_BUFFERS; ++i)
            signalFence(true);

        g_d3d12Fence->Release();
        g_d3d12Fence = nullptr;
    }

    if (g_d3d12FenceEvent)
    {
        CloseHandle(g_d3d12FenceEvent);
        g_d3d12FenceEvent = nullptr;
    }

    SafeRelease(g_d3d12CommandQueue);
    SafeRelease(g_d3d12Device);
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameD3D11On12()
{
    return "Direct3D 11On12";
}
//==============================================================================
//  Framebuffer
//==============================================================================
struct D3D11ON12FRAMEBUFFER
{
    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* depthStencilView;
};
//==============================================================================
//  Swapchain
//==============================================================================
struct D3D11ON12SWAPCHAIN : public D3D11ON12FRAMEBUFFER
{
    IDXGISwapChain3*        dxgiSwapchain;
    ID3D11Texture2D*        depthStencilTexture;
    ID3D11DeviceContext*    deviceContext;
};
//------------------------------------------------------------------------------
void xxPresentSwapchainD3D11On12(uint64_t swapchain)
{
    D3D11ON12SWAPCHAIN* d3dSwapchain = reinterpret_cast<D3D11ON12SWAPCHAIN*>(swapchain);
    if (d3dSwapchain == nullptr)
        return;

    int bufferIndex = d3dSwapchain->dxgiSwapchain->GetCurrentBackBufferIndex();
    g_d3d12FenceValues[bufferIndex] = g_d3d12FenceValue;
    signalFence(false);

    xxPresentSwapchainD3D11(swapchain);
}
//==============================================================================
//  Command Buffer
//==============================================================================
uint64_t xxGetCommandBufferD3D11On12(uint64_t device, uint64_t swapchain)
{
    D3D11ON12SWAPCHAIN* d3dSwapchain = reinterpret_cast<D3D11ON12SWAPCHAIN*>(swapchain);
    if (d3dSwapchain == nullptr)
        return 0;

    int bufferIndex = d3dSwapchain->dxgiSwapchain->GetCurrentBackBufferIndex();
    if (g_d3d12Fence->GetCompletedValue() < g_d3d12FenceValues[bufferIndex])
    {
        g_d3d12Fence->SetEventOnCompletion(g_d3d12FenceValues[bufferIndex], g_d3d12FenceEvent);
        WaitForSingleObjectEx(g_d3d12FenceEvent, 1000, FALSE);
    }

    return xxGetCommandBufferD3D11(device, swapchain);
}
//==============================================================================
