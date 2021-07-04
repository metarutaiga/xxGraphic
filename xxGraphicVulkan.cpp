//==============================================================================
// xxGraphic : Vulkan Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalVulkan.h"
#include "xxGraphicVulkanAsm.h"
#include "xxGraphicVulkan.h"

#include "vulkan/vulkan.h"
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
VKAPI_ATTR void* VKAPI_CALL vkGetProcAddress(const char* name)
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

    return ptr;
}
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
    VK_MVK_moltenvk = true;
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

#if defined(xxWINDOWS)
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
#if defined(xxMACOS_INTEL)
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
uint64_t xxCreateBlendStateVulkan(uint64_t device, const char* sourceColor, const char* operationColor, const char* destinationColor, const char* sourceAlpha, const char* operationAlpha, const char* destinationAlpha)
{
    VkPipelineColorBlendAttachmentState* blendState = xxAlloc(VkPipelineColorBlendAttachmentState);
    if (blendState == nullptr)
        return 0;
    memset(blendState, 0, sizeof(VkPipelineColorBlendAttachmentState));

    blendState->srcColorBlendFactor = vkBlendFactor(sourceColor);
    blendState->dstColorBlendFactor = vkBlendFactor(destinationColor);
    blendState->colorBlendOp = vkBlendOp(operationColor);
    blendState->srcAlphaBlendFactor = vkBlendFactor(sourceAlpha);
    blendState->dstAlphaBlendFactor = vkBlendFactor(destinationAlpha);
    blendState->alphaBlendOp = vkBlendOp(operationAlpha);
    blendState->blendEnable = (blendState->srcColorBlendFactor != VK_BLEND_FACTOR_ONE ||
                               blendState->dstColorBlendFactor != VK_BLEND_FACTOR_ZERO ||
                               blendState->colorBlendOp != VK_BLEND_OP_ADD ||
                               blendState->srcAlphaBlendFactor != VK_BLEND_FACTOR_ONE ||
                               blendState->dstAlphaBlendFactor != VK_BLEND_FACTOR_ZERO ||
                               blendState->alphaBlendOp != VK_BLEND_OP_ADD) ? VK_TRUE : VK_FALSE;
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
    BUFFERVK* vkIndexBuffer = reinterpret_cast<BUFFERVK*>(indexBuffer);

    if (VK_KHR_push_descriptor == false)
    {
        VkDescriptorSet set = g_descriptorSetCurrent;
        if (set != VK_NULL_HANDLE)
        {
            vkCmdBindDescriptorSets(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, 1, &set, 0, nullptr);
            g_descriptorSetCurrent = VK_NULL_HANDLE;
        }
    }

    VkIndexType indexType = (INDEX_BUFFER_WIDTH == 2) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
    vkCmdBindIndexBuffer(vkCommandBuffer, vkIndexBuffer->buffer, 0, indexType);
    vkCmdDrawIndexed(vkCommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
//==============================================================================
