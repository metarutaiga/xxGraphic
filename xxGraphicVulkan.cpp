//==============================================================================
// xxGraphic : Vulkan Source
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "xxGraphicVulkanAsm.h"
#include "xxGraphicVulkan.h"

#if defined(xxANDROID)
#   define VK_USE_PLATFORM_ANDROID_KHR  1
#   include <android/native_window_jni.h>
#endif

#if defined(xxMACOS)
#   define VK_USE_PLATFORM_MACOS_MVK    1
#   define VK_MVK_moltenvk              1
#   include <CoreGraphics/CoreGraphics.h>
#   include <Metal/Metal.h>
#endif

#if defined(xxIOS)
#   define VK_USE_PLATFORM_IOS_MVK      1
#   define VK_MVK_moltenvk              1
#   include <CoreGraphics/CoreGraphics.h>
#   include <Metal/Metal.h>
#endif

#if defined(xxWINDOWS)
#   define VK_USE_PLATFORM_WIN32_KHR    1
#endif

#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef uint64_t object;
#include "vulkan/vulkan.h"
#undef VK_EXT_debug_report
#undef VK_KHR_push_descriptor
bool VK_KHR_push_descriptor;
bool VK_EXT_debug_report;
#if VK_MVK_moltenvk
#undef VK_MVK_moltenvk
bool VK_MVK_moltenvk;
extern "C" VkResult VKAPI_CALL vkSetMTLTextureMVK(VkImage image, id<MTLTexture> pMTLTexture);
extern "C" void VKAPI_CALL vkGetMTLTextureMVK(VkImage image, id<MTLTexture>* pMTLTexture);
#endif
#define PERSISTENT_BUFFER               1

static void*                            g_vulkanLibrary = nullptr;
static void*                            g_vulkanBackendLibrary = nullptr;
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
static VkDescriptorPool                 g_descriptorPools[256] = {};
static VkDescriptorSet                  g_descriptorSetArray[65536] = {};
static VkDescriptorSet                  g_descriptorSetCurrent = VK_NULL_HANDLE;
static int                              g_descriptorSetAvailable = 0;
static int                              g_descriptorSetSwapIndex = 0;

