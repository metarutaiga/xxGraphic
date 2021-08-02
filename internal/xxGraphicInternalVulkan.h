//==============================================================================
// xxGraphic : Vulkan Internal Header
//
// Copyright (c) 2019-2021 TAiGA
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
extern bool VK_KHR_push_descriptor;
extern bool VK_EXT_debug_report;
#if VK_MVK_moltenvk
#undef VK_MVK_moltenvk
extern bool VK_MVK_moltenvk;
extern "C" VkResult VKAPI_CALL vkSetMTLTextureMVK(VkImage image, id<MTLTexture> pMTLTexture);
extern "C" void VKAPI_CALL vkGetMTLTextureMVK(VkImage image, id<MTLTexture>* pMTLTexture);
#endif

extern VKAPI_ATTR void* VKAPI_CALL vkGetProcAddress(const char* name);
//==============================================================================
//  Blend Factor
//==============================================================================
inline VkBlendFactor vkBlendFactor(const char* name)
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
inline VkBlendOp vkBlendOp(const char* name)
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
inline VkCompareOp vkCompareOp(const char* name)
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
