//==============================================================================
// xxGraphic : Mantle Internal Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"
#include "xxGraphicInternal.h"

#define GR_PROTOTYPES 1
#include "mantle/mantle.h"
#include "mantle/mantleWsiWinExt.h"
#define NUM_DESCRIPTOR_COUNT        (8)
#define BASE_VERTEX_CONSTANT        (0)
#define BASE_PIXEL_CONSTANT         (1)
#define BASE_VERTEX_ATTRIBUTE       (2)
#define BASE_VERTEX_TEXTURE         (2 + 16)
#define BASE_PIXEL_TEXTURE          (2 + 16 + NUM_DESCRIPTOR_COUNT * 2)
#define BASE_VERTEX_SAMPLER         (2 + 16 + NUM_DESCRIPTOR_COUNT * 3)
#define BASE_PIXEL_SAMPLER          (2 + 16 + NUM_DESCRIPTOR_COUNT * 4)

extern void* grGetProcAddress(const char* name);
//==============================================================================
//  Blend Factor
//==============================================================================
inline GR_BLEND grBlendFactor(const char* name)
{
    return xxTemplateBlendFactor<GR_BLEND, GR_BLEND_ZERO,
                                           GR_BLEND_ONE,
                                           GR_BLEND_SRC_COLOR,
                                           GR_BLEND_ONE_MINUS_SRC_COLOR,
                                           GR_BLEND_DEST_COLOR,
                                           GR_BLEND_ONE_MINUS_DEST_COLOR,
                                           GR_BLEND_SRC_ALPHA,
                                           GR_BLEND_ONE_MINUS_SRC_ALPHA,
                                           GR_BLEND_DEST_ALPHA,
                                           GR_BLEND_ONE_MINUS_DEST_ALPHA>(name);
}
//==============================================================================
//  Blend Operation
//==============================================================================
inline GR_BLEND_FUNC grBlendOp(const char* name)
{
    return xxTemplateBlendOp<GR_BLEND_FUNC, GR_BLEND_FUNC_ADD,
                                            GR_BLEND_FUNC_SUBTRACT,
                                            GR_BLEND_FUNC_REVERSE_SUBTRACT,
                                            GR_BLEND_FUNC_MIN,
                                            GR_BLEND_FUNC_MAX>(name);
}
//==============================================================================
//  Depth Comparison
//==============================================================================
inline GR_COMPARE_FUNC grCompareOp(const char* name)
{
    return xxTemplateCompareOp<GR_COMPARE_FUNC, GR_COMPARE_NEVER,
                                                GR_COMPARE_LESS,
                                                GR_COMPARE_EQUAL,
                                                GR_COMPARE_LESS_EQUAL,
                                                GR_COMPARE_GREATER,
                                                GR_COMPARE_NOT_EQUAL,
                                                GR_COMPARE_GREATER_EQUAL,
                                                GR_COMPARE_ALWAYS>(name);
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
//==============================================================================
//  Vertex Attribute
//==============================================================================
struct VERTEXATTRIBUTEGR
{
    GR_MEMORY_VIEW_ATTACH_INFO  infos[16];
    int                         count;
    int                         stride;
};
//==============================================================================
