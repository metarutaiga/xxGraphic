//==============================================================================
// xxGraphic : Vulkan Internal Header
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"
#include "xxGraphicInternal.h"

#if defined(xxANDROID)
#   define VK_USE_PLATFORM_ANDROID_KHR  1
#   include <android/native_window_jni.h>
#endif

#if defined(xxMACOS)
#   define VK_USE_PLATFORM_MACOS_MVK    1
#   define VK_MVK_moltenvk              1
#   import <Cocoa/Cocoa.h>
#   import <Metal/Metal.h>
#   import <QuartzCore/CAMetalLayer.h>
#endif

#if defined(xxIOS)
#   define VK_USE_PLATFORM_IOS_MVK      1
#   define VK_MVK_moltenvk              1
#   import <UIKit/UIKit.h>
#   import <Metal/Metal.h>
#endif

#if defined(xxWINDOWS)
#   define VK_USE_PLATFORM_WIN32_KHR    1
#endif

#define VK_DEFINE_NON_DISPATCHABLE_HANDLE(object) typedef uint64_t object;
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_beta.h"
#undef VK_EXT_debug_report
#undef VK_KHR_push_descriptor
extern bool VK_EXT_debug_report;
extern bool VK_KHR_push_descriptor;
#if VK_MVK_moltenvk
#undef VK_MVK_moltenvk
extern bool VK_MVK_moltenvk;
extern "C" VkResult VKAPI_CALL vkSetMTLTextureMVK(VkImage image, id<MTLTexture> pMTLTexture);
extern "C" void VKAPI_CALL vkGetMTLTextureMVK(VkImage image, id<MTLTexture>* pMTLTexture);
#endif

extern VKAPI_ATTR void* VKAPI_CALL vkGetProcAddress(char const* name);
extern VKAPI_ATTR void VKAPI_CALL vkExitProcAddress();
extern VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetKHREmulate(VkDevice device, VkDescriptorSetLayout setLayout, VkPipelineLayout pipelineLayout);
extern VKAPI_ATTR void VKAPI_CALL vkCompileShader(char const* code, char const*const* macro, int type, uint32_t** output, size_t* size);
//==============================================================================
//  Blend Factor
//==============================================================================
inline VkBlendFactor vkBlendFactor(char const* name)
{
    return xxTemplateBlendFactor<VkBlendFactor, VK_BLEND_FACTOR_ZERO,
                                                VK_BLEND_FACTOR_ONE,
                                                VK_BLEND_FACTOR_SRC_COLOR,
                                                VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
                                                VK_BLEND_FACTOR_DST_COLOR,
                                                VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
                                                VK_BLEND_FACTOR_SRC_ALPHA,
                                                VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                                                VK_BLEND_FACTOR_DST_ALPHA,
                                                VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA>(name);
}
//==============================================================================
//  Blend Operation
//==============================================================================
inline VkBlendOp vkBlendOp(char const* name)
{
    return xxTemplateBlendOp<VkBlendOp, VK_BLEND_OP_ADD,
                                        VK_BLEND_OP_SUBTRACT,
                                        VK_BLEND_OP_REVERSE_SUBTRACT,
                                        VK_BLEND_OP_MIN,
                                        VK_BLEND_OP_MAX>(name);
}
//==============================================================================
//  Depth Comparison
//==============================================================================
inline VkCompareOp vkCompareOp(char const* name)
{
    return xxTemplateCompareOp<VkCompareOp, VK_COMPARE_OP_NEVER,
                                            VK_COMPARE_OP_LESS,
                                            VK_COMPARE_OP_EQUAL,
                                            VK_COMPARE_OP_LESS_OR_EQUAL,
                                            VK_COMPARE_OP_GREATER,
                                            VK_COMPARE_OP_NOT_EQUAL,
                                            VK_COMPARE_OP_GREATER_OR_EQUAL,
                                            VK_COMPARE_OP_ALWAYS>(name);
}
//==============================================================================
//  Framebuffer
//==============================================================================
struct VKFRAMEBUFFER
{
    VkFramebuffer       framebuffer;
};
//==============================================================================
//  Swapchain
//==============================================================================
struct VKSWAPCHAIN : public VKFRAMEBUFFER
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
    VkDeviceMemory      depthStencilMemory;

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
//==============================================================================
//  Vertex Attribute
//==============================================================================
struct VKVERTEXATTRIBUTE
{
    VkVertexInputAttributeDescription   attributeDesc[16];
    uint32_t                            attributeCount;
    VkVertexInputBindingDescription     bindingDesc[4];
    uint32_t                            bindingCount;
};
//==============================================================================
//  Buffer
//==============================================================================
struct VKBUFFER
{
    VkBuffer        buffer;
    VkDeviceMemory  memory;
    VkDeviceSize    size;
    void*           ptr;
    bool            persistent;
};
//==============================================================================
//  Texture
//==============================================================================
struct VKTEXTURE
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
    int             strides[16];
    VkBuffer        uploadBuffer;
    VkDeviceMemory  uploadMemory;
    VkDeviceSize    uploadSize;
    VkDeviceSize    uploadStride;
};
//==============================================================================
