#include "xxGraphicInternal.h"
#include "xxGraphicVulkan.h"

#if defined(xxMACOS)
#   define VK_USE_PLATFORM_MACOS_MVK    1
#   include <dlfcn.h>
#   define OBJC_OLD_DISPATCH_PROTOTYPES 1
#   include <objc/runtime.h>
#   include <objc/message.h>
#endif

#if defined(xxWINDOWS)
#   define VK_USE_PLATFORM_WIN32_KHR    1
#endif

#define VK_NO_PROTOTYPES                1
#define VK_PROTOTYPES                   1
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_prototype.h"

static void*                        g_vulkanLibrary = nullptr;
static VkInstance                   g_instance = VK_NULL_HANDLE;
static VkDevice                     g_device = VK_NULL_HANDLE;
static VkPhysicalDevice             g_physicalDevice = VK_NULL_HANDLE;
static VkAllocationCallbacks*       g_callbacks = nullptr;
static VkQueue                      g_queue;
static uint32_t                     g_graphicFamily = 0;

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

#if defined(xxMACOS)
    if (ptr == nullptr && g_vulkanLibrary)
        ptr = dlsym(g_vulkanLibrary, name);
#endif

#if defined(xxWINDOWS)
    if (ptr == nullptr && g_vulkanLibrary)
        ptr = GetProcAddress(g_vulkanLibrary, name);
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
#if defined(xxMACOS)
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
#if defined(xxMACOS)
        dlclose(g_vulkanLibrary);
#endif

#if defined(xxWINDOWS)
        FreeLibrary(g_vulkanLibrary);
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
    uint32_t            imageIndex;
};
//------------------------------------------------------------------------------
uint64_t xxCreateSwapchainVulkan(uint64_t device, void* view, unsigned int width, unsigned int height)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
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
    float contentsScale = ((float(*)(id, SEL, ...))objc_msgSend)(nsWindow, sel_registerName("backingScaleFactor"));

    if (width == 0 || height == 0)
    {
        struct Rect { double x; double y; double width; double height; };
        Rect rect = ((Rect(*)(id, SEL, ...))objc_msgSend_stret)(nsView, sel_registerName("bounds"));
        width = (int)rect.width;
        height = (int)rect.height;
    }

    id layer = objc_msgSend((id)objc_getClass("CAMetalLayer"), sel_getUid("layer"));
    objc_msgSend(layer, sel_getUid("setDisplaySyncEnabled:"), NO);
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
#endif

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
    swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
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

    for (uint32_t i = 0; i < vkSwapchain->imageCount; i++)
    {
        if (vkSwapchain->imageViews[i])
            vkDestroyImageView(g_device, vkSwapchain->imageViews[i], g_callbacks);
    }
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
}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferVulkan(uint64_t device, uint64_t swapchain)
{
    SWAPCHAINVK* vkSwapchain = reinterpret_cast<SWAPCHAINVK*>(swapchain);
    if (vkSwapchain == nullptr)
        return 0;

    uint32_t imageIndex = vkSwapchain->imageIndex;
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

    return reinterpret_cast<uint64_t>(framebuffer);
}
//==============================================================================
//  Command Buffer
//==============================================================================
bool xxBeginCommandBufferVulkan(uint64_t commandBuffer)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer);
    if (vkCommandBuffer == VK_NULL_HANDLE)
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
void xxSubmitCommandBufferVulkan(uint64_t commandBuffer)
{
#if 0
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &fsd->ImageAcquiredSemaphore;
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &fd->CommandBuffer;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &fsd->RenderCompleteSemaphore;

    err = vkEndCommandBuffer(fd->CommandBuffer);
    check_vk_result(err);
    err = vkResetFences(v->Device, 1, &fd->Fence);
    check_vk_result(err);
    err = vkQueueSubmit(v->Queue, 1, &info, fd->Fence);
    
    vkQueueSubmit
#endif
}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxCreateRenderPassVulkan(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
{
    VkDevice vkDevice = reinterpret_cast<VkDevice>(device);
    if (vkDevice == VK_NULL_HANDLE)
        return 0;

    VkAttachmentDescription attachment = {};
    attachment.format = VK_FORMAT_B8G8R8A8_UNORM;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

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
    info.pAttachments = &attachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;

    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkResult result = vkCreateRenderPass(vkDevice, &info, g_callbacks, &renderPass);
    if (result != VK_SUCCESS)
        return 0;

    return reinterpret_cast<uint64_t>(renderPass);
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassVulkan(uint64_t renderPass)
{
    VkRenderPass vkRenderPass = reinterpret_cast<VkRenderPass>(renderPass);
    if (vkRenderPass == VK_NULL_HANDLE)
        return;

    vkDestroyRenderPass(g_device, vkRenderPass, g_callbacks);
}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassVulkan(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, float r, float g, float b, float a, float depth, unsigned char stencil)
{
    VkCommandBuffer vkCommandBuffer = reinterpret_cast<VkCommandBuffer>(commandBuffer);
    if (vkCommandBuffer == VK_NULL_HANDLE)
        return 0;
    VkFramebuffer vkFramebuffer = reinterpret_cast<VkFramebuffer>(framebuffer);
    if (vkFramebuffer == VK_NULL_HANDLE)
        return 0;
    VkRenderPass vkRenderPass = reinterpret_cast<VkRenderPass>(renderPass);
    if (vkRenderPass == VK_NULL_HANDLE)
        return 0;

#if 0
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = vkRenderPass;
    info.framebuffer = vkFramebuffer;
    info.renderArea.extent.width = wd->Width;
    info.renderArea.extent.height = wd->Height;
    info.clearValueCount = 1;
    info.pClearValues = wd->ClearValue;
    vkCmdBeginRenderPass(vkCommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
#endif

    return commandBuffer;
}
//------------------------------------------------------------------------------
void xxEndRenderPassVulkan(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
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
void xxSetViewportVulkan(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{

}
//------------------------------------------------------------------------------
void xxSetScissorVulkan(uint64_t commandEncoder, int x, int y, int width, int height)
{

}
//------------------------------------------------------------------------------
void xxSetPipelineVulkan(uint64_t commandEncoder, uint64_t pipeline)
{

}
//------------------------------------------------------------------------------
void xxSetIndexBufferVulkan(uint64_t commandEncoder, uint64_t buffer)
{

}
//------------------------------------------------------------------------------
void xxSetVertexBuffersVulkan(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{

}
//------------------------------------------------------------------------------
void xxSetVertexTexturesVulkan(uint64_t commandEncoder, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesVulkan(uint64_t commandEncoder, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetVertexSamplersVulkan(uint64_t commandEncoder, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersVulkan(uint64_t commandEncoder, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferVulkan(uint64_t commandEncoder, uint64_t buffer, unsigned int size)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferVulkan(uint64_t commandEncoder, uint64_t buffer, unsigned int size)
{

}
//------------------------------------------------------------------------------
void xxDrawIndexedVulkan(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{

}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetTransformVulkan(uint64_t commandEncoder, const float* world, const float* view, const float* projection)
{

}
//==============================================================================
