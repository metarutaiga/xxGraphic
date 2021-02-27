//==============================================================================
// xxGraphic : Direct3D 12.0 Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalD3D.h"
#include "xxGraphicD3DAsm.h"
#include "xxGraphicD3D12.h"

#include "dxsdk/d3d12.h"
#include "dxsdk/dxgi1_4.h"
typedef HRESULT (WINAPI *PFN_CREATE_DXGI_FACTORY1)(REFIID, void**);
#define NUM_BACK_BUFFERS            3
#define PERSISTENT_BUFFER           1

static void*                        g_d3dLibrary = nullptr;
static void*                        g_dxgiLibrary = nullptr;
static IDXGIFactory4*               g_dxgiFactory = nullptr;
static ID3D12CommandQueue*          g_commandQueue = nullptr;
static ID3D12Fence*                 g_fence = nullptr;
static HANDLE                       g_fenceEvent = nullptr;
static UINT64                       g_fenceValue = 0;
static UINT64                       g_fenceValues[NUM_BACK_BUFFERS] = {};
static ID3D12DescriptorHeap*        g_shaderHeap = nullptr;
static UINT                         g_shaderDescriptorSize = 0;
static UINT                         g_shaderAvailableHandle = 0;
static D3D12_CPU_DESCRIPTOR_HANDLE  g_shaderCPUHandles[256] = {};
static D3D12_GPU_DESCRIPTOR_HANDLE  g_shaderGPUHandles[256] = {};
static ID3D12DescriptorHeap*        g_samplerHeap = nullptr;
static UINT                         g_samplerDescriptorSize = 0;
static UINT                         g_samplerAvailableHandle = 0;
static D3D12_CPU_DESCRIPTOR_HANDLE  g_samplerCPUHandles[256] = {};
static D3D12_GPU_DESCRIPTOR_HANDLE  g_samplerGPUHandles[256] = {};
static ID3D12RootSignature*         g_rootSignature = nullptr;

