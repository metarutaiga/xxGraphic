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
inline GrAlphaBlendFnc_t grBlendFactor(const char* name)
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
            return GR_BLEND_ZERO;
        }
    }
    if (d)
    {
        if (c) return o ? GR_BLEND_ONE_MINUS_DST_COLOR : GR_BLEND_DST_COLOR;
        if (a) return o ? GR_BLEND_ONE_MINUS_DST_ALPHA : GR_BLEND_DST_ALPHA;
    }
    if (s)
    {
        if (c) return o ? GR_BLEND_ONE_MINUS_SRC_COLOR : GR_BLEND_SRC_COLOR;
        if (a) return o ? GR_BLEND_ONE_MINUS_SRC_ALPHA : GR_BLEND_SRC_ALPHA;
    }
    return o ? GR_BLEND_ONE : GR_BLEND_ZERO;
}
//==============================================================================
//  Depth Comparison
//==============================================================================
inline GrCmpFnc_t grCompareOp(const char* name)
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
            return GR_CMP_NEVER;
        case 'W':
        case 'w':
            return GR_CMP_ALWAYS;
        case 'N':
        case 'n':
        case '!':
            return GR_CMP_NOTEQUAL;
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
        if (l) return GR_CMP_LEQUAL;
        if (g) return GR_CMP_GEQUAL;
        return GR_CMP_EQUAL;
    }
    if (l) return GR_CMP_LESS;
    if (g) return GR_CMP_GREATER;
    return GR_CMP_ALWAYS;
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
