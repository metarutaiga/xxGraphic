//==============================================================================
// xxGraphic : Mantle Source
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "xxGraphicMantleAsm.h"
#include "xxGraphicMantle.h"

#define GR_PROTOTYPES 1
#include "mantle/mantle.h"
#include "mantle/mantleWsiWinExt.h"
#define NUM_BACK_BUFFERS 3
#define NUM_DESCRIPTOR_COUNT        (8)
#define BASE_VERTEX_CONSTANT        (0)
#define BASE_PIXEL_CONSTANT         (1)
#define BASE_VERTEX_ATTRIBUTE       (2)
#define BASE_VERTEX_TEXTURE         (2 + 16)
#define BASE_PIXEL_TEXTURE          (2 + 16 + NUM_DESCRIPTOR_COUNT * 2)
#define BASE_VERTEX_SAMPLER         (2 + 16 + NUM_DESCRIPTOR_COUNT * 3)
#define BASE_PIXEL_SAMPLER          (2 + 16 + NUM_DESCRIPTOR_COUNT * 4)

static void*                        g_mantleLibrary = nullptr;
static GR_QUEUE                     g_queue = GR_NULL_HANDLE;
static GR_FENCE                     g_fences[NUM_BACK_BUFFERS] = {};
static int                          g_fenceIndex = 0;
static GR_MEMORY_HEAP_PROPERTIES    g_heapProps = {};
static GR_UINT                      g_suitableHeap = 0;

