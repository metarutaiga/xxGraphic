//==============================================================================
// xxGraphic : Vulkan Internal Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

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
inline VkBlendFactor vkBlendFactor(xxGraphicBlendFactor blend)
{
    switch (blend)
    {
    default:
    case BLEND_FACTOR_ZERO:                     return VK_BLEND_FACTOR_ZERO;
    case BLEND_FACTOR_ONE:                      return VK_BLEND_FACTOR_ONE;
    case BLEND_FACTOR_SRC_COLOR:                return VK_BLEND_FACTOR_SRC_COLOR;
    case BLEND_FACTOR_ONE_MINUS_SRC_COLOR:      return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case BLEND_FACTOR_DST_COLOR:                return VK_BLEND_FACTOR_DST_COLOR;
    case BLEND_FACTOR_ONE_MINUS_DST_COLOR:      return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case BLEND_FACTOR_SRC_ALPHA:                return VK_BLEND_FACTOR_SRC_ALPHA;
    case BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:      return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case BLEND_FACTOR_DST_ALPHA:                return VK_BLEND_FACTOR_DST_ALPHA;
    case BLEND_FACTOR_ONE_MINUS_DST_ALPHA:      return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case BLEND_FACTOR_CONSTANT_COLOR:           return VK_BLEND_FACTOR_CONSTANT_COLOR;
    case BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    case BLEND_FACTOR_CONSTANT_ALPHA:           return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    case BLEND_FACTOR_SRC_ALPHA_SATURATE:       return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    case BLEND_FACTOR_SRC1_COLOR:               return VK_BLEND_FACTOR_SRC1_COLOR;
    case BLEND_FACTOR_ONE_MINUS_SRC1_COLOR:     return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
    case BLEND_FACTOR_SRC1_ALPHA:               return VK_BLEND_FACTOR_SRC1_ALPHA;
    case BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA:     return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    }
}
//==============================================================================
