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

    xxCreateVertexShader = xxCreateVertexShaderMetal2;
    xxCreateFragmentShader = xxCreateFragmentShaderMetal2;

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
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(swapchain);

    id <MTLCommandBuffer> commandBuffer = [mtlSwapchain->commandQueue commandBuffer];
    mtlSwapchain->commandBuffer = commandBuffer;

    int argumentBufferIndex = mtlSwapchain->argumentBufferIndex = (mtlSwapchain->argumentBufferIndex + 1) % xxCountOf(mtlSwapchain->argumentBuffers);
    if (mtlSwapchain->argumentBuffers[argumentBufferIndex] == nil || [mtlSwapchain->argumentBuffers[argumentBufferIndex] length] < mtlSwapchain->argumentBufferStep)
    {
        mtlSwapchain->argumentBuffers[argumentBufferIndex] = [[mtlSwapchain->commandQueue device] newBufferWithLength:(mtlSwapchain->argumentBufferStep + 1) * 2
                                                                                                              options:MTLResourceStorageModeShared];
    }
    mtlSwapchain->argumentBufferStep = 0;

    return reinterpret_cast<uint64_t>(mtlSwapchain);
}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxBeginRenderPassMetal2(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float r, float g, float b, float a, float depth, unsigned char stencil)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandBuffer);
    MTLFRAMEBUFFER* mtlFramebuffer = reinterpret_cast<MTLFRAMEBUFFER*>(framebuffer);
    MTLRenderPassDescriptor* __unsafe_unretained mtlRenderPass = (__bridge MTLRenderPassDescriptor*)reinterpret_cast<void*>(renderPass);

    mtlRenderPass.colorAttachments[0].texture = mtlFramebuffer->texture;

    mtlRenderPass.colorAttachments[0].clearColor = MTLClearColorMake(r, g, b, a);
    mtlRenderPass.depthAttachment.clearDepth = depth;
    mtlRenderPass.stencilAttachment.clearStencil = stencil;

    mtlSwapchain->commandEncoder = [mtlSwapchain->commandBuffer renderCommandEncoderWithDescriptor:mtlRenderPass];

    MTLViewport vp;
    vp.originX = 0;
    vp.originY = 0;
    vp.width = width;
    vp.height = height;
    vp.znear = 0.0f;
    vp.zfar = 1.0f;
    [mtlSwapchain->commandEncoder setViewport:vp];

    MTLScissorRect rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = width;
    rect.height = height;
    [mtlSwapchain->commandEncoder setScissorRect:rect];

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
//  Shader
//==============================================================================
static const char* const defaultShaderCode =
R"(#include <metal_stdlib>
using namespace metal;

struct Uniforms
{
    float4x4 projectionMatrix;
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
    texture2d<float> tex;
    sampler sam;
};

vertex VertexOut VSMain(VertexIn in [[stage_in]], constant Uniforms& uniforms [[buffer(0)]])
{
    VertexOut out;
    out.position = uniforms.projectionMatrix * float4(in.position, 1);
    out.color = in.color;
    out.uv = in.uv;
    return out;
}

fragment float4 FSMain(VertexOut in [[stage_in]], constant TextureSampler& textureSampler [[buffer(1)]])
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

    mtlSwapchain->vertexArgumentEncoder = [mtlPipeline->vertexShader newArgumentEncoderWithBufferIndex:1];
    mtlSwapchain->fragmentArgumentEncoder = [mtlPipeline->fragmentShader newArgumentEncoderWithBufferIndex:1];
}
//------------------------------------------------------------------------------
void xxSetIndexBufferMetal2(uint64_t commandEncoder, uint64_t buffer)
{

}
//------------------------------------------------------------------------------
void xxSetVertexBuffersMetal2(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLBuffer> mtlBuffers[8];
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
    id <MTLTexture> mtlTextures[8];

    for (int i = 0; i < count; ++i)
    {
        MTLTEXTURE* mtlTexture = reinterpret_cast<MTLTEXTURE*>(textures[i]);
        mtlTextures[i] = mtlTexture->texture;
    }

    [mtlSwapchain->commandEncoder setVertexTextures:mtlTextures
                                          withRange:NSMakeRange(0, count)];
}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesMetal2(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLTexture> mtlTextures[8];

    for (int i = 0; i < count; ++i)
    {
        MTLTEXTURE* mtlTexture = reinterpret_cast<MTLTEXTURE*>(textures[i]);
        mtlTextures[i] = mtlTexture->texture;
    }

    [mtlSwapchain->commandEncoder setFragmentTextures:mtlTextures
                                            withRange:NSMakeRange(0, count)];
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersMetal2(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLSamplerState> mtlSamplers[8];

    for (int i = 0; i < count; ++i)
    {
        mtlSamplers[i] = (__bridge id)reinterpret_cast<void*>(samplers[i]);
    }

    [mtlSwapchain->commandEncoder setVertexSamplerStates:mtlSamplers
                                               withRange:NSMakeRange(0, count)];
}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersMetal2(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLSamplerState> mtlSamplers[8];

    for (int i = 0; i < count; ++i)
    {
        mtlSamplers[i] = (__bridge id)reinterpret_cast<void*>(samplers[i]);
    }

    [mtlSwapchain->commandEncoder setFragmentSamplerStates:mtlSamplers
                                                 withRange:NSMakeRange(0, count)];
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferMetal2(uint64_t commandEncoder, uint64_t buffer, unsigned int size)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLBuffer> mtlBuffer = (__bridge id)reinterpret_cast<void*>(buffer);

    [mtlSwapchain->commandEncoder setVertexBuffer:mtlBuffer
                                           offset:0
                                          atIndex:0];
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferMetal2(uint64_t commandEncoder, uint64_t buffer, unsigned int size)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLBuffer> mtlBuffer = (__bridge id)reinterpret_cast<void*>(buffer);

    [mtlSwapchain->commandEncoder setFragmentBuffer:mtlBuffer
                                             offset:0
                                            atIndex:0];
}
//------------------------------------------------------------------------------
void xxDrawIndexedMetal2(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(commandEncoder);
    id <MTLBuffer> mtlIndexBuffer = (__bridge id)reinterpret_cast<void*>(indexBuffer);

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