//==============================================================================
//  Instance
//==============================================================================
static bool grSymbolFailed = false;
static void* grSymbolImpl(const char* name)
{
    void* ptr = nullptr;

    if (ptr == nullptr && g_mantleLibrary)
        ptr = xxGetProcAddress(g_mantleLibrary, name);

    if (ptr == nullptr)
        xxLog("Mantle", "%s is not found", name);

    grSymbolFailed |= (ptr == nullptr);

    return ptr;
}
#define grSymbol(var) (void*&)var = grSymbolImpl(#var);
//------------------------------------------------------------------------------
uint64_t xxCreateInstanceMantle()
{
    if (g_mantleLibrary == nullptr)
        g_mantleLibrary = xxLoadLibrary("mantle32.dll");
    if (g_mantleLibrary == nullptr)
        g_mantleLibrary = xxLoadLibrary("mantle64.dll");
    if (g_mantleLibrary == nullptr)
        return 0;

    GR_APPLICATION_INFO appInfo = {};
    appInfo.apiVersion = GR_API_VERSION;

    GR_PHYSICAL_GPU gpus[GR_MAX_PHYSICAL_GPUS] = {};
    GR_UINT gpuCount = GR_MAX_PHYSICAL_GPUS;

    if (grInitAndEnumerateGpus(&appInfo, nullptr, &gpuCount, gpus) != GR_SUCCESS)
        return 0;

    xxRegisterFunction(Mantle);

    return reinterpret_cast<uint64_t>(gpus[0]);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceMantle(uint64_t instance)
{
    if (g_mantleLibrary)
    {
        xxFreeLibrary(g_mantleLibrary);
        g_mantleLibrary = nullptr;
    }

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceMantle(uint64_t instance)
{
    GR_PHYSICAL_GPU gpu = reinterpret_cast<GR_PHYSICAL_GPU>(instance);

    GR_DEVICE_QUEUE_CREATE_INFO queueInfo = {};
    queueInfo.queueType = GR_QUEUE_UNIVERSAL;
    queueInfo.queueCount = 1;

    const GR_CHAR* const extensions[] = { "GR_WSI_WINDOWS" };

    GR_DEVICE_CREATE_INFO deviceInfo = {};
    deviceInfo.queueRecordCount = 1;
    deviceInfo.pRequestedQueues = &queueInfo;
    deviceInfo.extensionCount = 1;
    deviceInfo.ppEnabledExtensionNames = extensions;
    deviceInfo.flags |= GR_DEVICE_CREATE_VALIDATION;
    deviceInfo.maxValidationLevel = GR_VALIDATION_LEVEL_4;

    GR_DEVICE device = GR_NULL_HANDLE;
    if (grCreateDevice(gpu, &deviceInfo, &device) != GR_SUCCESS)
        return 0;

    if (grGetDeviceQueue(device, GR_QUEUE_UNIVERSAL, 0, &g_queue) != GR_SUCCESS)
        return 0;

    GR_FENCE_CREATE_INFO fenceCreateInfo = {};
    for (int i = 0; i < NUM_BACK_BUFFERS; ++i)
    {
        GR_FENCE fence = GR_NULL_HANDLE;
        grCreateFence(device, &fenceCreateInfo, &fence);
        g_fences[i] = fence;
    }

    GR_UINT heapCount;
    grGetMemoryHeapCount(device, &heapCount);

    for (GR_UINT i = 0; i < heapCount; i++)
    {
        memset(&g_heapProps, 0, sizeof(g_heapProps));
        GR_SIZE heapPropSize = sizeof(g_heapProps);

        grGetMemoryHeapInfo(device, i, GR_INFO_TYPE_MEMORY_HEAP_PROPERTIES, &heapPropSize, &g_heapProps);
        if (g_heapProps.flags & GR_MEMORY_HEAP_CPU_VISIBLE)
        {
            g_suitableHeap = i;
            break;
        }
    }

    return reinterpret_cast<uint64_t>(device);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceMantle(uint64_t device)
{
    GR_DEVICE grDevice = reinterpret_cast<GR_DEVICE>(device);
    if (grDevice == GR_NULL_HANDLE)
        return;

    for (int i = 0; i < NUM_BACK_BUFFERS; ++i)
    {
        GR_FENCE fence = g_fences[i];
        grDestroyObject(fence);
    }

    grDestroyDevice(grDevice);
}
//------------------------------------------------------------------------------
bool xxResetDeviceMantle(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
bool xxTestDeviceMantle(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameMantle()
{
    return "Mantle";
}
//==============================================================================
//  Framebuffer
//==============================================================================
struct FRAMEBUFFERGR
{
    GR_IMAGE                colorImage;
    GR_GPU_MEMORY           colorMemory;
    GR_COLOR_TARGET_VIEW    colorTargetView;
    GR_IMAGE                depthStencilImage;
    GR_GPU_MEMORY           depthStencilMemory;
    GR_COLOR_TARGET_VIEW    depthStencilTargetView;
};
//==============================================================================
//  Swapchain
//==============================================================================
struct SWAPCHAINGR : public FRAMEBUFFERGR
{
    GR_CMD_BUFFER   commandBuffers[NUM_BACK_BUFFERS];
    int             commandBufferIndex;
    void*           view;
    int             width;
    int             height;
};
//------------------------------------------------------------------------------
uint64_t xxCreateSwapchainMantle(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain)
{
    GR_DEVICE grDevice = reinterpret_cast<GR_DEVICE>(device);
    if (grDevice == GR_NULL_HANDLE)
        return 0;
    SWAPCHAINGR* grOldSwapchain = reinterpret_cast<SWAPCHAINGR*>(oldSwapchain);
    if (grOldSwapchain && grOldSwapchain->view == view && grOldSwapchain->width == width && grOldSwapchain->height == height)
        return oldSwapchain;
    SWAPCHAINGR* grSwapchain = xxAlloc(SWAPCHAINGR, 1);
    if (grSwapchain == nullptr)
        return 0;

    xxDestroySwapchainMantle(oldSwapchain);

    HWND hWnd = (HWND)view;

    GR_WSI_WIN_PRESENTABLE_IMAGE_CREATE_INFO imageCreateInfo = {};
    imageCreateInfo.format.channelFormat = GR_CH_FMT_B8G8R8A8;
    imageCreateInfo.format.numericFormat = GR_NUM_FMT_UNORM;
    imageCreateInfo.usage = GR_IMAGE_USAGE_COLOR_TARGET;
    imageCreateInfo.extent.width = width;
    imageCreateInfo.extent.height = height;

    GR_IMAGE colorImage = GR_NULL_HANDLE;
    GR_GPU_MEMORY colorMemory = GR_NULL_HANDLE;
    if (grWsiWinCreatePresentableImage(grDevice, &imageCreateInfo, &colorImage, &colorMemory) != GR_SUCCESS)
        return 0;

    GR_COLOR_TARGET_VIEW_CREATE_INFO colorTargetViewCreateInfo = {};
    colorTargetViewCreateInfo.image = colorImage;
    colorTargetViewCreateInfo.arraySize = 1;
    colorTargetViewCreateInfo.baseArraySlice = 0;
    colorTargetViewCreateInfo.mipLevel = 0;
    colorTargetViewCreateInfo.format.channelFormat = GR_CH_FMT_B8G8R8A8;
    colorTargetViewCreateInfo.format.numericFormat = GR_NUM_FMT_UNORM;

    GR_COLOR_TARGET_VIEW colorTargetView = GR_NULL_HANDLE;
    grCreateColorTargetView(grDevice, &colorTargetViewCreateInfo, &colorTargetView);

    GR_MEMORY_REF imageMemoryRef = {};
    imageMemoryRef.mem = colorMemory;

    GR_IMAGE_SUBRESOURCE_RANGE imageColorRange = {};
    imageColorRange.aspect = GR_IMAGE_ASPECT_COLOR;
    imageColorRange.baseMipLevel = 0;
    imageColorRange.mipLevels = 1;
    imageColorRange.baseArraySlice = 0;
    imageColorRange.arraySize = 1;

    GR_CMD_BUFFER_CREATE_INFO bufferCreateInfo = {};
    bufferCreateInfo.queueType = GR_QUEUE_UNIVERSAL;

    GR_CMD_BUFFER initCmdBuffer;
    grCreateCommandBuffer(grDevice, &bufferCreateInfo, &initCmdBuffer);
    grBeginCommandBuffer(initCmdBuffer, GR_CMD_BUFFER_OPTIMIZE_ONE_TIME_SUBMIT);

    GR_IMAGE_STATE_TRANSITION initTransition = {};
    initTransition.image = colorImage;
    initTransition.oldState = GR_IMAGE_STATE_UNINITIALIZED;
    initTransition.newState = GR_WSI_WIN_IMAGE_STATE_PRESENT_WINDOWED;
    initTransition.subresourceRange = imageColorRange;
    grCmdPrepareImages(initCmdBuffer, 1, &initTransition);

    grEndCommandBuffer(initCmdBuffer);

    GR_FENCE fence = g_fences[g_fenceIndex];
    grQueueSubmit(g_queue, 1, &initCmdBuffer, 1, &imageMemoryRef, fence);
    grWaitForFences(grDevice, 1, &fence, GR_TRUE, 2.0f);
    grDestroyObject(initCmdBuffer);

    g_fenceIndex++;
    if (g_fenceIndex >= NUM_BACK_BUFFERS)
        g_fenceIndex = 0;

    grSwapchain->colorImage = colorImage;
    grSwapchain->colorMemory = colorMemory;
    grSwapchain->colorTargetView = colorTargetView;
    for (int i = 0; i < NUM_BACK_BUFFERS; ++i)
    {
        GR_CMD_BUFFER commandBuffer;
        grCreateCommandBuffer(grDevice, &bufferCreateInfo, &commandBuffer);
        grSwapchain->commandBuffers[i] = commandBuffer;
    }
    grSwapchain->commandBufferIndex = 0;
    grSwapchain->view = view;
    grSwapchain->width = width;
    grSwapchain->height = height;

    return reinterpret_cast<uint64_t>(grSwapchain);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainMantle(uint64_t swapchain)
{
    SWAPCHAINGR* grSwapchain = reinterpret_cast<SWAPCHAINGR*>(swapchain);
    if (grSwapchain == nullptr)
        return;

    xxFree(grSwapchain);
}
//------------------------------------------------------------------------------
void xxPresentSwapchainMantle(uint64_t swapchain)
{
    SWAPCHAINGR* grSwapchain = reinterpret_cast<SWAPCHAINGR*>(swapchain);
    if (grSwapchain == nullptr)
        return;

    GR_WSI_WIN_PRESENT_INFO presentInfo = {};
    presentInfo.hWndDest = (HWND)grSwapchain->view;
    presentInfo.srcImage = grSwapchain->colorImage;
    presentInfo.presentMode = GR_WSI_WIN_PRESENT_MODE_WINDOWED;

    grWsiWinQueuePresent(g_queue, &presentInfo);

    grSwapchain->commandBufferIndex++;
    if (grSwapchain->commandBufferIndex >= NUM_BACK_BUFFERS)
        grSwapchain->commandBufferIndex = 0;
}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferMantle(uint64_t device, uint64_t swapchain)
{
    GR_DEVICE grDevice = reinterpret_cast<GR_DEVICE>(device);
    if (grDevice == GR_NULL_HANDLE)
        return 0;
    SWAPCHAINGR* grSwapchain = reinterpret_cast<SWAPCHAINGR*>(swapchain);
    if (grSwapchain == nullptr)
        return 0;

    GR_FENCE fence = g_fences[g_fenceIndex];
    grWaitForFences(grDevice, 1, &fence, GR_TRUE, 1.0f);

    g_fenceIndex++;
    if (g_fenceIndex >= NUM_BACK_BUFFERS)
        g_fenceIndex = 0;

    GR_CMD_BUFFER commandBuffer = grSwapchain->commandBuffers[grSwapchain->commandBufferIndex];
    grResetCommandBuffer(commandBuffer);

    return reinterpret_cast<uint64_t>(commandBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferMantle(uint64_t device, uint64_t swapchain, float* scale)
{
    if (scale)
    {
        (*scale) = 1.0f;
    }

    FRAMEBUFFERGR* grFramebuffer = reinterpret_cast<SWAPCHAINGR*>(swapchain);
    if (grFramebuffer == nullptr)
        return 0;

    return reinterpret_cast<uint64_t>(grFramebuffer);
}
//==============================================================================
//  Command Buffer
//==============================================================================
bool xxBeginCommandBufferMantle(uint64_t commandBuffer)
{
    GR_CMD_BUFFER grCommandBuffer = reinterpret_cast<GR_CMD_BUFFER>(commandBuffer);
    if (grCommandBuffer == GR_NULL_HANDLE)
        return false;

    grBeginCommandBuffer(grCommandBuffer, 0);

    return true;
}
//------------------------------------------------------------------------------
void xxEndCommandBufferMantle(uint64_t commandBuffer)
{
    GR_CMD_BUFFER grCommandBuffer = reinterpret_cast<GR_CMD_BUFFER>(commandBuffer);

    grEndCommandBuffer(grCommandBuffer);
}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferMantle(uint64_t commandBuffer, uint64_t swapchain)
{
    GR_CMD_BUFFER grCommandBuffer = reinterpret_cast<GR_CMD_BUFFER>(commandBuffer);

    grQueueSubmit(g_queue, 1, &grCommandBuffer, 0, nullptr, g_fences[g_fenceIndex]);
}
//==============================================================================
//  Render Pass
//==============================================================================
union RENDERPASSGR
{
    uint64_t    value;
    struct
    {
        bool    clearColor;
        bool    clearDepthStencil;
    };
};
//------------------------------------------------------------------------------
uint64_t xxCreateRenderPassMantle(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
{
    RENDERPASSGR grRenderPass = {};

    if (clearColor)
        grRenderPass.clearColor = true;
    if (clearDepth || clearStencil)
        grRenderPass.clearDepthStencil = true;

    return grRenderPass.value;
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassMantle(uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassMantle(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    GR_CMD_BUFFER grCommandBuffer = reinterpret_cast<GR_CMD_BUFFER>(commandBuffer);
    if (grCommandBuffer == GR_NULL_HANDLE)
        return 0;
    FRAMEBUFFERGR* grFramebuffer = reinterpret_cast<SWAPCHAINGR*>(framebuffer);
    if (grFramebuffer == nullptr)
        return 0;
    RENDERPASSGR grRenderPass = { renderPass };

    if (grRenderPass.clearColor)
    {
        grCmdClearColorImage(grCommandBuffer, grFramebuffer->colorImage, color, 0, nullptr);
    }
    if (grRenderPass.clearDepthStencil)
    {
        grCmdClearDepthStencil(grCommandBuffer, grFramebuffer->depthStencilImage, depth, stencil, 0, nullptr);
    }

    return commandBuffer;
}
//------------------------------------------------------------------------------
void xxEndRenderPassMantle(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{

}
//==============================================================================
//  Vertex Attribute
//==============================================================================
struct VERTEXATTRIBUTEGR
{
    GR_MEMORY_VIEW_ATTACH_INFO  infos[16];
    int                         count;
    int                         stride;
};
//------------------------------------------------------------------------------
uint64_t xxCreateVertexAttributeMantle(uint64_t device, int count, int* attribute)
{
    VERTEXATTRIBUTEGR* grVertexAttribute = xxAlloc(VERTEXATTRIBUTEGR, 1);
    if (grVertexAttribute == nullptr)
        return 0;

    GR_MEMORY_VIEW_ATTACH_INFO* infos = grVertexAttribute->infos;
    int stride = 0;

    for (int i = 0; i < count; ++i)
    {
        int stream = (*attribute++);
        int offset = (*attribute++);
        int element = (*attribute++);
        int size = (*attribute++);

        stride += size;

        GR_MEMORY_VIEW_ATTACH_INFO& info = infos[i];

        info.mem = GR_NULL_HANDLE;
        info.offset = offset;
        info.range = size;
        info.stride = 0;
        info.format.channelFormat = GR_CH_FMT_R32;
        info.format.numericFormat = GR_NUM_FMT_FLOAT;
        info.state = GR_MEMORY_STATE_GRAPHICS_SHADER_READ_ONLY;

        if (element == 3 && size == sizeof(float) * 3)
        {
            info.format.channelFormat = GR_CH_FMT_R32G32B32;
            continue;
        }

        if (element == 4 && size == sizeof(char) * 4)
        {
            info.format.channelFormat = GR_CH_FMT_B8G8R8A8;
            info.format.numericFormat = GR_NUM_FMT_UNORM;
            continue;
        }

        if (element == 2 && size == sizeof(float) * 2)
        {
            info.format.channelFormat = GR_CH_FMT_R32G32;
            continue;
        }
    }

    for (int i = 0; i < count; ++i)
    {
        infos[i].stride = stride;
    }

    grVertexAttribute->count = count;
    grVertexAttribute->stride = stride;

    return reinterpret_cast<uint64_t>(grVertexAttribute);
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeMantle(uint64_t vertexAttribute)
{
    VERTEXATTRIBUTEGR* grVertexAttribute = reinterpret_cast<VERTEXATTRIBUTEGR*>(vertexAttribute);

    xxFree(grVertexAttribute);
}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateConstantBufferMantle(uint64_t device, int size)
{
    GR_DEVICE grDevice = reinterpret_cast<GR_DEVICE>(device);
    if (grDevice == GR_NULL_HANDLE)
        return 0;

    intptr_t pageSize = (intptr_t)g_heapProps.pageSize;

    GR_MEMORY_ALLOC_INFO allocInfo = {};
    allocInfo.size = ((1 + size) / pageSize) * pageSize;
    allocInfo.alignment = 0;
    allocInfo.memPriority = GR_MEMORY_PRIORITY_HIGH;
    allocInfo.heapCount = 1;
    allocInfo.heaps[0] = g_suitableHeap;

    GR_GPU_MEMORY memory = GR_NULL_HANDLE;
    grAllocMemory(grDevice, &allocInfo, &memory);

    return reinterpret_cast<uint64_t>(memory);
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferMantle(uint64_t device, int size)
{
    GR_DEVICE grDevice = reinterpret_cast<GR_DEVICE>(device);
    if (grDevice == GR_NULL_HANDLE)
        return 0;

    intptr_t pageSize = (intptr_t)g_heapProps.pageSize;

    GR_MEMORY_ALLOC_INFO allocInfo = {};
    allocInfo.size = ((1 + size) / pageSize) * pageSize;
    allocInfo.alignment = 0;
    allocInfo.memPriority = GR_MEMORY_PRIORITY_HIGH;
    allocInfo.heapCount = 1;
    allocInfo.heaps[0] = g_suitableHeap;

    GR_GPU_MEMORY memory = GR_NULL_HANDLE;
    grAllocMemory(grDevice, &allocInfo, &memory);

    return reinterpret_cast<uint64_t>(memory);
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferMantle(uint64_t device, int size, uint64_t vertexAttribute)
{
    GR_DEVICE grDevice = reinterpret_cast<GR_DEVICE>(device);
    if (grDevice == GR_NULL_HANDLE)
        return 0;

    intptr_t pageSize = (intptr_t)g_heapProps.pageSize;

    GR_MEMORY_ALLOC_INFO allocInfo = {};
    allocInfo.size = ((1 + size) / pageSize) * pageSize;
    allocInfo.alignment = 0;
    allocInfo.memPriority = GR_MEMORY_PRIORITY_HIGH;
    allocInfo.heapCount = 1;
    allocInfo.heaps[0] = g_suitableHeap;

    GR_GPU_MEMORY memory = GR_NULL_HANDLE;
    grAllocMemory(grDevice, &allocInfo, &memory);

    return reinterpret_cast<uint64_t>(memory);
}
//------------------------------------------------------------------------------
void xxDestroyBufferMantle(uint64_t device, uint64_t buffer)
{
    GR_GPU_MEMORY memory = reinterpret_cast<GR_GPU_MEMORY>(buffer);
    if (memory == GR_NULL_HANDLE)
        return;

    grFreeMemory(memory);
}
//------------------------------------------------------------------------------
void* xxMapBufferMantle(uint64_t device, uint64_t buffer)
{
    GR_GPU_MEMORY memory = reinterpret_cast<GR_GPU_MEMORY>(buffer);
    if (memory == GR_NULL_HANDLE)
        return nullptr;

    void* ptr = nullptr;
    grMapMemory(memory, 0, &ptr);

    return ptr;
}
//------------------------------------------------------------------------------
void xxUnmapBufferMantle(uint64_t device, uint64_t buffer)
{
    GR_GPU_MEMORY memory = reinterpret_cast<GR_GPU_MEMORY>(buffer);
    if (memory == GR_NULL_HANDLE)
        return;

    grUnmapMemory(memory);
}
//==============================================================================
//  Texture
//==============================================================================
uint64_t xxCreateTextureMantle(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, const void* external)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyTextureMantle(uint64_t texture)
{

}
//------------------------------------------------------------------------------
void* xxMapTextureMantle(uint64_t device, uint64_t texture, int* stride, int level, int array)
{
    return nullptr;
}
//------------------------------------------------------------------------------
void xxUnmapTextureMantle(uint64_t device, uint64_t texture, int level, int array)
{

}
//==============================================================================
//  Sampler
//==============================================================================
uint64_t xxCreateSamplerMantle(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    GR_DEVICE grDevice = reinterpret_cast<GR_DEVICE>(device);
    if (grDevice == GR_NULL_HANDLE)
        return 0;

    const GR_TEX_FILTER filters[2][2][2] =
    {
        {
            { GR_TEX_FILTER_MAG_POINT_MIN_POINT_MIP_POINT, GR_TEX_FILTER_MAG_POINT_MIN_POINT_MIP_LINEAR },
            { GR_TEX_FILTER_MAG_POINT_MIN_LINEAR_MIP_POINT, GR_TEX_FILTER_MAG_POINT_MIN_LINEAR_MIP_LINEAR },
        },  {
            { GR_TEX_FILTER_MAG_LINEAR_MIN_POINT_MIP_POINT, GR_TEX_FILTER_MAG_LINEAR_MIN_POINT_MIP_LINEAR },
            { GR_TEX_FILTER_MAG_LINEAR_MIN_LINEAR_MIP_POINT, GR_TEX_FILTER_MAG_LINEAR_MIN_LINEAR_MIP_LINEAR },
        }
    };

    GR_SAMPLER_CREATE_INFO info = {};
    info.filter = filters[linearMag][linearMin][linearMip];
    info.addressU = clampU ? GR_TEX_ADDRESS_CLAMP : GR_TEX_ADDRESS_WRAP;
    info.addressV = clampV ? GR_TEX_ADDRESS_CLAMP : GR_TEX_ADDRESS_WRAP;
    info.addressW = clampW ? GR_TEX_ADDRESS_CLAMP : GR_TEX_ADDRESS_WRAP;
    info.maxAnisotropy = anisotropy;
    info.compareFunc = GR_COMPARE_ALWAYS;

    GR_SAMPLER sampler = GR_NULL_HANDLE;
    grCreateSampler(grDevice, &info, &sampler);

    return reinterpret_cast<uint64_t>(sampler);
}
//------------------------------------------------------------------------------
void xxDestroySamplerMantle(uint64_t sampler)
{
    GR_SAMPLER grSampler = reinterpret_cast<GR_SAMPLER>(sampler);
    if (grSampler == GR_NULL_HANDLE)
        return;

    grDestroyObject(grSampler);
}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderMantle(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    GR_DEVICE grDevice = reinterpret_cast<GR_DEVICE>(device);
    if (grDevice == GR_NULL_HANDLE)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        GR_SHADER_CREATE_INFO info = {};
        info.pCode = vertexShaderCodeIL;
        info.codeSize = sizeof(vertexShaderCodeIL);

        GR_SHADER grShader = GR_NULL_HANDLE;
        grCreateShader(grDevice, &info, &grShader);

        return reinterpret_cast<uint64_t>(grShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderMantle(uint64_t device, const char* shader)
{
    GR_DEVICE grDevice = reinterpret_cast<GR_DEVICE>(device);
    if (grDevice == GR_NULL_HANDLE)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        GR_SHADER_CREATE_INFO info = {};
        info.pCode = pixelShaderCodeIL;
        info.codeSize = sizeof(pixelShaderCodeIL);

        GR_SHADER grShader = GR_NULL_HANDLE;
        grCreateShader(grDevice, &info, &grShader);

        return reinterpret_cast<uint64_t>(grShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderMantle(uint64_t device, uint64_t shader)
{
    GR_SHADER grShader = reinterpret_cast<GR_SHADER>(shader);
    if (grShader == GR_NULL_HANDLE)
        return;

    grDestroyObject(grShader);
}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreateBlendStateMantle(uint64_t device, bool blending)
{
    GR_DEVICE grDevice = reinterpret_cast<GR_DEVICE>(device);
    if (grDevice == GR_NULL_HANDLE)
        return 0;

    GR_COLOR_BLEND_STATE_CREATE_INFO info = {};
    info.target[0].blendEnable = blending ? GR_TRUE : GR_FALSE;
    info.target[0].srcBlendColor = GR_BLEND_SRC_ALPHA;
    info.target[0].destBlendColor = GR_BLEND_ONE_MINUS_SRC_ALPHA;
    info.target[0].blendFuncColor = GR_BLEND_FUNC_ADD;
    info.target[0].srcBlendAlpha = GR_BLEND_SRC_ALPHA;
    info.target[0].destBlendAlpha = GR_BLEND_ONE_MINUS_SRC_ALPHA;
    info.target[0].blendFuncAlpha = GR_BLEND_FUNC_ADD;

    GR_COLOR_BLEND_STATE_OBJECT blendState = GR_NULL_HANDLE;
    grCreateColorBlendState(grDevice, &info, &blendState);

    return reinterpret_cast<uint64_t>(blendState);
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateMantle(uint64_t device, bool depthTest, bool depthWrite)
{
    GR_DEVICE grDevice = reinterpret_cast<GR_DEVICE>(device);
    if (grDevice == GR_NULL_HANDLE)
        return 0;

    GR_DEPTH_STENCIL_STATE_CREATE_INFO info = {};
    info.depthEnable = depthTest ? GR_TRUE : GR_FALSE;
    info.depthWriteEnable = depthWrite ? GR_TRUE : GR_FALSE;
    info.depthFunc = GR_COMPARE_LESS_EQUAL;

    GR_DEPTH_STENCIL_STATE_OBJECT depthStencilState = GR_NULL_HANDLE;
    grCreateDepthStencilState(grDevice, &info, &depthStencilState);

    return reinterpret_cast<uint64_t>(depthStencilState);
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateMantle(uint64_t device, bool cull, bool scissor)
{
    GR_DEVICE grDevice = reinterpret_cast<GR_DEVICE>(device);
    if (grDevice == GR_NULL_HANDLE)
        return 0;

    GR_RASTER_STATE_CREATE_INFO info = {};
    info.fillMode = GR_FILL_SOLID;
    info.cullMode = GR_CULL_NONE;
    info.frontFace = GR_FRONT_FACE_CCW;

    GR_RASTER_STATE_OBJECT rasterizerState = GR_NULL_HANDLE;
    grCreateRasterState(grDevice, &info, &rasterizerState);

    return reinterpret_cast<uint64_t>(rasterizerState);
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineMantle(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    GR_DEVICE grDevice = reinterpret_cast<GR_DEVICE>(device);
    if (grDevice == GR_NULL_HANDLE)
        return 0;
    GR_SHADER grVertexShader = reinterpret_cast<GR_SHADER>(vertexShader);
    if (grVertexShader == GR_NULL_HANDLE)
        return 0;
    GR_SHADER grPixelShader = reinterpret_cast<GR_SHADER>(fragmentShader);
    if (grPixelShader == GR_NULL_HANDLE)
        return 0;

    GR_DESCRIPTOR_SLOT_INFO slotInfo = {};
    slotInfo.slotObjectType = GR_SLOT_SHADER_RESOURCE;
    slotInfo.shaderEntityIndex = 0;

    GR_GRAPHICS_PIPELINE_CREATE_INFO pipelineInfo = {};
    pipelineInfo.vs.shader = grVertexShader;
    pipelineInfo.vs.descriptorSetMapping[0].descriptorCount = 1;
    pipelineInfo.vs.descriptorSetMapping[0].pDescriptorInfo = &slotInfo;
    pipelineInfo.vs.dynamicMemoryViewMapping.slotObjectType = GR_SLOT_UNUSED;
    pipelineInfo.ps.shader = grPixelShader;
    pipelineInfo.ps.dynamicMemoryViewMapping.slotObjectType = GR_SLOT_UNUSED;

    pipelineInfo.iaState.topology = GR_TOPOLOGY_TRIANGLE_LIST;
    pipelineInfo.rsState.depthClipEnable = GR_FALSE;
    pipelineInfo.cbState.logicOp = GR_LOGIC_OP_COPY;
    pipelineInfo.cbState.target[0].blendEnable = GR_FALSE;
    pipelineInfo.cbState.target[0].channelWriteMask = 0xF;
    pipelineInfo.cbState.target[0].format.channelFormat = GR_CH_FMT_B8G8R8A8;
    pipelineInfo.cbState.target[0].format.numericFormat = GR_NUM_FMT_UNORM;
    pipelineInfo.dbState.format.channelFormat = GR_CH_FMT_R32G8;
    pipelineInfo.dbState.format.numericFormat = GR_NUM_FMT_DS;

    GR_PIPELINE pipeline = GR_NULL_HANDLE;
    grCreateGraphicsPipeline(grDevice, &pipelineInfo, &pipeline);

    return reinterpret_cast<uint64_t>(pipeline);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateMantle(uint64_t blendState)
{
    GR_COLOR_BLEND_STATE_OBJECT grBlendState = reinterpret_cast<GR_COLOR_BLEND_STATE_OBJECT>(blendState);
    if (grBlendState == GR_NULL_HANDLE)
        return;

    grDestroyObject(grBlendState);
}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateMantle(uint64_t depthStencilState)
{
    GR_DEPTH_STENCIL_STATE_OBJECT grDepthStencilState = reinterpret_cast<GR_DEPTH_STENCIL_STATE_OBJECT>(depthStencilState);
    if (grDepthStencilState == GR_NULL_HANDLE)
        return;

    grDestroyObject(grDepthStencilState);
}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateMantle(uint64_t rasterizerState)
{
    GR_RASTER_STATE_OBJECT grRasterizerState = reinterpret_cast<GR_PIPELINE>(rasterizerState);
    if (grRasterizerState == GR_NULL_HANDLE)
        return;

    grDestroyObject(grRasterizerState);
}
//------------------------------------------------------------------------------
void xxDestroyPipelineMantle(uint64_t pipeline)
{
    GR_PIPELINE grPipeline = reinterpret_cast<GR_PIPELINE>(pipeline);
    if (grPipeline == GR_NULL_HANDLE)
        return;

    grDestroyObject(grPipeline);
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportMantle(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{

}
//------------------------------------------------------------------------------
void xxSetScissorMantle(uint64_t commandEncoder, int x, int y, int width, int height)
{

}
//------------------------------------------------------------------------------
void xxSetPipelineMantle(uint64_t commandEncoder, uint64_t pipeline)
{
    GR_CMD_BUFFER grCommandBuffer = reinterpret_cast<GR_CMD_BUFFER>(commandEncoder);
    GR_PIPELINE grPipeline = reinterpret_cast<GR_PIPELINE>(pipeline);

    grCmdBindPipeline(grCommandBuffer, GR_PIPELINE_BIND_POINT_GRAPHICS, grPipeline);
}
//------------------------------------------------------------------------------
void xxSetIndexBufferMantle(uint64_t commandEncoder, uint64_t buffer)
{
    GR_CMD_BUFFER grCommandBuffer = reinterpret_cast<GR_CMD_BUFFER>(commandEncoder);
    GR_GPU_MEMORY grMemory = reinterpret_cast<GR_GPU_MEMORY>(buffer);

    grCmdBindIndexData(grCommandBuffer, grMemory, 0, GR_INDEX_32);
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersMantle(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    VERTEXATTRIBUTEGR* grVertexAttribute = reinterpret_cast<VERTEXATTRIBUTEGR*>(vertexAttribute);
    GR_MEMORY_VIEW_ATTACH_INFO* infos = grVertexAttribute->infos;

    for (int i = 0; i < count; ++i)
    {
        GR_GPU_MEMORY grMemory = reinterpret_cast<GR_GPU_MEMORY>(buffers[i]);
        GR_MEMORY_VIEW_ATTACH_INFO& info = infos[i];
        info.mem = grMemory;
    }

    grAttachMemoryViewDescriptors(0, BASE_VERTEX_ATTRIBUTE, count, infos);
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesMantle(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    GR_IMAGE_VIEW_ATTACH_INFO infos[8];

    for (int i = 0; i < count; ++i)
    {
        GR_IMAGE_VIEW_ATTACH_INFO& info = infos[i];
        info.view;
        info.state = GR_MEMORY_STATE_GRAPHICS_SHADER_READ_ONLY;
    }

    grAttachImageViewDescriptors(0, BASE_VERTEX_TEXTURE, count, infos);
}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesMantle(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    GR_IMAGE_VIEW_ATTACH_INFO infos[8];

    for (int i = 0; i < count; ++i)
    {
        GR_IMAGE_VIEW_ATTACH_INFO& info = infos[i];
        info.view;
        info.state = GR_MEMORY_STATE_GRAPHICS_SHADER_READ_ONLY;
    }

    grAttachImageViewDescriptors(0, BASE_PIXEL_TEXTURE, count, infos);
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersMantle(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    GR_SAMPLER grSamplers[8];

    for (int i = 0; i < count; ++i)
    {
        GR_SAMPLER grSampler = reinterpret_cast<GR_SAMPLER>(samplers[i]);
        grSamplers[i] = grSampler;
    }

    grAttachSamplerDescriptors(0, BASE_VERTEX_SAMPLER, count, grSamplers);
}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersMantle(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    GR_SAMPLER grSamplers[8];

    for (int i = 0; i < count; ++i)
    {
        GR_SAMPLER grSampler = reinterpret_cast<GR_SAMPLER>(samplers[i]);
        grSamplers[i] = grSampler;
    }

    grAttachSamplerDescriptors(0, BASE_PIXEL_SAMPLER, count, grSamplers);
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferMantle(uint64_t commandEncoder, uint64_t buffer, int size)
{
    GR_GPU_MEMORY grMemory = reinterpret_cast<GR_GPU_MEMORY>(buffer);

    GR_MEMORY_VIEW_ATTACH_INFO info = {};
    info.mem = grMemory;
    info.offset = 0;
    info.range = size;
    info.stride = 0;
    info.format.channelFormat = GR_CH_FMT_R32G32B32A32;
    info.format.numericFormat = GR_NUM_FMT_FLOAT;
    info.state = GR_MEMORY_STATE_GRAPHICS_SHADER_READ_ONLY;

    grAttachMemoryViewDescriptors(0, BASE_VERTEX_CONSTANT, 1, &info);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferMantle(uint64_t commandEncoder, uint64_t buffer, int size)
{
    GR_GPU_MEMORY grMemory = reinterpret_cast<GR_GPU_MEMORY>(buffer);

    GR_MEMORY_VIEW_ATTACH_INFO info = {};
    info.mem = grMemory;
    info.offset = 0;
    info.range = size;
    info.stride = 0;
    info.format.channelFormat = GR_CH_FMT_R32G32B32A32;
    info.format.numericFormat = GR_NUM_FMT_FLOAT;
    info.state = GR_MEMORY_STATE_GRAPHICS_SHADER_READ_ONLY;

    grAttachMemoryViewDescriptors(0, BASE_PIXEL_CONSTANT, 1, &info);
}
//------------------------------------------------------------------------------
void xxDrawIndexedMantle(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    GR_CMD_BUFFER grCommandBuffer = reinterpret_cast<GR_CMD_BUFFER>(commandEncoder);

    grCmdDrawIndexed(grCommandBuffer, firstIndex, indexCount, vertexOffset, firstInstance, instanceCount);
}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetTransformMantle(uint64_t commandEncoder, const float* world, const float* view, const float* projection)
{

}
//==============================================================================
//  Loader
//==============================================================================
#define GR_PROTOTYPE(type, prototype, parameter, ...) \
extern "C" type prototype parameter \
{ \
    static type (GR_STDCALL* prototype ## Entry) parameter = nullptr; \
    if (prototype ## Entry == nullptr) \
    { \
        (void*&)prototype ## Entry = grSymbolImpl(#prototype); \
        if (prototype ## Entry == nullptr) \
        { \
            return type(); \
        } \
    } \
    return prototype ## Entry(__VA_ARGS__); \
}
//------------------------------------------------------------------------------
GR_PROTOTYPE(GR_RESULT, grInitAndEnumerateGpus, (const GR_APPLICATION_INFO* pAppInfo, const GR_ALLOC_CALLBACKS* pAllocCb, GR_UINT* pGpuCount, GR_PHYSICAL_GPU gpus[GR_MAX_PHYSICAL_GPUS]), pAppInfo, pAllocCb, pGpuCount, gpus);
GR_PROTOTYPE(GR_RESULT, grGetGpuInfo, (GR_PHYSICAL_GPU gpu, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData), gpu, infoType, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grCreateDevice, (GR_PHYSICAL_GPU gpu, const GR_DEVICE_CREATE_INFO* pCreateInfo, GR_DEVICE* pDevice), gpu, pCreateInfo, pDevice);
GR_PROTOTYPE(GR_RESULT, grDestroyDevice, (GR_DEVICE device), device);
GR_PROTOTYPE(GR_RESULT, grGetExtensionSupport, (GR_PHYSICAL_GPU gpu, const GR_CHAR* pExtName), gpu, pExtName);
GR_PROTOTYPE(GR_RESULT, grGetDeviceQueue, (GR_DEVICE device, GR_ENUM queueType, GR_UINT queueId, GR_QUEUE* pQueue), device, queueType, queueId, pQueue);
GR_PROTOTYPE(GR_RESULT, grQueueWaitIdle, (GR_QUEUE queue), queue);
GR_PROTOTYPE(GR_RESULT, grDeviceWaitIdle, (GR_QUEUE queue), queue);
GR_PROTOTYPE(GR_RESULT, grQueueSubmit, (GR_QUEUE queue, GR_UINT cmdBufferCount, const GR_CMD_BUFFER* pCmdBuffers, GR_UINT memRefCount, const GR_MEMORY_REF* pMemRefs, GR_FENCE fence), queue, cmdBufferCount, pCmdBuffers, memRefCount, pMemRefs, fence);
GR_PROTOTYPE(GR_RESULT, grQueueSetGlobalMemReferences, (GR_QUEUE queue, GR_UINT memRefCount, const GR_MEMORY_REF* pMemRefs), queue, memRefCount, pMemRefs);
GR_PROTOTYPE(GR_RESULT, grGetMemoryHeapCount, (GR_DEVICE device, GR_UINT* pCount), device, pCount);
GR_PROTOTYPE(GR_RESULT, grGetMemoryHeapInfo, (GR_DEVICE device, GR_UINT heapId, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData), device, heapId, infoType, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grAllocMemory, (GR_DEVICE device, const GR_MEMORY_ALLOC_INFO* pAllocInfo, GR_GPU_MEMORY* pMem), device, pAllocInfo, pMem);
GR_PROTOTYPE(GR_RESULT, grFreeMemory, (GR_GPU_MEMORY mem), mem);
GR_PROTOTYPE(GR_RESULT, grSetMemoryPriority, (GR_GPU_MEMORY mem, GR_ENUM priority), mem, priority);
GR_PROTOTYPE(GR_RESULT, grMapMemory, (GR_GPU_MEMORY mem, GR_FLAGS flags, GR_VOID** ppData), mem, flags, ppData);
GR_PROTOTYPE(GR_RESULT, grUnmapMemory, (GR_GPU_MEMORY mem), mem);
GR_PROTOTYPE(GR_RESULT, grRemapVirtualMemoryPages, (GR_DEVICE device, GR_UINT rangeCount, const GR_VIRTUAL_MEMORY_REMAP_RANGE* pRanges, GR_UINT preWaitSemaphoreCount, const GR_QUEUE_SEMAPHORE* pPreWatSemaphores, GR_UINT postSignalSemaphoreCount, const GR_QUEUE_SEMAPHORE* pPostSignalSemaphores), device, rangeCount, pRanges, preWaitSemaphoreCount, pPreWatSemaphores, postSignalSemaphoreCount, pPostSignalSemaphores);
GR_PROTOTYPE(GR_RESULT, grPinSystemMemory, (GR_DEVICE device, const GR_VOID* pSysMem, GR_SIZE memSize, GR_GPU_MEMORY* pMem), device, pSysMem, memSize, pMem);
GR_PROTOTYPE(GR_RESULT, grDestroyObject, (GR_OBJECT object), object);
GR_PROTOTYPE(GR_RESULT, grGetObjectInfo, (GR_BASE_OBJECT object, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData), object, infoType, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grBindObjectMemory, (GR_OBJECT object, GR_GPU_MEMORY mem, GR_GPU_SIZE offset), object, mem, offset);
GR_PROTOTYPE(GR_RESULT, grGetFormatInfo, (GR_DEVICE device, GR_FORMAT format, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData), device, format, infoType, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grCreateImage, (GR_DEVICE device, const GR_IMAGE_CREATE_INFO* pCreateInfo, GR_IMAGE* pImage), device, pCreateInfo, pImage);
GR_PROTOTYPE(GR_RESULT, grGetImageSubresourceInfo, (GR_IMAGE image, const GR_IMAGE_SUBRESOURCE* pSubresource, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData), image, pSubresource, infoType, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grCreateSampler, (GR_DEVICE device, const GR_SAMPLER_CREATE_INFO* pCreateInfo, GR_SAMPLER* pSampler), device, pCreateInfo, pSampler);
GR_PROTOTYPE(GR_RESULT, grCreateImageView, (GR_DEVICE device, const GR_IMAGE_VIEW_CREATE_INFO* pCreateInfo, GR_IMAGE_VIEW* pView), device, pCreateInfo, pView);
GR_PROTOTYPE(GR_RESULT, grCreateColorTargetView, (GR_DEVICE device, const GR_COLOR_TARGET_VIEW_CREATE_INFO* pCreateInfo, GR_COLOR_TARGET_VIEW* pView), device, pCreateInfo, pView);
GR_PROTOTYPE(GR_RESULT, grCreateShader, (GR_DEVICE device, const GR_SHADER_CREATE_INFO* pCreateInfo, GR_SHADER* pShader), device, pCreateInfo, pShader);
GR_PROTOTYPE(GR_RESULT, grCreateGraphicsPipeline, (GR_DEVICE device, const GR_GRAPHICS_PIPELINE_CREATE_INFO* pCreateInfo, GR_PIPELINE* pPipeline), device, pCreateInfo, pPipeline);
GR_PROTOTYPE(GR_RESULT, grCreateComputePipeline, (GR_DEVICE device, const GR_COMPUTE_PIPELINE_CREATE_INFO* pCreateInfo, GR_PIPELINE* pPipeline), device, pCreateInfo, pPipeline);
GR_PROTOTYPE(GR_RESULT, grStorePipeline, (GR_PIPELINE pipeline, GR_SIZE* pDataSize, GR_VOID* pData), pipeline, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grLoadPipeline, (GR_DEVICE device, GR_SIZE dataSize, const GR_VOID* pData, GR_PIPELINE* pPipeline), device, dataSize, pData, pPipeline);
GR_PROTOTYPE(GR_RESULT, grCreateDescriptorSet, (GR_DEVICE device, const GR_DESCRIPTOR_SET_CREATE_INFO* pCreateInfo, GR_DESCRIPTOR_SET* pDescriptorSet), device, pCreateInfo, pDescriptorSet);
GR_PROTOTYPE(GR_VOID, grBeginDescriptorSetUpdate, (GR_DESCRIPTOR_SET descriptorSet), descriptorSet);
GR_PROTOTYPE(GR_VOID, grEndDescriptorSetUpdate, (GR_DESCRIPTOR_SET descriptorSet), descriptorSet);
GR_PROTOTYPE(GR_VOID, grAttachSamplerDescriptors, (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_SAMPLER* pSamplers), descriptorSet, startSlot, slotCount, pSamplers);
GR_PROTOTYPE(GR_VOID, grAttachImageViewDescriptors, (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_IMAGE_VIEW_ATTACH_INFO* pImageViews), descriptorSet, startSlot, slotCount, pImageViews);
GR_PROTOTYPE(GR_VOID, grAttachMemoryViewDescriptors, (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_MEMORY_VIEW_ATTACH_INFO* pMemViews), descriptorSet, startSlot, slotCount, pMemViews);
GR_PROTOTYPE(GR_VOID, grAttachNestedDescriptors, (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_DESCRIPTOR_SET_ATTACH_INFO* pNestedDescriptorSets), descriptorSet, startSlot, slotCount, pNestedDescriptorSets);
GR_PROTOTYPE(GR_VOID, grClearDescriptorSetSlots, (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount), descriptorSet, startSlot, slotCount);
GR_PROTOTYPE(GR_RESULT, grCreateViewportState, (GR_DEVICE device, const GR_VIEWPORT_STATE_CREATE_INFO* pCreateInfo, GR_VIEWPORT_STATE_OBJECT* pState), device, pCreateInfo, pState);
GR_PROTOTYPE(GR_RESULT, grCreateRasterState, (GR_DEVICE device, const GR_RASTER_STATE_CREATE_INFO* pCreateInfo, GR_RASTER_STATE_OBJECT* pState), device, pCreateInfo, pState);
GR_PROTOTYPE(GR_RESULT, grCreateColorBlendState, (GR_DEVICE device, const GR_COLOR_BLEND_STATE_CREATE_INFO* pCreateInfo, GR_COLOR_BLEND_STATE_OBJECT* pState), device, pCreateInfo, pState);
GR_PROTOTYPE(GR_RESULT, grCreateDepthStencilState, (GR_DEVICE device, const GR_DEPTH_STENCIL_STATE_CREATE_INFO* pCreateInfo, GR_DEPTH_STENCIL_STATE_OBJECT* pState), device, pCreateInfo, pState);
GR_PROTOTYPE(GR_RESULT, grCreateMsaaState, (GR_DEVICE device, const GR_MSAA_STATE_CREATE_INFO* pCreateInfo, GR_MSAA_STATE_OBJECT* pState), device, pCreateInfo, pState);
GR_PROTOTYPE(GR_RESULT, grCreateQueryPool, (GR_DEVICE device, const GR_QUERY_POOL_CREATE_INFO* pCreateInfo, GR_QUERY_POOL* pQueryPool), device, pCreateInfo, pQueryPool);
GR_PROTOTYPE(GR_RESULT, grGetQueryPoolResults, (GR_QUERY_POOL queryPool, GR_UINT startQuery, GR_UINT queryCount, GR_SIZE* pDataSize, GR_VOID* pData), queryPool, startQuery, queryCount, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grCreateFence, (GR_DEVICE device, const GR_FENCE_CREATE_INFO* pCreateInfo, GR_FENCE* pFence), device, pCreateInfo, pFence);
GR_PROTOTYPE(GR_RESULT, grGetFenceStatus, (GR_FENCE fence), fence);
GR_PROTOTYPE(GR_RESULT, grWaitForFences, (GR_DEVICE device, GR_UINT fenceCount, const GR_FENCE* pFences, GR_BOOL waitAll, GR_FLOAT timeout), device, fenceCount, pFences, waitAll, timeout);
GR_PROTOTYPE(GR_RESULT, grCreateQueueSemaphore, (GR_DEVICE device, const GR_QUEUE_SEMAPHORE_CREATE_INFO* pCreateInfo, GR_QUEUE_SEMAPHORE* pSemaphore), device, pCreateInfo, pSemaphore);
GR_PROTOTYPE(GR_RESULT, grSignalQueueSemaphore, (GR_QUEUE queue, GR_QUEUE_SEMAPHORE semaphore), queue, semaphore);
GR_PROTOTYPE(GR_RESULT, grWaitQueueSemaphore, (GR_QUEUE queue, GR_QUEUE_SEMAPHORE semaphore), queue, semaphore);
GR_PROTOTYPE(GR_RESULT, grCreateEvent, (GR_DEVICE device, const GR_EVENT_CREATE_INFO* pCreateInfo, GR_EVENT* pEvent), device, pCreateInfo, pEvent);
GR_PROTOTYPE(GR_RESULT, grGetEventStatus, (GR_EVENT event), event);
GR_PROTOTYPE(GR_RESULT, grSetEvent, (GR_EVENT event), event);
GR_PROTOTYPE(GR_RESULT, grResetEvent, (GR_EVENT event), event);
GR_PROTOTYPE(GR_RESULT, grGetMultiGpuCompatibility, (GR_PHYSICAL_GPU gpu0, GR_PHYSICAL_GPU gpu1, GR_GPU_COMPATIBILITY_INFO* pInfo), gpu0, gpu1, pInfo);
GR_PROTOTYPE(GR_RESULT, grOpenSharedMemory, (GR_DEVICE device, const GR_MEMORY_OPEN_INFO* pOpenInfo, GR_GPU_MEMORY* pMem), device, pOpenInfo, pMem);
GR_PROTOTYPE(GR_RESULT, grOpenSharedQueueSemaphore, (GR_DEVICE device, const GR_QUEUE_SEMAPHORE_OPEN_INFO* pOpenInfo, GR_QUEUE_SEMAPHORE* pSemaphore), device, pOpenInfo, pSemaphore);
GR_PROTOTYPE(GR_RESULT, grOpenPeerMemory, (GR_DEVICE device, const GR_PEER_MEMORY_OPEN_INFO* pOpenInfo, GR_GPU_MEMORY* pMem), device, pOpenInfo, pMem);
GR_PROTOTYPE(GR_RESULT, grOpenPeerImage, (GR_DEVICE device, const GR_PEER_IMAGE_OPEN_INFO* pOpenInfo, GR_IMAGE* pImage, GR_GPU_MEMORY* pMem), device, pOpenInfo, pImage, pMem);
GR_PROTOTYPE(GR_RESULT, grCreateCommandBuffer, (GR_DEVICE device, const GR_CMD_BUFFER_CREATE_INFO* pCreateInfo, GR_CMD_BUFFER* pCmdBuffer), device, pCreateInfo, pCmdBuffer);
GR_PROTOTYPE(GR_RESULT, grBeginCommandBuffer, (GR_CMD_BUFFER cmdBuffer, GR_FLAGS flags), cmdBuffer, flags);
GR_PROTOTYPE(GR_RESULT, grEndCommandBuffer, (GR_CMD_BUFFER cmdBuffer), cmdBuffer);
GR_PROTOTYPE(GR_RESULT, grResetCommandBuffer, (GR_CMD_BUFFER cmdBuffer), cmdBuffer);
GR_PROTOTYPE(GR_VOID, grCmdBindPipeline, (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_PIPELINE pipeline), cmdBuffer, pipelineBindPoint, pipeline);
GR_PROTOTYPE(GR_VOID, grCmdBindStateObject, (GR_CMD_BUFFER cmdBuffer, GR_ENUM stateBindPoint, GR_STATE_OBJECT state), cmdBuffer, stateBindPoint, state);
GR_PROTOTYPE(GR_VOID, grCmdBindDescriptorSet, (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_UINT index, GR_DESCRIPTOR_SET descriptorSet, GR_UINT slotOffset), cmdBuffer, pipelineBindPoint, index, descriptorSet, slotOffset);
GR_PROTOTYPE(GR_VOID, grCmdBindDynamicMemoryView, (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, const GR_MEMORY_VIEW_ATTACH_INFO* pMemView), cmdBuffer, pipelineBindPoint, pMemView);
GR_PROTOTYPE(GR_VOID, grCmdBindIndexData, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY mem, GR_GPU_SIZE offset, GR_ENUM indexType), cmdBuffer, mem, offset, indexType);
GR_PROTOTYPE(GR_VOID, grCmdBindTargets, (GR_CMD_BUFFER cmdBuffer, GR_UINT colorTargetCount, const GR_COLOR_TARGET_BIND_INFO* pColorTargets, const GR_DEPTH_STENCIL_BIND_INFO* pDepthTarget), cmdBuffer, colorTargetCount, pColorTargets, pDepthTarget);
GR_PROTOTYPE(GR_VOID, grCmdPrepareMemoryRegions, (GR_CMD_BUFFER cmdBuffer, GR_UINT transitionCount, const GR_MEMORY_STATE_TRANSITION* pStateTransitions), cmdBuffer, transitionCount, pStateTransitions);
GR_PROTOTYPE(GR_VOID, grCmdPrepareImages, (GR_CMD_BUFFER cmdBuffer, GR_UINT transitionCount, const GR_IMAGE_STATE_TRANSITION* pStateTransitions), cmdBuffer, transitionCount, pStateTransitions);
GR_PROTOTYPE(GR_VOID, grCmdDraw, (GR_CMD_BUFFER cmdBuffer, GR_UINT firstVertex, GR_UINT vertexCount, GR_UINT firstInstance, GR_UINT instanceCount), cmdBuffer, firstVertex, vertexCount, firstInstance, instanceCount);
GR_PROTOTYPE(GR_VOID, grCmdDrawIndexed, (GR_CMD_BUFFER cmdBuffer, GR_UINT firstIndex, GR_UINT indexCount, GR_INT vertexOffset, GR_UINT firstInstance, GR_UINT instanceCount), cmdBuffer, firstIndex, indexCount, vertexOffset, firstInstance, instanceCount);
GR_PROTOTYPE(GR_VOID, grCmdDrawIndirect, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY mem, GR_GPU_SIZE offset), cmdBuffer, mem, offset);
GR_PROTOTYPE(GR_VOID, grCmdDrawIndexedIndirect, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY mem, GR_GPU_SIZE offset), cmdBuffer, mem, offset);
GR_PROTOTYPE(GR_VOID, grCmdDispatch, (GR_CMD_BUFFER cmdBuffer, GR_UINT x, GR_UINT y, GR_UINT z), cmdBuffer, x, y, z);
GR_PROTOTYPE(GR_VOID, grCmdDispatchIndirect, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY mem, GR_GPU_SIZE offset), cmdBuffer, mem, offset);
GR_PROTOTYPE(GR_VOID, grCmdCopyMemory, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY srcMem, GR_GPU_MEMORY destMem, GR_UINT regionCount, const GR_MEMORY_COPY* pRegions), cmdBuffer, srcMem, destMem, regionCount, pRegions);
GR_PROTOTYPE(GR_VOID, grCmdCopyImage, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE srcImage, GR_IMAGE destImage, GR_UINT regionCount, const GR_IMAGE_COPY* pRegions), cmdBuffer, srcImage, destImage, regionCount, pRegions);
GR_PROTOTYPE(GR_VOID, grCmdCopyMemoryToImage, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY srcMem, GR_IMAGE destImage, GR_UINT regionCount, const GR_MEMORY_IMAGE_COPY* pRegions), cmdBuffer, srcMem, destImage, regionCount, pRegions);
GR_PROTOTYPE(GR_VOID, grCmdCopyImageToMemory, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE srcImage, GR_GPU_MEMORY destMem, GR_UINT regionCount, const GR_MEMORY_IMAGE_COPY* pRegions), cmdBuffer, srcImage, destMem, regionCount, pRegions);
GR_PROTOTYPE(GR_VOID, grCmdResolveImage, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE srcImage, GR_IMAGE destImage, GR_UINT regionCount, const GR_IMAGE_RESOLVE* pRegions), cmdBuffer, srcImage, destImage, regionCount, pRegions);
GR_PROTOTYPE(GR_VOID, grCmdCloneImageData, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE srcImage, GR_ENUM srcImageState, GR_IMAGE destImage, GR_ENUM destImageState), cmdBuffer, srcImage, srcImageState, destImage, destImageState);
GR_PROTOTYPE(GR_VOID, grCmdUpdateMemory, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY destMem, GR_GPU_SIZE destOffset, GR_GPU_SIZE dataSize, const GR_UINT32* pData), cmdBuffer, destMem, destOffset, dataSize, pData);
GR_PROTOTYPE(GR_VOID, grCmdFillMemory, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY destMem, GR_GPU_SIZE destOffset, GR_GPU_SIZE fillSize, GR_UINT32 data), cmdBuffer, destMem, destOffset, fillSize, data);
GR_PROTOTYPE(GR_VOID, grCmdClearColorImage, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE image, const GR_FLOAT color[4], GR_UINT rangeCount, const GR_IMAGE_SUBRESOURCE_RANGE* pRanges), cmdBuffer, image, color, rangeCount, pRanges);
GR_PROTOTYPE(GR_VOID, grCmdClearColorImageRaw, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE image, const GR_UINT32 color[4], GR_UINT rangeCount, const GR_IMAGE_SUBRESOURCE_RANGE* pRanges), cmdBuffer, image, color, rangeCount, pRanges);
GR_PROTOTYPE(GR_VOID, grCmdClearDepthStencil, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE image, GR_FLOAT depth, GR_UINT8 stencil, GR_UINT rangeCount, const GR_IMAGE_SUBRESOURCE_RANGE* pRanges), cmdBuffer, image, depth, stencil, rangeCount, pRanges);
GR_PROTOTYPE(GR_VOID, grCmdSetEvent, (GR_CMD_BUFFER cmdBuffer, GR_EVENT event), cmdBuffer, event);
GR_PROTOTYPE(GR_VOID, grCmdResetEvent, (GR_CMD_BUFFER cmdBuffer, GR_EVENT event), cmdBuffer, event);
GR_PROTOTYPE(GR_VOID, grCmdMemoryAtomic, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY destMem, GR_GPU_SIZE destOffset, GR_UINT64 srcData, GR_ENUM atomicOp), cmdBuffer, destMem, destOffset, srcData, atomicOp);
GR_PROTOTYPE(GR_VOID, grCmdBeginQuery, (GR_CMD_BUFFER cmdBuffer, GR_QUERY_POOL queryPool, GR_UINT slot, GR_FLAGS flags), cmdBuffer, queryPool, slot, flags);
GR_PROTOTYPE(GR_VOID, grCmdEndQuery, (GR_CMD_BUFFER cmdBuffer, GR_QUERY_POOL queryPool, GR_UINT slot), cmdBuffer, queryPool, slot);
GR_PROTOTYPE(GR_VOID, grCmdResetQueryPool, (GR_CMD_BUFFER cmdBuffer, GR_QUERY_POOL queryPool, GR_UINT startQuery, GR_UINT queryCount), cmdBuffer, queryPool, startQuery, queryCount);
GR_PROTOTYPE(GR_VOID, grCmdWriteTimestamp, (GR_CMD_BUFFER cmdBuffer, GR_ENUM timestampType, GR_GPU_MEMORY destMem, GR_GPU_SIZE destOffset), cmdBuffer, timestampType, destMem, destOffset);
GR_PROTOTYPE(GR_VOID, grCmdInitAtomicCounters, (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_UINT startCounter, GR_UINT counterCount, const GR_UINT32* pData), cmdBuffer, pipelineBindPoint, startCounter, counterCount, pData);
GR_PROTOTYPE(GR_VOID, grCmdLoadAtomicCounters, (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_UINT startCounter, GR_UINT counterCount, GR_GPU_MEMORY srcMem, GR_GPU_SIZE srcOffset), cmdBuffer, pipelineBindPoint, startCounter, counterCount, srcMem, srcOffset);
GR_PROTOTYPE(GR_VOID, grCmdSaveAtomicCounters, (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_UINT startCounter, GR_UINT counterCount, GR_GPU_MEMORY destMem, GR_GPU_SIZE destOffset), cmdBuffer, pipelineBindPoint, startCounter, counterCount, destMem, destOffset);
//------------------------------------------------------------------------------
GR_PROTOTYPE(GR_RESULT, grWsiWinGetDisplays, (GR_DEVICE device, GR_UINT* pDisplayCount, GR_WSI_WIN_DISPLAY* pDisplayList), device, pDisplayCount, pDisplayList);
GR_PROTOTYPE(GR_RESULT, grWsiWinGetDisplayModeList, (GR_WSI_WIN_DISPLAY display, GR_UINT* pDisplayModeCount, GR_WSI_WIN_DISPLAY_MODE* pDisplayModeList), display, pDisplayModeCount, pDisplayModeList);
GR_PROTOTYPE(GR_RESULT, grWsiWinTakeFullscreenOwnership, (GR_WSI_WIN_DISPLAY display, GR_IMAGE image), display, image);
GR_PROTOTYPE(GR_RESULT, grWsiWinReleaseFullscreenOwnership, (GR_WSI_WIN_DISPLAY display), display);
GR_PROTOTYPE(GR_RESULT, grWsiWinSetGammaRamp, (GR_WSI_WIN_DISPLAY display, const GR_WSI_WIN_GAMMA_RAMP* pGammaRamp), display, pGammaRamp);
GR_PROTOTYPE(GR_RESULT, grWsiWinWaitForVerticalBlank, (GR_WSI_WIN_DISPLAY display), display);
GR_PROTOTYPE(GR_RESULT, grWsiWinGetScanLine, (GR_WSI_WIN_DISPLAY display, GR_INT* pScanLine), display, pScanLine);
GR_PROTOTYPE(GR_RESULT, grWsiWinCreatePresentableImage, (GR_DEVICE device, const GR_WSI_WIN_PRESENTABLE_IMAGE_CREATE_INFO* pCreateInfo, GR_IMAGE* pImage, GR_GPU_MEMORY* pMem), device, pCreateInfo, pImage, pMem);
GR_PROTOTYPE(GR_RESULT, grWsiWinQueuePresent, (GR_QUEUE queue, const GR_WSI_WIN_PRESENT_INFO* pPresentInfo), queue, pPresentInfo);
GR_PROTOTYPE(GR_RESULT, grWsiWinSetMaxQueuedFrames, (GR_DEVICE device, GR_UINT maxFrames), device, maxFrames);
//==============================================================================
