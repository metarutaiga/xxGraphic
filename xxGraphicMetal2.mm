//==============================================================================
// xxGraphic : Metal 2 Source
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalMetal.h"
#include "xxGraphicMetal.h"
#include "xxGraphicMetal2.h"

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceMetal2()
{
    uint64_t instance = xxCreateInstanceMetal();
    if (instance == 0)
        return 0;

    xxGetDeviceName = xxGetDeviceNameMetal2;

    xxGetCommandBuffer = xxGetCommandBufferMetal2;

    xxBeginRenderPass = xxBeginRenderPassMetal2;
    xxEndRenderPass = xxEndRenderPassMetal2;

    xxCreateSampler = xxCreateSamplerMetal2;

    xxCreateVertexShader = xxCreateVertexShaderMetal2;
    xxCreateFragmentShader = xxCreateFragmentShaderMetal2;

    xxCreatePipeline = xxCreatePipelineMetal2;

    xxSetViewport = xxSetViewportMetal2;
    xxSetScissor = xxSetScissorMetal2;
    xxSetPipeline = xxSetPipelineMetal2;
    xxSetIndexBuffer = xxSetIndexBufferMetal2;
    xxSetVertexBuffers = xxSetVertexBuffersMetal2;
    xxSetVertexTextures = xxSetVertexTexturesMetal2;
    xxSetFragmentTextures = xxSetFragmentTexturesMetal2;
    xxSetVertexSamplers = xxSetVertexSamplersMetal2;
    xxSetFragmentSamplers = xxSetFragmentSamplersMetal2;
    xxSetVertexConstantBuffer = xxSetVertexConstantBufferMetal2;
    xxSetFragmentConstantBuffer = xxSetFragmentConstantBufferMetal2;
    xxDrawIndexed = xxDrawIndexedMetal2;

    return instance;
}
//==============================================================================
//  Device
//==============================================================================
const char* xxGetDeviceNameMetal2()
{
    return "Metal 2";
}
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t xxGetCommandBufferMetal2(uint64_t device, uint64_t swapchain)
{
    uint64_t commandBuffer = xxGetCommandBufferMetal(device, swapchain);
    if (commandBuffer == 0)
        return 0;

    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(swapchain);

    mtlSwapchain->commandEncoder = nil;
    mtlSwapchain->argumentBufferIndex = (mtlSwapchain->argumentBufferIndex + 1) % xxCountOf(mtlSwapchain->argumentBuffers);
    mtlSwapchain->argumentBufferStep = 0;
    mtlSwapchain->vertexArgumentEncoder = nil;
    mtlSwapchain->fragmentArgumentEncoder = nil;

    return reinterpret_cast<uint64_t>(mtlSwapchain);
}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxBeginRenderPassMetal2(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandBuffer);

    uint64_t commandEncoder = xxBeginRenderPassMetal(reinterpret_cast<uint64_t>(mtlSwapchain->commandBuffer), framebuffer, renderPass, width, height, color, depth, stencil);
    if (commandEncoder == 0)
        return 0;

    mtlSwapchain->commandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);

    return reinterpret_cast<uint64_t>(mtlSwapchain);
}
//------------------------------------------------------------------------------
void xxEndRenderPassMetal2(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);

    [mtlSwapchain->commandEncoder endEncoding];
    mtlSwapchain->commandEncoder = nil;
}
//==============================================================================
//  Sampler
//==============================================================================
uint64_t xxCreateSamplerMetal2(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    MTLSamplerDescriptor* desc = [classMTLSamplerDescriptor new];
    desc.sAddressMode = clampU ? MTLSamplerAddressModeClampToEdge : MTLSamplerAddressModeRepeat;
    desc.tAddressMode = clampV ? MTLSamplerAddressModeClampToEdge : MTLSamplerAddressModeRepeat;
    desc.rAddressMode = clampW ? MTLSamplerAddressModeClampToEdge : MTLSamplerAddressModeRepeat;
    desc.magFilter = linearMag ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
    desc.minFilter = linearMin ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
    desc.mipFilter = linearMip ? MTLSamplerMipFilterLinear : MTLSamplerMipFilterNearest;
    desc.maxAnisotropy = anisotropy;
    desc.supportArgumentBuffers = YES;

    id <MTLSamplerState> sampler = [mtlDevice newSamplerStateWithDescriptor:desc];

    return reinterpret_cast<uint64_t>((__bridge_retained void*)sampler);
}
//==============================================================================
//  Shader
//==============================================================================
static const char* const defaultShaderCode =
R"(#include <metal_stdlib>
using namespace metal;

