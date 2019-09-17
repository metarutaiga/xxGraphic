#include "xxGraphicInternal.h"
#include "xxGraphicVulkanAsm.h"
#include "xxGraphicVulkan.h"

#if defined(xxMACOS)
#   define VK_USE_PLATFORM_MACOS_MVK    1
#   include <dlfcn.h>
#   define OBJC_OLD_DISPATCH_PROTOTYPES 1
#   include <CoreGraphics/CoreGraphics.h>
#   include <objc/runtime.h>
#   include <objc/message.h>
#endif

#if defined(xxIOS)
#   define VK_USE_PLATFORM_IOS_MVK      1
#   include <dlfcn.h>
#   define OBJC_OLD_DISPATCH_PROTOTYPES 1
#   include <CoreGraphics/CoreGraphics.h>
#   include <objc/runtime.h>
#   include <objc/message.h>
#endif

#if defined(xxWINDOWS)
#   define VK_USE_PLATFORM_WIN32_KHR    1
#endif

#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef uint64_t object;
#define VK_NO_PROTOTYPES                1
#define VK_PROTOTYPES                   1
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_prototype.h"

static void*                            g_vulkanLibrary = nullptr;
static VkInstance                       g_instance = VK_NULL_HANDLE;
static VkDevice                         g_device = VK_NULL_HANDLE;
static VkPhysicalDevice                 g_physicalDevice = VK_NULL_HANDLE;
static VkAllocationCallbacks*           g_callbacks = nullptr;
static VkQueue                          g_queue = VK_NULL_HANDLE;
static VkCommandPool                    g_commandPool = VK_NULL_HANDLE;
static VkDescriptorSetLayout            g_setLayout = VK_NULL_HANDLE;
static VkPipelineLayout                 g_pipelineLayout = VK_NULL_HANDLE;
static VkDebugUtilsMessengerEXT         g_debugReport = VK_NULL_HANDLE;
static uint32_t                         g_graphicFamily = 0;

//==============================================================================
//  Instance
//==============================================================================
static bool vkSymbolFailed = false;
static void* vkSymbol(const char* name)
{
    void* ptr = nullptr;

    if (ptr == nullptr && vkGetDeviceProcAddr && g_device)
        ptr = (void*)vkGetDeviceProcAddr(g_device, name);

    if (ptr == nullptr && vkGetInstanceProcAddr && g_instance)
        ptr = (void*)vkGetInstanceProcAddr(g_instance, name);

#if defined(xxMACOS) || defined(xxIOS)
    if (ptr == nullptr && g_vulkanLibrary)
        ptr = dlsym(g_vulkanLibrary, name);
#endif

#if defined(xxWINDOWS)
    if (ptr == nullptr && g_vulkanLibrary)
        ptr = GetProcAddress((HMODULE)g_vulkanLibrary, name);
#endif

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

#if VK_MVK_macos_surface
    vkSymbol(vkCreateMacOSSurfaceMVK);
    if (vkSymbolFailed)
        return false;
#endif

#if VK_MVK_ios_surface
    vkSymbol(vkCreateIOSSurfaceMVK);
    if (vkSymbolFailed)
        return false;
#endif

#if VK_KHR_win32_surface
    vkSymbol(vkCreateWin32SurfaceKHR);
    vkSymbol(vkGetPhysicalDeviceWin32PresentationSupportKHR);
    if (vkSymbolFailed)
        return false;
#endif

    if (g_instance == nullptr)
        return true;

#if VK_EXT_debug_report
    vkSymbol(vkCreateDebugReportCallbackEXT);
    vkSymbol(vkDestroyDebugReportCallbackEXT);
    vkSymbol(vkDebugReportMessageEXT);
    if (vkSymbolFailed)
        return false;
#endif

#if VK_KHR_push_descriptor
    vkSymbol(vkCmdPushDescriptorSetKHR);
    vkSymbol(vkCmdPushDescriptorSetWithTemplateKHR);
    if (vkSymbolFailed)
        return false;
#endif

    return true;
}
//------------------------------------------------------------------------------
static VkBool32 VKAPI_CALL vkDebugReportCallbackEXT(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
    xxLog("Vulkan", "%s : %s", pLayerPrefix, pMessage);
    return VK_FALSE;
}
//------------------------------------------------------------------------------
uint64_t xxCreateInstanceVulkan()
{
#if defined(xxMACOS) || defined(xxIOS)
    if (g_vulkanLibrary == nullptr)
        g_vulkanLibrary = dlopen("libvulkan.dylib", RTLD_LAZY);
    if (g_vulkanLibrary == nullptr)
        g_vulkanLibrary = dlopen("libMoltenVK.dylib", RTLD_LAZY);
#endif

#if defined(xxWINDOWS)
    if (g_vulkanLibrary == nullptr)
        g_vulkanLibrary = LoadLibraryW(L"vulkan-1.dll");
#endif

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

    uint32_t instanceLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);

    VkLayerProperties* instanceLayerProperties = xxAlloc(VkLayerProperties, instanceLayerCount);
    vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties);

    const char** instanceLayerNames = xxAlloc(const char*, instanceLayerCount);
    if (instanceLayerNames && instanceLayerProperties)
    {
        for (uint32_t i = 0; i < instanceLayerCount; ++i)
        {
            instanceLayerNames[i] = instanceLayerProperties[i].layerName;
            xxLog("xxGraphic", "%s : %s", "VkInstance", instanceLayerNames[i]);
        }
    }
    instanceLayerCount = 0;
    instanceLayerNames[0] = "VK_LAYER_LUNARG_core_validation";
    instanceLayerNames[1] = "VK_LAYER_LUNARG_standard_validation";
    instanceLayerNames[2] = "VK_LAYER_LUNARG_parameter_validation";

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.enabledExtensionCount = instanceExtensionCount;
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensionNames;
    instanceCreateInfo.enabledLayerCount = instanceLayerCount;
    instanceCreateInfo.ppEnabledLayerNames = instanceLayerNames;

    VkInstance instance = VK_NULL_HANDLE;
    VkResult instanceResult = vkCreateInstance(&instanceCreateInfo, g_callbacks, &instance);
    xxFree(instanceExtensionProperties);
    xxFree(instanceExtensionNames);
    xxFree(instanceLayerProperties);
    xxFree(instanceLayerNames);

    if (instanceResult != VK_SUCCESS)
        return 0;
    g_instance = instance;

    vkSymbols(vkSymbol);

#if VK_EXT_debug_report
    VkDebugReportCallbackCreateInfoEXT debugReportInfo = {};
    debugReportInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debugReportInfo.flags = VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT;
    debugReportInfo.pfnCallback = vkDebugReportCallbackEXT;

    VkDebugUtilsMessengerEXT debugReport = VK_NULL_HANDLE;
    vkCreateDebugReportCallbackEXT(instance, &debugReportInfo, g_callbacks, &debugReport);

    g_debugReport = debugReport;
