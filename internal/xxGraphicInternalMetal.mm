//==============================================================================
// xxGraphic : Metal Internal Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicInternal.h"
#include "xxGraphicInternalMetal.h"

//==============================================================================
char const* const mtlDefaultShaderCode =
R"(struct Uniform
{
#if SHADER_MSL_ARGUMENT
    device float4* Buffer [[id(0)]];
#else
    float4 Buffer[12];
#endif
};

struct Attribute
{
    float3 Position [[attribute(0)]];
    float4 Color    [[attribute(1)]];
    float2 UV0      [[attribute(2)]];
};

struct Varying
{
    float4 Position [[position]];
    float4 Color;
    float2 UV0;
};

struct Sampler
{
#if SHADER_MSL_ARGUMENT
    texture2d<float> Diffuse    [[id(4)]];
    sampler DiffuseSampler      [[id(18)]];
#else
    texture2d<float> Diffuse    [[texture(0)]];
    sampler DiffuseSampler      [[sampler(0)]];
#endif
};

#if SHADER_VERTEX
vertex Varying Main(Attribute attr [[stage_in]],
                    constant Uniform& uni [[buffer(0)]])
{
    auto uniBuffer = uni.Buffer;
    float4x4 world = float4x4(uniBuffer[0], uniBuffer[1], uniBuffer[2], uniBuffer[3]);
    float4x4 view = float4x4(uniBuffer[4], uniBuffer[5], uniBuffer[6], uniBuffer[7]);
    float4x4 projection = float4x4(uniBuffer[8], uniBuffer[9], uniBuffer[10], uniBuffer[11]);
    Varying vary;
    vary.Position = projection * (view * (world * float4(attr.Position, 1.0)));
    vary.Color = attr.Color;
    vary.UV0 = attr.UV0;
    return vary;
}
#endif

#if SHADER_FRAGMENT
fragment float4 Main(Varying vary [[stage_in]],
#if SHADER_MSL_ARGUMENT
                     constant Sampler& sam [[buffer(0)]])
#else
                     Sampler sam)
#endif
{
    return vary.Color * sam.Diffuse.sample(sam.DiffuseSampler, vary.UV0);
}
#endif)";
//==============================================================================
void mtlUpdateArgumentEncoderInternal(MTLSWAPCHAIN* swapchain)
{
    MTLPIPELINE* pipeline = swapchain->pipeline;

    NSUInteger vertexLength = pipeline->vertexArgumentEncodedLength;
    NSUInteger fragmentLength = pipeline->fragmentArgumentEncodedLength;
    NSUInteger requestLength = vertexLength + fragmentLength;
    NSUInteger vertexOffset = swapchain->argumentBufferStep * requestLength;
    NSUInteger fragmentOffset = vertexOffset + vertexLength;
    NSUInteger capacityLength = fragmentOffset + fragmentLength;

    int argumentBufferIndex = swapchain->argumentBufferIndex;
    id <MTLBuffer> __unsafe_unretained argumentBuffer = swapchain->argumentBuffers[argumentBufferIndex];
    if (argumentBuffer == nil || argumentBuffer.length < capacityLength)
    {
        argumentBuffer = swapchain->argumentBuffers[argumentBufferIndex] = [swapchain->commandQueue.device newBufferWithLength:capacityLength * 2
                                                                                                                       options:MTLResourceStorageModeShared];
        vertexOffset = 0;
        fragmentOffset = vertexOffset + vertexLength;
    }
    swapchain->argumentBufferStep++;

    swapchain->vertexArgumentEncoder = pipeline->vertexArgumentEncoder;
    swapchain->fragmentArgumentEncoder = pipeline->fragmentArgumentEncoder;
    [swapchain->vertexArgumentEncoder setArgumentBuffer:argumentBuffer
                                                 offset:vertexOffset];
    [swapchain->fragmentArgumentEncoder setArgumentBuffer:argumentBuffer
                                                   offset:fragmentOffset];
    swapchain->argumentEncoderComplete = false;

    if (vertexOffset == 0)
    {
        [swapchain->commandEncoder setVertexBuffer:argumentBuffer
                                            offset:vertexOffset
                                           atIndex:0];
        [swapchain->commandEncoder setFragmentBuffer:argumentBuffer
                                              offset:fragmentOffset
                                             atIndex:0];
    }
    else
    {
        [swapchain->commandEncoder setVertexBufferOffset:vertexOffset
                                                 atIndex:0];
        [swapchain->commandEncoder setFragmentBufferOffset:fragmentOffset
                                                   atIndex:0];
    }
}
//==============================================================================