struct Uniforms
{
    device float4x4* projectionMatrix [[id(0)]];
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
    texture2d<float> tex [[id(6)]];
    sampler sam          [[id(18)]];
};

vertex VertexOut VSMain(VertexIn in [[stage_in]], constant Uniforms& uniforms [[buffer(0)]])
{
    VertexOut out;
    out.position = uniforms.projectionMatrix[0] * float4(in.position, 1);
    out.color = in.color;
    out.uv = in.uv;
    return out;
}

fragment float4 FSMain(VertexOut in [[stage_in]], constant TextureSampler& textureSampler [[buffer(0)]])
{
    return in.color * textureSampler.tex.sample(textureSampler.sam, in.uv);
})";
//------------------------------------------------------------------------------
uint64_t xxCreateVertexShaderMetal2(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = defaultShaderCode;
    }

    NSError* error;
    MTLCompileOptions* options = [classMTLCompileOptions new];
    options.fastMathEnabled = YES;

    id <MTLLibrary> library = [mtlDevice newLibraryWithSource:[NSString stringWithUTF8String:shader]
                                                      options:options
                                                        error:&error];
    if (library == nil)
    {
        xxLog("xxGraphic", "%s", [[error localizedDescription] UTF8String]);
        return 0;
    }

    id <MTLFunction> function = [library newFunctionWithName:@"VSMain"];

    return reinterpret_cast<uint64_t>((__bridge_retained void*)function);
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderMetal2(uint64_t device, const char* shader)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = defaultShaderCode;
    }

    NSError* error;
    MTLCompileOptions* options = [classMTLCompileOptions new];
    options.fastMathEnabled = YES;

    id <MTLLibrary> library = [mtlDevice newLibraryWithSource:[NSString stringWithUTF8String:shader]
                                                      options:options
                                                        error:&error];
    if (library == nil)
    {
        xxLog("xxGraphic", "%s", [[error localizedDescription] UTF8String]);
        return 0;
    }

    id <MTLFunction> function = [library newFunctionWithName:@"FSMain"];

    return reinterpret_cast<uint64_t>((__bridge_retained void*)function);
}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreatePipelineMetal2(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    uint64_t pipeline = xxCreatePipelineMetal(device, renderPass, blendState, depthStencilState, rasterizerState, vertexAttribute, vertexShader, fragmentShader);
    if (pipeline == 0)
        return 0;

    MTLPIPELINE* mtlPipeline = reinterpret_cast<MTLPIPELINE*>(pipeline);
    id <MTLFunction> mtlVertexShader = (__bridge id)reinterpret_cast<void*>(vertexShader);
    id <MTLFunction> mtlFragmentShader = (__bridge id)reinterpret_cast<void*>(fragmentShader);

    mtlPipeline->vertexShader = mtlVertexShader;
    mtlPipeline->fragmentShader = mtlFragmentShader;
    mtlPipeline->vertexArgumentEncoder = [mtlPipeline->vertexShader newArgumentEncoderWithBufferIndex:0];
    mtlPipeline->fragmentArgumentEncoder = [mtlPipeline->fragmentShader newArgumentEncoderWithBufferIndex:0];
    mtlPipeline->vertexArgumentEncodedLength = [mtlPipeline->vertexArgumentEncoder encodedLength];
    mtlPipeline->fragmentArgumentEncodedLength = [mtlPipeline->fragmentArgumentEncoder encodedLength];
    mtlPipeline->vertexArgumentEncodedLength = (mtlPipeline->vertexArgumentEncodedLength + 255) & ~255;
    mtlPipeline->fragmentArgumentEncodedLength = (mtlPipeline->fragmentArgumentEncodedLength + 255) & ~255;

    return pipeline;
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportMetal2(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);

    MTLViewport vp;
    vp.originX = x;
    vp.originY = y;
    vp.width = width;
    vp.height = height;
    vp.znear = minZ;
    vp.zfar = maxZ;
    [mtlSwapchain->commandEncoder setViewport:vp];
}
//------------------------------------------------------------------------------
void xxSetScissorMetal2(uint64_t commandEncoder, int x, int y, int width, int height)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);

    MTLScissorRect rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    [mtlSwapchain->commandEncoder setScissorRect:rect];
}
//------------------------------------------------------------------------------
void xxSetPipelineMetal2(uint64_t commandEncoder, uint64_t pipeline)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    MTLPIPELINE* mtlPipeline = reinterpret_cast<MTLPIPELINE*>(pipeline);

    [mtlSwapchain->commandEncoder setRenderPipelineState:mtlPipeline->pipeline];
    [mtlSwapchain->commandEncoder setDepthStencilState:mtlPipeline->depthStencil];

    NSUInteger vertexLength = mtlPipeline->vertexArgumentEncodedLength;
    NSUInteger fragmentLength = mtlPipeline->fragmentArgumentEncodedLength;
    NSUInteger requestLength = vertexLength + fragmentLength;
    NSUInteger vertexOffset = mtlSwapchain->argumentBufferStep * requestLength;
    NSUInteger fragmentOffset = vertexOffset + vertexLength;
    NSUInteger capacityLength = fragmentOffset + fragmentLength;

    int argumentBufferIndex = mtlSwapchain->argumentBufferIndex;
    id <MTLBuffer> __unsafe_unretained argumentBuffer = mtlSwapchain->argumentBuffers[argumentBufferIndex];
    if (argumentBuffer == nil || [argumentBuffer length] < capacityLength)
    {
        argumentBuffer = mtlSwapchain->argumentBuffers[argumentBufferIndex] = [[mtlSwapchain->commandQueue device] newBufferWithLength:capacityLength * 2
                                                                                                                               options:MTLResourceStorageModeShared];
    }
    mtlSwapchain->argumentBufferStep++;

    mtlSwapchain->vertexArgumentEncoder = mtlPipeline->vertexArgumentEncoder;
    mtlSwapchain->fragmentArgumentEncoder = mtlPipeline->fragmentArgumentEncoder;
    [mtlSwapchain->vertexArgumentEncoder setArgumentBuffer:argumentBuffer
                                                    offset:vertexOffset];
    [mtlSwapchain->fragmentArgumentEncoder setArgumentBuffer:argumentBuffer
                                                      offset:fragmentOffset];

    [mtlSwapchain->commandEncoder setVertexBuffer:argumentBuffer
                                           offset:vertexOffset
                                          atIndex:0];
    [mtlSwapchain->commandEncoder setFragmentBuffer:argumentBuffer
                                             offset:fragmentOffset
                                            atIndex:0];
}
//------------------------------------------------------------------------------
void xxSetIndexBufferMetal2(uint64_t commandEncoder, uint64_t buffer)
{

}
//------------------------------------------------------------------------------
void xxSetVertexBuffersMetal2(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLBuffer> __unsafe_unretained mtlBuffers[8];
    NSUInteger offsets[8];

    for (int i = 0; i < count; ++i)
    {
        mtlBuffers[i] = (__bridge id)reinterpret_cast<void*>(buffers[i]);
        offsets[i] = 0;
    }

    [mtlSwapchain->commandEncoder setVertexBuffers:mtlBuffers
                                           offsets:offsets
                                         withRange:NSMakeRange(xxGraphicDescriptor::VERTEX_BUFFER, count)];
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesMetal2(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLTexture> __unsafe_unretained mtlTextures[8];

    for (int i = 0; i < count; ++i)
    {
        MTLTEXTURE* mtlTexture = reinterpret_cast<MTLTEXTURE*>(textures[i]);
        mtlTextures[i] = mtlTexture->texture;
    }

    [mtlSwapchain->vertexArgumentEncoder setTextures:mtlTextures
                                           withRange:NSMakeRange(xxGraphicDescriptor::VERTEX_SAMPLER, count)];
    [mtlSwapchain->commandEncoder useResources:mtlTextures
                                         count:count
                                        usage:MTLResourceUsageSample];
}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesMetal2(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLTexture> __unsafe_unretained mtlTextures[8];

    for (int i = 0; i < count; ++i)
    {
        MTLTEXTURE* mtlTexture = reinterpret_cast<MTLTEXTURE*>(textures[i]);
        mtlTextures[i] = mtlTexture->texture;
    }

    [mtlSwapchain->fragmentArgumentEncoder setTextures:mtlTextures
                                             withRange:NSMakeRange(xxGraphicDescriptor::FRAGMENT_TEXTURE, count)];
    [mtlSwapchain->commandEncoder useResources:mtlTextures
                                         count:count
                                        usage:MTLResourceUsageSample];
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersMetal2(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLSamplerState> __unsafe_unretained mtlSamplers[8];

    for (int i = 0; i < count; ++i)
    {
        mtlSamplers[i] = (__bridge id)reinterpret_cast<void*>(samplers[i]);
    }

    [mtlSwapchain->vertexArgumentEncoder setSamplerStates:mtlSamplers
                                                withRange:NSMakeRange(xxGraphicDescriptor::VERTEX_SAMPLER, count)];
}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersMetal2(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLSamplerState> __unsafe_unretained mtlSamplers[8];

    for (int i = 0; i < count; ++i)
    {
        mtlSamplers[i] = (__bridge id)reinterpret_cast<void*>(samplers[i]);
    }

    [mtlSwapchain->fragmentArgumentEncoder setSamplerStates:mtlSamplers
                                                  withRange:NSMakeRange(xxGraphicDescriptor::FRAGMENT_SAMPLER, count)];
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferMetal2(uint64_t commandEncoder, uint64_t buffer, int size)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLBuffer> __unsafe_unretained mtlBuffer = (__bridge id)reinterpret_cast<void*>(buffer);

    [mtlSwapchain->vertexArgumentEncoder setBuffer:mtlBuffer
                                            offset:0
                                           atIndex:xxGraphicDescriptor::VERTEX_UNIFORM];
    [mtlSwapchain->commandEncoder useResource:mtlBuffer
                                        usage:MTLResourceUsageRead];
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferMetal2(uint64_t commandEncoder, uint64_t buffer, int size)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLBuffer> __unsafe_unretained mtlBuffer = (__bridge id)reinterpret_cast<void*>(buffer);

    [mtlSwapchain->fragmentArgumentEncoder setBuffer:mtlBuffer
                                              offset:0
                                             atIndex:xxGraphicDescriptor::FRAGMENT_UNIFORM];
    [mtlSwapchain->commandEncoder useResource:mtlBuffer
                                        usage:MTLResourceUsageRead];
}
//------------------------------------------------------------------------------
void xxDrawIndexedMetal2(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLBuffer> __unsafe_unretained mtlIndexBuffer = (__bridge id)reinterpret_cast<void*>(indexBuffer);

    MTLIndexType indexType = (INDEX_BUFFER_WIDTH == /* DISABLES CODE */(2)) ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
    [mtlSwapchain->commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                             indexCount:indexCount
                                              indexType:indexType
                                            indexBuffer:mtlIndexBuffer
                                      indexBufferOffset:firstIndex * INDEX_BUFFER_WIDTH
                                          instanceCount:instanceCount
                                             baseVertex:vertexOffset
                                           baseInstance:firstInstance];
}
//------------------------------------------------------------------------------