#endif

    xxRegisterFunction(Vulkan);

    return reinterpret_cast<uint64_t>(instance);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceVulkan(uint64_t instance)
{
    VkInstance vkInstance = reinterpret_cast<VkInstance>(instance);

    if (vkInstance)
    {
#if VK_EXT_debug_report
        if (g_debugReport != VK_NULL_HANDLE)
        {
            vkDestroyDebugReportCallbackEXT(vkInstance, g_debugReport, g_callbacks);
            g_debugReport = VK_NULL_HANDLE;
        }
#endif
        vkDestroyInstance(vkInstance, g_callbacks);
    }
    g_instance = VK_NULL_HANDLE;

    vkSymbols(vkSymbolNULL);

    if (g_vulkanLibrary)
    {
#if defined(xxMACOS) || defined(xxIOS)
        dlclose(g_vulkanLibrary);
#endif

#if defined(xxWINDOWS)
        FreeLibrary((HMODULE)g_vulkanLibrary);
#endif
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
#if VK_MVK_macos_surface
            presentSupport = true;
#elif VK_MVK_ios_surface
            presentSupport = true;
#elif VK_KHR_win32_surface
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

    vkSymbols(vkSymbol);

    vkGetDeviceQueue(vkDevice, g_graphicFamily, 0, &g_queue);
    if (g_queue == VK_NULL_HANDLE)
        return 0;

    VkCommandPoolCreateInfo commandPoolInfo = {};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.queueFamilyIndex = g_graphicFamily;
    VkResult commandPoolResult = vkCreateCommandPool(vkDevice, &commandPoolInfo, g_callbacks, &g_commandPool);
    if (commandPoolResult != VK_SUCCESS)
        return 0;

    VkDescriptorSetLayoutBinding layoutBindings[xxGraphicDescriptor::TOTAL] = {};
    for (int i = 0; i < xxGraphicDescriptor::VERTEX_UNIFORM_COUNT; ++i)
    {
        layoutBindings[xxGraphicDescriptor::VERTEX_UNIFORM + i].binding = xxGraphicDescriptor::VERTEX_UNIFORM + i;
        layoutBindings[xxGraphicDescriptor::VERTEX_UNIFORM + i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layoutBindings[xxGraphicDescriptor::VERTEX_UNIFORM + i].descriptorCount = 1;
        layoutBindings[xxGraphicDescriptor::VERTEX_UNIFORM + i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    }
    for (int i = 0; i < xxGraphicDescriptor::FRAGMENT_UNIFORM_COUNT; ++i)
    {
        layoutBindings[xxGraphicDescriptor::FRAGMENT_UNIFORM + i].binding = xxGraphicDescriptor::FRAGMENT_UNIFORM + i;
        layoutBindings[xxGraphicDescriptor::FRAGMENT_UNIFORM + i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layoutBindings[xxGraphicDescriptor::FRAGMENT_UNIFORM + i].descriptorCount = 1;
        layoutBindings[xxGraphicDescriptor::FRAGMENT_UNIFORM + i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    for (int i = 0; i < xxGraphicDescriptor::VERTEX_TEXTURE_COUNT; ++i)
    {
        layoutBindings[xxGraphicDescriptor::VERTEX_TEXTURE + i].binding = xxGraphicDescriptor::VERTEX_TEXTURE + i;
        layoutBindings[xxGraphicDescriptor::VERTEX_TEXTURE + i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        layoutBindings[xxGraphicDescriptor::VERTEX_TEXTURE + i].descriptorCount = 1;
        layoutBindings[xxGraphicDescriptor::VERTEX_TEXTURE + i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    }
    for (int i = 0; i < xxGraphicDescriptor::FRAGMENT_TEXTURE_COUNT; ++i)
    {
        layoutBindings[xxGraphicDescriptor::FRAGMENT_TEXTURE + i].binding = xxGraphicDescriptor::FRAGMENT_TEXTURE + i;
        layoutBindings[xxGraphicDescriptor::FRAGMENT_TEXTURE + i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        layoutBindings[xxGraphicDescriptor::FRAGMENT_TEXTURE + i].descriptorCount = 1;
        layoutBindings[xxGraphicDescriptor::FRAGMENT_TEXTURE + i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    for (int i = 0; i < xxGraphicDescriptor::VERTEX_SAMPLER_COUNT; ++i)
    {
        layoutBindings[xxGraphicDescriptor::VERTEX_SAMPLER + i].binding = xxGraphicDescriptor::VERTEX_SAMPLER + i;
        layoutBindings[xxGraphicDescriptor::VERTEX_SAMPLER + i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        layoutBindings[xxGraphicDescriptor::VERTEX_SAMPLER + i].descriptorCount = 1;
        layoutBindings[xxGraphicDescriptor::VERTEX_SAMPLER + i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    }
    for (int i = 0; i < xxGraphicDescriptor::FRAGMENT_SAMPLER_COUNT; ++i)
    {
        layoutBindings[xxGraphicDescriptor::FRAGMENT_SAMPLER + i].binding = xxGraphicDescriptor::FRAGMENT_SAMPLER + i;
        layoutBindings[xxGraphicDescriptor::FRAGMENT_SAMPLER + i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        layoutBindings[xxGraphicDescriptor::FRAGMENT_SAMPLER + i].descriptorCount = 1;
        layoutBindings[xxGraphicDescriptor::FRAGMENT_SAMPLER + i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    VkDescriptorSetLayoutCreateInfo setLayoutInfo = {};
    setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    setLayoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
    setLayoutInfo.pBindings = layoutBindings;
    setLayoutInfo.bindingCount = xxGraphicDescriptor::TOTAL;

    VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;
    VkResult setLayoutResult = vkCreateDescriptorSetLayout(vkDevice, &setLayoutInfo, nullptr, &setLayout);
    if (setLayoutResult != VK_SUCCESS)
        return 0;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &setLayout;

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkResult pipelineLayoutResult = vkCreatePipelineLayout(vkDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout);
    if (pipelineLayoutResult != VK_SUCCESS)
        return 0;

    g_device = vkDevice;
    g_setLayout = setLayout;
    g_pipelineLayout = pipelineLayout;

    return reinterpret_cast<uint64_t>(vkDevice);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceVulkan(uint64_t device)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return;

    vkDestroyPipelineLayout(vkDevice, g_pipelineLayout, g_callbacks);
    vkDestroyDescriptorSetLayout(vkDevice, g_setLayout, g_callbacks);
    vkDestroyCommandPool(vkDevice, g_commandPool, g_callbacks);
    vkDestroyDevice(vkDevice, g_callbacks);
    g_device = VK_NULL_HANDLE;
    g_physicalDevice = VK_NULL_HANDLE;
    g_queue = VK_NULL_HANDLE;
    g_commandPool = VK_NULL_HANDLE;
    g_setLayout = VK_NULL_HANDLE;
    g_pipelineLayout = VK_NULL_HANDLE;
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

    VkFence             fences[8];
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
    uint32_t            imageIndex;
};
//------------------------------------------------------------------------------
uint64_t xxCreateSwapchainVulkan(uint64_t device, uint64_t renderPass, void* view, unsigned int width, unsigned int height)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;
    VkRenderPass vkRenderPass = static_cast<uint64_t>(renderPass);
    if (vkRenderPass == VK_NULL_HANDLE)
        return 0;
    SWAPCHAINVK* vkSwapchain = new SWAPCHAINVK {};
    if (vkSwapchain == nullptr)
        return 0;

#if defined(xxMACOS)
    id nsWindow = (id)view;
    if (nsWindow == nil)
        return 0;
    id nsViewController = objc_msgSend(nsWindow, sel_getUid("contentViewController"));
    if (nsViewController == nil)
        return 0;
    id nsView = objc_msgSend(nsViewController, sel_getUid("view"));
    if (nsView == nil)
        return 0;
    CGFloat contentsScale = ((CGFloat(*)(id, SEL, ...))objc_msgSend)(nsWindow, sel_registerName("backingScaleFactor"));

    if (width == 0 || height == 0)
    {
        CGRect rect = ((CGRect(*)(id, SEL, ...))objc_msgSend_stret)(nsView, sel_registerName("bounds"));
        width = (int)rect.size.width;
        height = (int)rect.size.height;
    }

    id layer = objc_msgSend((id)objc_getClass("CAMetalLayer"), sel_getUid("layer"));
    objc_msgSend(layer, sel_getUid("setContentsScale:"), contentsScale);
    objc_msgSend(nsView, sel_getUid("setLayer:"), layer);
    objc_msgSend(nsView, sel_getUid("setWantsLayer:"), YES);

    VkMacOSSurfaceCreateInfoMVK surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    surfaceInfo.pView = nsView;

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (surface == VK_NULL_HANDLE)
    {
        VkResult result = vkCreateMacOSSurfaceMVK(g_instance, &surfaceInfo, g_callbacks, &surface);
        if (result != VK_SUCCESS)
        {
            xxDestroySwapchain(reinterpret_cast<uint64_t>(vkSwapchain));
            return 0;
        }
    }
#elif defined(xxIOS)
    id nsWindow = (id)view;
    if (nsWindow == nil)
        return 0;
    id nsViewController = objc_msgSend(nsWindow, sel_getUid("rootViewController"));
    if (nsViewController == nil)
        return 0;
    id nsView = objc_msgSend(nsViewController, sel_getUid("view"));
    if (nsView == nil)
        return 0;
    CGFloat contentsScale = ((CGFloat(*)(id, SEL, ...))objc_msgSend)(objc_msgSend(nsWindow, sel_getUid("screen")), sel_registerName("nativeScale"));

    if (width == 0 || height == 0)
    {
        CGRect rect = ((CGRect(*)(id, SEL, ...))objc_msgSend)(nsView, sel_registerName("bounds"));
        width = (int)rect.size.width;
        height = (int)rect.size.height;
    }

    id layer = objc_msgSend(nsView, sel_getUid("layer"));
    objc_msgSend(layer, sel_getUid("setContentsScale:"), contentsScale);

    VkIOSSurfaceCreateInfoMVK surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
    surfaceInfo.pView = nsView;

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (surface == VK_NULL_HANDLE)
    {
        VkResult result = vkCreateIOSSurfaceMVK(g_instance, &surfaceInfo, g_callbacks, &surface);
        if (result != VK_SUCCESS)
        {
            xxDestroySwapchain(reinterpret_cast<uint64_t>(vkSwapchain));
            return 0;
        }
    }
#elif defined(xxWINDOWS)
    VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.hinstance = GetModuleHandle(nullptr);
    surfaceInfo.hwnd = (HWND)view;

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (surface == VK_NULL_HANDLE)
    {
        VkResult result = vkCreateWin32SurfaceKHR(g_instance, &surfaceInfo, nullptr, &surface);
        if (result != VK_SUCCESS)
        {
            xxDestroySwapchain(reinterpret_cast<uint64_t>(vkSwapchain));
            return 0;
        }
    }

    if (width == 0 || height == 0)
    {
        RECT rect = {};
        if (GetWindowRect((HWND)view, &rect) == TRUE)
        {
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
        }
    }
#else
    VkSurfaceKHR surface = VK_NULL_HANDLE;
#endif

    VkBool32 presentSupport = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(g_physicalDevice, g_graphicFamily, surface, &presentSupport);
    if (presentSupport == VK_FALSE)
    {
        vkDestroySurfaceKHR(g_instance, surface, g_callbacks);
        xxDestroySwapchain(reinterpret_cast<uint64_t>(vkSwapchain));
        return 0;
    }

    uint32_t surfaceFormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(g_physicalDevice, surface, &surfaceFormatCount, nullptr);

    VkSurfaceFormatKHR* surfaceFormats = xxAlloc(VkSurfaceFormatKHR, surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(g_physicalDevice, surface, &surfaceFormatCount, surfaceFormats);
    xxFree(surfaceFormats);

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(g_physicalDevice, surface, &presentModeCount, nullptr);

    VkPresentModeKHR* presentModes = xxAlloc(VkPresentModeKHR, presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(g_physicalDevice, surface, &presentModeCount, presentModes);
    xxFree(presentModes);

    VkSwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = surface;
    swapchainInfo.minImageCount = 1;
    swapchainInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSurfaceCapabilitiesKHR cap = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_physicalDevice, surface, &cap);
    if (swapchainInfo.minImageCount < cap.minImageCount)
    {
        swapchainInfo.minImageCount = cap.minImageCount;
    }
    else if (cap.maxImageCount != 0 && swapchainInfo.minImageCount > cap.maxImageCount)
    {
        swapchainInfo.minImageCount = cap.maxImageCount;
    }

    if (cap.currentExtent.width == 0 || cap.currentExtent.width == 0xffffffff)
    {
        swapchainInfo.imageExtent.width = width;
        swapchainInfo.imageExtent.height = height;
    }
    else
    {
        swapchainInfo.imageExtent.width = width = cap.currentExtent.width;
        swapchainInfo.imageExtent.height = height = cap.currentExtent.height;
    }

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    if (swapchain == VK_NULL_HANDLE)
    {
        VkResult result = vkCreateSwapchainKHR(vkDevice, &swapchainInfo, g_callbacks, &swapchain);
        if (result != VK_SUCCESS)
        {
            xxDestroySwapchain(reinterpret_cast<uint64_t>(vkSwapchain));
            return 0;
        }
    }
    if (vkSwapchain->imageCount == 0)
    {
        VkResult result = vkGetSwapchainImagesKHR(vkDevice, swapchain, &vkSwapchain->imageCount, nullptr);
        if (result != VK_SUCCESS)
        {
            xxDestroySwapchain(reinterpret_cast<uint64_t>(vkSwapchain));
            return 0;
        }
    }
    if (vkSwapchain->images[0] == VK_NULL_HANDLE)
    {
        VkResult result = vkGetSwapchainImagesKHR(vkDevice, swapchain, &vkSwapchain->imageCount, vkSwapchain->images);
        if (result != VK_SUCCESS)
        {
            xxDestroySwapchain(reinterpret_cast<uint64_t>(vkSwapchain));
            return 0;
        }
    }

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = swapchainInfo.imageFormat;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    for (uint32_t i = 0; i < vkSwapchain->imageCount; i++)
    {
        viewInfo.image = vkSwapchain->images[i];
        VkResult result = vkCreateImageView(vkDevice, &viewInfo, g_callbacks, &vkSwapchain->imageViews[i]);
        if (result != VK_SUCCESS)
        {
            xxDestroySwapchain(reinterpret_cast<uint64_t>(vkSwapchain));
            return 0;
        }
    }

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = vkRenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;
    for (uint32_t i = 0; i < vkSwapchain->imageCount; i++)
    {
        framebufferInfo.pAttachments = &vkSwapchain->imageViews[i];
        VkResult result = vkCreateFramebuffer(vkDevice, &framebufferInfo, g_callbacks, &vkSwapchain->framebuffers[i]);
        if (result != VK_SUCCESS)
        {
            xxDestroySwapchain(reinterpret_cast<uint64_t>(vkSwapchain));
            return 0;
        }
    }

    VkCommandBufferAllocateInfo commandBufferInfo = {};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferInfo.commandPool = g_commandPool;
    commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferInfo.commandBufferCount = 1;
    for (uint32_t i = 0; i < vkSwapchain->imageCount; i++)
    {
        vkAllocateCommandBuffers(vkDevice, &commandBufferInfo, &vkSwapchain->commandBuffers[i]);
    }

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (uint32_t i = 0; i < vkSwapchain->imageCount; i++)
    {
        vkCreateFence(vkDevice, &fenceInfo, g_callbacks, &vkSwapchain->fences[i]);
    }

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    for (uint32_t i = 0; i < vkSwapchain->imageCount; i++)
    {
        vkCreateSemaphore(vkDevice, &semaphoreInfo, g_callbacks, &vkSwapchain->imageSemaphores[i]);
        vkCreateSemaphore(vkDevice, &semaphoreInfo, g_callbacks, &vkSwapchain->presentSemaphores[i]);
    }

    vkSwapchain->surface = surface;
    vkSwapchain->surfaceFormat = swapchainInfo.imageFormat;
    vkSwapchain->presentMode = swapchainInfo.presentMode;
    vkSwapchain->swapchain = swapchain;
    vkSwapchain->width = swapchainInfo.imageExtent.width;
    vkSwapchain->height = swapchainInfo.imageExtent.height;

    return reinterpret_cast<uint64_t>(vkSwapchain);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainVulkan(uint64_t swapchain)
{
    SWAPCHAINVK* vkSwapchain = reinterpret_cast<SWAPCHAINVK*>(swapchain);
    if (vkSwapchain == nullptr)
        return;

    vkQueueWaitIdle(g_queue);

    for (uint32_t i = 0; i < vkSwapchain->imageCount; i++)
    {
        if (vkSwapchain->commandBuffers[i])
            vkFreeCommandBuffers(g_device, g_commandPool, 1, &vkSwapchain->commandBuffers[i]);
        if (vkSwapchain->fences[i])
            vkDestroyFence(g_device, vkSwapchain->fences[i], g_callbacks);
        if (vkSwapchain->imageSemaphores[i])
            vkDestroySemaphore(g_device, vkSwapchain->imageSemaphores[i], g_callbacks);
        if (vkSwapchain->presentSemaphores[i])
            vkDestroySemaphore(g_device, vkSwapchain->presentSemaphores[i], g_callbacks);
        if (vkSwapchain->framebuffers[i])
            vkDestroyFramebuffer(g_device, vkSwapchain->framebuffers[i], g_callbacks);
        if (vkSwapchain->imageViews[i])
            vkDestroyImageView(g_device, vkSwapchain->imageViews[i], g_callbacks);
    }
    if (vkSwapchain->depthStencil)
        vkDestroyImage(g_device, vkSwapchain->depthStencil, g_callbacks);
    if (vkSwapchain->depthStencilView)
        vkDestroyImageView(g_device, vkSwapchain->depthStencilView, g_callbacks);
    if (vkSwapchain->swapchain)
        vkDestroySwapchainKHR(g_device, vkSwapchain->swapchain, g_callbacks);
    if (vkSwapchain->surface)
        vkDestroySurfaceKHR(g_instance, vkSwapchain->surface, g_callbacks);

    delete vkSwapchain;
}
//------------------------------------------------------------------------------
void xxPresentSwapchainVulkan(uint64_t swapchain)
{
    SWAPCHAINVK* vkSwapchain = reinterpret_cast<SWAPCHAINVK*>(swapchain);

    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &vkSwapchain->presentSemaphores[vkSwapchain->semaphoreIndex];
    info.swapchainCount = 1;
    info.pSwapchains = &vkSwapchain->swapchain;
    info.pImageIndices = &vkSwapchain->imageIndex;
    vkQueuePresentKHR(g_queue, &info);

    vkSwapchain->imageIndex++;
    if (vkSwapchain->imageIndex >= vkSwapchain->imageCount)
        vkSwapchain->imageIndex = 0;
    vkSwapchain->semaphoreIndex++;
    if (vkSwapchain->semaphoreIndex >= vkSwapchain->imageCount)
        vkSwapchain->semaphoreIndex = 0;
}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferVulkan(uint64_t device, uint64_t swapchain)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;
    SWAPCHAINVK* vkSwapchain = reinterpret_cast<SWAPCHAINVK*>(swapchain);
    if (vkSwapchain == nullptr)
        return 0;

    VkResult result = vkAcquireNextImageKHR(vkDevice, vkSwapchain->swapchain, UINT64_MAX, vkSwapchain->imageSemaphores[vkSwapchain->semaphoreIndex], VK_NULL_HANDLE, &vkSwapchain->imageIndex);
    if (result != VK_SUCCESS)
        return 0;

    uint32_t imageIndex = vkSwapchain->imageIndex;
    VkFence fence = vkSwapchain->fences[imageIndex];
    vkWaitForFences(vkDevice, 1, &fence, VK_TRUE, UINT64_MAX);
    vkResetFences(vkDevice, 1, &fence);

    VkCommandBuffer commandBuffer = vkSwapchain->commandBuffers[imageIndex];
    vkResetCommandBuffer(commandBuffer, 0);

    return reinterpret_cast<uint64_t>(commandBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferVulkan(uint64_t device, uint64_t swapchain)
{
    SWAPCHAINVK* vkSwapchain = reinterpret_cast<SWAPCHAINVK*>(swapchain);
    if (vkSwapchain == nullptr)
        return 0;

    uint32_t imageIndex = vkSwapchain->imageIndex;
    VkFramebuffer framebuffer = vkSwapchain->framebuffers[imageIndex];
    vkSwapchain->framebuffer = framebuffer;

    return static_cast<uint64_t>(framebuffer);
}
//==============================================================================
//  Command Buffer
//==============================================================================
bool xxBeginCommandBufferVulkan(uint64_t commandBuffer)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer);
    if (vkCommandBuffer == VK_NULL_HANDLE)
        return false;

    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VkResult result = vkBeginCommandBuffer(vkCommandBuffer, &info);
    if (result != VK_SUCCESS)
        return false;

    return true;
}
//------------------------------------------------------------------------------
void xxEndCommandBufferVulkan(uint64_t commandBuffer)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer);
    if (vkCommandBuffer == VK_NULL_HANDLE)
        return;

    vkEndCommandBuffer(vkCommandBuffer);
}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferVulkan(uint64_t commandBuffer, uint64_t swapchain)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer);
    if (vkCommandBuffer == VK_NULL_HANDLE)
        return;
    SWAPCHAINVK* vkSwapchain = reinterpret_cast<SWAPCHAINVK*>(swapchain);

    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &vkCommandBuffer;

    VkFence fence = VK_NULL_HANDLE;
    if (vkSwapchain)
    {
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &vkSwapchain->imageSemaphores[vkSwapchain->semaphoreIndex];
        info.pWaitDstStageMask = &wait_stage;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &vkSwapchain->presentSemaphores[vkSwapchain->semaphoreIndex];

        fence = vkSwapchain->fences[vkSwapchain->imageIndex];
    }

    vkQueueSubmit(g_queue, 1, &info, fence);
}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxCreateRenderPassVulkan(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;

#if defined(xxMACOS) || defined(xxMACCATALYST) || defined(xxWINDOWS)
    VkFormat imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
#else
    VkFormat imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
#endif

    VkAttachmentDescription attachments[1] = {};
    attachments[0].format = imageFormat;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = clearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].storeOp = storeColor ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachment = {};
    colorAttachment.attachment = 0;
    colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachment;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = attachments;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;

    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkResult result = vkCreateRenderPass(vkDevice, &info, g_callbacks, &renderPass);
    if (result != VK_SUCCESS)
        return 0;

    return static_cast<uint64_t>(renderPass);
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassVulkan(uint64_t renderPass)
{
    VkRenderPass vkRenderPass = static_cast<VkRenderPass>(renderPass);
    if (vkRenderPass == VK_NULL_HANDLE)
        return;

    vkDestroyRenderPass(g_device, vkRenderPass, g_callbacks);
}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassVulkan(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float r, float g, float b, float a, float depth, unsigned char stencil)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer);
    if (vkCommandBuffer == VK_NULL_HANDLE)
        return 0;
    VkFramebuffer vkFramebuffer = static_cast<VkFramebuffer>(framebuffer);
    if (vkFramebuffer == VK_NULL_HANDLE)
        return 0;
    VkRenderPass vkRenderPass = static_cast<VkRenderPass>(renderPass);
    if (vkRenderPass == VK_NULL_HANDLE)
        return 0;

    VkClearValue clearValues[2] = {};
    clearValues[0].color.float32[0] = r;
    clearValues[0].color.float32[1] = g;
    clearValues[0].color.float32[2] = b;
    clearValues[0].color.float32[3] = a;
    clearValues[1].depthStencil.depth = depth;
    clearValues[1].depthStencil.stencil = stencil;

    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = vkRenderPass;
    info.framebuffer = vkFramebuffer;
    info.renderArea.extent.width = width;
    info.renderArea.extent.height = height;
    info.clearValueCount = 1;
    info.pClearValues = clearValues;
    vkCmdBeginRenderPass(vkCommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);

    return commandBuffer;
}
//------------------------------------------------------------------------------
void xxEndRenderPassVulkan(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);
    if (vkCommandBuffer == VK_NULL_HANDLE)
        return;

    vkCmdEndRenderPass(vkCommandBuffer);
}
//==============================================================================
//  Buffer
//==============================================================================
struct BUFFERVK
{
    VkBuffer        buffer;
    VkDeviceMemory  memory;
    VkDeviceSize    size;
};
//------------------------------------------------------------------------------
uint64_t xxCreateConstantBufferVulkan(uint64_t device, unsigned int size)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;

    BUFFERVK* vkBuffer = new BUFFERVK {};
    if (vkBuffer == nullptr)
        return 0;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer = VK_NULL_HANDLE;
    VkResult bufferResult = vkCreateBuffer(vkDevice, &bufferInfo, g_callbacks, &buffer);
    if (bufferResult != VK_SUCCESS)
        return 0;

    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(vkDevice, buffer, &req);

    VkPhysicalDeviceMemoryProperties prop;
    vkGetPhysicalDeviceMemoryProperties(g_physicalDevice, &prop);

    uint32_t memoryTypeIndex = 0;
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
    {
        if ((prop.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT && req.memoryTypeBits & (1 << i))
        {
            memoryTypeIndex = i;
            break;
        }
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = req.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkResult memoryResult = vkAllocateMemory(vkDevice, &allocInfo, g_callbacks, &memory);
    if (memoryResult != VK_SUCCESS)
        return 0;

    VkResult bindResult = vkBindBufferMemory(vkDevice, buffer, memory, 0);
    if (bindResult != VK_SUCCESS)
        return 0;

    vkBuffer->buffer = buffer;
    vkBuffer->memory = memory;
    vkBuffer->size = size;

    return reinterpret_cast<uint64_t>(vkBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferVulkan(uint64_t device, unsigned int size)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;

    BUFFERVK* vkBuffer = new BUFFERVK {};
    if (vkBuffer == nullptr)
        return 0;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer = VK_NULL_HANDLE;
    VkResult bufferResult = vkCreateBuffer(vkDevice, &bufferInfo, g_callbacks, &buffer);
    if (bufferResult != VK_SUCCESS)
        return 0;

    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(vkDevice, buffer, &req);

    VkPhysicalDeviceMemoryProperties prop;
    vkGetPhysicalDeviceMemoryProperties(g_physicalDevice, &prop);

    uint32_t memoryTypeIndex = 0;
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
    {
        if ((prop.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT && req.memoryTypeBits & (1 << i))
        {
            memoryTypeIndex = i;
            break;
        }
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = req.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkResult memoryResult = vkAllocateMemory(vkDevice, &allocInfo, g_callbacks, &memory);
    if (memoryResult != VK_SUCCESS)
        return 0;

    VkResult bindResult = vkBindBufferMemory(vkDevice, buffer, memory, 0);
    if (bindResult != VK_SUCCESS)
        return 0;

    vkBuffer->buffer = buffer;
    vkBuffer->memory = memory;
    vkBuffer->size = size;

    return reinterpret_cast<uint64_t>(vkBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferVulkan(uint64_t device, unsigned int size)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;

    BUFFERVK* vkBuffer = new BUFFERVK {};
    if (vkBuffer == nullptr)
        return 0;

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer = VK_NULL_HANDLE;
    VkResult bufferResult = vkCreateBuffer(vkDevice, &bufferInfo, g_callbacks, &buffer);
    if (bufferResult != VK_SUCCESS)
        return 0;

    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(vkDevice, buffer, &req);

    VkPhysicalDeviceMemoryProperties prop;
    vkGetPhysicalDeviceMemoryProperties(g_physicalDevice, &prop);

    uint32_t memoryTypeIndex = 0;
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
    {
        if ((prop.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT && req.memoryTypeBits & (1 << i))
        {
            memoryTypeIndex = i;
            break;
        }
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = req.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkResult memoryResult = vkAllocateMemory(vkDevice, &allocInfo, g_callbacks, &memory);
    if (memoryResult != VK_SUCCESS)
        return 0;

    VkResult bindResult = vkBindBufferMemory(vkDevice, buffer, memory, 0);
    if (bindResult != VK_SUCCESS)
        return 0;

    vkBuffer->buffer = buffer;
    vkBuffer->memory = memory;
    vkBuffer->size = size;

    return reinterpret_cast<uint64_t>(vkBuffer);
}
//------------------------------------------------------------------------------
void xxDestroyBufferVulkan(uint64_t device, uint64_t buffer)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return;
    BUFFERVK* vkBuffer = reinterpret_cast<BUFFERVK*>(buffer);
    if (vkBuffer == nullptr)
        return;

    vkQueueWaitIdle(g_queue);

    if (vkBuffer->buffer)
        vkDestroyBuffer(vkDevice, vkBuffer->buffer, g_callbacks);
    if (vkBuffer->memory)
        vkFreeMemory(vkDevice, vkBuffer->memory, g_callbacks);
    delete vkBuffer;
}
//------------------------------------------------------------------------------
void* xxMapBufferVulkan(uint64_t device, uint64_t buffer)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return nullptr;
    BUFFERVK* vkBuffer = reinterpret_cast<BUFFERVK*>(buffer);
    if (vkBuffer == nullptr)
        return nullptr;

    void* ptr = nullptr;
    vkMapMemory(vkDevice, vkBuffer->memory, 0, vkBuffer->size, 0, &ptr);

    return ptr;
}
//------------------------------------------------------------------------------
void xxUnmapBufferVulkan(uint64_t device, uint64_t buffer)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return;
    BUFFERVK* vkBuffer = reinterpret_cast<BUFFERVK*>(buffer);
    if (vkBuffer == nullptr)
        return;

    vkUnmapMemory(vkDevice, vkBuffer->memory);

    VkMappedMemoryRange range = {};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = vkBuffer->memory;
    range.size = VK_WHOLE_SIZE;
    vkFlushMappedMemoryRanges(vkDevice, 1, &range);
}
//==============================================================================
//  Texture
//==============================================================================
struct TEXTUREVK
{
    VkImage         image;
    VkImageView     imageView;
    VkDeviceMemory  memory;
    VkDeviceSize    size;
    uint32_t        width;
    uint32_t        height;
    uint32_t        depth;
    uint32_t        mipmap;
    uint32_t        array;
    VkBuffer        uploadBuffer;
    VkDeviceMemory  uploadMemory;
    VkDeviceSize    uploadSize;
    VkDeviceSize    uploadStride;
};
//------------------------------------------------------------------------------
uint64_t xxCreateTextureVulkan(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;

    TEXTUREVK* vkTexture = new TEXTUREVK {};
    if (vkTexture == nullptr)
        return 0;

#if defined(xxMACOS) || defined(xxMACCATALYST) || defined(xxWINDOWS)
    VkFormat imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
#else
    VkFormat imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
#endif

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = imageFormat;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkImage image = VK_NULL_HANDLE;
    VkResult imageResult = vkCreateImage(vkDevice, &imageInfo, g_callbacks, &image);
    if (imageResult != VK_SUCCESS)
        return 0;

    VkMemoryRequirements req;
    vkGetImageMemoryRequirements(vkDevice, image, &req);

    VkPhysicalDeviceMemoryProperties prop;
    vkGetPhysicalDeviceMemoryProperties(g_physicalDevice, &prop);

    uint32_t memoryTypeIndex = 0;
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
    {
        if ((prop.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT && req.memoryTypeBits & (1 << i))
        {
            memoryTypeIndex = i;
            break;
        }
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = req.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkResult memoryResult = vkAllocateMemory(vkDevice, &allocInfo, g_callbacks, &memory);
    if (memoryResult != VK_SUCCESS)
        return 0;

    VkResult bindResult = vkBindImageMemory(vkDevice, image, memory, 0);
    if (bindResult != VK_SUCCESS)
        return 0;

    VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.image = image;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = imageFormat;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView = VK_NULL_HANDLE;
    VkResult imageViewResult = vkCreateImageView(vkDevice, &imageViewInfo, g_callbacks, &imageView);
    if (imageViewResult != VK_SUCCESS)
        return 0;

    vkTexture->image = image;
    vkTexture->imageView = imageView;
    vkTexture->memory = memory;
    vkTexture->size = req.size;
    vkTexture->width = width;
    vkTexture->height = height;
    vkTexture->depth = depth;
    vkTexture->mipmap = mipmap;
    vkTexture->array = array;

    return reinterpret_cast<uint64_t>(vkTexture);
}
//------------------------------------------------------------------------------
void xxDestroyTextureVulkan(uint64_t texture)
{
    TEXTUREVK* vkTexture = reinterpret_cast<TEXTUREVK*>(texture);
    if (vkTexture == nullptr)
        return;

    vkQueueWaitIdle(g_queue);

    if (vkTexture->image != VK_NULL_HANDLE)
        vkDestroyImage(g_device, vkTexture->image, g_callbacks);
    if (vkTexture->imageView != VK_NULL_HANDLE)
        vkDestroyImageView(g_device, vkTexture->imageView, g_callbacks);
    if (vkTexture->memory != VK_NULL_HANDLE)
        vkFreeMemory(g_device, vkTexture->memory, g_callbacks);
    if (vkTexture->uploadBuffer != VK_NULL_HANDLE)
        vkDestroyBuffer(g_device, vkTexture->uploadBuffer, g_callbacks);
    if (vkTexture->uploadMemory != VK_NULL_HANDLE)
        vkFreeMemory(g_device, vkTexture->uploadMemory, g_callbacks);

    delete vkTexture;
}
//------------------------------------------------------------------------------
void* xxMapTextureVulkan(uint64_t device, uint64_t texture, unsigned int& stride, unsigned int level, unsigned int array, unsigned int mipmap)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return nullptr;
    TEXTUREVK* vkTexture = reinterpret_cast<TEXTUREVK*>(texture);
    if (vkTexture == nullptr)
        return nullptr;

    if (vkTexture->uploadBuffer == VK_NULL_HANDLE)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = vkTexture->size;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkBuffer buffer = VK_NULL_HANDLE;
        VkResult bufferResult = vkCreateBuffer(vkDevice, &bufferInfo, g_callbacks, &buffer);
        if (bufferResult != VK_SUCCESS)
            return nullptr;

        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(vkDevice, buffer, &req);

        VkPhysicalDeviceMemoryProperties prop;
        vkGetPhysicalDeviceMemoryProperties(g_physicalDevice, &prop);

        uint32_t memoryTypeIndex = 0;
        for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
        {
            if ((prop.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT && req.memoryTypeBits & (1 << i))
            {
                memoryTypeIndex = i;
                break;
            }
        }

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = req.size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;

        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkResult memoryResult = vkAllocateMemory(vkDevice, &allocInfo, g_callbacks, &memory);
        if (memoryResult != VK_SUCCESS)
            return nullptr;

        VkResult bindResult = vkBindBufferMemory(vkDevice, buffer, memory, 0);
        if (bindResult != VK_SUCCESS)
            return nullptr;

        vkTexture->uploadBuffer = buffer;
        vkTexture->uploadMemory = memory;
        vkTexture->uploadSize = req.size;
        vkTexture->uploadStride = req.size / vkTexture->array / vkTexture->depth / vkTexture->height;
    }

    stride = (int)vkTexture->uploadStride;

    void* ptr = nullptr;
    vkMapMemory(vkDevice, vkTexture->uploadMemory, 0, vkTexture->uploadSize, 0, &ptr);

    return ptr;
}
//------------------------------------------------------------------------------
void xxUnmapTextureVulkan(uint64_t device, uint64_t texture, unsigned int level, unsigned int array, unsigned int mipmap)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return;
    TEXTUREVK* vkTexture = reinterpret_cast<TEXTUREVK*>(texture);
    if (vkTexture == nullptr)
        return;

    if (vkTexture->uploadBuffer == VK_NULL_HANDLE)
        return;
    vkUnmapMemory(vkDevice, vkTexture->uploadMemory);

    VkCommandBufferAllocateInfo commandBufferInfo = {};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferInfo.commandPool = g_commandPool;
    commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    VkResult commandBufferResult = vkAllocateCommandBuffers(vkDevice, &commandBufferInfo, &commandBuffer);
    if (commandBufferResult != VK_SUCCESS)
        return;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (result != VK_SUCCESS)
        return;

    VkImageMemoryBarrier copyBarrier = {};
    copyBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    copyBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    copyBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    copyBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    copyBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    copyBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    copyBarrier.image = vkTexture->image;
    copyBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyBarrier.subresourceRange.levelCount = 1;
    copyBarrier.subresourceRange.layerCount = 1;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &copyBarrier);

    VkBufferImageCopy region = {};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.layerCount = vkTexture->array;
    region.imageExtent.width = vkTexture->width;
    region.imageExtent.height = vkTexture->height;
    region.imageExtent.depth = vkTexture->depth;
    vkCmdCopyBufferToImage(commandBuffer, vkTexture->uploadBuffer, vkTexture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    VkImageMemoryBarrier useBarrier = {};
    useBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    useBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    useBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    useBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    useBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    useBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    useBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    useBarrier.image = vkTexture->image;
    useBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    useBarrier.subresourceRange.levelCount = 1;
    useBarrier.subresourceRange.layerCount = 1;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &useBarrier);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(g_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(g_queue);

    if (vkTexture->uploadBuffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(vkDevice, vkTexture->uploadBuffer, g_callbacks);
        vkTexture->uploadBuffer = VK_NULL_HANDLE;
    }
    if (vkTexture->uploadMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(vkDevice, vkTexture->uploadMemory, g_callbacks);
        vkTexture->uploadMemory = VK_NULL_HANDLE;
    }
    vkTexture->uploadSize = 0;
}
//==============================================================================
//  Sampler
//==============================================================================
uint64_t xxCreateSamplerVulkan(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;

    VkSamplerCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.magFilter = linearMag ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
    info.minFilter = linearMin ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
    info.mipmapMode = linearMip ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
    info.addressModeU = clampU ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.addressModeV = clampV ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.addressModeW = clampW ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : VK_SAMPLER_ADDRESS_MODE_REPEAT;
    info.maxAnisotropy = (float)anisotropy;

    VkSampler sampler = VK_NULL_HANDLE;
    VkResult result = vkCreateSampler(vkDevice, &info, g_callbacks, &sampler);
    if (result != VK_SUCCESS)
        return 0;

    return static_cast<uint64_t>(sampler);
}
//------------------------------------------------------------------------------
void xxDestroySamplerVulkan(uint64_t sampler)
{
    VkSampler vkSampler = static_cast<uint64_t>(sampler);
    if (vkSampler == VK_NULL_HANDLE)
        return;

    vkDestroySampler(g_device, vkSampler, g_callbacks);
}
//==============================================================================
//  Vertex Attribute
//==============================================================================
struct VERTEXATTRIBUTEVK
{
    VkVertexInputAttributeDescription   attributeDesc[16];
    uint32_t                            attributeCount;
    VkVertexInputBindingDescription     bindingDesc[4];
    uint32_t                            bindingCount;
};
//------------------------------------------------------------------------------
uint64_t xxCreateVertexAttributeVulkan(uint64_t device, int count, ...)
{
    VERTEXATTRIBUTEVK* vkVertexAttribute = new VERTEXATTRIBUTEVK {};
    if (vkVertexAttribute == nullptr)
        return 0;

    VkVertexInputAttributeDescription* attributeDescs = vkVertexAttribute->attributeDesc;
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

        VkVertexInputAttributeDescription& attributeDesc = attributeDescs[i];
        attributeDesc.location = i;
        attributeDesc.binding = stream;
        attributeDesc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDesc.offset = offset;

        if (element == 3 && size == sizeof(float) * 3)
        {
            attributeDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
            continue;
        }

        if (element == 4 && size == sizeof(char) * 4)
        {
#if defined(xxMACOS) || defined(xxMACCATALYST) || defined(xxWINDOWS)
            attributeDesc.format = VK_FORMAT_B8G8R8A8_UNORM;
#else
            attributeDesc.format = VK_FORMAT_R8G8B8A8_UNORM;
#endif
            continue;
        }

        if (element == 2 && size == sizeof(float) * 2)
        {
            attributeDesc.format = VK_FORMAT_R32G32_SFLOAT;
            continue;
        }
    }
    va_end(args);

    vkVertexAttribute->attributeCount = count;

    VkVertexInputBindingDescription* bindingDesc = vkVertexAttribute->bindingDesc;
    bindingDesc[0].binding = 0;
    bindingDesc[0].stride = stride;
    bindingDesc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    vkVertexAttribute->bindingCount = 1;

    return reinterpret_cast<uint64_t>(vkVertexAttribute);
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeVulkan(uint64_t vertexAttribute)
{
    VERTEXATTRIBUTEVK* vkVertexAttribute = reinterpret_cast<VERTEXATTRIBUTEVK*>(vertexAttribute);
    if (vkVertexAttribute == nullptr)
        return;

    delete vkVertexAttribute;
}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderVulkan(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        VkShaderModuleCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = vertexShaderCodeSPIRVSize;
        info.pCode = vertexShaderCodeSPIRV;

        VkShaderModule vkShader = VK_NULL_HANDLE;
        VkResult result = vkCreateShaderModule(vkDevice, &info, g_callbacks, &vkShader);
        if (result != VK_SUCCESS)
            return 0;

        return static_cast<uint64_t>(vkShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderVulkan(uint64_t device, const char* shader)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        VkShaderModuleCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = fragmentShaderCodeSPIRVSize;
        info.pCode = fragmentShaderCodeSPIRV;

        VkShaderModule vkShader = VK_NULL_HANDLE;
        VkResult result = vkCreateShaderModule(vkDevice, &info, g_callbacks, &vkShader);
        if (result != VK_SUCCESS)
            return 0;

        return static_cast<uint64_t>(vkShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderVulkan(uint64_t device, uint64_t shader)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return;
    VkShaderModule vkShader = static_cast<VkShaderModule>(shader);
    if (vkShader == VK_NULL_HANDLE)
        return;

    vkDestroyShaderModule(vkDevice, vkShader, g_callbacks);
}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreateBlendStateVulkan(uint64_t device, bool blending)
{
    VkPipelineColorBlendAttachmentState* blendState = new VkPipelineColorBlendAttachmentState {};
    if (blendState == nullptr)
        return 0;

    blendState->blendEnable = blending ? VK_TRUE : VK_FALSE;
    blendState->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendState->dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendState->colorBlendOp = VK_BLEND_OP_ADD;
    blendState->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendState->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendState->alphaBlendOp = VK_BLEND_OP_ADD;
    blendState->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    return reinterpret_cast<uint64_t>(blendState);
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateVulkan(uint64_t device, bool depthTest, bool depthWrite)
{
    VkPipelineDepthStencilStateCreateInfo* depthStencilState = new VkPipelineDepthStencilStateCreateInfo {};
    if (depthStencilState == nullptr)
        return 0;

    depthStencilState->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    return reinterpret_cast<uint64_t>(depthStencilState);
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateVulkan(uint64_t device, bool cull, bool scissor)
{
    VkPipelineRasterizationStateCreateInfo* rasterizerState = new VkPipelineRasterizationStateCreateInfo {};
    if (rasterizerState == nullptr)
        return 0;

    rasterizerState->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerState->polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerState->cullMode = VK_CULL_MODE_NONE;
    rasterizerState->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizerState->lineWidth = 1.0f;

    return reinterpret_cast<uint64_t>(rasterizerState);
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineVulkan(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;
    VkRenderPass vkRenderPass = static_cast<VkRenderPass>(renderPass);
    if (vkRenderPass == VK_NULL_HANDLE)
        return 0;
    VkPipelineColorBlendAttachmentState* vkBlendState = reinterpret_cast<VkPipelineColorBlendAttachmentState*>(blendState);
    if (vkBlendState == nullptr)
        return 0;
    VkPipelineDepthStencilStateCreateInfo* vkDepthStencilState = reinterpret_cast<VkPipelineDepthStencilStateCreateInfo*>(depthStencilState);
    if (vkDepthStencilState == nullptr)
        return 0;
    VkPipelineRasterizationStateCreateInfo* vkRasterizerState = reinterpret_cast<VkPipelineRasterizationStateCreateInfo*>(rasterizerState);
    if (vkRasterizerState == nullptr)
        return 0;
    VERTEXATTRIBUTEVK* vkVertexAttribute = reinterpret_cast<VERTEXATTRIBUTEVK*>(vertexAttribute);
    if (vkVertexAttribute == nullptr)
        return 0;
    VkShaderModule vkVertexShader = static_cast<VkShaderModule>(vertexShader);
    if (vkVertexShader == VK_NULL_HANDLE)
        return 0;
    VkShaderModule vkFragmentShader = static_cast<VkShaderModule>(fragmentShader);
    if (vkFragmentShader == VK_NULL_HANDLE)
        return 0;

    VkPipelineShaderStageCreateInfo stage[2] = {};
    stage[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stage[0].module = vkVertexShader;
    stage[0].pName = "main";
    stage[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stage[1].module = vkFragmentShader;
    stage[1].pName = "main";

    VkPipelineVertexInputStateCreateInfo vertexInfo = {};
    vertexInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInfo.vertexBindingDescriptionCount = vkVertexAttribute->bindingCount;
    vertexInfo.pVertexBindingDescriptions = vkVertexAttribute->bindingDesc;
    vertexInfo.vertexAttributeDescriptionCount = vkVertexAttribute->attributeCount;
    vertexInfo.pVertexAttributeDescriptions = vkVertexAttribute->attributeDesc;

    VkPipelineInputAssemblyStateCreateInfo iaInfo = {};
    iaInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    iaInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineViewportStateCreateInfo viewportInfo = {};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.scissorCount = 1;

    VkPipelineMultisampleStateCreateInfo msInfo = {};
    msInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    msInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendStateCreateInfo blendInfo = {};
    blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendInfo.attachmentCount = 1;
    blendInfo.pAttachments = vkBlendState;

    VkDynamicState dynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkGraphicsPipelineCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    info.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
    info.stageCount = 2;
    info.pStages = stage;
    info.pVertexInputState = &vertexInfo;
    info.pInputAssemblyState = &iaInfo;
    info.pViewportState = &viewportInfo;
    info.pRasterizationState = vkRasterizerState;
    info.pMultisampleState = &msInfo;
    info.pDepthStencilState = vkDepthStencilState;
    info.pColorBlendState = &blendInfo;
    info.pDynamicState = &dynamicState;
    info.layout = g_pipelineLayout;
    info.renderPass = vkRenderPass;

    VkPipeline pipeline = VK_NULL_HANDLE;
    VkResult result = vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &info, g_callbacks, &pipeline);
    if (result != VK_SUCCESS)
        return 0;

    return static_cast<uint64_t>(pipeline);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateVulkan(uint64_t blendState)
{
    VkPipelineColorBlendAttachmentState* vkBlendState = reinterpret_cast<VkPipelineColorBlendAttachmentState*>(blendState);

    delete vkBlendState;
}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateVulkan(uint64_t depthStencilState)
{
    VkPipelineDepthStencilStateCreateInfo* vkDepthStencilState = reinterpret_cast<VkPipelineDepthStencilStateCreateInfo*>(depthStencilState);

    delete vkDepthStencilState;
}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateVulkan(uint64_t rasterizerState)
{
    VkPipelineRasterizationStateCreateInfo* vkRasterizerState = reinterpret_cast<VkPipelineRasterizationStateCreateInfo*>(rasterizerState);

    delete vkRasterizerState;
}
//------------------------------------------------------------------------------
void xxDestroyPipelineVulkan(uint64_t pipeline)
{
    VkPipeline vkPipeline = static_cast<uint64_t>(pipeline);
    if (vkPipeline == VK_NULL_HANDLE)
        return;

    vkQueueWaitIdle(g_queue);

    vkDestroyPipeline(g_device, vkPipeline, g_callbacks);
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportVulkan(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);

    VkViewport vp;
    vp.x = (float)x;
    vp.y = (float)y;
    vp.width = (float)width;
    vp.height = (float)height;
    vp.minDepth = minZ;
    vp.maxDepth = maxZ;
    vkCmdSetViewport(vkCommandBuffer, 0, 1, &vp);
}
//------------------------------------------------------------------------------
void xxSetScissorVulkan(uint64_t commandEncoder, int x, int y, int width, int height)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);

    VkRect2D rect;
    rect.offset.x = x;
    rect.offset.y = y;
    rect.extent.width = width;
    rect.extent.height = height;
    vkCmdSetScissor(vkCommandBuffer, 0, 1, &rect);
}
//------------------------------------------------------------------------------
void xxSetPipelineVulkan(uint64_t commandEncoder, uint64_t pipeline)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);
    VkPipeline vkPipeline = static_cast<VkPipeline>(pipeline);

    vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
}
//------------------------------------------------------------------------------
void xxSetIndexBufferVulkan(uint64_t commandEncoder, uint64_t buffer)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);
    BUFFERVK* vkBuffer = reinterpret_cast<BUFFERVK*>(buffer);

    vkCmdBindIndexBuffer(vkCommandBuffer, vkBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersVulkan(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);
    VkBuffer vkBuffers[xxGraphicDescriptor::VERTEX_BUFFER_COUNT];
    VkDeviceSize vkOffsets[xxGraphicDescriptor::VERTEX_BUFFER_COUNT];

    for (int i = 0; i < count; ++i)
    {
        BUFFERVK* vkBuffer = reinterpret_cast<BUFFERVK*>(buffers[i]);
        vkBuffers[i] = vkBuffer->buffer;
        vkOffsets[i] = 0;
    }

    vkCmdBindVertexBuffers(vkCommandBuffer, 0, count, vkBuffers, vkOffsets);
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesVulkan(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);
    VkDescriptorImageInfo imageInfos[xxGraphicDescriptor::VERTEX_TEXTURE_COUNT];
    VkWriteDescriptorSet sets[xxGraphicDescriptor::VERTEX_TEXTURE_COUNT];

    for (int i = 0; i < count; ++i)
    {
        TEXTUREVK* vkTexture = reinterpret_cast<TEXTUREVK*>(textures[i]);
        imageInfos[i].sampler = VK_NULL_HANDLE;
        imageInfos[i].imageView = vkTexture->imageView;
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        sets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        sets[i].pNext = nullptr;
        sets[i].dstSet = VK_NULL_HANDLE;
        sets[i].dstBinding = xxGraphicDescriptor::VERTEX_TEXTURE + i;
        sets[i].dstArrayElement = 0;
        sets[i].descriptorCount = 1;
        sets[i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        sets[i].pImageInfo = &imageInfos[i];
        sets[i].pBufferInfo = nullptr;
        sets[i].pTexelBufferView = nullptr;
    }

    vkCmdPushDescriptorSetKHR(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, count, sets);
}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesVulkan(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);
    VkDescriptorImageInfo imageInfos[xxGraphicDescriptor::FRAGMENT_TEXTURE_COUNT];
    VkWriteDescriptorSet sets[xxGraphicDescriptor::FRAGMENT_TEXTURE_COUNT];

    for (int i = 0; i < count; ++i)
    {
        TEXTUREVK* vkTexture = reinterpret_cast<TEXTUREVK*>(textures[i]);
        imageInfos[i].sampler = VK_NULL_HANDLE;
        imageInfos[i].imageView = vkTexture->imageView;
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        sets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        sets[i].pNext = nullptr;
        sets[i].dstSet = VK_NULL_HANDLE;
        sets[i].dstBinding = xxGraphicDescriptor::FRAGMENT_TEXTURE + i;
        sets[i].dstArrayElement = 0;
        sets[i].descriptorCount = 1;
        sets[i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        sets[i].pImageInfo = &imageInfos[i];
        sets[i].pBufferInfo = nullptr;
        sets[i].pTexelBufferView = nullptr;
    }

    vkCmdPushDescriptorSetKHR(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, count, sets);
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersVulkan(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);
    VkDescriptorImageInfo imageInfos[xxGraphicDescriptor::VERTEX_SAMPLER_COUNT];
    VkWriteDescriptorSet sets[xxGraphicDescriptor::VERTEX_SAMPLER_COUNT];

    for (int i = 0; i < count; ++i)
    {
        VkSampler vkSampler = static_cast<VkSampler>(samplers[i]);
        imageInfos[i].sampler = vkSampler;
        imageInfos[i].imageView = VK_NULL_HANDLE;
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        sets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        sets[i].pNext = nullptr;
        sets[i].dstSet = VK_NULL_HANDLE;
        sets[i].dstBinding = xxGraphicDescriptor::VERTEX_SAMPLER + i;
        sets[i].dstArrayElement = 0;
        sets[i].descriptorCount = 1;
        sets[i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        sets[i].pImageInfo = &imageInfos[i];
        sets[i].pBufferInfo = nullptr;
        sets[i].pTexelBufferView = nullptr;
    }

    vkCmdPushDescriptorSetKHR(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, count, sets);
}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersVulkan(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);
    VkDescriptorImageInfo imageInfos[xxGraphicDescriptor::FRAGMENT_SAMPLER_COUNT];
    VkWriteDescriptorSet sets[xxGraphicDescriptor::FRAGMENT_SAMPLER_COUNT];

    for (int i = 0; i < count; ++i)
    {
        VkSampler vkSampler = static_cast<VkSampler>(samplers[i]);
        imageInfos[i].sampler = vkSampler;
        imageInfos[i].imageView = VK_NULL_HANDLE;
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        sets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        sets[i].pNext = nullptr;
        sets[i].dstSet = VK_NULL_HANDLE;
        sets[i].dstBinding = xxGraphicDescriptor::FRAGMENT_SAMPLER + i;
        sets[i].dstArrayElement = 0;
        sets[i].descriptorCount = 1;
        sets[i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        sets[i].pImageInfo = &imageInfos[i];
        sets[i].pBufferInfo = nullptr;
        sets[i].pTexelBufferView = nullptr;
    }

    vkCmdPushDescriptorSetKHR(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, count, sets);
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferVulkan(uint64_t commandEncoder, uint64_t buffer, unsigned int size)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);
    BUFFERVK* vkBuffer = reinterpret_cast<BUFFERVK*>(buffer);
    VkDescriptorBufferInfo bufferInfo;
    VkWriteDescriptorSet set;

    bufferInfo.buffer = vkBuffer->buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = vkBuffer->size;
    set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    set.pNext = nullptr;
    set.dstSet = VK_NULL_HANDLE;
    set.dstBinding = xxGraphicDescriptor::VERTEX_UNIFORM;
    set.dstArrayElement = 0;
    set.descriptorCount = 1;
    set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    set.pImageInfo = nullptr;
    set.pBufferInfo = &bufferInfo;
    set.pTexelBufferView = nullptr;

    vkCmdPushDescriptorSetKHR(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, 1, &set);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferVulkan(uint64_t commandEncoder, uint64_t buffer, unsigned int size)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);
    BUFFERVK* vkBuffer = reinterpret_cast<BUFFERVK*>(buffer);
    VkDescriptorBufferInfo bufferInfo;
    VkWriteDescriptorSet set;

    bufferInfo.buffer = vkBuffer->buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = vkBuffer->size;
    set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    set.pNext = nullptr;
    set.dstSet = VK_NULL_HANDLE;
    set.dstBinding = xxGraphicDescriptor::FRAGMENT_UNIFORM;
    set.dstArrayElement = 0;
    set.descriptorCount = 1;
    set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    set.pImageInfo = nullptr;
    set.pBufferInfo = &bufferInfo;
    set.pTexelBufferView = nullptr;

    vkCmdPushDescriptorSetKHR(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, 1, &set);
}
//------------------------------------------------------------------------------
void xxDrawIndexedVulkan(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);

    vkCmdDrawIndexed(vkCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetTransformVulkan(uint64_t commandEncoder, const float* world, const float* view, const float* projection)
{

}
//==============================================================================
