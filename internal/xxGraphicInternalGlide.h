//==============================================================================
// xxGraphic : Glide Internal Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

#include "glide/glide.h"

#if defined(xxMACOS) || defined(xxWINDOWS)
GrProc FX_CALL gto_grGetProcAddress(char* name);
#endif

//==============================================================================
//  Blend Factor
//==============================================================================
inline GrAlphaBlendFnc_t grBlendFactor(/*xxGraphicBlendFactor*/int blend)
{
    switch (blend)
    {
    default:
    case BLEND_FACTOR_ZERO:                     return GR_BLEND_ZERO;
    case BLEND_FACTOR_ONE:                      return GR_BLEND_ONE;
    case BLEND_FACTOR_SRC_COLOR:                return GR_BLEND_SRC_COLOR;
    case BLEND_FACTOR_ONE_MINUS_SRC_COLOR:      return GR_BLEND_ONE_MINUS_SRC_COLOR;
    case BLEND_FACTOR_DST_COLOR:                return GR_BLEND_DST_COLOR;
    case BLEND_FACTOR_ONE_MINUS_DST_COLOR:      return GR_BLEND_ONE_MINUS_DST_COLOR;
    case BLEND_FACTOR_SRC_ALPHA:                return GR_BLEND_SRC_ALPHA;
    case BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:      return GR_BLEND_ONE_MINUS_SRC_ALPHA;
    case BLEND_FACTOR_DST_ALPHA:                return GR_BLEND_DST_ALPHA;
    case BLEND_FACTOR_ONE_MINUS_DST_ALPHA:      return GR_BLEND_ONE_MINUS_DST_ALPHA;
    case BLEND_FACTOR_SRC_ALPHA_SATURATE:       return GR_BLEND_ALPHA_SATURATE;
    }
}
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
        uint64_t    depthTest:1;
        uint64_t    depthWrite:1;
        uint64_t    cull:1;
        uint64_t    scissor:1;
    };
};
//==============================================================================
