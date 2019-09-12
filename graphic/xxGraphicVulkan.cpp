#include "xxGraphicInternal.h"
#include "xxGraphicVulkan.h"

#define VK_USE_PLATFORM_WIN32_KHR   1
#define VK_NO_PROTOTYPES            1
#define VK_PROTOTYPES               1
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_prototype.h"

static HMODULE                      g_vulkanLibrary = nullptr;
static VkInstance                   g_instance = VK_NULL_HANDLE;
static VkDevice                     g_device = VK_NULL_HANDLE;
static VkPhysicalDevice             g_physicalDevice = VK_NULL_HANDLE;
static VkAllocationCallbacks*       g_callbacks = nullptr;
static uint32_t                     g_graphicFamily = 0;

//==============================================================================
//  Instance
//==============================================================================
static bool vkSymbolFailed = false;
static void* vkSymbol(const char* name)
{
    void* ptr = nullptr;

    if (ptr == nullptr && vkGetDeviceProcAddr && g_device)
        ptr = vkGetDeviceProcAddr(g_device, name);

    if (ptr == nullptr && vkGetInstanceProcAddr && g_instance)
        ptr = vkGetInstanceProcAddr(g_instance, name);

    if (ptr == nullptr && g_vulkanLibrary)
        ptr = GetProcAddress(g_vulkanLibrary, name);

    if (ptr == nullptr)
        xxLog("Vulkan", "%s is not found", name);

    vkSymbolFailed |= (ptr == nullptr);

    return ptr;
}
#define vkSymbol(var) (void*&)var = vkSymbol(#var);
//------------------------------------------------------------------------------
static void* vkSymbolNULL(const char* name)
{
    return nullptr;
}
//------------------------------------------------------------------------------
bool vkSymbols(void* (*grSymbol)(const char* name))
{
    vkSymbolFailed = false;
    vkSymbol(vkCreateInstance);
    vkSymbol(vkDestroyInstance);
    vkSymbol(vkEnumeratePhysicalDevices);
    vkSymbol(vkGetPhysicalDeviceFeatures);
    vkSymbol(vkGetPhysicalDeviceFormatProperties);
    vkSymbol(vkGetPhysicalDeviceImageFormatProperties);
    vkSymbol(vkGetPhysicalDeviceProperties);
    vkSymbol(vkGetPhysicalDeviceQueueFamilyProperties);
    vkSymbol(vkGetPhysicalDeviceMemoryProperties);
    vkSymbol(vkGetInstanceProcAddr);
    vkSymbol(vkGetDeviceProcAddr);
    vkSymbol(vkCreateDevice);
    vkSymbol(vkDestroyDevice);
    vkSymbol(vkEnumerateInstanceExtensionProperties);
    vkSymbol(vkEnumerateDeviceExtensionProperties);
    vkSymbol(vkEnumerateInstanceLayerProperties);
    vkSymbol(vkEnumerateDeviceLayerProperties);
    vkSymbol(vkGetDeviceQueue);
    vkSymbol(vkQueueSubmit);
    vkSymbol(vkQueueWaitIdle);
    vkSymbol(vkDeviceWaitIdle);
    vkSymbol(vkAllocateMemory);
    vkSymbol(vkFreeMemory);
    vkSymbol(vkMapMemory);
    vkSymbol(vkUnmapMemory);
    vkSymbol(vkFlushMappedMemoryRanges);
    vkSymbol(vkInvalidateMappedMemoryRanges);
    vkSymbol(vkGetDeviceMemoryCommitment);
    vkSymbol(vkBindBufferMemory);
    vkSymbol(vkBindImageMemory);
    vkSymbol(vkGetBufferMemoryRequirements);
    vkSymbol(vkGetImageMemoryRequirements);
    vkSymbol(vkGetImageSparseMemoryRequirements);
    vkSymbol(vkGetPhysicalDeviceSparseImageFormatProperties);
    vkSymbol(vkQueueBindSparse);
    vkSymbol(vkCreateFence);
    vkSymbol(vkDestroyFence);
    vkSymbol(vkResetFences);
    vkSymbol(vkGetFenceStatus);
    vkSymbol(vkWaitForFences);
    vkSymbol(vkCreateSemaphore);
    vkSymbol(vkDestroySemaphore);
    vkSymbol(vkCreateEvent);
    vkSymbol(vkDestroyEvent);
    vkSymbol(vkGetEventStatus);
    vkSymbol(vkSetEvent);
    vkSymbol(vkResetEvent);
    vkSymbol(vkCreateQueryPool);
    vkSymbol(vkDestroyQueryPool);
    vkSymbol(vkGetQueryPoolResults);
    vkSymbol(vkCreateBuffer);
    vkSymbol(vkDestroyBuffer);
    vkSymbol(vkCreateBufferView);
    vkSymbol(vkDestroyBufferView);
    vkSymbol(vkCreateImage);
    vkSymbol(vkDestroyImage);
    vkSymbol(vkGetImageSubresourceLayout);
    vkSymbol(vkCreateImageView);
    vkSymbol(vkDestroyImageView);
    vkSymbol(vkCreateShaderModule);
    vkSymbol(vkDestroyShaderModule);
    vkSymbol(vkCreatePipelineCache);
    vkSymbol(vkDestroyPipelineCache);
    vkSymbol(vkGetPipelineCacheData);
    vkSymbol(vkMergePipelineCaches);
    vkSymbol(vkCreateGraphicsPipelines);
    vkSymbol(vkCreateComputePipelines);
    vkSymbol(vkDestroyPipeline);
    vkSymbol(vkCreatePipelineLayout);
    vkSymbol(vkDestroyPipelineLayout);
    vkSymbol(vkCreateSampler);
    vkSymbol(vkDestroySampler);
    vkSymbol(vkCreateDescriptorSetLayout);
    vkSymbol(vkDestroyDescriptorSetLayout);
    vkSymbol(vkCreateDescriptorPool);
    vkSymbol(vkDestroyDescriptorPool);
    vkSymbol(vkResetDescriptorPool);
    vkSymbol(vkAllocateDescriptorSets);
    vkSymbol(vkFreeDescriptorSets);
    vkSymbol(vkUpdateDescriptorSets);
    vkSymbol(vkCreateFramebuffer);
    vkSymbol(vkDestroyFramebuffer);
    vkSymbol(vkCreateRenderPass);
    vkSymbol(vkDestroyRenderPass);
    vkSymbol(vkGetRenderAreaGranularity);
    vkSymbol(vkCreateCommandPool);
    vkSymbol(vkDestroyCommandPool);
    vkSymbol(vkResetCommandPool);
    vkSymbol(vkAllocateCommandBuffers);
    vkSymbol(vkFreeCommandBuffers);
    vkSymbol(vkBeginCommandBuffer);
    vkSymbol(vkEndCommandBuffer);
    vkSymbol(vkResetCommandBuffer);
    vkSymbol(vkCmdBindPipeline);
    vkSymbol(vkCmdSetViewport);
    vkSymbol(vkCmdSetScissor);
    vkSymbol(vkCmdSetLineWidth);
    vkSymbol(vkCmdSetDepthBias);
    vkSymbol(vkCmdSetBlendConstants);
    vkSymbol(vkCmdSetDepthBounds);
    vkSymbol(vkCmdSetStencilCompareMask);
    vkSymbol(vkCmdSetStencilWriteMask);
    vkSymbol(vkCmdSetStencilReference);
    vkSymbol(vkCmdBindDescriptorSets);
    vkSymbol(vkCmdBindIndexBuffer);
    vkSymbol(vkCmdBindVertexBuffers);
    vkSymbol(vkCmdDraw);
    vkSymbol(vkCmdDrawIndexed);
    vkSymbol(vkCmdDrawIndirect);
    vkSymbol(vkCmdDrawIndexedIndirect);
    vkSymbol(vkCmdDispatch);
    vkSymbol(vkCmdDispatchIndirect);
    vkSymbol(vkCmdCopyBuffer);
    vkSymbol(vkCmdCopyImage);
    vkSymbol(vkCmdBlitImage);
    vkSymbol(vkCmdCopyBufferToImage);
    vkSymbol(vkCmdCopyImageToBuffer);
    vkSymbol(vkCmdUpdateBuffer);
    vkSymbol(vkCmdFillBuffer);
    vkSymbol(vkCmdClearColorImage);
    vkSymbol(vkCmdClearDepthStencilImage);
    vkSymbol(vkCmdClearAttachments);
    vkSymbol(vkCmdResolveImage);
    vkSymbol(vkCmdSetEvent);
    vkSymbol(vkCmdResetEvent);
    vkSymbol(vkCmdWaitEvents);
    vkSymbol(vkCmdPipelineBarrier);
    vkSymbol(vkCmdBeginQuery);
    vkSymbol(vkCmdEndQuery);
    vkSymbol(vkCmdResetQueryPool);
    vkSymbol(vkCmdWriteTimestamp);
    vkSymbol(vkCmdCopyQueryPoolResults);
    vkSymbol(vkCmdPushConstants);
    vkSymbol(vkCmdBeginRenderPass);
    vkSymbol(vkCmdNextSubpass);
    vkSymbol(vkCmdEndRenderPass);
    vkSymbol(vkCmdExecuteCommands);
    if (vkSymbolFailed)
        return false;

#if VK_KHR_surface
    vkSymbol(vkDestroySurfaceKHR);
    vkSymbol(vkGetPhysicalDeviceSurfaceSupportKHR);
    vkSymbol(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    vkSymbol(vkGetPhysicalDeviceSurfaceFormatsKHR);
    vkSymbol(vkGetPhysicalDeviceSurfacePresentModesKHR);
    if (vkSymbolFailed)
        return false;
#endif

#if VK_KHR_swapchain
    vkSymbol(vkCreateSwapchainKHR);
    vkSymbol(vkDestroySwapchainKHR);
    vkSymbol(vkGetSwapchainImagesKHR);
    vkSymbol(vkAcquireNextImageKHR);
    vkSymbol(vkQueuePresentKHR);
    vkSymbol(vkGetDeviceGroupPresentCapabilitiesKHR);
    vkSymbol(vkGetDeviceGroupSurfacePresentModesKHR);
    vkSymbol(vkGetPhysicalDevicePresentRectanglesKHR);
    vkSymbol(vkAcquireNextImage2KHR);
    if (vkSymbolFailed)
        return false;
#endif

#if VK_KHR_win32_surface
    vkSymbol(vkCreateWin32SurfaceKHR);
    vkSymbol(vkGetPhysicalDeviceWin32PresentationSupportKHR);
    if (vkSymbolFailed)
        return false;
#endif

    return true;
}
//------------------------------------------------------------------------------
uint64_t xxCreateInstanceVulkan()
{
    if (g_vulkanLibrary == nullptr)
        g_vulkanLibrary = LoadLibraryW(L"vulkan-1.dll");
    if (g_vulkanLibrary == nullptr)
        return 0;

    if (vkSymbols(vkSymbol) == false)
        return 0;

    uint32_t instanceExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);

    VkExtensionProperties* instanceExtensionProperties = xxAlloc(VkExtensionProperties, instanceExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensionProperties);

    const char** instanceExtensionNames = xxAlloc(const char*, instanceExtensionCount);
    if (instanceExtensionNames && instanceExtensionProperties)
    {
        for (uint32_t i = 0; i < instanceExtensionCount; ++i)
        {
            instanceExtensionNames[i] = instanceExtensionProperties[i].extensionName;
            xxLog("xxGraphic", "%s : %s", "VkInstance", instanceExtensionNames[i]);
        }
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.enabledExtensionCount = instanceExtensionCount;
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensionNames;

    VkInstance instance = VK_NULL_HANDLE;
    VkResult instanceResult = vkCreateInstance(&instanceCreateInfo, g_callbacks, &instance);
    xxFree(instanceExtensionProperties);
    xxFree(instanceExtensionNames);

    if (instanceResult != VK_SUCCESS)
        return 0;
    g_instance = instance;

    xxRegisterFunction(Vulkan);

    return reinterpret_cast<uint64_t>(instance);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceVulkan(uint64_t instance)
{
    VkInstance vkInstance = reinterpret_cast<VkInstance>(instance);
    if (vkInstance)
    {
        vkDestroyInstance(vkInstance, g_callbacks);
    }
    g_instance = VK_NULL_HANDLE;

    vkSymbols(vkSymbolNULL);

    if (g_vulkanLibrary)
    {
        FreeLibrary(g_vulkanLibrary);
        g_vulkanLibrary = nullptr;
    }

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceVulkan(uint64_t instance)
{
    VkInstance vkInstance = reinterpret_cast<VkInstance>(instance);
    if (vkInstance == VK_NULL_HANDLE)
        return 0;

    VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;

    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);

    VkPhysicalDevice* physicalDevices = xxAlloc(VkPhysicalDevice, physicalDeviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, physicalDevices);

    for (uint32_t i = 0; i < physicalDeviceCount; ++i)
    {
        VkPhysicalDevice physicalDevice = physicalDevices[i];

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        VkQueueFamilyProperties* queueFamilies = xxAlloc(VkQueueFamilyProperties, queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

        g_graphicFamily = queueFamilyCount;

        for (uint32_t j = 0; j < queueFamilyCount; ++j)
        {
            const VkQueueFamilyProperties& queueFamily = queueFamilies[j];
            if (queueFamily.queueCount == 0 || (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
                continue;

            VkBool32 presentSupport = false;
#if VK_KHR_win32_surface
            presentSupport = vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, j);
#else
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, j, surface, &presentSupport);
#endif
            if (presentSupport)
            {
                g_graphicFamily = j;
                break;
            }
        }

        xxFree(queueFamilies);

        if (g_graphicFamily < queueFamilyCount)
        {
            vkPhysicalDevice = physicalDevice;
            break;
        }
    }
    xxFree(physicalDevices);

    if (vkPhysicalDevice == VK_NULL_HANDLE)
        return 0;
    g_physicalDevice = vkPhysicalDevice;

    uint32_t deviceExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &deviceExtensionCount, nullptr);

    VkExtensionProperties* deviceExtensionProperties = xxAlloc(VkExtensionProperties, deviceExtensionCount);
    vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &deviceExtensionCount, deviceExtensionProperties);

    const char** deviceExtensionNames = xxAlloc(const char*, deviceExtensionCount);
    if (deviceExtensionNames && deviceExtensionProperties)
    {
        for (uint32_t i = 0; i < deviceExtensionCount; ++i)
        {
            deviceExtensionNames[i] = deviceExtensionProperties[i].extensionName;
            xxLog("xxGraphic", "%s : %s", "VkDevice", deviceExtensionNames[i]);
        }
    }

    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = g_graphicFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.enabledExtensionCount = deviceExtensionCount;
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensionNames;

    VkDevice vkDevice = VK_NULL_HANDLE;
    VkResult deviceResult = vkCreateDevice(vkPhysicalDevice, &deviceCreateInfo, g_callbacks, &vkDevice);
    xxFree(deviceExtensionProperties);
    xxFree(deviceExtensionNames);

    if (deviceResult != VK_SUCCESS)
        return 0;
    g_device = vkDevice;

    vkSymbols(vkSymbol);

    return reinterpret_cast<uint64_t>(vkDevice);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceVulkan(uint64_t device)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return;

    vkDestroyDevice(vkDevice, g_callbacks);
    g_device = VK_NULL_HANDLE;
    g_physicalDevice = VK_NULL_HANDLE;
}
//------------------------------------------------------------------------------
void xxResetDeviceVulkan(uint64_t device)
{

}
//------------------------------------------------------------------------------
bool xxTestDeviceVulkan(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceStringVulkan(uint64_t device)
{
    return "Vulkan";
}
//==============================================================================
//  Framebuffer
//==============================================================================
struct FRAMEBUFFERVK
{
    VkFramebuffer       framebuffer;
};
//==============================================================================
//  Swapchain
//==============================================================================
struct SWAPCHAINVK : public FRAMEBUFFERVK
{
    VkCommandBuffer     commandBuffers[8];

    VkSemaphore         imageSemaphores[8];
    VkSemaphore         presentSemaphores[8];
    uint32_t            semaphoreIndex;

    VkFramebuffer       framebuffers[8];

    VkImage             images[8];
    VkImageView         imageViews[8];
    VkImage             depthStencil;
    VkImageView         depthStencilView;

    VkSurfaceKHR        surface;
    VkFormat            surfaceFormat;
    VkPresentModeKHR    presentMode;
    VkSwapchainKHR      swapchain;

    uint32_t            width;
    uint32_t            height;
    uint32_t            imageCount;
};
//------------------------------------------------------------------------------
uint64_t xxCreateSwapchainVulkan(uint64_t device, void* view, unsigned int width, unsigned int height)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroySwapchainVulkan(uint64_t swapchain)
{

}
//------------------------------------------------------------------------------
void xxPresentSwapchainVulkan(uint64_t swapchain)
{

}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferVulkan(uint64_t device, uint64_t swapchain)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferVulkan(uint64_t device, uint64_t swapchain)
{
    return 0;
}
//==============================================================================
//  Command Buffer
//==============================================================================
bool xxBeginCommandBufferVulkan(uint64_t commandBuffer)
{
    return true;
}
//------------------------------------------------------------------------------
void xxEndCommandBufferVulkan(uint64_t commandBuffer)
{

}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferVulkan(uint64_t commandBuffer)
{
}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxCreateRenderPassVulkan(uint64_t device, float r, float g, float b, float a, float depth, unsigned char stencil)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassVulkan(uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
bool xxBeginRenderPassVulkan(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass)
{
    return true;
}
//------------------------------------------------------------------------------
void xxEndRenderPassVulkan(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass)
{

}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateConstantBufferVulkan(uint64_t device, unsigned int size)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferVulkan(uint64_t device, unsigned int size)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferVulkan(uint64_t device, unsigned int size)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyBufferVulkan(uint64_t device, uint64_t buffer)
{

}
//------------------------------------------------------------------------------
void* xxMapBufferVulkan(uint64_t device, uint64_t buffer)
{
    return nullptr;
}
//------------------------------------------------------------------------------
void xxUnmapBufferVulkan(uint64_t device, uint64_t buffer)
{

}
//==============================================================================
//  Texture
//==============================================================================
uint64_t xxCreateTextureVulkan(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyTextureVulkan(uint64_t texture)
{

}
//------------------------------------------------------------------------------
void* xxMapTextureVulkan(uint64_t device, uint64_t texture, unsigned int& stride, unsigned int level, unsigned int array, unsigned int mipmap)
{
    return nullptr;
}
//------------------------------------------------------------------------------
void xxUnmapTextureVulkan(uint64_t device, uint64_t texture, unsigned int level, unsigned int array, unsigned int mipmap)
{

}
//==============================================================================
//  Sampler
//==============================================================================
uint64_t xxCreateSamplerVulkan(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroySamplerVulkan(uint64_t sampler)
{

}
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t xxCreateVertexAttributeVulkan(uint64_t device, int count, ...)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeVulkan(uint64_t vertexAttribute)
{

}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderVulkan(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderVulkan(uint64_t device, const char* shader)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderVulkan(uint64_t device, uint64_t shader)
{

}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreateBlendStateVulkan(uint64_t device, bool blending)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateVulkan(uint64_t device, bool depthTest, bool depthWrite)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateVulkan(uint64_t device, bool cull, bool scissor)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineVulkan(uint64_t device, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateVulkan(uint64_t blendState)
{

}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateVulkan(uint64_t depthStencilState)
{

}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateVulkan(uint64_t rasterizerState)
{

}
//------------------------------------------------------------------------------
void xxDestroyPipelineVulkan(uint64_t pipeline)
{

}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportVulkan(uint64_t commandBuffer, int x, int y, int width, int height, float minZ, float maxZ)
{

}
//------------------------------------------------------------------------------
void xxSetScissorVulkan(uint64_t commandBuffer, int x, int y, int width, int height)
{

}
//------------------------------------------------------------------------------
void xxSetPipelineVulkan(uint64_t commandBuffer, uint64_t pipeline)
{

}
//------------------------------------------------------------------------------
void xxSetIndexBufferVulkan(uint64_t commandBuffer, uint64_t buffer)
{

}
//------------------------------------------------------------------------------
void xxSetVertexBuffersVulkan(uint64_t commandBuffer, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{

}
//------------------------------------------------------------------------------
void xxSetVertexTexturesVulkan(uint64_t commandBuffer, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesVulkan(uint64_t commandBuffer, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetVertexSamplersVulkan(uint64_t commandBuffer, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersVulkan(uint64_t commandBuffer, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferVulkan(uint64_t commandBuffer, uint64_t buffer, unsigned int size)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferVulkan(uint64_t commandBuffer, uint64_t buffer, unsigned int size)
{

}
//------------------------------------------------------------------------------
void xxDrawIndexedVulkan(uint64_t commandBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{

}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetTransformVulkan(uint64_t commandBuffer, const float* world, const float* view, const float* projection)
{

}
//==============================================================================