//==============================================================================
//  DescriptorSet
//==============================================================================
static VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetEXT(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites)
{
    if (VK_KHR_push_descriptor)
    {
        return vkCmdPushDescriptorSetKHR(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
    }

    if (g_descriptorSetCurrent == VK_NULL_HANDLE)
    {
        g_descriptorSetAvailable--;
        g_descriptorSetCurrent = g_descriptorSetArray[g_descriptorSetAvailable];
        if (xxUnlikely(g_descriptorSetCurrent == VK_NULL_HANDLE))
        {
            int descriptorSetChunk = xxCountOf(g_descriptorSetArray) / xxCountOf(g_descriptorPools);
            int descriptorPoolIndex = g_descriptorSetAvailable / descriptorSetChunk;

            VkDescriptorPool pool = g_descriptorPools[descriptorPoolIndex];
            if (xxUnlikely(pool == VK_NULL_HANDLE))
            {
                unsigned int count = xxCountOf(g_descriptorSetArray) / xxCountOf(g_descriptorPools);

                VkDescriptorPoolSize sizes[3];
                sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                sizes[0].descriptorCount = (xxGraphicDescriptor::VERTEX_UNIFORM_COUNT + xxGraphicDescriptor::FRAGMENT_UNIFORM_COUNT) * count;
                sizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                sizes[1].descriptorCount = (xxGraphicDescriptor::VERTEX_TEXTURE_COUNT + xxGraphicDescriptor::FRAGMENT_TEXTURE_COUNT) * count;
                sizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLER;
                sizes[2].descriptorCount = (xxGraphicDescriptor::VERTEX_SAMPLER_COUNT + xxGraphicDescriptor::FRAGMENT_SAMPLER_COUNT) * count;

                VkDescriptorPoolCreateInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                info.poolSizeCount = xxCountOf(sizes);
                info.pPoolSizes = sizes;
                info.maxSets = count;
                vkCreateDescriptorPool(g_device, &info, nullptr, &pool);
                if (pool == VK_NULL_HANDLE)
                    return;

                g_descriptorPools[descriptorPoolIndex] = pool;
            }

            VkDescriptorSetAllocateInfo setInfo = {};
            setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            setInfo.descriptorPool = pool;
            setInfo.descriptorSetCount = 1;
            setInfo.pSetLayouts = &g_setLayout;
            vkAllocateDescriptorSets(g_device, &setInfo, &g_descriptorSetCurrent);
            if (g_descriptorSetCurrent == VK_NULL_HANDLE)
                return;

            g_descriptorSetArray[g_descriptorSetAvailable] = g_descriptorSetCurrent;
        }
    }

    for (uint32_t i = 0; i < descriptorWriteCount; ++i)
    {
        VkWriteDescriptorSet& kWrite = *(VkWriteDescriptorSet*)&pDescriptorWrites[i];
        kWrite.dstSet = g_descriptorSetCurrent;
    }

    vkUpdateDescriptorSets(g_device, descriptorWriteCount, pDescriptorWrites, 0, nullptr);
}
//==============================================================================
//  Instance
//==============================================================================
static bool vkSymbolFailed = false;
static VKAPI_ATTR void* VKAPI_CALL vkSymbolImpl(const char* name)
{
    void* ptr = nullptr;

    if (ptr == nullptr && g_device && strcmp(name, "vkGetDeviceProcAddr") != 0)
        ptr = (void*)vkGetDeviceProcAddr(g_device, name);

    if (ptr == nullptr && g_instance && strcmp(name, "vkGetInstanceProcAddr") != 0)
        ptr = (void*)vkGetInstanceProcAddr(g_instance, name);

    if (ptr == nullptr && g_vulkanLibrary)
        ptr = xxGetProcAddress(g_vulkanLibrary, name);

    if (ptr == nullptr && g_vulkanBackendLibrary)
        ptr = xxGetProcAddress(g_vulkanBackendLibrary, name);

    if (ptr == nullptr)
        xxLog("Vulkan", "%s is not found", name);

    vkSymbolFailed |= (ptr == nullptr);

    return ptr;
}
#define vkSymbol(var) (void*&)var = vkSymbolImpl(#var);
//------------------------------------------------------------------------------
static VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugReportCallbackEXT(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
    xxLog("Vulkan", "%s : %s", pLayerPrefix, pMessage);
    return VK_FALSE;
}
//------------------------------------------------------------------------------
uint64_t xxCreateInstanceVulkan()
{
#if defined(xxANDROID)
    if (g_vulkanLibrary == nullptr)
        g_vulkanLibrary = xxLoadLibrary("libvulkan.so");
#endif

#if defined(xxMACOS) || defined(xxIOS)
    if (g_vulkanLibrary == nullptr)
        g_vulkanLibrary = xxLoadLibrary("libvulkan.dylib");
    if (g_vulkanLibrary == nullptr)
        g_vulkanLibrary = xxLoadLibrary("libMoltenVK.dylib");
    if (g_vulkanBackendLibrary == nullptr)
        g_vulkanBackendLibrary = xxLoadLibrary("libMoltenVK.dylib");
#endif

#if defined(xxWINDOWS)
    if (g_vulkanLibrary == nullptr)
        g_vulkanLibrary = xxLoadLibrary("vulkan-1.dll");
#endif

    if (g_vulkanLibrary == nullptr)
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

#if defined(_DEBUG)
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
#else
    uint32_t instanceLayerCount = 0;
    VkLayerProperties* instanceLayerProperties = nullptr;
    const char** instanceLayerNames = nullptr;
#endif

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
    if (instanceResult != VK_SUCCESS)
    {
        xxFree(instanceExtensionProperties);
        xxFree(instanceExtensionNames);
        xxFree(instanceLayerProperties);
        xxFree(instanceLayerNames);
        return 0;
    }
    g_instance = instance;

    VK_EXT_debug_report = false;
    for (uint32_t i = 0; i < instanceExtensionCount; ++i)
    {
        if (VK_EXT_debug_report == false && strcmp(instanceExtensionNames[i], "VK_EXT_debug_report") == 0)
        {
            VK_EXT_debug_report = true;
        }
    }
    xxFree(instanceExtensionProperties);
    xxFree(instanceExtensionNames);
    xxFree(instanceLayerProperties);
    xxFree(instanceLayerNames);

    if (VK_EXT_debug_report)
    {
        VkDebugReportCallbackCreateInfoEXT debugReportInfo = {};
        debugReportInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        debugReportInfo.flags = VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT;
        debugReportInfo.pfnCallback = vkDebugReportCallbackEXT;

        VkDebugUtilsMessengerEXT debugReport = VK_NULL_HANDLE;
        vkCreateDebugReportCallbackEXT(instance, &debugReportInfo, g_callbacks, &debugReport);

        g_debugReport = debugReport;
    }

    xxRegisterFunction(Vulkan);

    return reinterpret_cast<uint64_t>(instance);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceVulkan(uint64_t instance)
{
    VkInstance vkInstance = reinterpret_cast<VkInstance>(instance);

    if (vkInstance)
    {
        if (VK_EXT_debug_report && g_debugReport != VK_NULL_HANDLE)
        {
            vkDestroyDebugReportCallbackEXT(vkInstance, g_debugReport, g_callbacks);
            g_debugReport = VK_NULL_HANDLE;
        }
        vkDestroyInstance(vkInstance, g_callbacks);
    }
    g_instance = VK_NULL_HANDLE;

    if (g_vulkanLibrary)
    {
        xxFreeLibrary(g_vulkanLibrary);
        g_vulkanLibrary = nullptr;
    }

    if (g_vulkanBackendLibrary)
    {
        xxFreeLibrary(g_vulkanBackendLibrary);
        g_vulkanBackendLibrary = nullptr;
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
#if VK_KHR_android_surface
            presentSupport = true;
#elif VK_MVK_macos_surface
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
    if (deviceResult != VK_SUCCESS)
    {
        xxFree(deviceExtensionProperties);
        xxFree(deviceExtensionNames);
        return 0;
    }
    g_device = vkDevice;

    VK_KHR_push_descriptor = false;
    for (uint32_t i = 0; i < deviceExtensionCount; ++i)
    {
        if (VK_KHR_push_descriptor == false && strcmp(deviceExtensionNames[i], "VK_KHR_push_descriptor") == 0)
        {
            VK_KHR_push_descriptor = true;
        }
    }
    xxFree(deviceExtensionProperties);
    xxFree(deviceExtensionNames);

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
    setLayoutInfo.flags = VK_KHR_push_descriptor ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR : VkDescriptorSetLayoutCreateFlags(0);
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
    g_descriptorSetAvailable = xxCountOf(g_descriptorSetArray);

    return reinterpret_cast<uint64_t>(vkDevice);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceVulkan(uint64_t device)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return;

    vkQueueWaitIdle(g_queue);

    if (VK_KHR_push_descriptor == false)
    {
        for (int i = 0; i < xxCountOf(g_descriptorPools); ++i)
        {
            VkDescriptorPool pool = g_descriptorPools[i];
            if (pool == VK_NULL_HANDLE)
                continue;
            g_descriptorPools[i] = VK_NULL_HANDLE;
            vkDestroyDescriptorPool(vkDevice, pool, g_callbacks);
        }
        memset(g_descriptorSetArray, 0, sizeof(g_descriptorSetArray));
    }

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
bool xxResetDeviceVulkan(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
bool xxTestDeviceVulkan(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameVulkan()
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

    void*               view;
    uint32_t            width;
    uint32_t            height;
    float               scale;
    uint32_t            imageCount;
    uint32_t            imageIndex;
};
//------------------------------------------------------------------------------
uint64_t xxCreateSwapchainVulkan(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;
    VkRenderPass vkRenderPass = static_cast<uint64_t>(renderPass);
    if (vkRenderPass == VK_NULL_HANDLE)
        return 0;
    SWAPCHAINVK* vkOldSwapchain = reinterpret_cast<SWAPCHAINVK*>(oldSwapchain);
    if (vkOldSwapchain && vkOldSwapchain->view == view && vkOldSwapchain->width == width && vkOldSwapchain->height == height)
        return oldSwapchain;
    SWAPCHAINVK* vkSwapchain = xxAlloc(SWAPCHAINVK);
    if (vkSwapchain == nullptr)
        return 0;
    memset(vkSwapchain, 0, sizeof(SWAPCHAINVK));

#if defined(xxANDROID)
    float contentsScale = 1.0f;

    VkAndroidSurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.window = (ANativeWindow*)view;

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (surface == VK_NULL_HANDLE)
    {
        VkResult result = vkCreateAndroidSurfaceKHR(g_instance, &surfaceInfo, nullptr, &surface);
        if (result != VK_SUCCESS)
        {
            xxDestroySwapchain(reinterpret_cast<uint64_t>(vkSwapchain));
            return 0;
        }
    }
#elif defined(xxMACOS)
    id nsWindow = (__bridge id)view;
    if (nsWindow == nil)
        return 0;
    id nsViewController = objc_msgSend(nsWindow, sel_getUid("contentViewController"));
    if (nsViewController == nil)
        return 0;
    id nsView = objc_msgSend(nsViewController, sel_getUid("view"));
    if (nsView == nil)
        return 0;
    CGFloat contentsScale = ((CGFloat(*)(id, SEL, ...))objc_msgSend)(nsWindow, sel_registerName("backingScaleFactor"));

    id layer = objc_msgSend((id)objc_getClass("CAMetalLayer"), sel_getUid("layer"));
    objc_msgSend(layer, sel_getUid("setContentsScale:"), contentsScale);
    objc_msgSend(nsView, sel_getUid("setLayer:"), layer);
    objc_msgSend(nsView, sel_getUid("setWantsLayer:"), YES);

    VkMacOSSurfaceCreateInfoMVK surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    surfaceInfo.pView = (__bridge void*)nsView;

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
    id nsWindow = (__bridge id)view;
    if (nsWindow == nil)
        return 0;
    id nsViewController = objc_msgSend(nsWindow, sel_getUid("rootViewController"));
    if (nsViewController == nil)
        return 0;
    id nsView = objc_msgSend(nsViewController, sel_getUid("view"));
    if (nsView == nil)
        return 0;
    CGFloat contentsScale = ((CGFloat(*)(id, SEL, ...))objc_msgSend)(objc_msgSend(nsWindow, sel_getUid("screen")), sel_registerName("nativeScale"));

    id layer = objc_msgSend(nsView, sel_getUid("layer"));
    objc_msgSend(layer, sel_getUid("setContentsScale:"), contentsScale);

    VkIOSSurfaceCreateInfoMVK surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
    surfaceInfo.pView = (__bridge void*)nsView;

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
    float contentsScale = 1.0f;

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
#else
    float contentsScale = 1.0f;

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

    VkFormat imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    if (imageFormat == VK_FORMAT_R8G8B8A8_UNORM)
    {
        for (uint32_t i = 0; i < surfaceFormatCount; ++i)
        {
            if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM && surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
                break;
            }
        }
    }
    xxFree(surfaceFormats);

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(g_physicalDevice, surface, &presentModeCount, nullptr);

    VkPresentModeKHR* presentModes = xxAlloc(VkPresentModeKHR, presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(g_physicalDevice, surface, &presentModeCount, presentModes);

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
    {
        for (uint32_t i = 0; i < presentModeCount; ++i)
        {
            if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                break;
            }
        }
    }
    xxFree(presentModes);

    VkSwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = surface;
    swapchainInfo.minImageCount = 3;
    swapchainInfo.imageFormat = imageFormat;
    swapchainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkOldSwapchain)
    {
        swapchainInfo.oldSwapchain = vkOldSwapchain->swapchain;
    }

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

    xxDestroySwapchainVulkan(oldSwapchain);

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
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
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
    vkSwapchain->view = view;
    vkSwapchain->width = width;
    vkSwapchain->height = height;
    vkSwapchain->scale = contentsScale;

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

    xxFree(vkSwapchain);
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
    VkResult result = vkQueuePresentKHR(g_queue, &info);
    switch (result)
    {
    case VK_SUCCESS:
        break;
    case VK_ERROR_OUT_OF_DATE_KHR:
        xxLog("xxGraphic", "%s : %s", "vkQueuePresentKHR", "VK_ERROR_OUT_OF_DATE_KHR");
        break;
    case VK_SUBOPTIMAL_KHR:
        xxLog("xxGraphic", "%s : %s", "vkQueuePresentKHR", "VK_SUBOPTIMAL_KHR");
        break;
    default:
        xxLog("xxGraphic", "%s : %s", "vkQueuePresentKHR", "Unknown");
        break;
    }

    vkSwapchain->imageIndex++;
    if (vkSwapchain->imageIndex >= vkSwapchain->imageCount)
        vkSwapchain->imageIndex = 0;
    vkSwapchain->semaphoreIndex++;
    if (vkSwapchain->semaphoreIndex >= vkSwapchain->imageCount)
        vkSwapchain->semaphoreIndex = 0;

    if (VK_KHR_push_descriptor == false)
    {
        g_descriptorSetSwapIndex++;
        if (g_descriptorSetSwapIndex >= 8)
        {
            g_descriptorSetSwapIndex = 0;
            g_descriptorSetAvailable = xxCountOf(g_descriptorSetArray);
        }
        g_descriptorSetCurrent = VK_NULL_HANDLE;
    }
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
    switch (result)
    {
    case VK_SUCCESS:
        break;
    case VK_SUBOPTIMAL_KHR:
        xxLog("xxGraphic", "%s : %s", "vkAcquireNextImageKHR", "VK_SUBOPTIMAL_KHR");
        break;
    case VK_ERROR_OUT_OF_DATE_KHR:
        xxLog("xxGraphic", "%s : %s", "vkAcquireNextImageKHR", "VK_ERROR_OUT_OF_DATE_KHR");
        return 0;
    default:
        xxLog("xxGraphic", "%s : %s", "vkAcquireNextImageKHR", "Unknown");
        return 0;
    }

    uint32_t imageIndex = vkSwapchain->imageIndex;
    VkFence fence = vkSwapchain->fences[imageIndex];
    vkWaitForFences(vkDevice, 1, &fence, VK_TRUE, UINT64_MAX);
    vkResetFences(vkDevice, 1, &fence);

    VkCommandBuffer commandBuffer = vkSwapchain->commandBuffers[imageIndex];
    vkResetCommandBuffer(commandBuffer, 0);

    return reinterpret_cast<uint64_t>(commandBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferVulkan(uint64_t device, uint64_t swapchain, float* scale)
{
    SWAPCHAINVK* vkSwapchain = reinterpret_cast<SWAPCHAINVK*>(swapchain);
    if (vkSwapchain == nullptr)
        return 0;

    if (scale)
    {
        (*scale) = vkSwapchain->scale;
    }

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
uint64_t xxBeginRenderPassVulkan(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
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
    clearValues[0].color.float32[0] = color[0];
    clearValues[0].color.float32[1] = color[1];
    clearValues[0].color.float32[2] = color[2];
    clearValues[0].color.float32[3] = color[3];
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

    VkViewport vp;
    vp.x = 0;
    vp.y = 0;
    vp.width = (float)width;
    vp.height = (float)height;
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;
    vkCmdSetViewport(vkCommandBuffer, 0, 1, &vp);

    VkRect2D rect;
    rect.offset.x = 0;
    rect.offset.y = 0;
    rect.extent.width = width;
    rect.extent.height = height;
    vkCmdSetScissor(vkCommandBuffer, 0, 1, &rect);

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
uint64_t xxCreateVertexAttributeVulkan(uint64_t device, int count, int* attribute)
{
    VERTEXATTRIBUTEVK* vkVertexAttribute = xxAlloc(VERTEXATTRIBUTEVK);
    if (vkVertexAttribute == nullptr)
        return 0;
    memset(vkVertexAttribute, 0, sizeof(VERTEXATTRIBUTEVK));

    VkVertexInputAttributeDescription* attributeDescs = vkVertexAttribute->attributeDesc;
    int stride = 0;

    for (int i = 0; i < count; ++i)
    {
        int stream = (*attribute++);
        int offset = (*attribute++);
        int element = (*attribute++);
        int size = (*attribute++);

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
#if defined(xxWINDOWS)
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

    xxFree(vkVertexAttribute);
}
//==============================================================================
//  Buffer
//==============================================================================
struct BUFFERVK
{
    VkBuffer        buffer;
    VkDeviceMemory  memory;
    VkDeviceSize    size;
    void*           ptr;
};
//------------------------------------------------------------------------------
uint64_t xxCreateConstantBufferVulkan(uint64_t device, int size)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;

    BUFFERVK* vkBuffer = xxAlloc(BUFFERVK);
    if (vkBuffer == nullptr)
        return 0;
    memset(vkBuffer, 0, sizeof(BUFFERVK));

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

    uint32_t persistentMemoryTypeIndex = UINT32_MAX;
#if PERSISTENT_BUFFER
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
    {
        if ((prop.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == VK_MEMORY_PROPERTY_HOST_COHERENT_BIT && req.memoryTypeBits & (1 << i))
        {
            persistentMemoryTypeIndex = i;
            break;
        }
    }
#endif

    uint32_t localMemoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
    {
        if ((prop.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT && req.memoryTypeBits & (1 << i))
        {
            localMemoryTypeIndex = i;
            break;
        }
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = req.size;
    allocInfo.memoryTypeIndex = persistentMemoryTypeIndex != UINT32_MAX ? persistentMemoryTypeIndex : localMemoryTypeIndex;

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
    vkBuffer->ptr = nullptr;
#if PERSISTENT_BUFFER
    if (persistentMemoryTypeIndex != UINT32_MAX)
    {
        vkMapMemory(vkDevice, memory, 0, size, 0, &vkBuffer->ptr);
    }
#endif

    return reinterpret_cast<uint64_t>(vkBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferVulkan(uint64_t device, int size)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;

    BUFFERVK* vkBuffer = xxAlloc(BUFFERVK);
    if (vkBuffer == nullptr)
        return 0;
    memset(vkBuffer, 0, sizeof(BUFFERVK));

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

    uint32_t persistentMemoryTypeIndex = UINT32_MAX;
#if PERSISTENT_BUFFER
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
    {
        if ((prop.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == VK_MEMORY_PROPERTY_HOST_COHERENT_BIT && req.memoryTypeBits & (1 << i))
        {
            persistentMemoryTypeIndex = i;
            break;
        }
    }
#endif

    uint32_t localMemoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
    {
        if ((prop.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT && req.memoryTypeBits & (1 << i))
        {
            localMemoryTypeIndex = i;
            break;
        }
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = req.size;
    allocInfo.memoryTypeIndex = persistentMemoryTypeIndex != UINT32_MAX ? persistentMemoryTypeIndex : localMemoryTypeIndex;

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
    vkBuffer->ptr = nullptr;
#if PERSISTENT_BUFFER
    if (persistentMemoryTypeIndex != UINT32_MAX)
    {
        vkMapMemory(vkDevice, memory, 0, size, 0, &vkBuffer->ptr);
    }
#endif

    return reinterpret_cast<uint64_t>(vkBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferVulkan(uint64_t device, int size, uint64_t vertexAttribute)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;

    BUFFERVK* vkBuffer = xxAlloc(BUFFERVK);
    if (vkBuffer == nullptr)
        return 0;
    memset(vkBuffer, 0, sizeof(BUFFERVK));

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

    uint32_t persistentMemoryTypeIndex = UINT32_MAX;
#if PERSISTENT_BUFFER
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
    {
        if ((prop.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == VK_MEMORY_PROPERTY_HOST_COHERENT_BIT && req.memoryTypeBits & (1 << i))
        {
            persistentMemoryTypeIndex = i;
            break;
        }
    }
#endif

    uint32_t localMemoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
    {
        if ((prop.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT && req.memoryTypeBits & (1 << i))
        {
            localMemoryTypeIndex = i;
            break;
        }
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = req.size;
    allocInfo.memoryTypeIndex = persistentMemoryTypeIndex != UINT32_MAX ? persistentMemoryTypeIndex : localMemoryTypeIndex;

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
    vkBuffer->ptr = nullptr;
#if PERSISTENT_BUFFER
    if (persistentMemoryTypeIndex != UINT32_MAX)
    {
        vkMapMemory(vkDevice, memory, 0, size, 0, &vkBuffer->ptr);
    }
#endif

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

    xxFree(vkBuffer);
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
    if (vkBuffer->ptr)
        return vkBuffer->ptr;

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
    if (vkBuffer->ptr)
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
    VkImageView     imageView;
    VkImage         image;
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
uint64_t xxCreateTextureVulkan(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, const void* external)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;

    TEXTUREVK* vkTexture = xxAlloc(TEXTUREVK);
    if (vkTexture == nullptr)
        return 0;
    memset(vkTexture, 0, sizeof(TEXTUREVK));

#if defined(xxWINDOWS)
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

    if (external)
    {
#if defined(xxMACOS) || defined(xxIOS)
        if (VK_MVK_moltenvk)
        {
            id <MTLBuffer> mtlBuffer = nil;
            IOSurfaceRef ioSurface = nullptr;

            if ([NSStringFromClass([(__bridge id)external class]) containsString:@"MTLBuffer"])
                mtlBuffer = (__bridge id)external;
            if ([NSStringFromClass([(__bridge id)external class]) containsString:@"IOSurface"])
                ioSurface = (IOSurfaceRef)external;

            if (mtlBuffer || ioSurface)
            {
                id <MTLTexture> mtlTexture = nil;
                vkGetMTLTextureMVK(image, &mtlTexture);

                MTLTextureDescriptor* desc = [NSClassFromString(@"MTLTextureDescriptor") texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                                                                                                      width:width
                                                                                                                     height:height
                                                                                                                  mipmapped:NO];
#if defined(xxMACOS) || defined(xxMACCATALYST)
                desc.resourceOptions = MTLResourceStorageModeManaged;
#else
                desc.resourceOptions = MTLResourceStorageModeShared;
#endif
                if (mtlBuffer)
                    mtlTexture = [mtlBuffer newTextureWithDescriptor:desc
                                                              offset:0
                                                         bytesPerRow:[mtlBuffer length] / height];
                if (ioSurface)
                    mtlTexture = [[mtlTexture device] newTextureWithDescriptor:desc
                                                                     iosurface:ioSurface
                                                                         plane:0];
                vkBindBufferMemory(vkDevice, image, VK_NULL_HANDLE, 0);
                vkFreeMemory(g_device, memory, g_callbacks);
                vkSetMTLTextureMVK(image, mtlTexture);

                vkTexture->memory = VK_NULL_HANDLE;
            }
        }
#endif
    }

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

    xxFree(vkTexture);
}
//------------------------------------------------------------------------------
void* xxMapTextureVulkan(uint64_t device, uint64_t texture, int* stride, int level, int array)
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

        intptr_t size = (intptr_t)req.size;

        vkTexture->uploadBuffer = buffer;
        vkTexture->uploadMemory = memory;
        vkTexture->uploadSize = size;
        vkTexture->uploadStride = size / vkTexture->array / vkTexture->depth / vkTexture->height;
    }

    void* ptr = nullptr;
    vkMapMemory(vkDevice, vkTexture->uploadMemory, 0, vkTexture->uploadSize, 0, &ptr);

    (*stride) = (int)vkTexture->uploadStride;
    return ptr;
}
//------------------------------------------------------------------------------
void xxUnmapTextureVulkan(uint64_t device, uint64_t texture, int level, int array)
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
    VkPipelineColorBlendAttachmentState* blendState = xxAlloc(VkPipelineColorBlendAttachmentState);
    if (blendState == nullptr)
        return 0;
    memset(blendState, 0, sizeof(VkPipelineColorBlendAttachmentState));

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
    VkPipelineDepthStencilStateCreateInfo* depthStencilState = xxAlloc(VkPipelineDepthStencilStateCreateInfo);
    if (depthStencilState == nullptr)
        return 0;
    memset(depthStencilState, 0, sizeof(VkPipelineDepthStencilStateCreateInfo));

    depthStencilState->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    return reinterpret_cast<uint64_t>(depthStencilState);
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateVulkan(uint64_t device, bool cull, bool scissor)
{
    VkPipelineRasterizationStateCreateInfo* rasterizerState = xxAlloc(VkPipelineRasterizationStateCreateInfo);
    if (rasterizerState == nullptr)
        return 0;
    memset(rasterizerState, 0, sizeof(VkPipelineRasterizationStateCreateInfo));

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

    xxFree(vkBlendState);
}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateVulkan(uint64_t depthStencilState)
{
    VkPipelineDepthStencilStateCreateInfo* vkDepthStencilState = reinterpret_cast<VkPipelineDepthStencilStateCreateInfo*>(depthStencilState);

    xxFree(vkDepthStencilState);
}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateVulkan(uint64_t rasterizerState)
{
    VkPipelineRasterizationStateCreateInfo* vkRasterizerState = reinterpret_cast<VkPipelineRasterizationStateCreateInfo*>(rasterizerState);

    xxFree(vkRasterizerState);
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

    VkIndexType indexType = (INDEX_BUFFER_WIDTH == /* DISABLES CODE */(2)) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
    vkCmdBindIndexBuffer(vkCommandBuffer, vkBuffer->buffer, 0, indexType);
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

    vkCmdPushDescriptorSetEXT(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, count, sets);
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

    vkCmdPushDescriptorSetEXT(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, count, sets);
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

    vkCmdPushDescriptorSetEXT(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, count, sets);
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

    vkCmdPushDescriptorSetEXT(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, count, sets);
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferVulkan(uint64_t commandEncoder, uint64_t buffer, int size)
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

    vkCmdPushDescriptorSetEXT(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, 1, &set);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferVulkan(uint64_t commandEncoder, uint64_t buffer, int size)
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

    vkCmdPushDescriptorSetEXT(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, 1, &set);
}
//------------------------------------------------------------------------------
void xxDrawIndexedVulkan(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandEncoder);

    if (VK_KHR_push_descriptor == false)
    {
        VkDescriptorSet set = g_descriptorSetCurrent;
        if (set != VK_NULL_HANDLE)
        {
            vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, 1, &set, 0, nullptr);
            g_descriptorSetCurrent = VK_NULL_HANDLE;
        }
    }
    vkCmdDrawIndexed(vkCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetTransformVulkan(uint64_t commandEncoder, const float* world, const float* view, const float* projection)
{

}
//==============================================================================
//  Loader
//==============================================================================
#define VK_PROTOTYPE(type, prototype, parameter, ...) \
extern "C" type prototype parameter \
{ \
    static type (VKAPI_CALL* prototype ## Entry) parameter = nullptr; \
    if (prototype ## Entry == nullptr) \
    { \
        (void*&)prototype ## Entry = vkSymbolImpl(#prototype); \
        if (prototype ## Entry == nullptr) \
        { \
            return type(); \
        } \
    } \
    return prototype ## Entry(__VA_ARGS__); \
}
//------------------------------------------------------------------------------
VK_PROTOTYPE(VkResult, vkCreateInstance, (const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance), pCreateInfo, pAllocator, pInstance);
VK_PROTOTYPE(void, vkDestroyInstance, (VkInstance instance, const VkAllocationCallbacks* pAllocator), instance, pAllocator);
VK_PROTOTYPE(VkResult, vkEnumeratePhysicalDevices, (VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices), instance, pPhysicalDeviceCount, pPhysicalDevices);
VK_PROTOTYPE(void, vkGetPhysicalDeviceFeatures, (VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures), physicalDevice, pFeatures);
VK_PROTOTYPE(void, vkGetPhysicalDeviceFormatProperties, (VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties), physicalDevice, format, pFormatProperties);
VK_PROTOTYPE(VkResult, vkGetPhysicalDeviceImageFormatProperties, (VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties), physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
VK_PROTOTYPE(void, vkGetPhysicalDeviceProperties, (VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties), physicalDevice, pProperties);
VK_PROTOTYPE(void, vkGetPhysicalDeviceQueueFamilyProperties, (VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties), physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
VK_PROTOTYPE(void, vkGetPhysicalDeviceMemoryProperties, (VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties), physicalDevice, pMemoryProperties);
VK_PROTOTYPE(PFN_vkVoidFunction, vkGetInstanceProcAddr, (VkInstance instance, const char* pName), instance, pName);
VK_PROTOTYPE(PFN_vkVoidFunction, vkGetDeviceProcAddr, (VkDevice device, const char* pName), device, pName);
VK_PROTOTYPE(VkResult, vkCreateDevice, (VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice), physicalDevice, pCreateInfo, pAllocator, pDevice);
VK_PROTOTYPE(void, vkDestroyDevice, (VkDevice device, const VkAllocationCallbacks* pAllocator), device, pAllocator);
VK_PROTOTYPE(VkResult, vkEnumerateInstanceExtensionProperties, (const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties), pLayerName, pPropertyCount, pProperties);
VK_PROTOTYPE(VkResult, vkEnumerateDeviceExtensionProperties, (VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties), physicalDevice, pLayerName, pPropertyCount, pProperties);
VK_PROTOTYPE(VkResult, vkEnumerateInstanceLayerProperties, (uint32_t* pPropertyCount, VkLayerProperties* pProperties), pPropertyCount, pProperties);
VK_PROTOTYPE(VkResult, vkEnumerateDeviceLayerProperties, (VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkLayerProperties* pProperties), physicalDevice, pPropertyCount, pProperties);
VK_PROTOTYPE(void, vkGetDeviceQueue, (VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue), device, queueFamilyIndex, queueIndex, pQueue);
VK_PROTOTYPE(VkResult, vkQueueSubmit, (VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence), queue, submitCount, pSubmits, fence);
VK_PROTOTYPE(VkResult, vkQueueWaitIdle, (VkQueue queue), queue);
VK_PROTOTYPE(VkResult, vkDeviceWaitIdle, (VkDevice device), device);
VK_PROTOTYPE(VkResult, vkAllocateMemory, (VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory), device, pAllocateInfo, pAllocator, pMemory);
VK_PROTOTYPE(void, vkFreeMemory, (VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator), device, memory, pAllocator);
VK_PROTOTYPE(VkResult, vkMapMemory, (VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData), device, memory, offset, size, flags, ppData);
VK_PROTOTYPE(void, vkUnmapMemory, (VkDevice device, VkDeviceMemory memory), device, memory);
VK_PROTOTYPE(VkResult, vkFlushMappedMemoryRanges, (VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges), device, memoryRangeCount, pMemoryRanges);
VK_PROTOTYPE(VkResult, vkInvalidateMappedMemoryRanges, (VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges), device, memoryRangeCount, pMemoryRanges);
VK_PROTOTYPE(void, vkGetDeviceMemoryCommitment, (VkDevice device, VkDeviceMemory memory, VkDeviceSize* pCommittedMemoryInBytes), device, memory, pCommittedMemoryInBytes);
VK_PROTOTYPE(VkResult, vkBindBufferMemory, (VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset), device, buffer, memory, memoryOffset);
VK_PROTOTYPE(VkResult, vkBindImageMemory, (VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset), device, image, memory, memoryOffset);
VK_PROTOTYPE(void, vkGetBufferMemoryRequirements, (VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements), device, buffer, pMemoryRequirements);
VK_PROTOTYPE(void, vkGetImageMemoryRequirements, (VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements), device, image, pMemoryRequirements);
VK_PROTOTYPE(void, vkGetImageSparseMemoryRequirements, (VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements* pSparseMemoryRequirements), device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
VK_PROTOTYPE(void, vkGetPhysicalDeviceSparseImageFormatProperties, (VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties), physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
VK_PROTOTYPE(VkResult, vkQueueBindSparse, (VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo* pBindInfo, VkFence fence), queue, bindInfoCount, pBindInfo, fence);
VK_PROTOTYPE(VkResult, vkCreateFence, (VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence), device, pCreateInfo, pAllocator, pFence);
VK_PROTOTYPE(void, vkDestroyFence, (VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator), device, fence, pAllocator);
VK_PROTOTYPE(VkResult, vkResetFences, (VkDevice device, uint32_t fenceCount, const VkFence* pFences), device, fenceCount, pFences);
VK_PROTOTYPE(VkResult, vkGetFenceStatus, (VkDevice device, VkFence fence), device, fence);
VK_PROTOTYPE(VkResult, vkWaitForFences, (VkDevice device, uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout), device, fenceCount, pFences, waitAll, timeout);
VK_PROTOTYPE(VkResult, vkCreateSemaphore, (VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore), device, pCreateInfo, pAllocator, pSemaphore);
VK_PROTOTYPE(void, vkDestroySemaphore, (VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator), device, semaphore, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateEvent, (VkDevice device, const VkEventCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkEvent* pEvent), device, pCreateInfo, pAllocator, pEvent);
VK_PROTOTYPE(void, vkDestroyEvent, (VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator), device, event, pAllocator);
VK_PROTOTYPE(VkResult, vkGetEventStatus, (VkDevice device, VkEvent event), device, event);
VK_PROTOTYPE(VkResult, vkSetEvent, (VkDevice device, VkEvent event), device, event);
VK_PROTOTYPE(VkResult, vkResetEvent, (VkDevice device, VkEvent event), device, event);
VK_PROTOTYPE(VkResult, vkCreateQueryPool, (VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool), device, pCreateInfo, pAllocator, pQueryPool);
VK_PROTOTYPE(void, vkDestroyQueryPool, (VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks* pAllocator), device, queryPool, pAllocator);
VK_PROTOTYPE(VkResult, vkGetQueryPoolResults, (VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags), device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
VK_PROTOTYPE(VkResult, vkCreateBuffer, (VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer), device, pCreateInfo, pAllocator, pBuffer);
VK_PROTOTYPE(void, vkDestroyBuffer, (VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator), device, buffer, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateBufferView, (VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView), device, pCreateInfo, pAllocator, pView);
VK_PROTOTYPE(void, vkDestroyBufferView, (VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator), device, bufferView, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateImage, (VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage), device, pCreateInfo, pAllocator, pImage);
VK_PROTOTYPE(void, vkDestroyImage, (VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator), device, image, pAllocator);
VK_PROTOTYPE(void, vkGetImageSubresourceLayout, (VkDevice device, VkImage image, const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout), device, image, pSubresource, pLayout);
VK_PROTOTYPE(VkResult, vkCreateImageView, (VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView), device, pCreateInfo, pAllocator, pView);
VK_PROTOTYPE(void, vkDestroyImageView, (VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator), device, imageView, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateShaderModule, (VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule), device, pCreateInfo, pAllocator, pShaderModule);
VK_PROTOTYPE(void, vkDestroyShaderModule, (VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator), device, shaderModule, pAllocator);
VK_PROTOTYPE(VkResult, vkCreatePipelineCache, (VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineCache* pPipelineCache), device, pCreateInfo, pAllocator, pPipelineCache);
VK_PROTOTYPE(void, vkDestroyPipelineCache, (VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks* pAllocator), device, pipelineCache, pAllocator);
VK_PROTOTYPE(VkResult, vkGetPipelineCacheData, (VkDevice device, VkPipelineCache pipelineCache, size_t* pDataSize, void* pData), device, pipelineCache, pDataSize, pData);
VK_PROTOTYPE(VkResult, vkMergePipelineCaches, (VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches), device, dstCache, srcCacheCount, pSrcCaches);
VK_PROTOTYPE(VkResult, vkCreateGraphicsPipelines, (VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines), device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
VK_PROTOTYPE(VkResult, vkCreateComputePipelines, (VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines), device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
VK_PROTOTYPE(void, vkDestroyPipeline, (VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator), device, pipeline, pAllocator);
VK_PROTOTYPE(VkResult, vkCreatePipelineLayout, (VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout), device, pCreateInfo, pAllocator, pPipelineLayout);
VK_PROTOTYPE(void, vkDestroyPipelineLayout, (VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator), device, pipelineLayout, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateSampler, (VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler), device, pCreateInfo, pAllocator, pSampler);
VK_PROTOTYPE(void, vkDestroySampler, (VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator), device, sampler, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateDescriptorSetLayout, (VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout), device, pCreateInfo, pAllocator, pSetLayout);
VK_PROTOTYPE(void, vkDestroyDescriptorSetLayout, (VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator), device, descriptorSetLayout, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateDescriptorPool, (VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool), device, pCreateInfo, pAllocator, pDescriptorPool);
VK_PROTOTYPE(void, vkDestroyDescriptorPool, (VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator), device, descriptorPool, pAllocator);
VK_PROTOTYPE(VkResult, vkResetDescriptorPool, (VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags), device, descriptorPool, flags);
VK_PROTOTYPE(VkResult, vkAllocateDescriptorSets, (VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets), device, pAllocateInfo, pDescriptorSets);
VK_PROTOTYPE(VkResult, vkFreeDescriptorSets, (VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets), device, descriptorPool, descriptorSetCount, pDescriptorSets);
VK_PROTOTYPE(void, vkUpdateDescriptorSets, (VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies), device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
VK_PROTOTYPE(VkResult, vkCreateFramebuffer, (VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer), device, pCreateInfo, pAllocator, pFramebuffer);
VK_PROTOTYPE(void, vkDestroyFramebuffer, (VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator), device, framebuffer, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateRenderPass, (VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass), device, pCreateInfo, pAllocator, pRenderPass);
VK_PROTOTYPE(void, vkDestroyRenderPass, (VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator), device, renderPass, pAllocator);
VK_PROTOTYPE(void, vkGetRenderAreaGranularity, (VkDevice device, VkRenderPass renderPass, VkExtent2D* pGranularity), device, renderPass, pGranularity);
VK_PROTOTYPE(VkResult, vkCreateCommandPool, (VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool), device, pCreateInfo, pAllocator, pCommandPool);
VK_PROTOTYPE(void, vkDestroyCommandPool, (VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator), device, commandPool, pAllocator);
VK_PROTOTYPE(VkResult, vkResetCommandPool, (VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags), device, commandPool, flags);
VK_PROTOTYPE(VkResult, vkAllocateCommandBuffers, (VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers), device, pAllocateInfo, pCommandBuffers);
VK_PROTOTYPE(void, vkFreeCommandBuffers, (VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers), device, commandPool, commandBufferCount, pCommandBuffers);
VK_PROTOTYPE(VkResult, vkBeginCommandBuffer, (VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo), commandBuffer, pBeginInfo);
VK_PROTOTYPE(VkResult, vkEndCommandBuffer, (VkCommandBuffer commandBuffer), commandBuffer);
VK_PROTOTYPE(VkResult, vkResetCommandBuffer, (VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags), commandBuffer, flags);
VK_PROTOTYPE(void, vkCmdBindPipeline, (VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline), commandBuffer, pipelineBindPoint, pipeline);
VK_PROTOTYPE(void, vkCmdSetViewport, (VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports), commandBuffer, firstViewport, viewportCount, pViewports);
VK_PROTOTYPE(void, vkCmdSetScissor, (VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors), commandBuffer, firstScissor, scissorCount, pScissors);
VK_PROTOTYPE(void, vkCmdSetLineWidth, (VkCommandBuffer commandBuffer, float lineWidth), commandBuffer, lineWidth);
VK_PROTOTYPE(void, vkCmdSetDepthBias, (VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor), commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
VK_PROTOTYPE(void, vkCmdSetBlendConstants, (VkCommandBuffer commandBuffer, const float blendConstants[4]), commandBuffer, blendConstants);
VK_PROTOTYPE(void, vkCmdSetDepthBounds, (VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds), commandBuffer, minDepthBounds, maxDepthBounds);
VK_PROTOTYPE(void, vkCmdSetStencilCompareMask, (VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask), commandBuffer, faceMask, compareMask);
VK_PROTOTYPE(void, vkCmdSetStencilWriteMask, (VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask), commandBuffer, faceMask, writeMask);
VK_PROTOTYPE(void, vkCmdSetStencilReference, (VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference), commandBuffer, faceMask, reference);
VK_PROTOTYPE(void, vkCmdBindDescriptorSets, (VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets), commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
VK_PROTOTYPE(void, vkCmdBindIndexBuffer, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType), commandBuffer, buffer, offset, indexType);
VK_PROTOTYPE(void, vkCmdBindVertexBuffers, (VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets), commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
VK_PROTOTYPE(void, vkCmdDraw, (VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance), commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
VK_PROTOTYPE(void, vkCmdDrawIndexed, (VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance), commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
VK_PROTOTYPE(void, vkCmdDrawIndirect, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride), commandBuffer, buffer, offset, drawCount, stride);
VK_PROTOTYPE(void, vkCmdDrawIndexedIndirect, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride), commandBuffer, buffer, offset, drawCount, stride);
VK_PROTOTYPE(void, vkCmdDispatch, (VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ), commandBuffer, groupCountX, groupCountY, groupCountZ);
VK_PROTOTYPE(void, vkCmdDispatchIndirect, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset), commandBuffer, buffer, offset);
VK_PROTOTYPE(void, vkCmdCopyBuffer, (VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions), commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
VK_PROTOTYPE(void, vkCmdCopyImage, (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions), commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
VK_PROTOTYPE(void, vkCmdBlitImage, (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter), commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
VK_PROTOTYPE(void, vkCmdCopyBufferToImage, (VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions), commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
VK_PROTOTYPE(void, vkCmdCopyImageToBuffer, (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions), commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
VK_PROTOTYPE(void, vkCmdUpdateBuffer, (VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData), commandBuffer, dstBuffer, dstOffset, dataSize, pData);
VK_PROTOTYPE(void, vkCmdFillBuffer, (VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data), commandBuffer, dstBuffer, dstOffset, size, data);
VK_PROTOTYPE(void, vkCmdClearColorImage, (VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges), commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
VK_PROTOTYPE(void, vkCmdClearDepthStencilImage, (VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges), commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
VK_PROTOTYPE(void, vkCmdClearAttachments, (VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects), commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
VK_PROTOTYPE(void, vkCmdResolveImage, (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions), commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
VK_PROTOTYPE(void, vkCmdSetEvent, (VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask), commandBuffer, event, stageMask);
VK_PROTOTYPE(void, vkCmdResetEvent, (VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask), commandBuffer, event, stageMask);
VK_PROTOTYPE(void, vkCmdWaitEvents, (VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers), commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
VK_PROTOTYPE(void, vkCmdPipelineBarrier, (VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers), commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
VK_PROTOTYPE(void, vkCmdBeginQuery, (VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags), commandBuffer, queryPool, query, flags);
VK_PROTOTYPE(void, vkCmdEndQuery, (VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query), commandBuffer, queryPool, query);
VK_PROTOTYPE(void, vkCmdResetQueryPool, (VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount), commandBuffer, queryPool, firstQuery, queryCount);
VK_PROTOTYPE(void, vkCmdWriteTimestamp, (VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query), commandBuffer, pipelineStage, queryPool, query);
VK_PROTOTYPE(void, vkCmdCopyQueryPoolResults, (VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags), commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
VK_PROTOTYPE(void, vkCmdPushConstants, (VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues), commandBuffer, layout, stageFlags, offset, size, pValues);
VK_PROTOTYPE(void, vkCmdBeginRenderPass, (VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents), commandBuffer, pRenderPassBegin, contents);
VK_PROTOTYPE(void, vkCmdNextSubpass, (VkCommandBuffer commandBuffer, VkSubpassContents contents), commandBuffer, contents);
VK_PROTOTYPE(void, vkCmdEndRenderPass, (VkCommandBuffer commandBuffer), commandBuffer);
VK_PROTOTYPE(void, vkCmdExecuteCommands, (VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers), commandBuffer, commandBufferCount, pCommandBuffers);
//------------------------------------------------------------------------------
VK_PROTOTYPE(VkResult, vkCreateDebugReportCallbackEXT, (VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback), instance, pCreateInfo, pAllocator, pCallback);
VK_PROTOTYPE(void, vkDestroyDebugReportCallbackEXT, (VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator), instance, callback, pAllocator);
VK_PROTOTYPE(void, vkDebugReportMessageEXT, (VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage), instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
//------------------------------------------------------------------------------
VK_PROTOTYPE(void, vkDestroySurfaceKHR, (VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator), instance, surface, pAllocator);
VK_PROTOTYPE(VkResult, vkGetPhysicalDeviceSurfaceSupportKHR, (VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32* pSupported), physicalDevice, queueFamilyIndex, surface, pSupported);
VK_PROTOTYPE(VkResult, vkGetPhysicalDeviceSurfaceCapabilitiesKHR, (VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities), physicalDevice, surface, pSurfaceCapabilities);
VK_PROTOTYPE(VkResult, vkGetPhysicalDeviceSurfaceFormatsKHR, (VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats), physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
VK_PROTOTYPE(VkResult, vkGetPhysicalDeviceSurfacePresentModesKHR, (VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes), physicalDevice, surface, pPresentModeCount, pPresentModes);
//------------------------------------------------------------------------------
VK_PROTOTYPE(VkResult, vkCreateSwapchainKHR, (VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain), device, pCreateInfo, pAllocator, pSwapchain);
VK_PROTOTYPE(void, vkDestroySwapchainKHR, (VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator), device, swapchain, pAllocator);
VK_PROTOTYPE(VkResult, vkGetSwapchainImagesKHR, (VkDevice device, VkSwapchainKHR swapchain, uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages), device, swapchain, pSwapchainImageCount, pSwapchainImages);
VK_PROTOTYPE(VkResult, vkAcquireNextImageKHR, (VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex), device, swapchain, timeout, semaphore, fence, pImageIndex);
VK_PROTOTYPE(VkResult, vkQueuePresentKHR, (VkQueue queue, const VkPresentInfoKHR* pPresentInfo), queue, pPresentInfo);
VK_PROTOTYPE(VkResult, vkGetDeviceGroupPresentCapabilitiesKHR, (VkDevice device, VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities), device, pDeviceGroupPresentCapabilities);
VK_PROTOTYPE(VkResult, vkGetDeviceGroupSurfacePresentModesKHR, (VkDevice device, VkSurfaceKHR surface, VkDeviceGroupPresentModeFlagsKHR* pModes), device, surface, pModes);
VK_PROTOTYPE(VkResult, vkGetPhysicalDevicePresentRectanglesKHR, (VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pRectCount, VkRect2D* pRects), physicalDevice, surface, pRectCount, pRects);
VK_PROTOTYPE(VkResult, vkAcquireNextImage2KHR, (VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex), device, pAcquireInfo, pImageIndex);
//------------------------------------------------------------------------------
VK_PROTOTYPE(void, vkCmdPushDescriptorSetKHR, (VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites), commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
VK_PROTOTYPE(void, vkCmdPushDescriptorSetWithTemplateKHR, (VkCommandBuffer commandBuffer, VkDescriptorUpdateTemplate descriptorUpdateTemplate, VkPipelineLayout layout, uint32_t set, const void* pData), commandBuffer, descriptorUpdateTemplate, layout, set, pData);
//------------------------------------------------------------------------------
#if VK_USE_PLATFORM_ANDROID_KHR
VK_PROTOTYPE(VkResult, vkCreateAndroidSurfaceKHR, (VkInstance instance, const VkAndroidSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface), instance, pCreateInfo, pAllocator, pSurface);
#endif
//------------------------------------------------------------------------------
#if VK_USE_PLATFORM_IOS_MVK
VK_PROTOTYPE(VkResult, vkCreateIOSSurfaceMVK, (VkInstance instance, const VkIOSSurfaceCreateInfoMVK* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface), instance, pCreateInfo, pAllocator, pSurface);
#endif
//------------------------------------------------------------------------------
#if VK_USE_PLATFORM_MACOS_MVK
VK_PROTOTYPE(VkResult, vkCreateMacOSSurfaceMVK, (VkInstance instance, const VkMacOSSurfaceCreateInfoMVK* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface), instance, pCreateInfo, pAllocator, pSurface);
#endif
//------------------------------------------------------------------------------
#if VK_USE_PLATFORM_WIN32_KHR
VK_PROTOTYPE(VkResult, vkCreateWin32SurfaceKHR, (VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface), instance, pCreateInfo, pAllocator, pSurface);
VK_PROTOTYPE(VkBool32, vkGetPhysicalDeviceWin32PresentationSupportKHR, (VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex), physicalDevice, queueFamilyIndex);
#endif
//------------------------------------------------------------------------------
#if VK_USE_PLATFORM_MACOS_MVK || VK_USE_PLATFORM_IOS_MVK
VK_PROTOTYPE(VkResult, vkSetMTLTextureMVK, (VkImage image, id<MTLTexture> pMTLTexture), image, pMTLTexture);
VK_PROTOTYPE(void, vkGetMTLTextureMVK, (VkImage image, id<MTLTexture>* pMTLTexture), image, pMTLTexture);
#endif
//------------------------------------------------------------------------------
