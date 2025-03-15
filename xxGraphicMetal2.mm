//==============================================================================
// xxGraphic : Metal 2 Source
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#define xxCreateInstanceMetal xxConcate(xxCreateInstanceMetal, __LINE__)
#define xxCreateInstanceMetal2 xxConcate(xxCreateInstanceMetal2, __LINE__)
#include "internal/xxGraphicInternalMetal.h"
#include "xxGraphicMetal.h"
#include "xxGraphicMetal2.h"

#define HAVE_CHECK_USE_RESOURCE 0

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

    xxRegisterFunctionSingle(xxCreateSampler, xxCreateSamplerMetal2);

    xxRegisterFunctionSingle(xxCreateMeshShader, xxCreateMeshShaderMetal2);
    xxRegisterFunctionSingle(xxCreateVertexShader, xxCreateVertexShaderMetal2);
    xxRegisterFunctionSingle(xxCreateFragmentShader, xxCreateFragmentShaderMetal2);

    xxRegisterFunctionSingle(xxCreatePipeline, xxCreatePipelineMetal2);

    xxRegisterFunctionSingle(xxSetViewport, xxSetViewportMetal2);
    xxRegisterFunctionSingle(xxSetScissor, xxSetScissorMetal2);
    xxRegisterFunctionSingle(xxSetPipeline, xxSetPipelineMetal2);
    xxRegisterFunctionSingle(xxSetMeshBuffers, xxSetMeshBuffersMetal2);
    xxRegisterFunctionSingle(xxSetVertexBuffers, xxSetVertexBuffersMetal2);
    xxRegisterFunctionSingle(xxSetMeshTextures, xxSetMeshTexturesMetal2);
    xxRegisterFunctionSingle(xxSetVertexTextures, xxSetVertexTexturesMetal2);
    xxRegisterFunctionSingle(xxSetFragmentTextures, xxSetFragmentTexturesMetal2);
    xxRegisterFunctionSingle(xxSetMeshSamplers, xxSetMeshSamplersMetal2);
    xxRegisterFunctionSingle(xxSetVertexSamplers, xxSetVertexSamplersMetal2);
    xxRegisterFunctionSingle(xxSetFragmentSamplers, xxSetFragmentSamplersMetal2);
    xxRegisterFunctionSingle(xxSetMeshConstantBuffer, xxSetMeshConstantBufferMetal2);
    xxRegisterFunctionSingle(xxSetVertexConstantBuffer, xxSetVertexConstantBufferMetal2);
    xxRegisterFunctionSingle(xxSetFragmentConstantBuffer, xxSetFragmentConstantBufferMetal2);
    xxRegisterFunctionSingle(xxDraw, xxDrawMetal2);
    xxRegisterFunctionSingle(xxDrawMeshed, xxDrawMeshedMetal2);
    xxRegisterFunctionSingle(xxDrawIndexed, xxDrawIndexedMetal2);

    return instance;
}
//==============================================================================
//  Command Buffer
//==============================================================================
MTLSWAPCHAIN* xxGetCommandBufferMetal2(id <MTLDevice> __unsafe_unretained device, MTLSWAPCHAIN* swapchain)
{
    id commandBuffer = xxGetCommandBufferMetal(device, swapchain);
    if (commandBuffer == 0)
        return 0;

    swapchain->pipeline = nullptr;
    swapchain->frameCount++;
    swapchain->frameString = [NSString stringWithFormat:@"%zd", swapchain->frameCount];
    swapchain->commandEncoder = nil;
    swapchain->argumentBufferIndex = (swapchain->argumentBufferIndex + 1) % xxCountOf(swapchain->argumentBuffers);
    swapchain->argumentBufferStep = 0;
    swapchain->meshArgumentEncoder = nil;
    swapchain->vertexArgumentEncoder = nil;
    swapchain->fragmentArgumentEncoder = nil;
    swapchain->argumentEncoderComplete = true;

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
id xxCreateMeshShaderMetal2(id <MTLDevice> __unsafe_unretained device, char const* shader)
{
    if (device == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = mtlDefaultShaderCode;
    }

    NSError* error;
    MTLCompileOptions* options = [classMTLCompileOptions new];
    options.preprocessorMacros = @{ @"SHADER_MSL" : @(2),
                                    @"SHADER_MESH" : @(1) };;
    options.fastMathEnabled = YES;

    NSMutableString* code = [NSMutableString new];
    [code appendString:@"#include <metal_stdlib>"];
    [code appendString:@"\n"];
    [code appendString:@"using namespace metal;"];
    [code appendString:@"\n"];
    [code appendString:@(shader)];
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
    options.preprocessorMacros = @{ @"SHADER_MSL" : @(2),
                                    @"SHADER_VERTEX" : @(1) };;
    options.fastMathEnabled = YES;

    NSMutableString* code = [NSMutableString new];
    [code appendString:@"#include <metal_stdlib>"];
    [code appendString:@"\n"];
    [code appendString:@"using namespace metal;"];
    [code appendString:@"\n"];
    [code appendString:@(shader)];
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
    options.preprocessorMacros = @{ @"SHADER_MSL" : @(2),
                                    @"SHADER_FRAGMENT" : @(1) };;
    options.fastMathEnabled = YES;

    NSMutableString* code = [NSMutableString new];
    [code appendString:@"#include <metal_stdlib>"];
    [code appendString:@"\n"];
    [code appendString:@"using namespace metal;"];
    [code appendString:@"\n"];
    [code appendString:@(shader)];
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
MTLPIPELINE* xxCreatePipelineMetal2(id <MTLDevice> __unsafe_unretained device, MTLRenderPassDescriptor* __unsafe_unretained renderPass, MTLRenderPipelineColorAttachmentDescriptor* __unsafe_unretained blendState, id <MTLDepthStencilState> __unsafe_unretained depthStencilState, uint64_t rasterizerState, MTLVertexDescriptor* __unsafe_unretained vertexAttribute, id <MTLFunction> __unsafe_unretained meshShader, id <MTLFunction> __unsafe_unretained vertexShader, id <MTLFunction> __unsafe_unretained fragmentShader)
{
    MTLPIPELINE* pipeline = xxCreatePipelineMetal(device, renderPass, blendState, depthStencilState, rasterizerState, vertexAttribute, meshShader, vertexShader, fragmentShader);
    if (pipeline == 0)
        return 0;

    pipeline->meshShader = meshShader;
    pipeline->vertexShader = vertexShader;
    pipeline->fragmentShader = fragmentShader;
    pipeline->meshArgumentEncoder = [pipeline->meshShader newArgumentEncoderWithBufferIndex:0];
    pipeline->vertexArgumentEncoder = [pipeline->vertexShader newArgumentEncoderWithBufferIndex:0];
    pipeline->fragmentArgumentEncoder = [pipeline->fragmentShader newArgumentEncoderWithBufferIndex:0];
    pipeline->meshArgumentEncodedLength = pipeline->meshArgumentEncoder.encodedLength;
    pipeline->vertexArgumentEncodedLength = pipeline->vertexArgumentEncoder.encodedLength;
    pipeline->fragmentArgumentEncodedLength = pipeline->fragmentArgumentEncoder.encodedLength;
    pipeline->meshArgumentEncodedLength = (pipeline->meshArgumentEncodedLength + 255) & ~255;
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
    swapchain->pipeline = pipeline;
    xxSetPipelineMetal(swapchain->commandEncoder, pipeline);
}
//------------------------------------------------------------------------------
void xxSetMeshBuffersMetal2(MTLSWAPCHAIN* swapchain, int count, id <MTLBuffer> __unsafe_unretained* buffers)
{
    id <MTLBuffer> __unsafe_unretained meshBuffers[8];

    for (int i = 0; i < count; ++i)
    {
        id <MTLBuffer> __unsafe_unretained buffer = buffers[i];
        meshBuffers[i] = buffer;

        if (buffer == nil)
            continue;
#if HAVE_CHECK_USE_RESOURCE
        NSString* __unsafe_unretained label = buffer.label;
        if (label != swapchain->frameString)
        {
            buffer.label = swapchain->frameString;
#else
        {
#endif
            [swapchain->commandEncoder useResource:buffer
                                             usage:MTLResourceUsageRead
                                            stages:MTLRenderStageMesh];
        }
    }

    static NSUInteger const offsets[8] = {};

    mtlUpdateArgumentEncoder(swapchain);
    [swapchain->meshArgumentEncoder setBuffers:meshBuffers
                                       offsets:offsets
                                     withRange:NSMakeRange(0, count)];
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersMetal2(MTLSWAPCHAIN* swapchain, int count, id <MTLBuffer> __unsafe_unretained* buffers, MTLVertexDescriptor* __unsafe_unretained vertexAttribute)
{
    xxSetVertexBuffersMetal(swapchain->commandEncoder, count, buffers, vertexAttribute);
}
//------------------------------------------------------------------------------
void xxSetMeshTexturesMetal2(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures)
{
    id <MTLTexture> __unsafe_unretained meshTextures[8];

    for (int i = 0; i < count; ++i)
    {
        id <MTLTexture> __unsafe_unretained texture = textures[i] ? textures[i]->texture : nil;
        meshTextures[i] = texture;

        if (texture == nil)
            continue;
#if HAVE_CHECK_USE_RESOURCE
        NSString* __unsafe_unretained label = texture.label;
        if (label != swapchain->frameString)
        {
            texture.label = swapchain->frameString;
#else
        {
#endif
            [swapchain->commandEncoder useResource:texture
                                             usage:MTLResourceUsageRead
                                            stages:MTLRenderStageMesh];
        }
    }

    mtlUpdateArgumentEncoder(swapchain);
    [swapchain->meshArgumentEncoder setTextures:meshTextures
                                      withRange:NSMakeRange(xxGraphicDescriptor::VERTEX_TEXTURE, count)];
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesMetal2(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures)
{
    id <MTLTexture> __unsafe_unretained vertexTextures[8];

    for (int i = 0; i < count; ++i)
    {
        id <MTLTexture> __unsafe_unretained texture = textures[i] ? textures[i]->texture : nil;
        vertexTextures[i] = texture;

        if (texture == nil)
            continue;
#if HAVE_CHECK_USE_RESOURCE
        NSString* __unsafe_unretained label = texture.label;
        if (label != swapchain->frameString)
        {
            texture.label = swapchain->frameString;
#else
        {
#endif
            [swapchain->commandEncoder useResource:texture
                                             usage:MTLResourceUsageRead
                                            stages:MTLRenderStageVertex];
        }
    }

    mtlUpdateArgumentEncoder(swapchain);
    [swapchain->vertexArgumentEncoder setTextures:vertexTextures
                                        withRange:NSMakeRange(xxGraphicDescriptor::VERTEX_TEXTURE, count)];
}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesMetal2(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures)
{
    id <MTLTexture> __unsafe_unretained fragmentTextures[8];

    for (int i = 0; i < count; ++i)
    {
        id <MTLTexture> __unsafe_unretained texture = textures[i] ? textures[i]->texture : nil;
        fragmentTextures[i] = texture;

        if (texture == nil)
            continue;
#if HAVE_CHECK_USE_RESOURCE
        NSString* __unsafe_unretained label = texture.label;
        if (label != swapchain->frameString)
        {
            texture.label = swapchain->frameString;
#else
        {
#endif
            [swapchain->commandEncoder useResource:texture
                                             usage:MTLResourceUsageRead
                                            stages:MTLRenderStageFragment];
        }
    }

    mtlUpdateArgumentEncoder(swapchain);
    [swapchain->fragmentArgumentEncoder setTextures:fragmentTextures
                                          withRange:NSMakeRange(xxGraphicDescriptor::FRAGMENT_TEXTURE, count)];
}
//------------------------------------------------------------------------------
void xxSetMeshSamplersMetal2(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers)
{
    mtlUpdateArgumentEncoder(swapchain);
    [swapchain->meshArgumentEncoder setSamplerStates:samplers
                                           withRange:NSMakeRange(xxGraphicDescriptor::VERTEX_SAMPLER, count)];
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersMetal2(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers)
{
    mtlUpdateArgumentEncoder(swapchain);
    [swapchain->vertexArgumentEncoder setSamplerStates:samplers
                                             withRange:NSMakeRange(xxGraphicDescriptor::VERTEX_SAMPLER, count)];
}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersMetal2(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers)
{
    mtlUpdateArgumentEncoder(swapchain);
    [swapchain->fragmentArgumentEncoder setSamplerStates:samplers
                                               withRange:NSMakeRange(xxGraphicDescriptor::FRAGMENT_SAMPLER, count)];
}
//------------------------------------------------------------------------------
void xxSetMeshConstantBufferMetal2(MTLSWAPCHAIN* swapchain, id <MTLBuffer> __unsafe_unretained buffer, int size)
{
#if HAVE_CHECK_USE_RESOURCE
    NSString* __unsafe_unretained label = buffer.label;
    if (label != swapchain->frameString)
    {
        buffer.label = swapchain->frameString;
#else
    {
#endif
        [swapchain->commandEncoder useResource:buffer
                                         usage:MTLResourceUsageRead
                                        stages:MTLRenderStageMesh];
    }

    mtlUpdateArgumentEncoder(swapchain);
    [swapchain->meshArgumentEncoder setBuffer:buffer
                                       offset:0
                                      atIndex:xxGraphicDescriptor::MESH_UNIFORM];
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferMetal2(MTLSWAPCHAIN* swapchain, id <MTLBuffer> __unsafe_unretained buffer, int size)
{
#if HAVE_CHECK_USE_RESOURCE
    NSString* __unsafe_unretained label = buffer.label;
    if (label != swapchain->frameString)
    {
        buffer.label = swapchain->frameString;
#else
    {
#endif
        [swapchain->commandEncoder useResource:buffer
                                         usage:MTLResourceUsageRead
                                        stages:MTLRenderStageVertex];
    }

    mtlUpdateArgumentEncoder(swapchain);
    [swapchain->vertexArgumentEncoder setBuffer:buffer
                                         offset:0
                                        atIndex:xxGraphicDescriptor::VERTEX_UNIFORM];
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferMetal2(MTLSWAPCHAIN* swapchain, id <MTLBuffer> __unsafe_unretained buffer, int size)
{
#if HAVE_CHECK_USE_RESOURCE
    NSString* __unsafe_unretained label = buffer.label;
    if (label != swapchain->frameString)
    {
        buffer.label = swapchain->frameString;
#else
    {
#endif
        [swapchain->commandEncoder useResource:buffer
                                         usage:MTLResourceUsageRead
                                        stages:MTLRenderStageFragment];
    }

    mtlUpdateArgumentEncoder(swapchain);
    [swapchain->fragmentArgumentEncoder setBuffer:buffer
                                           offset:0
                                          atIndex:xxGraphicDescriptor::FRAGMENT_UNIFORM];
}
//------------------------------------------------------------------------------
void xxDrawMetal2(MTLSWAPCHAIN* swapchain, int vertexCount, int instanceCount, int firstVertex, int firstInstance)
{
    swapchain->argumentEncoderComplete = true;
    xxDrawMetal(swapchain->commandEncoder, vertexCount, instanceCount, firstVertex, firstInstance);
}
//------------------------------------------------------------------------------
void xxDrawMeshedMetal2(MTLSWAPCHAIN* swapchain, int x, int y, int z)
{
    swapchain->argumentEncoderComplete = true;
    xxDrawMeshedMetal(swapchain->commandEncoder, x, y, z);
}
//------------------------------------------------------------------------------
void xxDrawIndexedMetal2(MTLSWAPCHAIN* swapchain, id <MTLBuffer> __unsafe_unretained indexBuffer, int indexCount, int vertexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    swapchain->argumentEncoderComplete = true;
    xxDrawIndexedMetal(swapchain->commandEncoder, indexBuffer, indexCount, vertexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
//------------------------------------------------------------------------------
