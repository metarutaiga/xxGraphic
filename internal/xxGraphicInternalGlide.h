//==============================================================================
// xxGraphic : Glide Internal Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

#include "glide/glide.h"

#if defined(xxMACOS)
GrProc FX_CALL gto_grGetProcAddress(char* name);
#endif

//==============================================================================
//  Context
//==============================================================================
union GrContext
{
    uint64_t            value;
    struct
    {
        GrContext_t     context;
        unsigned short  width;
        unsigned short  height;
    };
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
        uint64_t    alphaBlending:1;
        uint64_t    alphaTesting:1;
        uint64_t    depthTest:1;
        uint64_t    depthWrite:1;
        uint64_t    cull:1;
        uint64_t    scissor:1;
    };
};
//==============================================================================
