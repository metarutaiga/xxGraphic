//==============================================================================
// xxGraphic : Glide Internal Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"
#include "xxGraphicInternal.h"

#include "glide/glide.h"

#if defined(xxMACOS) || defined(xxWINDOWS)
GrProc FX_CALL gto_grGetProcAddress(char* name);
#endif

//==============================================================================
//  Blend Factor
//==============================================================================
inline GrAlphaBlendFnc_t grBlendFactor(const char* name)
{
    return xxTemplateBlendFactor<GrAlphaBlendFnc_t, GR_BLEND_ZERO,
                                                    GR_BLEND_ONE,
                                                    GR_BLEND_SRC_COLOR,
                                                    GR_BLEND_ONE_MINUS_SRC_COLOR,
                                                    GR_BLEND_DST_COLOR,
                                                    GR_BLEND_ONE_MINUS_DST_COLOR,
                                                    GR_BLEND_SRC_ALPHA,
                                                    GR_BLEND_ONE_MINUS_SRC_ALPHA,
                                                    GR_BLEND_DST_ALPHA,
                                                    GR_BLEND_ONE_MINUS_DST_ALPHA>(name);
}
//==============================================================================
//  Depth Comparison
//==============================================================================
inline GrCmpFnc_t grCompareOp(const char* name)
{
    return xxTemplateCompareOp<GrCmpFnc_t, GR_CMP_NEVER,
                                           GR_CMP_LESS,
                                           GR_CMP_EQUAL,
                                           GR_CMP_LEQUAL,
                                           GR_CMP_GREATER,
                                           GR_CMP_NOTEQUAL,
                                           GR_CMP_GEQUAL,
                                           GR_CMP_ALWAYS>(name);
}
//==============================================================================
//==============================================================================
//  Context
//==============================================================================
struct GrContext
{
    GrContext_t context;
    void*       view;
    int         width;
    int         height;
    float       scale;
};
//==============================================================================
//  Vertex
//==============================================================================
struct GrVertex
{
    float   x;
    float   y;
    float   ooz;
    float   oow;
    int     rgba;
    float   sow;
    float   tow;
};
//==============================================================================
//  Vertex Attribute
//==============================================================================
union GrVertexAttribute
{
    uint64_t    value;
    struct
    {
        int     flags;
        int     stride;
    };
};
//==============================================================================
//  Texture
//==============================================================================
struct GrTexture : public GrTexInfo
{
    FxU32   startAddress;
    int     width;
    int     height;
    int     stride;
};
//==============================================================================
//  Sampler
//==============================================================================
union GrSampler
{
    uint64_t    value;
    struct
    {
        uint8_t addressU;
        uint8_t addressV;
        uint8_t magFilter;
        uint8_t minFilter;
        uint8_t mipFilter;
    };
};
//==============================================================================
//  Pipeline
//==============================================================================
union GrPipeline
{
    uint64_t        value;
    struct
    {
        uint64_t    blendSourceColor:4;
        uint64_t    blendDestinationColor:4;
        uint64_t    blendSourceAlpha:4;
        uint64_t    blendDestinationAlpha:4;
        uint64_t    depthTest:3;
        uint64_t    depthWrite:1;
        uint64_t    cull:1;
        uint64_t    scissor:1;
    };
};
//==============================================================================
