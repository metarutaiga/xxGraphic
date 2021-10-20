//==============================================================================
// xxGraphic : Metal Internal Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicInternal.h"
#include "xxGraphicInternalMetal.h"

//==============================================================================
const char* const mtlDefaultShaderCode =
R"(#include <metal_stdlib>
using namespace metal;

struct Uniform
{
    float4x4 matrix[3];
};

struct UniformArgument
{
    device float4x4* matrix [[id(0)]];
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

struct TextureSampler
{
    texture2d<float> tex [[texture(0)]];
    sampler sam          [[sampler(0)]];
};

struct TextureSamplerArgument
{
    texture2d<float> tex [[id(6)]];
    sampler sam          [[id(18)]];
};

#if __METAL_USE_ARGUMENT__ == 0
vertex VertexOut VSMain(VertexIn in [[stage_in]], constant Uniform& uniforms [[buffer(0)]])
#else
vertex VertexOut VSMain(VertexIn in [[stage_in]], constant UniformArgument& uniforms [[buffer(0)]])
#endif
{
    VertexOut out;
    float4x4 world = uniforms.matrix[0];
    float4x4 view = uniforms.matrix[1];
    float4x4 projection = uniforms.matrix[2];
    out.position = projection * (view * (world * float4(in.position, 1.0)));
    out.color = in.color;
    out.uv = in.uv;
    return out;
}

#if __METAL_USE_ARGUMENT__ == 0
fragment float4 FSMain(VertexOut in [[stage_in]], TextureSampler textureSampler)
#else
fragment float4 FSMain(VertexOut in [[stage_in]], constant TextureSamplerArgument& textureSampler [[buffer(0)]])
#endif
{
    return in.color * textureSampler.tex.sample(textureSampler.sam, in.uv);
})";
//==============================================================================