//==============================================================================
//  Fence
//==============================================================================
static void signalFence(bool wait)
{
    g_commandQueue->Signal(g_fence, g_fenceValue);
    if (wait)
    {
        g_fence->SetEventOnCompletion(g_fenceValue, g_fenceEvent);
        WaitForSingleObjectEx(g_fenceEvent, 1000, FALSE);
    }
    g_fenceValue++;
}
//==============================================================================
//  Heap
//==============================================================================
static void createShaderHeap(D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
    g_shaderAvailableHandle--;
    cpuHandle = g_shaderCPUHandles[g_shaderAvailableHandle];
    gpuHandle = g_shaderGPUHandles[g_shaderAvailableHandle];
}
//------------------------------------------------------------------------------
static void destroyShaderHeap(D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
    if (cpuHandle.ptr == 0 && gpuHandle.ptr == 0)
        return;
    if (cpuHandle.ptr == 0)
    {
        cpuHandle.ptr = g_shaderHeap->GetCPUDescriptorHandleForHeapStart().ptr + (SIZE_T)(gpuHandle.ptr - g_shaderHeap->GetGPUDescriptorHandleForHeapStart().ptr);
    }
    if (gpuHandle.ptr == 0)
    {
        gpuHandle.ptr = g_shaderHeap->GetGPUDescriptorHandleForHeapStart().ptr + (UINT64)(cpuHandle.ptr - g_shaderHeap->GetCPUDescriptorHandleForHeapStart().ptr);
    }
    g_shaderCPUHandles[g_shaderAvailableHandle] = cpuHandle;
    g_shaderGPUHandles[g_shaderAvailableHandle] = gpuHandle;
    g_shaderAvailableHandle++;
}
//------------------------------------------------------------------------------
static void createSamplerHeap(D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
    g_samplerAvailableHandle--;
    cpuHandle = g_samplerCPUHandles[g_samplerAvailableHandle];
    gpuHandle = g_samplerGPUHandles[g_samplerAvailableHandle];
}
//------------------------------------------------------------------------------
static void destroySamplerHeap(D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
    if (cpuHandle.ptr == 0 && gpuHandle.ptr == 0)
        return;
    if (cpuHandle.ptr == 0)
    {
        cpuHandle.ptr = g_samplerHeap->GetCPUDescriptorHandleForHeapStart().ptr + (SIZE_T)(gpuHandle.ptr - g_samplerHeap->GetGPUDescriptorHandleForHeapStart().ptr);
    }
    if (gpuHandle.ptr == 0)
    {
        gpuHandle.ptr = g_samplerHeap->GetGPUDescriptorHandleForHeapStart().ptr + (UINT64)(cpuHandle.ptr - g_samplerHeap->GetCPUDescriptorHandleForHeapStart().ptr);
    }
    g_samplerCPUHandles[g_samplerAvailableHandle] = cpuHandle;
    g_samplerGPUHandles[g_samplerAvailableHandle] = gpuHandle;
    g_samplerAvailableHandle++;
}
//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D12()
{
    if (g_d3dLibrary == nullptr)
        g_d3dLibrary = xxLoadLibrary("d3d12.dll");
    if (g_d3dLibrary == nullptr)
        return 0;

    if (g_dxgiLibrary == nullptr)
        g_dxgiLibrary = xxLoadLibrary("dxgi.dll");
    if (g_dxgiLibrary == nullptr)
        return 0;

    xxRegisterFunction(D3D12);

    return reinterpret_cast<uint64_t>(g_d3dLibrary);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceD3D12(uint64_t instance)
{
    if (g_dxgiLibrary)
    {
        xxFreeLibrary(g_dxgiLibrary);
        g_dxgiLibrary = nullptr;
    }

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
uint64_t xxCreateDeviceD3D12(uint64_t instance)
{
    PFN_D3D12_CREATE_DEVICE D3D12CreateDevice;
    (void*&)D3D12CreateDevice = xxGetProcAddress(g_d3dLibrary, "D3D12CreateDevice");
    if (D3D12CreateDevice == nullptr)
        return 0;

    PFN_D3D12_SERIALIZE_ROOT_SIGNATURE D3D12SerializeRootSignature;
    (void*&)D3D12SerializeRootSignature = xxGetProcAddress(g_d3dLibrary, "D3D12SerializeRootSignature");
    if (D3D12SerializeRootSignature == nullptr)
        return 0;

#if defined(_DEBUG)
    ID3D12Debug* debug = nullptr;
    if (debug == nullptr)
    {
        PFN_D3D12_GET_DEBUG_INTERFACE D3D12GetDebugInterface;
        (void*&)D3D12GetDebugInterface = xxGetProcAddress(g_d3dLibrary, "D3D12GetDebugInterface");
        if (D3D12GetDebugInterface)
        {
            HRESULT hResult = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
            if (hResult == S_OK)
            {
                debug->EnableDebugLayer();
                debug->Release();
            }
        }
    }
#endif

    ID3D12Device* d3dDevice = nullptr;
    HRESULT hResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice));
    if (hResult != S_OK)
        return 0;

    if (g_commandQueue == nullptr)
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        HRESULT hResult = d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_commandQueue));
        if (hResult != S_OK)
            return 0;
    }

    if (g_fence == nullptr)
    {
        HRESULT hResult = d3dDevice->CreateFence(g_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence));
        if (hResult != S_OK)
            return 0;

        g_fenceValue++;
        memset(g_fenceValues, 0, sizeof(g_fenceValues));
    }

    if (g_fenceEvent == nullptr)
    {
        g_fenceEvent = CreateEvent(nullptr, FALSE, TRUE, nullptr);
        if (g_fenceEvent == nullptr)
            return 0;
    }

    if (g_shaderHeap == nullptr)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 256;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.NodeMask = 1;

        HRESULT hResult = d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_shaderHeap));
        if (hResult != S_OK)
            return 0;

        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = g_shaderHeap->GetCPUDescriptorHandleForHeapStart();
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = g_shaderHeap->GetGPUDescriptorHandleForHeapStart();
        g_shaderDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        g_shaderAvailableHandle = 256;
        for (int i = 0; i < 256; ++i)
        {
            g_shaderCPUHandles[i] = cpuHandle;
            g_shaderGPUHandles[i] = gpuHandle;
            cpuHandle.ptr += g_shaderDescriptorSize;
            gpuHandle.ptr += g_shaderDescriptorSize;
        }
    }

    if (g_samplerHeap == nullptr)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        desc.NumDescriptors = 256;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.NodeMask = 1;

        HRESULT hResult = d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_samplerHeap));
        if (hResult != S_OK)
            return 0;

        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = g_samplerHeap->GetCPUDescriptorHandleForHeapStart();
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = g_samplerHeap->GetGPUDescriptorHandleForHeapStart();
        g_samplerDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        g_samplerAvailableHandle = 256;
        for (int i = 0; i < 256; ++i)
        {
            g_samplerCPUHandles[i] = cpuHandle;
            g_samplerGPUHandles[i] = gpuHandle;
            cpuHandle.ptr += g_samplerDescriptorSize;
            gpuHandle.ptr += g_samplerDescriptorSize;
        }
    }

    if (g_rootSignature == nullptr)
    {
        D3D12_DESCRIPTOR_RANGE constantRanges[8] = {};
        D3D12_DESCRIPTOR_RANGE resourceRanges[8] = {};
        D3D12_DESCRIPTOR_RANGE samplerRanges[8] = {};
        for (int i = 0; i < 8; ++i)
        {
            constantRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            constantRanges[i].BaseShaderRegister = i;
            constantRanges[i].NumDescriptors = 1;
            resourceRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            resourceRanges[i].BaseShaderRegister = i;
            resourceRanges[i].NumDescriptors = 1;
            samplerRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
            samplerRanges[i].BaseShaderRegister = i;
            samplerRanges[i].NumDescriptors = 1;
        }

        D3D12_ROOT_PARAMETER parameters[xxGraphicDescriptor::TOTAL] = {};
        for (int i = 0; i < xxGraphicDescriptor::VERTEX_UNIFORM_COUNT; ++i)
        {
            parameters[xxGraphicDescriptor::VERTEX_UNIFORM + i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            parameters[xxGraphicDescriptor::VERTEX_UNIFORM + i].DescriptorTable.NumDescriptorRanges = 1;
            parameters[xxGraphicDescriptor::VERTEX_UNIFORM + i].DescriptorTable.pDescriptorRanges = &constantRanges[i];
            parameters[xxGraphicDescriptor::VERTEX_UNIFORM + i].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        }
        for (int i = 0; i < xxGraphicDescriptor::FRAGMENT_UNIFORM_COUNT; ++i)
        {
            parameters[xxGraphicDescriptor::FRAGMENT_UNIFORM + i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            parameters[xxGraphicDescriptor::FRAGMENT_UNIFORM + i].DescriptorTable.NumDescriptorRanges = 1;
            parameters[xxGraphicDescriptor::FRAGMENT_UNIFORM + i].DescriptorTable.pDescriptorRanges = &constantRanges[i];
            parameters[xxGraphicDescriptor::FRAGMENT_UNIFORM + i].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        }
        for (int i = 0; i < xxGraphicDescriptor::VERTEX_TEXTURE_COUNT; ++i)
        {
            parameters[xxGraphicDescriptor::VERTEX_TEXTURE + i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            parameters[xxGraphicDescriptor::VERTEX_TEXTURE + i].DescriptorTable.NumDescriptorRanges = 1;
            parameters[xxGraphicDescriptor::VERTEX_TEXTURE + i].DescriptorTable.pDescriptorRanges = &resourceRanges[i];
            parameters[xxGraphicDescriptor::VERTEX_TEXTURE + i].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        }
        for (int i = 0; i < xxGraphicDescriptor::FRAGMENT_TEXTURE_COUNT; ++i)
        {
            parameters[xxGraphicDescriptor::FRAGMENT_TEXTURE + i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            parameters[xxGraphicDescriptor::FRAGMENT_TEXTURE + i].DescriptorTable.NumDescriptorRanges = 1;
            parameters[xxGraphicDescriptor::FRAGMENT_TEXTURE + i].DescriptorTable.pDescriptorRanges = &resourceRanges[i];
            parameters[xxGraphicDescriptor::FRAGMENT_TEXTURE + i].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        }
        for (int i = 0; i < xxGraphicDescriptor::VERTEX_SAMPLER_COUNT; ++i)
        {
            parameters[xxGraphicDescriptor::VERTEX_SAMPLER + i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            parameters[xxGraphicDescriptor::VERTEX_SAMPLER + i].DescriptorTable.NumDescriptorRanges = 1;
            parameters[xxGraphicDescriptor::VERTEX_SAMPLER + i].DescriptorTable.pDescriptorRanges = &samplerRanges[i];
            parameters[xxGraphicDescriptor::VERTEX_SAMPLER + i].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        }
        for (int i = 0; i < xxGraphicDescriptor::FRAGMENT_SAMPLER_COUNT; ++i)
        {
            parameters[xxGraphicDescriptor::FRAGMENT_SAMPLER + i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            parameters[xxGraphicDescriptor::FRAGMENT_SAMPLER + i].DescriptorTable.NumDescriptorRanges = 1;
            parameters[xxGraphicDescriptor::FRAGMENT_SAMPLER + i].DescriptorTable.pDescriptorRanges = &samplerRanges[i];
            parameters[xxGraphicDescriptor::FRAGMENT_SAMPLER + i].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        }

        D3D12_ROOT_SIGNATURE_DESC desc = {};
        desc.NumParameters = xxCountOf(parameters);
        desc.pParameters = parameters;
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                     D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                     D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                     D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        ID3DBlob* blob = nullptr;
        HRESULT hResult = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr);
        if (hResult != S_OK)
            return 0;

        if (blob)
        {
            HRESULT hResult = d3dDevice->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&g_rootSignature));
            if (hResult != S_OK)
            {
                blob->Release();
                return 0;
            }
            blob->Release();
        }
    }

    IUnknown* unknown = nullptr;
    xxLocalBreak()
    {
        if (d3dDevice->QueryInterface(__uuidof(ID3D12Device8*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "12.8", xxGetDeviceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(ID3D12Device7*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "12.7", xxGetDeviceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(ID3D12Device6*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "12.6", xxGetDeviceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(ID3D12Device5*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "12.5", xxGetDeviceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(ID3D12Device4*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "12.4", xxGetDeviceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(ID3D12Device3*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "12.3", xxGetDeviceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(ID3D12Device2*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "12.2", xxGetDeviceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(ID3D12Device1*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "12.1", xxGetDeviceName());
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(ID3D12Device*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "12.0", xxGetDeviceName());
            break;
        }
    }
    SafeRelease(unknown);

    signalFence(true);

    return reinterpret_cast<uint64_t>(d3dDevice);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceD3D12(uint64_t device)
{
    ID3D12Device* d3dDevice = reinterpret_cast<ID3D12Device*>(device);

    if (g_fence)
    {
        for (int i = 0; i < NUM_BACK_BUFFERS; ++i)
            signalFence(true);

        g_fence->Release();
        g_fence = nullptr;
    }

    if (g_fenceEvent)
    {
        CloseHandle(g_fenceEvent);
        g_fenceEvent = nullptr;
    }

    SafeRelease(g_shaderHeap);
    SafeRelease(g_samplerHeap);
    SafeRelease(g_rootSignature);
    SafeRelease(g_commandQueue);
    SafeRelease(g_dxgiFactory);
    SafeRelease(d3dDevice);
}
//------------------------------------------------------------------------------
bool xxResetDeviceD3D12(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
bool xxTestDeviceD3D12(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameD3D12()
{
    return "Direct3D 12.0";
}
//==============================================================================
//  Framebuffer
//==============================================================================
struct D3D12FRAMEBUFFER
{
    ID3D12Resource*             renderTargetResource;
    D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle;
};
//==============================================================================
//  Swapchain
//==============================================================================
struct D3D12SWAPCHAIN : public D3D12FRAMEBUFFER
{
    IDXGISwapChain3*            dxgiSwapchain;
    ID3D12DescriptorHeap*       renderTargetHeap;
    ID3D12Resource*             renderTargetResources[NUM_BACK_BUFFERS];
    D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandles[NUM_BACK_BUFFERS];
    ID3D12CommandAllocator*     commandAllocators[NUM_BACK_BUFFERS];
    ID3D12GraphicsCommandList*  commandList;
    HWND                        hWnd;
    int                         width;
    int                         height;
};
//------------------------------------------------------------------------------
uint64_t xxCreateSwapchainD3D12(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain)
{
    ID3D12Device* d3dDevice = reinterpret_cast<ID3D12Device*>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3D12SWAPCHAIN* d3dOldSwapchain = reinterpret_cast<D3D12SWAPCHAIN*>(oldSwapchain);
    if (d3dOldSwapchain && d3dOldSwapchain->hWnd == view && d3dOldSwapchain->width == width && d3dOldSwapchain->height == height)
        return oldSwapchain;
    D3D12SWAPCHAIN* d3dSwapchain = xxAlloc(D3D12SWAPCHAIN);
    if (d3dSwapchain == nullptr)
        return 0;

    xxDestroySwapchainD3D12(oldSwapchain);

    if (g_dxgiFactory == nullptr)
    {
        PFN_CREATE_DXGI_FACTORY1 CreateDXGIFactory1;
        (void*&)CreateDXGIFactory1 = xxGetProcAddress(g_dxgiLibrary, "CreateDXGIFactory1");
        if (CreateDXGIFactory1 == nullptr)
        {
            xxFree(d3dSwapchain);
            return 0;
        }

        HRESULT hResult = CreateDXGIFactory1(IID_PPV_ARGS(&g_dxgiFactory));
        if (hResult != S_OK)
        {
            xxFree(d3dSwapchain);
            return 0;
        }
    }

    HWND hWnd = (HWND)view;

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = NUM_BACK_BUFFERS;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    IDXGISwapChain1* dxgiSwapchain1 = nullptr;
    if (dxgiSwapchain1 == nullptr)
    {
        HRESULT hResult = g_dxgiFactory->CreateSwapChainForHwnd(g_commandQueue, hWnd, &desc, nullptr, nullptr, &dxgiSwapchain1);
        if (hResult != S_OK)
        {
            xxFree(d3dSwapchain);
            return 0;
        }
    }

    IDXGISwapChain3* dxgiSwapchain3 = nullptr;
    if (dxgiSwapchain3 == nullptr)
    {
        HRESULT hResult = dxgiSwapchain1->QueryInterface(IID_PPV_ARGS(&dxgiSwapchain3));
        if (hResult != S_OK)
        {
            dxgiSwapchain1->Release();
            xxFree(d3dSwapchain);
            return 0;
        }
        dxgiSwapchain1->Release();
        dxgiSwapchain1 = nullptr;
    }
    d3dSwapchain->dxgiSwapchain = dxgiSwapchain3;

    ID3D12DescriptorHeap* renderTargetHeap = nullptr;
    if (renderTargetHeap == nullptr)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = NUM_BACK_BUFFERS;
        desc.NodeMask = 1;

        HRESULT hResult = d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&renderTargetHeap));
        if (hResult != S_OK)
        {
            xxFree(d3dSwapchain);
            return 0;
        }
    }
    d3dSwapchain->renderTargetHeap = renderTargetHeap;

    D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
    for (int i = 0; i < NUM_BACK_BUFFERS; ++i)
    {
        d3dSwapchain->renderTargetHandles[i] = renderTargetHandle;
        renderTargetHandle.ptr += d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        ID3D12Resource* backBuffer = nullptr;
        if (backBuffer == nullptr)
        {
            HRESULT hResult = dxgiSwapchain3->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
            if (hResult != S_OK)
            {
            }
        }
        d3dDevice->CreateRenderTargetView(backBuffer, nullptr, d3dSwapchain->renderTargetHandles[i]);
        d3dSwapchain->renderTargetResources[i] = backBuffer;

        ID3D12CommandAllocator* commandAllocator = nullptr;
        if (commandAllocator == nullptr)
        {
            HRESULT hResult = d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
            if (hResult != S_OK)
            {
            }
        }
        d3dSwapchain->commandAllocators[i] = commandAllocator;
    }

    int bufferIndex = dxgiSwapchain3->GetCurrentBackBufferIndex();
    ID3D12GraphicsCommandList* commandList = nullptr;
    HRESULT hResult = d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3dSwapchain->commandAllocators[bufferIndex], nullptr, IID_PPV_ARGS(&commandList));
    if (hResult == S_OK)
    {
        HRESULT hResult = commandList->Close();
        if (hResult != S_OK)
        {
        }
    }
    d3dSwapchain->commandList = commandList;

    d3dSwapchain->hWnd = hWnd;
    d3dSwapchain->width = width;
    d3dSwapchain->height = height;

    return reinterpret_cast<uint64_t>(d3dSwapchain);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainD3D12(uint64_t swapchain)
{
    D3D12SWAPCHAIN* d3dSwapchain = reinterpret_cast<D3D12SWAPCHAIN*>(swapchain);
    if (d3dSwapchain == nullptr)
        return;

    for (int i = 0; i < NUM_BACK_BUFFERS; ++i)
        signalFence(true);

    SafeRelease(d3dSwapchain->dxgiSwapchain);
    SafeRelease(d3dSwapchain->commandList);
    for (int i = 0; i < NUM_BACK_BUFFERS; ++i)
    {
        SafeRelease(d3dSwapchain->renderTargetResources[i]);
        SafeRelease(d3dSwapchain->commandAllocators[i]);
    }
    SafeRelease(d3dSwapchain->renderTargetHeap);
    xxFree(d3dSwapchain);
}
//------------------------------------------------------------------------------
void xxPresentSwapchainD3D12(uint64_t swapchain)
{
    D3D12SWAPCHAIN* d3dSwapchain = reinterpret_cast<D3D12SWAPCHAIN*>(swapchain);
    if (d3dSwapchain == nullptr)
        return;

    int bufferIndex = d3dSwapchain->dxgiSwapchain->GetCurrentBackBufferIndex();
    g_fenceValues[bufferIndex] = g_fenceValue;
    signalFence(false);

    d3dSwapchain->dxgiSwapchain->Present(0, 0);
}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferD3D12(uint64_t device, uint64_t swapchain)
{
    D3D12SWAPCHAIN* d3dSwapchain = reinterpret_cast<D3D12SWAPCHAIN*>(swapchain);
    if (d3dSwapchain == nullptr)
        return 0;

    int bufferIndex = d3dSwapchain->dxgiSwapchain->GetCurrentBackBufferIndex();
    if (g_fence->GetCompletedValue() < g_fenceValues[bufferIndex])
    {
        g_fence->SetEventOnCompletion(g_fenceValues[bufferIndex], g_fenceEvent);
        WaitForSingleObjectEx(g_fenceEvent, 1000, FALSE);
    }

    ID3D12CommandAllocator* commandAllocator = d3dSwapchain->commandAllocators[bufferIndex];
    ID3D12GraphicsCommandList* commandList = d3dSwapchain->commandList;
    commandAllocator->Reset();
    commandList->Reset(commandAllocator, nullptr);

    ID3D12DescriptorHeap* heaps[] = { g_shaderHeap, g_samplerHeap };
    commandList->SetDescriptorHeaps(xxCountOf(heaps), heaps);

    return reinterpret_cast<uint64_t>(commandList);
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferD3D12(uint64_t device, uint64_t swapchain, float* scale)
{
    if (scale)
    {
        (*scale) = 1.0f;
    }

    D3D12SWAPCHAIN* d3dSwapchain = reinterpret_cast<D3D12SWAPCHAIN*>(swapchain);
    if (d3dSwapchain == nullptr)
        return 0;

    int bufferIndex = d3dSwapchain->dxgiSwapchain->GetCurrentBackBufferIndex();
    d3dSwapchain->renderTargetResource = d3dSwapchain->renderTargetResources[bufferIndex];
    d3dSwapchain->renderTargetHandle = d3dSwapchain->renderTargetHandles[bufferIndex];

    return swapchain;
}
//==============================================================================
//  Command Buffer
//==============================================================================
bool xxBeginCommandBufferD3D12(uint64_t commandBuffer)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandBuffer);
    if (d3dCommandList == nullptr)
        return false;

    return true;
}
//------------------------------------------------------------------------------
void xxEndCommandBufferD3D12(uint64_t commandBuffer)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandBuffer);
    if (d3dCommandList == nullptr)
        return;

    d3dCommandList->Close();
}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferD3D12(uint64_t commandBuffer, uint64_t swapchain)
{
    ID3D12CommandList* d3dCommandList = reinterpret_cast<ID3D12CommandList*>(commandBuffer);
    if (d3dCommandList == nullptr)
        return;

    g_commandQueue->ExecuteCommandLists(1, &d3dCommandList);
}
//==============================================================================
//  Render Pass
//==============================================================================
union D3D12RENDERPASS
{
    uint64_t    value;
    struct
    {
        bool    clearColor;
        bool    clearDepthStencil;
    };
};
//------------------------------------------------------------------------------
uint64_t xxCreateRenderPassD3D12(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
{
    D3D12RENDERPASS d3dRenderPass = {};

    if (clearColor)
        d3dRenderPass.clearColor = true;
    if (clearDepth || clearStencil)
        d3dRenderPass.clearDepthStencil = true;

    return d3dRenderPass.value;
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassD3D12(uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassD3D12(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandBuffer);
    if (d3dCommandList == nullptr)
        return 0;
    D3D12FRAMEBUFFER* d3dFramebuffer = reinterpret_cast<D3D12FRAMEBUFFER*>(framebuffer);
    if (d3dFramebuffer == nullptr)
        return 0;
    D3D12RENDERPASS d3dRenderPass = { renderPass };

    d3dCommandList->OMSetRenderTargets(1, &d3dFramebuffer->renderTargetHandle, FALSE, nullptr);

    D3D12_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    d3dCommandList->RSSetViewports(1, &vp);

    D3D12_RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    d3dCommandList->RSSetScissorRects(1, &rect);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = d3dFramebuffer->renderTargetResource;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    d3dCommandList->ResourceBarrier(1, &barrier);
    if (d3dRenderPass.clearColor)
    {
        d3dCommandList->ClearRenderTargetView(d3dFramebuffer->renderTargetHandle, color, 0, nullptr);
    }

    return commandBuffer;
}
//------------------------------------------------------------------------------
void xxEndRenderPassD3D12(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandEncoder);
    if (d3dCommandList == nullptr)
        return;
    D3D12FRAMEBUFFER* d3dFramebuffer = reinterpret_cast<D3D12FRAMEBUFFER*>(framebuffer);
    if (d3dFramebuffer == nullptr)
        return;

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = d3dFramebuffer->renderTargetResource;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    d3dCommandList->ResourceBarrier(1, &barrier);
}
//==============================================================================
//  Vertex Attribute
//==============================================================================
struct D3D12VERTEXATTRIBUTE
{
    D3D12_INPUT_ELEMENT_DESC    inputElements[16];
    UINT                        inputElementCount;
    int                         stride;
};
//------------------------------------------------------------------------------
uint64_t xxCreateVertexAttributeD3D12(uint64_t device, int count, int* attribute)
{
    D3D12VERTEXATTRIBUTE* d3dVertexAttribute = xxAlloc(D3D12VERTEXATTRIBUTE);
    if (d3dVertexAttribute == nullptr)
        return 0;

    D3D12_INPUT_ELEMENT_DESC* inputElements = d3dVertexAttribute->inputElements;
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

        D3D12_INPUT_ELEMENT_DESC& inputElement = inputElements[i];
        inputElement.SemanticName;
        inputElement.SemanticIndex;
        inputElement.Format;
        inputElement.InputSlot = stream;
        inputElement.AlignedByteOffset = offset;
        inputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
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

    d3dVertexAttribute->inputElementCount = count;
    d3dVertexAttribute->stride = stride;

    return reinterpret_cast<uint64_t>(d3dVertexAttribute);
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeD3D12(uint64_t vertexAttribute)
{
    D3D12VERTEXATTRIBUTE* d3dVertexAttribute = reinterpret_cast<D3D12VERTEXATTRIBUTE*>(vertexAttribute);

    xxFree(d3dVertexAttribute);
}
//==============================================================================
//  Buffer
//==============================================================================
struct D3D12RESOURCE
{
    ID3D12Resource*             resource;
    D3D12_CPU_DESCRIPTOR_HANDLE resourceCPUHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE resourceGPUHandle;
    UINT                        size;
    void*                       cpuAddress;
};
//------------------------------------------------------------------------------
uint64_t xxCreateConstantBufferD3D12(uint64_t device, int size)
{
    ID3D12Device* d3dDevice = reinterpret_cast<ID3D12Device*>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3D12RESOURCE* d3dResource = xxAlloc(D3D12RESOURCE);
    if (d3dResource == nullptr)
        return 0;

    D3D12_HEAP_PROPERTIES properties = {};
    properties.Type = D3D12_HEAP_TYPE_UPLOAD;
    properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = (size + 255) & ~255;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    ID3D12Resource* resource = nullptr;
    HRESULT hResult = d3dDevice->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
    if (hResult != S_OK)
        return 0;

    D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {};
    viewDesc.BufferLocation = resource->GetGPUVirtualAddress();
    viewDesc.SizeInBytes = (size + 255) & ~255;

    d3dResource->resource = resource;
    d3dResource->resourceCPUHandle = {};
    d3dResource->resourceGPUHandle = {};
    createShaderHeap(d3dResource->resourceCPUHandle, d3dResource->resourceGPUHandle);
    d3dDevice->CreateConstantBufferView(&viewDesc, d3dResource->resourceCPUHandle);
    d3dResource->size = size;
    d3dResource->cpuAddress = nullptr;
#if PERSISTENT_BUFFER
    d3dResource->cpuAddress = xxMapBuffer(device, reinterpret_cast<uint64_t>(d3dResource));
#endif

    return reinterpret_cast<uint64_t>(d3dResource);
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferD3D12(uint64_t device, int size)
{
    ID3D12Device* d3dDevice = reinterpret_cast<ID3D12Device*>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3D12RESOURCE* d3dResource = xxAlloc(D3D12RESOURCE);
    if (d3dResource == nullptr)
        return 0;

    D3D12_HEAP_PROPERTIES properties = {};
    properties.Type = D3D12_HEAP_TYPE_UPLOAD;
    properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = size;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    ID3D12Resource* resource = nullptr;
    HRESULT hResult = d3dDevice->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
    if (hResult != S_OK)
        return 0;

    d3dResource->resource = resource;
    d3dResource->resourceCPUHandle = {};
    d3dResource->resourceGPUHandle = {};
    d3dResource->size = size;
    d3dResource->cpuAddress = nullptr;
#if PERSISTENT_BUFFER
    d3dResource->cpuAddress = xxMapBuffer(device, reinterpret_cast<uint64_t>(d3dResource));
#endif

    return reinterpret_cast<uint64_t>(d3dResource);
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferD3D12(uint64_t device, int size, uint64_t vertexAttribute)
{
    ID3D12Device* d3dDevice = reinterpret_cast<ID3D12Device*>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3D12RESOURCE* d3dResource = xxAlloc(D3D12RESOURCE);
    if (d3dResource == nullptr)
        return 0;

    D3D12_HEAP_PROPERTIES properties = {};
    properties.Type = D3D12_HEAP_TYPE_UPLOAD;
    properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = size;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    ID3D12Resource* resource = nullptr;
    HRESULT hResult = d3dDevice->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
    if (hResult != S_OK)
        return 0;

    d3dResource->resource = resource;
    d3dResource->resourceCPUHandle = {};
    d3dResource->resourceGPUHandle = {};
    d3dResource->size = size;
    d3dResource->cpuAddress = nullptr;
#if PERSISTENT_BUFFER
    d3dResource->cpuAddress = xxMapBuffer(device, reinterpret_cast<uint64_t>(d3dResource));
#endif

    return reinterpret_cast<uint64_t>(d3dResource);
}
//------------------------------------------------------------------------------
void xxDestroyBufferD3D12(uint64_t device, uint64_t buffer)
{
    D3D12RESOURCE* d3dResource = reinterpret_cast<D3D12RESOURCE*>(buffer);
    if (d3dResource == nullptr)
        return;

    for (int i = 0; i < NUM_BACK_BUFFERS; ++i)
        signalFence(true);

    if (d3dResource->resource)
    {
        if (d3dResource->cpuAddress)
        {
            D3D12_RANGE range = { 0, d3dResource->size };
            d3dResource->resource->Unmap(0, &range);
        }
        d3dResource->resource->Release();
    }
    destroyShaderHeap(d3dResource->resourceCPUHandle, d3dResource->resourceGPUHandle);
    xxFree(d3dResource);
}
//------------------------------------------------------------------------------
void* xxMapBufferD3D12(uint64_t device, uint64_t buffer)
{
    D3D12RESOURCE* d3dResource = reinterpret_cast<D3D12RESOURCE*>(buffer);
    if (d3dResource == nullptr)
        return nullptr;
    if (d3dResource->cpuAddress)
        return d3dResource->cpuAddress;

    D3D12_RANGE range = { 0, d3dResource->size };
    void* ptr = nullptr;
    HRESULT hResult = d3dResource->resource->Map(0, &range, &ptr);
    if (hResult != S_OK)
        return nullptr;

    return ptr;
}
//------------------------------------------------------------------------------
void xxUnmapBufferD3D12(uint64_t device, uint64_t buffer)
{
    D3D12RESOURCE* d3dResource = reinterpret_cast<D3D12RESOURCE*>(buffer);
    if (d3dResource == nullptr)
        return;
    if (d3dResource->cpuAddress)
        return;

    D3D12_RANGE range = { 0, d3dResource->size };
    d3dResource->resource->Unmap(0, &range);
}
//==============================================================================
//  Texture
//==============================================================================
struct D3D12TEXTURE
{
    ID3D12Resource*             texture;
    D3D12_CPU_DESCRIPTOR_HANDLE textureCPUHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE textureGPUHandle;
    UINT                        width;
    UINT                        height;
    UINT                        depth;
    ID3D12Resource*             upload;
    UINT                        uploadPitch;
    UINT                        uploadSize;
};
//------------------------------------------------------------------------------
uint64_t xxCreateTextureD3D12(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, const void* external)
{
    ID3D12Device* d3dDevice = reinterpret_cast<ID3D12Device*>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (width == 0 || height == 0 || depth == 0 || mipmap == 0 || array == 0)
        return 0;

    D3D12TEXTURE* d3dTexture = xxAlloc(D3D12TEXTURE);
    if (d3dTexture == nullptr)
        return 0;

    D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
    ID3D12Resource* d3dResource = nullptr;

    D3D12_HEAP_PROPERTIES properties = {};
    properties.Type = D3D12_HEAP_TYPE_DEFAULT;
    properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    if (depth == 1 && array == 1)
    {
        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width = width;
        desc.Height = height;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;

        ID3D12Resource* texture = nullptr;
        HRESULT hResult = d3dDevice->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&texture));
        if (hResult != S_OK)
        {
            xxFree(d3dTexture);
            return 0;
        }
        d3dResource = texture;

        viewDesc.Format = desc.Format;
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        viewDesc.Texture2D.MipLevels = mipmap;
        viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    }

    if (d3dResource == nullptr)
    {
        xxFree(d3dTexture);
        return 0;
    }

    d3dTexture->texture = d3dResource;
    createShaderHeap(d3dTexture->textureCPUHandle, d3dTexture->textureGPUHandle);
    d3dDevice->CreateShaderResourceView(d3dResource, &viewDesc, d3dTexture->textureCPUHandle);
    d3dTexture->width = width;
    d3dTexture->height = height;
    d3dTexture->depth = depth;
    d3dTexture->upload = nullptr;
    d3dTexture->uploadPitch = (width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
    d3dTexture->uploadSize = height * d3dTexture->uploadPitch;

    return reinterpret_cast<uint64_t>(d3dTexture);
}
//------------------------------------------------------------------------------
void xxDestroyTextureD3D12(uint64_t texture)
{
    D3D12TEXTURE* d3dTexture = reinterpret_cast<D3D12TEXTURE*>(texture);
    if (d3dTexture == nullptr)
        return;

    for (int i = 0; i < NUM_BACK_BUFFERS; ++i)
        signalFence(true);

    if (d3dTexture->textureCPUHandle.ptr && d3dTexture->textureGPUHandle.ptr)
        destroyShaderHeap(d3dTexture->textureCPUHandle, d3dTexture->textureGPUHandle);

    SafeRelease(d3dTexture->texture);
    SafeRelease(d3dTexture->upload);
    xxFree(d3dTexture);
}
//------------------------------------------------------------------------------
void* xxMapTextureD3D12(uint64_t device, uint64_t texture, int* stride, int level, int array)
{
    ID3D12Device* d3dDevice = reinterpret_cast<ID3D12Device*>(device);
    if (d3dDevice == nullptr)
        return nullptr;
    D3D12TEXTURE* d3dTexture = reinterpret_cast<D3D12TEXTURE*>(texture);
    if (d3dTexture == nullptr)
        return nullptr;

    if (d3dTexture->upload == nullptr)
    {
        D3D12_HEAP_PROPERTIES properties = {};
        properties.Type = D3D12_HEAP_TYPE_UPLOAD;
        properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Width = d3dTexture->uploadSize;
        desc.Height = 1;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        HRESULT hResult = d3dDevice->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&d3dTexture->upload));
        if (hResult != S_OK)
            return nullptr;
    }

    void* ptr = nullptr;
    HRESULT hResult = d3dTexture->upload->Map(0, nullptr, &ptr);
    if (hResult != S_OK)
        return nullptr;

    (*stride) = d3dTexture->uploadPitch;
    return ptr;
}
//------------------------------------------------------------------------------
void xxUnmapTextureD3D12(uint64_t device, uint64_t texture, int level, int array)
{
    ID3D12Device* d3dDevice = reinterpret_cast<ID3D12Device*>(device);
    if (d3dDevice == nullptr)
        return;
    D3D12TEXTURE* d3dTexture = reinterpret_cast<D3D12TEXTURE*>(texture);
    if (d3dTexture == nullptr)
        return;
    if (d3dTexture->upload == nullptr)
        return;

    d3dTexture->upload->Unmap(0, nullptr);

    ID3D12CommandAllocator* commandAllocator = nullptr;
    HRESULT hResult = d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    if (hResult == S_OK)
    {
        ID3D12GraphicsCommandList* commandList = nullptr;
        HRESULT hResult = d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
        if (hResult == S_OK)
        {
            D3D12_TEXTURE_COPY_LOCATION textureLocation = {};
            D3D12_TEXTURE_COPY_LOCATION uploadLocation = {};
            textureLocation.pResource = d3dTexture->texture;
            textureLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            uploadLocation.pResource = d3dTexture->upload;
            uploadLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            uploadLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            uploadLocation.PlacedFootprint.Footprint.Width = d3dTexture->width;
            uploadLocation.PlacedFootprint.Footprint.Height = d3dTexture->height;
            uploadLocation.PlacedFootprint.Footprint.Depth = d3dTexture->depth;
            uploadLocation.PlacedFootprint.Footprint.RowPitch = d3dTexture->uploadPitch;
            commandList->CopyTextureRegion(&textureLocation, 0, 0, 0, &uploadLocation, nullptr);

            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = d3dTexture->texture;
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            commandList->ResourceBarrier(1, &barrier);

            commandList->Close();
            g_commandQueue->ExecuteCommandLists(1, (ID3D12CommandList**)&commandList);
            signalFence(true);

            commandList->Release();
        }
        commandAllocator->Release();
    }
    d3dTexture->upload->Release();
    d3dTexture->upload = nullptr;
}
//==============================================================================
//  Sampler
//==============================================================================
uint64_t xxCreateSamplerD3D12(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    ID3D12Device* d3dDevice = reinterpret_cast<ID3D12Device*>(device);
    if (d3dDevice == nullptr)
        return 0;

    D3D12_SAMPLER_DESC desc = {};
    desc.Filter = D3D12_ENCODE_BASIC_FILTER(linearMin ? D3D12_FILTER_TYPE_LINEAR : D3D12_FILTER_TYPE_POINT,
                                            linearMag ? D3D12_FILTER_TYPE_LINEAR : D3D12_FILTER_TYPE_POINT,
                                            linearMip ? D3D12_FILTER_TYPE_LINEAR : D3D12_FILTER_TYPE_POINT, D3D12_FILTER_REDUCTION_TYPE_STANDARD);
    desc.AddressU = clampU ? D3D12_TEXTURE_ADDRESS_MODE_CLAMP : D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    desc.AddressV = clampV ? D3D12_TEXTURE_ADDRESS_MODE_CLAMP : D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    desc.AddressW = clampW ? D3D12_TEXTURE_ADDRESS_MODE_CLAMP : D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    desc.MaxAnisotropy = anisotropy;
    desc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    if (anisotropy > 1)
    {
        desc.Filter = D3D12_ENCODE_ANISOTROPIC_FILTER(D3D12_FILTER_REDUCTION_TYPE_STANDARD);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE samplerCPUHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE samplerGPUHandle;
    createSamplerHeap(samplerCPUHandle, samplerGPUHandle);

    d3dDevice->CreateSampler(&desc, samplerCPUHandle);

    return static_cast<uint64_t>(samplerGPUHandle.ptr);
}
//------------------------------------------------------------------------------
void xxDestroySamplerD3D12(uint64_t sampler)
{
    D3D12_CPU_DESCRIPTOR_HANDLE samplerCPUHandle = { 0 };
    D3D12_GPU_DESCRIPTOR_HANDLE samplerGPUHandle = { sampler };
    if (samplerGPUHandle.ptr == 0)
        return;

    destroySamplerHeap(samplerCPUHandle, samplerGPUHandle);
}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderD3D12(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    ID3D12Device* d3dDevice = reinterpret_cast<ID3D12Device*>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        void* d3dShader = xxAlloc(char, vertexShaderCode40[6]);
        if (d3dShader == nullptr)
            return 0;
        memcpy(d3dShader, vertexShaderCode40, vertexShaderCode40[6]);

        return reinterpret_cast<uint64_t>(d3dShader);
    }
    else
    {
        ID3D10Blob* blob = CreateD3D10Shader(shader, "VSMain", "vs_4_0");
        if (blob == nullptr)
            return 0;

        void* d3dShader = xxAlloc(char, blob->GetBufferSize());
        if (d3dShader == nullptr)
            return 0;
        memcpy(d3dShader, blob->GetBufferPointer(), blob->GetBufferSize());

        return reinterpret_cast<uint64_t>(d3dShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderD3D12(uint64_t device, const char* shader)
{
    ID3D12Device* d3dDevice = reinterpret_cast<ID3D12Device*>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        void* d3dShader = xxAlloc(char, pixelShaderCode40[6]);
        if (d3dShader == nullptr)
            return 0;
        memcpy(d3dShader, pixelShaderCode40, pixelShaderCode40[6]);

        return reinterpret_cast<uint64_t>(d3dShader);
    }
    else
    {
        ID3D10Blob* blob = CreateD3D10Shader(shader, "FSMain", "ps_4_0");
        if (blob == nullptr)
            return 0;

        void* d3dShader = xxAlloc(char, blob->GetBufferSize());
        if (d3dShader == nullptr)
            return 0;
        memcpy(d3dShader, blob->GetBufferPointer(), blob->GetBufferSize());

        return reinterpret_cast<uint64_t>(d3dShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderD3D12(uint64_t device, uint64_t shader)
{
    void* d3dShader = reinterpret_cast<void*>(shader);
    xxFree(d3dShader);
}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreateBlendStateD3D12(uint64_t device, bool blending)
{
    D3D12_BLEND_DESC* d3dDesc = xxAlloc(D3D12_BLEND_DESC);
    if (d3dDesc == nullptr)
        return 0;
    memset(d3dDesc, 0, sizeof(D3D12_BLEND_DESC));

    D3D12_BLEND_DESC& desc = (*d3dDesc);
    desc.RenderTarget[0].BlendEnable = blending;
    desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    return reinterpret_cast<uint64_t>(d3dDesc);
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateD3D12(uint64_t device, bool depthTest, bool depthWrite)
{
    D3D12_DEPTH_STENCIL_DESC* d3dDesc = xxAlloc(D3D12_DEPTH_STENCIL_DESC);
    if (d3dDesc == nullptr)
        return 0;
    memset(d3dDesc, 0, sizeof(D3D12_DEPTH_STENCIL_DESC));

    D3D12_DEPTH_STENCIL_DESC& desc = (*d3dDesc);
    desc.DepthEnable = depthTest;
    desc.DepthWriteMask = depthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    desc.BackFace = desc.FrontFace;

    return reinterpret_cast<uint64_t>(d3dDesc);
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateD3D12(uint64_t device, bool cull, bool scissor)
{
    D3D12_RASTERIZER_DESC* d3dDesc = xxAlloc(D3D12_RASTERIZER_DESC);
    if (d3dDesc == nullptr)
        return 0;
    memset(d3dDesc, 0, sizeof(D3D12_RASTERIZER_DESC));

    D3D12_RASTERIZER_DESC& desc = (*d3dDesc);
    desc.FillMode = D3D12_FILL_MODE_SOLID;
    desc.CullMode = D3D12_CULL_MODE_NONE;
    desc.DepthClipEnable = true;

    return reinterpret_cast<uint64_t>(d3dDesc);
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineD3D12(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    ID3D12Device* d3dDevice = reinterpret_cast<ID3D12Device*>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3D12_BLEND_DESC* d3dBlendState = reinterpret_cast<D3D12_BLEND_DESC*>(blendState);
    if (d3dBlendState == nullptr)
        return 0;
    D3D12_DEPTH_STENCIL_DESC* d3dDepthStencilState = reinterpret_cast<D3D12_DEPTH_STENCIL_DESC*>(depthStencilState);
    if (d3dDepthStencilState == nullptr)
        return 0;
    D3D12_RASTERIZER_DESC* d3dRasterizerState = reinterpret_cast<D3D12_RASTERIZER_DESC*>(rasterizerState);
    if (d3dRasterizerState == nullptr)
        return 0;
    D3D12VERTEXATTRIBUTE* d3dVertexAttribute = reinterpret_cast<D3D12VERTEXATTRIBUTE*>(vertexAttribute);
    if (d3dVertexAttribute == nullptr)
        return 0;
    DWORD* d3dVertexShader = reinterpret_cast<DWORD*>(vertexShader);
    if (d3dVertexShader == nullptr)
        return 0;
    DWORD* d3dPixelShader = reinterpret_cast<DWORD*>(fragmentShader);
    if (d3dPixelShader == nullptr)
        return 0;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
    desc.pRootSignature = g_rootSignature;
    desc.VS = { d3dVertexShader, d3dVertexShader[6] };
    desc.PS = { d3dPixelShader, d3dPixelShader[6] };
    desc.BlendState = (*d3dBlendState);
    desc.SampleMask = UINT_MAX;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.RasterizerState = (*d3dRasterizerState);
    desc.DepthStencilState = (*d3dDepthStencilState);
    desc.InputLayout = { d3dVertexAttribute->inputElements, d3dVertexAttribute->inputElementCount };
    desc.NumRenderTargets = 1;
    desc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.NodeMask = 1;

    ID3D12PipelineState* pipelineState = nullptr;
    HRESULT hResult = d3dDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));
    if (hResult != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(pipelineState);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateD3D12(uint64_t blendState)
{
    D3D12_BLEND_DESC* d3dBlendState = reinterpret_cast<D3D12_BLEND_DESC*>(blendState);

    xxFree(d3dBlendState);
}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateD3D12(uint64_t depthStencilState)
{
    D3D12_DEPTH_STENCIL_DESC* d3dDepthStencilState = reinterpret_cast<D3D12_DEPTH_STENCIL_DESC*>(depthStencilState);

    xxFree(d3dDepthStencilState);
}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateD3D12(uint64_t rasterizerState)
{
    D3D12_RASTERIZER_DESC* d3dRasterizerState = reinterpret_cast<D3D12_RASTERIZER_DESC*>(rasterizerState);

    xxFree(d3dRasterizerState);
}
//------------------------------------------------------------------------------
void xxDestroyPipelineD3D12(uint64_t pipeline)
{
    ID3D12PipelineState* d3dPipelineState = reinterpret_cast<ID3D12PipelineState*>(pipeline);

    SafeRelease(d3dPipelineState);
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportD3D12(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandEncoder);

    D3D12_VIEWPORT vp;
    vp.TopLeftX = (FLOAT)x;
    vp.TopLeftY = (FLOAT)y;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = minZ;
    vp.MaxDepth = maxZ;
    d3dCommandList->RSSetViewports(1, &vp);
}
//------------------------------------------------------------------------------
void xxSetScissorD3D12(uint64_t commandEncoder, int x, int y, int width, int height)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandEncoder);

    D3D12_RECT rect;
    rect.left = x;
    rect.top = y;
    rect.right = x + width;
    rect.bottom = y + height;
    d3dCommandList->RSSetScissorRects(1, &rect);
}
//------------------------------------------------------------------------------
void xxSetPipelineD3D12(uint64_t commandEncoder, uint64_t pipeline)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandEncoder);
    ID3D12PipelineState* d3dPipelineState = reinterpret_cast<ID3D12PipelineState*>(pipeline);

    d3dCommandList->SetGraphicsRootSignature(g_rootSignature);
    d3dCommandList->SetPipelineState(d3dPipelineState);
    d3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
//------------------------------------------------------------------------------
void xxSetIndexBufferD3D12(uint64_t commandEncoder, uint64_t buffer)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandEncoder);
    D3D12RESOURCE* d3dBuffer = reinterpret_cast<D3D12RESOURCE*>(buffer);

    DXGI_FORMAT format = (INDEX_BUFFER_WIDTH == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    D3D12_INDEX_BUFFER_VIEW view = {};
    view.BufferLocation = d3dBuffer->resource->GetGPUVirtualAddress();
    view.SizeInBytes = d3dBuffer->size;
    view.Format = format;
    d3dCommandList->IASetIndexBuffer(&view);
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersD3D12(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandEncoder);
    D3D12VERTEXATTRIBUTE* d3dVertexAttribute = reinterpret_cast<D3D12VERTEXATTRIBUTE*>(vertexAttribute);
    D3D12_VERTEX_BUFFER_VIEW views[16];

    for (int i = 0; i < count; ++i)
    {
        D3D12RESOURCE* d3dBuffer = reinterpret_cast<D3D12RESOURCE*>(buffers[i]);
        views[i].BufferLocation = d3dBuffer->resource->GetGPUVirtualAddress();
        views[i].SizeInBytes = d3dBuffer->size;
        views[i].StrideInBytes = d3dVertexAttribute->stride;
    }

    d3dCommandList->IASetVertexBuffers(0, count, views);
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesD3D12(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        D3D12TEXTURE* d3dTexture = reinterpret_cast<D3D12TEXTURE*>(textures[i]);
        d3dCommandList->SetGraphicsRootDescriptorTable(xxGraphicDescriptor::VERTEX_TEXTURE + i, d3dTexture->textureGPUHandle);
    }
}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesD3D12(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        D3D12TEXTURE* d3dTexture = reinterpret_cast<D3D12TEXTURE*>(textures[i]);
        d3dCommandList->SetGraphicsRootDescriptorTable(xxGraphicDescriptor::FRAGMENT_TEXTURE + i, d3dTexture->textureGPUHandle);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersD3D12(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE d3dHandle = { samplers[i] };
        d3dCommandList->SetGraphicsRootDescriptorTable(xxGraphicDescriptor::VERTEX_SAMPLER + i, d3dHandle);
    }
}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersD3D12(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE d3dHandle = { samplers[i] };
        d3dCommandList->SetGraphicsRootDescriptorTable(xxGraphicDescriptor::FRAGMENT_SAMPLER + i, d3dHandle);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferD3D12(uint64_t commandEncoder, uint64_t buffer, int size)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandEncoder);
    D3D12RESOURCE* d3dBuffer = reinterpret_cast<D3D12RESOURCE*>(buffer);

    d3dCommandList->SetGraphicsRootDescriptorTable(xxGraphicDescriptor::VERTEX_UNIFORM, d3dBuffer->resourceGPUHandle);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferD3D12(uint64_t commandEncoder, uint64_t buffer, int size)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandEncoder);
    D3D12RESOURCE* d3dBuffer = reinterpret_cast<D3D12RESOURCE*>(buffer);

    d3dCommandList->SetGraphicsRootDescriptorTable(xxGraphicDescriptor::FRAGMENT_UNIFORM, d3dBuffer->resourceGPUHandle);
}
//------------------------------------------------------------------------------
void xxDrawIndexedD3D12(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    ID3D12GraphicsCommandList* d3dCommandList = reinterpret_cast<ID3D12GraphicsCommandList*>(commandEncoder);

    d3dCommandList->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
//==============================================================================
