//==============================================================================
// xxGraphic : Metal Internal Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicInternalMetal.h"

//==============================================================================
const char* const mtlDefaultShaderCode =
R"(#include <metal_stdlib>
using namespace metal;

struct Uniforms
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

struct VertexIn
{
    float3 position [[attribute(0)]];
    float4 color    [[attribute(1)]];
    float2 uv       [[attribute(2)]];
};

struct VertexOut
{
    float4 position [[position]];
    float4 color;
    float2 uv;
};

vertex VertexOut VSMain(VertexIn in [[stage_in]], constant Uniforms& uniforms [[buffer(0)]])
{
    VertexOut out;
    out.position = uniforms.projection * (uniforms.view * (uniforms.world * float4(in.position, 1.0)));
    out.color = in.color;
    out.uv = in.uv;
    return out;
}

fragment float4 FSMain(VertexOut in [[stage_in]], texture2d<float> tex [[texture(0)]], sampler sam [[sampler(0)]])
{
    return in.color * tex.sample(sam, in.uv);
})";
//==============================================================================
