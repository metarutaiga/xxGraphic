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
inline VkBlendFactor vkBlendFactor(const char* name)
{
    bool a = false;
    bool c = false;
    bool d = false;
    bool o = false;
    bool s = false;
    for (char x; (x = (*name)); name++)
    {
        switch (x)
        {
        case 'A':
        case 'a':
            a = true;
            c = false;
            break;
        case 'C':
        case 'c':
            a = false;
            c = true;
            break;
        case 'D':
        case 'd':
            if (s) break;
            d = true;
            break;
        case 'O':
        case 'o':
        case '1':
            o = true;
            break;
        case 'S':
        case 's':
            if (s) break;
            s = true;
            break;
        case 'Z':
        case 'z':
        case '0':
            return VK_BLEND_FACTOR_ZERO;
        }
    }
    if (d)
    {
        if (c) return o ? VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR : VK_BLEND_FACTOR_DST_COLOR;
        if (a) return o ? VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA : VK_BLEND_FACTOR_DST_ALPHA;
    }
    if (s)
    {
        if (c) return o ? VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR : VK_BLEND_FACTOR_SRC_COLOR;
        if (a) return o ? VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA : VK_BLEND_FACTOR_SRC_ALPHA;
    }
    return o ? VK_BLEND_FACTOR_ONE : VK_BLEND_FACTOR_ZERO;
}
//==============================================================================
//  Blend Operation
//==============================================================================
inline VkBlendOp vkBlendOp(const char* name)
{
    for (char x; (x = (*name)); name++)
    {
        switch (x)
        {
        case 'A':
        case 'a':
        case '+':
            return VK_BLEND_OP_ADD;
        case 'S':
        case 's':
        case '-':
            return VK_BLEND_OP_SUBTRACT;
        case 'I':
        case 'i':
        case 'R':
        case 'r':
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case 'M':
        case 'm':
            name++;
            x = (*name);
            return (x == 'I' || x == 'i') ? VK_BLEND_OP_MIN : VK_BLEND_OP_MAX;
        }
    }
    return VK_BLEND_OP_ADD;
}
//==============================================================================
//  Depth Comparison
//==============================================================================
inline VkCompareOp vkCompareOp(const char* name)
{
    bool e = false;
    bool g = false;
    bool l = false;
    for (char x; (x = (*name)); name++)
    {
        switch (x)
        {
        case 'V':
        case 'v':
            return VK_COMPARE_OP_NEVER;
        case 'W':
        case 'w':
            return VK_COMPARE_OP_ALWAYS;
        case 'N':
        case 'n':
        case '!':
            return VK_COMPARE_OP_NOT_EQUAL;
        case 'Q':
        case 'q':
        case '=':
            e = true;
            break;
        case 'G':
        case 'g':
        case '>':
            if (e) break;
            g = true;
            break;
        case 'L':
        case 'l':
        case '<':
            if (e) break;
            l = true;
            break;
        }
    }
    if (e)
    {
        if (l) return VK_COMPARE_OP_LESS_OR_EQUAL;
        if (g) return VK_COMPARE_OP_GREATER_OR_EQUAL;
        return VK_COMPARE_OP_EQUAL;
    }
    if (l) return VK_COMPARE_OP_LESS;
    if (g) return VK_COMPARE_OP_GREATER;
    return VK_COMPARE_OP_ALWAYS;
}
//==============================================================================
