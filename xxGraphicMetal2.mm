//==============================================================================
// xxGraphic : Metal 2 Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#define xxCreateInstanceMetal xxConcate(xxCreateInstanceMetal, __LINE__)
#define xxCreateInstanceMetal2 xxConcate(xxCreateInstanceMetal2, __LINE__)
#include "internal/xxGraphicInternalMetal.h"
#include "xxGraphicMetal.h"
#include "xxGraphicMetal2.h"

//==============================================================================
//  Instance
//==============================================================================
id xxCreateInstanceMetal2()
{
    id instance = xxCreateInstanceMetal();
    if (instance == 0)
        return 0;

    xxRegisterFunctionSingle(xxCreateInstance, xxCreateInstanceMetal2);
    xxRegisterFunctionSingle(xxGetInstanceName, xxGetInstanceNameMetal2);

    xxRegisterFunctionSingle(xxGetCommandBuffer, xxGetCommandBufferMetal2);

    xxRegisterFunctionSingle(xxBeginRenderPass, xxBeginRenderPassMetal2);
    xxRegisterFunctionSingle(xxEndRenderPass, xxEndRenderPassMetal2);

    xxRegisterFunctionSingle(xxCreateConstantBuffer, xxCreateConstantBufferMetal2);
    xxRegisterFunctionSingle(xxCreateIndexBuffer, xxCreateIndexBufferMetal2);
    xxRegisterFunctionSingle(xxCreateVertexBuffer, xxCreateVertexBufferMetal2);
    xxRegisterFunctionSingle(xxDestroyBuffer, xxDestroyBufferMetal2);
    xxRegisterFunctionSingle(xxMapBuffer, xxMapBufferMetal2);
    xxRegisterFunctionSingle(xxUnmapBuffer, xxUnmapBufferMetal2);

    xxRegisterFunctionSingle(xxCreateSampler, xxCreateSamplerMetal2);

    xxRegisterFunctionSingle(xxCreateVertexShader, xxCreateVertexShaderMetal2);
    xxRegisterFunctionSingle(xxCreateFragmentShader, xxCreateFragmentShaderMetal2);

    xxRegisterFunctionSingle(xxCreatePipeline, xxCreatePipelineMetal2);

    xxRegisterFunctionSingle(xxSetViewport, xxSetViewportMetal2);
    xxRegisterFunctionSingle(xxSetScissor, xxSetScissorMetal2);
    xxRegisterFunctionSingle(xxSetPipeline, xxSetPipelineMetal2);
    xxRegisterFunctionSingle(xxSetVertexBuffers, xxSetVertexBuffersMetal2);
    xxRegisterFunctionSingle(xxSetVertexTextures, xxSetVertexTexturesMetal2);
    xxRegisterFunctionSingle(xxSetFragmentTextures, xxSetFragmentTexturesMetal2);
    xxRegisterFunctionSingle(xxSetVertexSamplers, xxSetVertexSamplersMetal2);
    xxRegisterFunctionSingle(xxSetFragmentSamplers, xxSetFragmentSamplersMetal2);
    xxRegisterFunctionSingle(xxSetVertexConstantBuffer, xxSetVertexConstantBufferMetal2);
    xxRegisterFunctionSingle(xxSetFragmentConstantBuffer, xxSetFragmentConstantBufferMetal2);
    xxRegisterFunctionSingle(xxDraw, xxDrawMetal2);
    xxRegisterFunctionSingle(xxDrawIndexed, xxDrawIndexedMetal2);

    return instance;
}
//==============================================================================
//  Swapchain
//==============================================================================
MTLSWAPCHAIN* xxGetCommandBufferMetal2(id <MTLDevice> __unsafe_unretained device, MTLSWAPCHAIN* swapchain)
{
    id commandBuffer = xxGetCommandBufferMetal(device, swapchain);
    if (commandBuffer == 0)
        return 0;

    swapchain->frameCount++;
    swapchain->commandEncoder = nil;
    swapchain->argumentBufferIndex = (swapchain->argumentBufferIndex + 1) % xxCountOf(swapchain->argumentBuffers);
    swapchain->argumentBufferStep = 0;
    swapchain->vertexArgumentEncoder = nil;
    swapchain->fragmentArgumentEncoder = nil;

    return swapchain;
}
//==============================================================================
//  Render Pass
//==============================================================================
MTLSWAPCHAIN* xxBeginRenderPassMetal2(MTLSWAPCHAIN* swapchain, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* __unsafe_unretained renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    id commandEncoder = xxBeginRenderPassMetal(swapchain->commandBuffer, framebuffer, renderPass, width, height, color, depth, stencil);
    if (commandEncoder == 0)
        return 0;

    swapchain->commandEncoder = commandEncoder;

    return swapchain;
}
//------------------------------------------------------------------------------
void xxEndRenderPassMetal2(MTLSWAPCHAIN* swapchain, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* __unsafe_unretained renderPass)
{
    [swapchain->commandEncoder endEncoding];
    swapchain->commandEncoder = nil;
}
//==============================================================================
//  Buffer
//==============================================================================
MTLBUFFER* xxCreateConstantBufferMetal2(id <MTLDevice> __unsafe_unretained device, int size)
{
    if (device == nil)
        return 0;

    MTLBUFFER* buffer = new MTLBUFFER{};
    if (buffer == nullptr)
        return 0;

    buffer->buffer = [device newBufferWithLength:size
                                         options:MTLResourceStorageModeShared];

    return buffer;
}
//------------------------------------------------------------------------------
MTLBUFFER* xxCreateIndexBufferMetal2(id <MTLDevice> __unsafe_unretained device, int size)
{
    if (device == nil)
        return 0;

    MTLBUFFER* buffer = new MTLBUFFER{};
    if (buffer == nullptr)
        return 0;

    buffer->buffer = [device newBufferWithLength:size
                                         options:MTLResourceStorageModeShared];

    return buffer;
}
//------------------------------------------------------------------------------
MTLBUFFER* xxCreateVertexBufferMetal2(id <MTLDevice> __unsafe_unretained device, int size, MTLVertexDescriptor* __unsafe_unretained vertexAttribute)
{
    if (device == nil)
        return 0;

    MTLBUFFER* buffer = new MTLBUFFER{};
    if (buffer == nullptr)
        return 0;

    buffer->buffer = [device newBufferWithLength:size
                                         options:MTLResourceStorageModeShared];

    return buffer;
}
//------------------------------------------------------------------------------
void xxDestroyBufferMetal2(id <MTLDevice> __unsafe_unretained device, MTLBUFFER* buffer)
{
    delete buffer;
}
//------------------------------------------------------------------------------
void* xxMapBufferMetal2(id <MTLDevice> __unsafe_unretained device, MTLBUFFER* buffer)
{
    return [buffer->buffer contents];
}
//------------------------------------------------------------------------------
void xxUnmapBufferMetal2(id <MTLDevice> __unsafe_unretained device, MTLBUFFER* buffer)
{

}
//==============================================================================
//  Sampler
//==============================================================================
id xxCreateSamplerMetal2(id <MTLDevice> __unsafe_unretained device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    if (device == nil)
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

    id <MTLSamplerState> sampler = [device newSamplerStateWithDescriptor:desc];

    objc_retain(sampler);
    return sampler;
}
//==============================================================================
//  Shader
//==============================================================================
id xxCreateVertexShaderMetal2(id <MTLDevice> __unsafe_unretained device, char const* shader, MTLVertexDescriptor* __unsafe_unretained vertexAttribute)
{
    if (device == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = mtlDefaultShaderCode;
    }

    NSError* error;
    MTLCompileOptions* options = [classMTLCompileOptions new];
    options.preprocessorMacros = @{ @"SHADER_MSL" : @(1), @"SHADER_VERTEX" : @(1), @"SHADER_MSL_ARGUMENT" : @(1) };
    options.fastMathEnabled = YES;

    NSString* code = [NSString stringWithFormat:@"%s%s", "#include <metal_stdlib>\nusing namespace metal;\n", shader];
    id <MTLLibrary> library = [device newLibraryWithSource:code
                                                   options:options
                                                     error:&error];
    if (library == nil)
    {
        xxLog("xxGraphic", "%s", error.localizedDescription.UTF8String);
        return 0;
    }

    id <MTLFunction> function = [library newFunctionWithName:@"Main"];

    objc_retain(function);
    return function;
}
//------------------------------------------------------------------------------
id xxCreateFragmentShaderMetal2(id <MTLDevice> __unsafe_unretained device, char const* shader)
{
    if (device == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = mtlDefaultShaderCode;
    }

    NSError* error;
    MTLCompileOptions* options = [classMTLCompileOptions new];
    options.preprocessorMacros = @{ @"SHADER_MSL" : @(1), @"SHADER_FRAGMENT" : @(1), @"SHADER_MSL_ARGUMENT" : @(1) };
    options.fastMathEnabled = YES;

    NSString* code = [NSString stringWithFormat:@"%s%s", "#include <metal_stdlib>\nusing namespace metal;\n", shader];
    id <MTLLibrary> library = [device newLibraryWithSource:code
                                                   options:options
                                                     error:&error];
    if (library == nil)
    {
        xxLog("xxGraphic", "%s", error.localizedDescription.UTF8String);
        return 0;
    }

    id <MTLFunction> function = [library newFunctionWithName:@"Main"];

    objc_retain(function);
    return function;
}
//==============================================================================
//  Pipeline
//==============================================================================
MTLPIPELINE* xxCreatePipelineMetal2(id <MTLDevice> __unsafe_unretained device, MTLRenderPassDescriptor* __unsafe_unretained renderPass, MTLRenderPipelineColorAttachmentDescriptor* __unsafe_unretained blendState, id <MTLDepthStencilState> __unsafe_unretained depthStencilState, uint64_t rasterizerState, MTLVertexDescriptor* __unsafe_unretained vertexAttribute, id <MTLFunction> __unsafe_unretained vertexShader, id <MTLFunction> __unsafe_unretained fragmentShader)
{
    MTLPIPELINE* pipeline = xxCreatePipelineMetal(device, renderPass, blendState, depthStencilState, rasterizerState, vertexAttribute, vertexShader, fragmentShader);
    if (pipeline == 0)
        return 0;

    pipeline->vertexShader = vertexShader;
    pipeline->fragmentShader = fragmentShader;
    pipeline->vertexArgumentEncoder = [pipeline->vertexShader newArgumentEncoderWithBufferIndex:0];
    pipeline->fragmentArgumentEncoder = [pipeline->fragmentShader newArgumentEncoderWithBufferIndex:0];
    pipeline->vertexArgumentEncodedLength = pipeline->vertexArgumentEncoder.encodedLength;
    pipeline->fragmentArgumentEncodedLength = pipeline->fragmentArgumentEncoder.encodedLength;
    pipeline->vertexArgumentEncodedLength = (pipeline->vertexArgumentEncodedLength + 255) & ~255;
    pipeline->fragmentArgumentEncodedLength = (pipeline->fragmentArgumentEncodedLength + 255) & ~255;

    return pipeline;
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportMetal2(MTLSWAPCHAIN* swapchain, int x, int y, int width, int height, float minZ, float maxZ)
{
    xxSetViewportMetal(swapchain->commandEncoder, x, y, width, height, minZ, maxZ);
}
//------------------------------------------------------------------------------
void xxSetScissorMetal2(MTLSWAPCHAIN* swapchain, int x, int y, int width, int height)
{
    xxSetScissorMetal(swapchain->commandEncoder, x, y, width, height);
}
//------------------------------------------------------------------------------
void xxSetPipelineMetal2(MTLSWAPCHAIN* swapchain, MTLPIPELINE* pipeline)
{
    xxSetPipelineMetal(swapchain->commandEncoder, pipeline);

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
    }
    swapchain->argumentBufferStep++;

    swapchain->vertexArgumentEncoder = pipeline->vertexArgumentEncoder;
    swapchain->fragmentArgumentEncoder = pipeline->fragmentArgumentEncoder;
    [swapchain->vertexArgumentEncoder setArgumentBuffer:argumentBuffer
                                                 offset:vertexOffset];
    [swapchain->fragmentArgumentEncoder setArgumentBuffer:argumentBuffer
                                                   offset:fragmentOffset];

    [swapchain->commandEncoder setVertexBuffer:argumentBuffer
                                        offset:vertexOffset
                                       atIndex:0];
    [swapchain->commandEncoder setFragmentBuffer:argumentBuffer
                                          offset:fragmentOffset
                                         atIndex:0];
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersMetal2(MTLSWAPCHAIN* swapchain, int count, MTLBUFFER** buffers, MTLVertexDescriptor* __unsafe_unretained vertexAttribute)
{
    id <MTLBuffer> __unsafe_unretained vertexBuffers[8];

    for (int i = 0; i < count; ++i)
    {
        vertexBuffers[i] = buffers[i]->buffer;
    }

    xxSetVertexBuffersMetal(swapchain->commandEncoder, count, vertexBuffers, vertexAttribute);
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesMetal2(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures)
{
    id <MTLTexture> __unsafe_unretained vertexTextures[8];

    for (int i = 0; i < count; ++i)
    {
        id <MTLTexture> __unsafe_unretained texture = textures[i]->texture;
        vertexTextures[i] = texture;

        if (textures[i]->frameCount != swapchain->frameCount)
        {
            textures[i]->frameCount = swapchain->frameCount;
            [swapchain->commandEncoder useResource:texture
                                             usage:MTLResourceUsageSample];
        }
    }

    [swapchain->vertexArgumentEncoder setTextures:vertexTextures
                                        withRange:NSMakeRange(xxGraphicDescriptor::VERTEX_TEXTURE, count)];
}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesMetal2(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures)
{
    id <MTLTexture> __unsafe_unretained fragmentTextures[8];

    for (int i = 0; i < count; ++i)
    {
        id <MTLTexture> __unsafe_unretained texture = textures[i]->texture;
        fragmentTextures[i] = texture;

        if (textures[i]->frameCount != swapchain->frameCount)
        {
            textures[i]->frameCount = swapchain->frameCount;
            [swapchain->commandEncoder useResource:texture
                                             usage:MTLResourceUsageSample];
        }
    }

    [swapchain->fragmentArgumentEncoder setTextures:fragmentTextures
                                          withRange:NSMakeRange(xxGraphicDescriptor::FRAGMENT_TEXTURE, count)];
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersMetal2(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers)
{
    [swapchain->vertexArgumentEncoder setSamplerStates:samplers
                                             withRange:NSMakeRange(xxGraphicDescriptor::VERTEX_SAMPLER, count)];
}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersMetal2(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers)
{
    [swapchain->fragmentArgumentEncoder setSamplerStates:samplers
                                               withRange:NSMakeRange(xxGraphicDescriptor::FRAGMENT_SAMPLER, count)];
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferMetal2(MTLSWAPCHAIN* swapchain, MTLBUFFER* buffer, int size)
{
    if (buffer->frameCount != swapchain->frameCount)
    {
        buffer->frameCount = swapchain->frameCount;
        [swapchain->commandEncoder useResource:buffer->buffer
                                         usage:MTLResourceUsageRead];
    }

    [swapchain->vertexArgumentEncoder setBuffer:buffer->buffer
                                         offset:0
                                        atIndex:xxGraphicDescriptor::VERTEX_UNIFORM];
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferMetal2(MTLSWAPCHAIN* swapchain, MTLBUFFER* buffer, int size)
{
    if (buffer->frameCount != swapchain->frameCount)
    {
        buffer->frameCount = swapchain->frameCount;
        [swapchain->commandEncoder useResource:buffer->buffer
                                         usage:MTLResourceUsageRead];
    }

    [swapchain->fragmentArgumentEncoder setBuffer:buffer->buffer
                                           offset:0
                                          atIndex:xxGraphicDescriptor::FRAGMENT_UNIFORM];
}
//------------------------------------------------------------------------------
void xxDrawMetal2(MTLSWAPCHAIN* swapchain, int vertexCount, int instanceCount, int firstVertex, int firstInstance)
{
    xxDrawMetal(swapchain->commandEncoder, vertexCount, instanceCount, firstVertex, firstInstance);
}
//------------------------------------------------------------------------------
void xxDrawIndexedMetal2(MTLSWAPCHAIN* swapchain, MTLBUFFER* indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    xxDrawIndexedMetal(swapchain->commandEncoder, indexBuffer->buffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
//------------------------------------------------------------------------------
