//==============================================================================
// xxGraphic : Metal 4 Source
//
// Copyright (c) 2019-2026 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#define xxCreateInstanceMetal xxConcate(xxCreateInstanceMetal, __LINE__)
#define xxCreateInstanceMetal4 xxConcate(xxCreateInstanceMetal4, __LINE__)
#include "internal/xxGraphicInternalMetal.h"
#include "xxGraphicMetal.h"
#include "xxGraphicMetal4.h"

static bool residencySetDirty;
static id <MTLResidencySet> residencySet;

//==============================================================================
//  Instance
//==============================================================================
id xxCreateInstanceMetal4()
{
    id instance = xxCreateInstanceMetal();
    if (instance == 0)
        return 0;

    xxRegisterFunctionSingle(xxCreateInstance, xxCreateInstanceMetal4);
    xxRegisterFunctionSingle(xxDestroyInstance, xxDestroyInstanceMetal4);
    xxRegisterFunctionSingle(xxGetInstanceName, xxGetInstanceNameMetal4);

    xxRegisterFunctionSingle(xxCreateSwapchain, xxCreateSwapchainMetal4);
    xxRegisterFunctionSingle(xxPresentSwapchain, xxPresentSwapchainMetal4);

    xxRegisterFunctionSingle(xxGetCommandBuffer, xxGetCommandBufferMetal4);
    xxRegisterFunctionSingle(xxBeginCommandBuffer, xxBeginCommandBufferMetal4);
    xxRegisterFunctionSingle(xxEndCommandBuffer, xxEndCommandBufferMetal4);

    xxRegisterFunctionSingle(xxCreateRenderPass, xxCreateRenderPassMetal4);
    xxRegisterFunctionSingle(xxBeginRenderPass, xxBeginRenderPassMetal4);
    xxRegisterFunctionSingle(xxEndRenderPass, xxEndRenderPassMetal4);

    xxRegisterFunctionSingle(xxCreateConstantBuffer, xxCreateConstantBufferMetal4);
    xxRegisterFunctionSingle(xxCreateIndexBuffer, xxCreateIndexBufferMetal4);
    xxRegisterFunctionSingle(xxCreateVertexBuffer, xxCreateVertexBufferMetal4);
    xxRegisterFunctionSingle(xxCreateStorageBuffer, xxCreateStorageBufferMetal4);
    xxRegisterFunctionSingle(xxCreateInstanceBuffer, xxCreateInstanceBufferMetal4);
    xxRegisterFunctionSingle(xxDestroyBuffer, xxDestroyBufferMetal4);

    xxRegisterFunctionSingle(xxCreateTexture, xxCreateTextureMetal4);
    xxRegisterFunctionSingle(xxDestroyTexture, xxDestroyTextureMetal4);

    xxRegisterFunctionSingle(xxCreateMeshShader, xxCreateMeshShaderMetal4);
    xxRegisterFunctionSingle(xxCreateVertexShader, xxCreateVertexShaderMetal4);
    xxRegisterFunctionSingle(xxCreateFragmentShader, xxCreateFragmentShaderMetal4);

    xxRegisterFunctionSingle(xxCreateBlendState, xxCreateBlendStateMetal4);
    xxRegisterFunctionSingle(xxCreatePipeline, xxCreatePipelineMetal4);

    xxRegisterFunctionSingle(xxSetViewport, xxSetViewportMetal4);
    xxRegisterFunctionSingle(xxSetScissor, xxSetScissorMetal4);
    xxRegisterFunctionSingle(xxSetPipeline, xxSetPipelineMetal4);
    xxRegisterFunctionSingle(xxSetMeshBuffers, xxSetMeshBuffersMetal4);
    xxRegisterFunctionSingle(xxSetVertexBuffers, xxSetVertexBuffersMetal4);
    xxRegisterFunctionSingle(xxSetMeshTextures, xxSetMeshTexturesMetal4);
    xxRegisterFunctionSingle(xxSetVertexTextures, xxSetVertexTexturesMetal4);
    xxRegisterFunctionSingle(xxSetFragmentTextures, xxSetFragmentTexturesMetal4);
    xxRegisterFunctionSingle(xxSetMeshSamplers, xxSetMeshSamplersMetal4);
    xxRegisterFunctionSingle(xxSetVertexSamplers, xxSetVertexSamplersMetal4);
    xxRegisterFunctionSingle(xxSetFragmentSamplers, xxSetFragmentSamplersMetal4);
    xxRegisterFunctionSingle(xxSetMeshConstantBuffer, xxSetMeshConstantBufferMetal4);
    xxRegisterFunctionSingle(xxSetVertexConstantBuffer, xxSetVertexConstantBufferMetal4);
    xxRegisterFunctionSingle(xxSetFragmentConstantBuffer, xxSetFragmentConstantBufferMetal4);
    xxRegisterFunctionSingle(xxDraw, xxDrawMetal4);
    xxRegisterFunctionSingle(xxDrawMeshed, xxDrawMeshedMetal4);
    xxRegisterFunctionSingle(xxDrawIndexed, xxDrawIndexedMetal4);

    return instance;
}
//------------------------------------------------------------------------------
void xxDestroyInstanceMetal4(id instance)
{
    residencySetDirty = false;
    residencySet = nil;
    xxDestroyInstanceMetal(instance);
}
//==============================================================================
//  Swapchain
//==============================================================================
MTLSWAPCHAIN* xxCreateSwapchainMetal4(id <MTLDevice> __unsafe_unretained device, MTLRenderPassDescriptor* renderPass, void* view, int width, int height, MTLSWAPCHAIN* oldSwapchain)
{
    MTLSWAPCHAIN* swapchain = xxCreateSwapchainMetal(device, renderPass, view, width, height, oldSwapchain);
    if (swapchain == nullptr)
        return nullptr;

    id commandQueue = [device newMTL4CommandQueue];
    id commandBuffer = [device newCommandBuffer];
    swapchain->commandQueue = commandQueue;
    swapchain->commandQueue4 = commandQueue;
    swapchain->commandBuffer = commandBuffer;
    swapchain->commandBuffer4 = commandBuffer;
    for (int i = 0; i < xxCountOf(swapchain->commandAllocators4); ++i)
    {
        swapchain->commandAllocators4[i] = [device newCommandAllocator];
    }

    MTL4ArgumentTableDescriptor* desc = [classMTL4ArgumentTableDescriptor new];
    desc.maxBufferBindCount = 31;
    desc.maxTextureBindCount = 31;
    desc.maxSamplerStateBindCount = 16;

    swapchain->argumentTable = [device newArgumentTableWithDescriptor:desc
                                                                error:nil];

    residencySetDirty = true;
    residencySet = [device newResidencySetWithDescriptor:[classMTLResidencySetDescriptor new]
                                                   error:nil];

    [commandQueue addResidencySet:residencySet];
    [commandQueue addResidencySet:swapchain->metalLayer.residencySet];

    return swapchain;
}
//------------------------------------------------------------------------------
void xxPresentSwapchainMetal4(MTLSWAPCHAIN* swapchain)
{
    id <CAMetalDrawable> __unsafe_unretained drawable = swapchain->drawable;
    id <MTL4CommandQueue> __unsafe_unretained commandQueue = swapchain->commandQueue4;
    id <MTL4CommandBuffer> __unsafe_unretained commandBuffer = swapchain->commandBuffer4;

    if (residencySetDirty)
    {
        residencySetDirty = false;
        [residencySet commit];
    }
    [commandQueue waitForDrawable:drawable];
    [commandQueue commit:&commandBuffer
                   count:1];
    [commandQueue signalDrawable:drawable];
    [drawable present];

    swapchain->texture = nil;
    swapchain->drawable = nil;
}
//==============================================================================
//  Command Buffer
//==============================================================================
MTLSWAPCHAIN* xxGetCommandBufferMetal4(id <MTLDevice> __unsafe_unretained device, MTLSWAPCHAIN* swapchain)
{
    swapchain->commandAllocatorIndex4 = (swapchain->commandAllocatorIndex4 + 1) & xxCountOf(swapchain->commandAllocators4);
    return swapchain;
}
//------------------------------------------------------------------------------
bool xxBeginCommandBufferMetal4(MTLSWAPCHAIN* swapchain)
{
    id <MTL4CommandAllocator> __unsafe_unretained commandAllocator = swapchain->commandAllocators4[swapchain->commandAllocatorIndex4];
    [commandAllocator reset];
    [swapchain->commandBuffer4 beginCommandBufferWithAllocator:commandAllocator];
    return true;
}
//------------------------------------------------------------------------------
void xxEndCommandBufferMetal4(MTLSWAPCHAIN* swapchain)
{
    [swapchain->commandBuffer4 endCommandBuffer];
}
//==============================================================================
//  Render Pass
//==============================================================================
id xxCreateRenderPassMetal4(id <MTLDevice> __unsafe_unretained device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
{
    MTL4RenderPassDescriptor* renderPass = [classMTL4RenderPassDescriptor new];

    renderPass.colorAttachments[0].loadAction = clearColor ? MTLLoadActionClear : MTLLoadActionDontCare;
    renderPass.depthAttachment.loadAction = clearDepth ? MTLLoadActionClear : MTLLoadActionDontCare;
    renderPass.stencilAttachment.loadAction = clearStencil ? MTLLoadActionClear : MTLLoadActionDontCare;

    renderPass.colorAttachments[0].storeAction = storeColor ? MTLStoreActionStore : MTLStoreActionDontCare;
    renderPass.depthAttachment.storeAction = storeDepth ? MTLStoreActionStore : MTLStoreActionDontCare;
    renderPass.stencilAttachment.storeAction = storeStencil ? MTLStoreActionStore : MTLStoreActionDontCare;

    objc_retain(renderPass);
    return renderPass;
}
//------------------------------------------------------------------------------
MTLSWAPCHAIN* xxBeginRenderPassMetal4(MTLSWAPCHAIN* swapchain, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* __unsafe_unretained renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    id commandEncoder = xxBeginRenderPassMetal(swapchain->commandBuffer, framebuffer, renderPass, width, height, color, depth, stencil);
    if (commandEncoder == 0)
        return 0;

    swapchain->commandEncoder = commandEncoder;
    swapchain->commandEncoder4 = commandEncoder;

    [swapchain->commandEncoder4 setArgumentTable:swapchain->argumentTable atStages:MTLRenderStageMesh];
    [swapchain->commandEncoder4 setArgumentTable:swapchain->argumentTable atStages:MTLRenderStageVertex];
    [swapchain->commandEncoder4 setArgumentTable:swapchain->argumentTable atStages:MTLRenderStageFragment];

    return swapchain;
}
//------------------------------------------------------------------------------
void xxEndRenderPassMetal4(MTLSWAPCHAIN* swapchain, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* __unsafe_unretained renderPass)
{
    [swapchain->commandEncoder4 endEncoding];
    swapchain->commandEncoder = nil;
    swapchain->commandEncoder4 = nil;
}
//==============================================================================
//  Buffer
//==============================================================================
id xxCreateConstantBufferMetal4(id <MTLDevice> __unsafe_unretained device, int size)
{
    id buffer = xxCreateConstantBufferMetal(device, size);
    if (buffer == nil)
        return nil;
    residencySetDirty = true;
    [residencySet addAllocation:buffer];
    return buffer;
}
//------------------------------------------------------------------------------
id xxCreateIndexBufferMetal4(id <MTLDevice> __unsafe_unretained device, int size, int bits)
{
    id buffer = xxCreateIndexBufferMetal(device, size, bits);
    if (buffer == nil)
        return nil;
    residencySetDirty = true;
    [residencySet addAllocation:buffer];
    return buffer;
}
//------------------------------------------------------------------------------
id xxCreateVertexBufferMetal4(id <MTLDevice> __unsafe_unretained device, int size, MTLVertexDescriptor* __unsafe_unretained vertexAttribute)
{
    id buffer = xxCreateVertexBufferMetal(device, size, vertexAttribute);
    if (buffer == nil)
        return nil;
    residencySetDirty = true;
    [residencySet addAllocation:buffer];
    return buffer;
}
//------------------------------------------------------------------------------
id xxCreateStorageBufferMetal4(id <MTLDevice> __unsafe_unretained device, int size)
{
    id buffer = xxCreateStorageBufferMetal(device, size);
    if (buffer == nil)
        return nil;
    residencySetDirty = true;
    [residencySet addAllocation:buffer];
    return buffer;
}
//------------------------------------------------------------------------------
id xxCreateInstanceBufferMetal4(id <MTLDevice> __unsafe_unretained device, int size)
{
    id buffer = xxCreateInstanceBufferMetal(device, size);
    if (buffer == nil)
        return nil;
    residencySetDirty = true;
    [residencySet addAllocation:buffer];
    return buffer;
}
//------------------------------------------------------------------------------
void xxDestroyBufferMetal4(id <MTLDevice> __unsafe_unretained device, id <MTLBuffer> __unsafe_unretained buffer)
{
    if (buffer != nil)
    {
        residencySetDirty = true;
        [residencySet removeAllocation:buffer];
    }
    xxDestroyBufferMetal(device, buffer);
}
//==============================================================================
//  Texture
//==============================================================================
MTLTEXTURE* xxCreateTextureMetal4(id <MTLDevice> device, uint64_t format, int width, int height, int depth, int mipmap, int array, void const* external)
{
    MTLTEXTURE* texture = xxCreateTextureMetal(device, format, width, height, depth, mipmap, array, external);
    if (texture == nullptr)
        return nullptr;
    residencySetDirty = true;
    [residencySet addAllocation:texture->texture];
    return texture;
}
//------------------------------------------------------------------------------
void xxDestroyTextureMetal4(MTLTEXTURE* texture)
{
    if (texture)
    {
        residencySetDirty = true;
        [residencySet removeAllocation:texture->texture];
    }
    xxDestroyTextureMetal(texture);
}
//==============================================================================
//  Shader
//==============================================================================
id xxCreateMeshShaderMetal4(id <MTLDevice> __unsafe_unretained device, char const* shader)
{
    if (device == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = mtlDefaultShaderCode;
    }

    NSError* error;
    MTLCompileOptions* options = [classMTLCompileOptions new];
    options.preprocessorMacros = @{ @"SHADER_MSL" : @(4),
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

    MTL4LibraryFunctionDescriptor* function = [classMTL4LibraryFunctionDescriptor new];
    function.library = library;
    function.name = @"Main";

    objc_retain(function);
    return function;
}
//------------------------------------------------------------------------------
id xxCreateVertexShaderMetal4(id <MTLDevice> __unsafe_unretained device, char const* shader, MTLVertexDescriptor* __unsafe_unretained vertexAttribute)
{
    if (device == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = mtlDefaultShaderCode;
    }

    NSError* error;
    MTLCompileOptions* options = [classMTLCompileOptions new];
    options.preprocessorMacros = @{ @"SHADER_MSL" : @(4),
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

    MTL4LibraryFunctionDescriptor* function = [classMTL4LibraryFunctionDescriptor new];
    function.library = library;
    function.name = @"Main";

    objc_retain(function);
    return function;
}
//------------------------------------------------------------------------------
id xxCreateFragmentShaderMetal4(id <MTLDevice> __unsafe_unretained device, char const* shader)
{
    if (device == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = mtlDefaultShaderCode;
    }

    NSError* error;
    MTLCompileOptions* options = [classMTLCompileOptions new];
    options.preprocessorMacros = @{ @"SHADER_MSL" : @(4),
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

    MTL4LibraryFunctionDescriptor* function = [classMTL4LibraryFunctionDescriptor new];
    function.library = library;
    function.name = @"Main";

    objc_retain(function);
    return function;
}
//==============================================================================
//  Pipeline
//==============================================================================
id xxCreateBlendStateMetal4(id <MTLDevice> __unsafe_unretained device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha)
{
    MTL4RenderPipelineColorAttachmentDescriptor* desc = [classMTL4RenderPipelineColorAttachmentDescriptor new];
    desc.pixelFormat = MTLPixelFormatBGRA8Unorm;
    desc.sourceRGBBlendFactor = mtlBlendFactor(sourceColor);
    desc.destinationRGBBlendFactor = mtlBlendFactor(destinationColor);
    desc.rgbBlendOperation = mtlBlendOp(operationColor);
    desc.sourceAlphaBlendFactor = mtlBlendFactor(sourceAlpha);
    desc.destinationAlphaBlendFactor = mtlBlendFactor(destinationAlpha);
    desc.alphaBlendOperation = mtlBlendOp(operationAlpha);
    desc.blendingState = (desc.sourceRGBBlendFactor != MTLBlendFactorOne ||
                          desc.destinationRGBBlendFactor != MTLBlendFactorZero ||
                          desc.rgbBlendOperation != MTLBlendOperationAdd ||
                          desc.sourceAlphaBlendFactor != MTLBlendFactorOne ||
                          desc.destinationAlphaBlendFactor != MTLBlendFactorZero ||
                          desc.alphaBlendOperation != MTLBlendOperationAdd) ? MTL4BlendStateEnabled : MTL4BlendStateDisabled;

    objc_retain(desc);
    return desc;
}
//------------------------------------------------------------------------------
MTLPIPELINE* xxCreatePipelineMetal4(id <MTLDevice> __unsafe_unretained device, MTLRenderPassDescriptor* __unsafe_unretained renderPass, MTL4RenderPipelineColorAttachmentDescriptor* __unsafe_unretained blendState, id <MTLDepthStencilState> __unsafe_unretained depthStencilState, uint64_t rasterizerState, MTLVertexDescriptor* __unsafe_unretained vertexAttribute, MTL4FunctionDescriptor* __unsafe_unretained meshShader, MTL4FunctionDescriptor* __unsafe_unretained vertexShader, MTL4FunctionDescriptor* __unsafe_unretained fragmentShader)
{
    if (device == nil)
        return 0;

    MTLPIPELINE* pipeline = new MTLPIPELINE{};
    if (pipeline == nullptr)
        return 0;

    id <MTL4Compiler> compiler = [device newCompilerWithDescriptor:[classMTL4CompilerDescriptor new]
                                                             error:nil];

    NSError* error;
    if (meshShader)
    {
        MTL4MeshRenderPipelineDescriptor* desc = [classMTL4MeshRenderPipelineDescriptor new];
        desc.meshFunctionDescriptor = meshShader;
        desc.fragmentFunctionDescriptor = fragmentShader;
        desc.colorAttachments[0] = blendState;
        pipeline->pipeline = [compiler newRenderPipelineStateWithDescriptor:desc
                                                        compilerTaskOptions:nil
                                                                      error:&error];
    }
    else
    {
        MTL4RenderPipelineDescriptor* desc = [classMTL4RenderPipelineDescriptor new];
        desc.vertexFunctionDescriptor = vertexShader;
        desc.fragmentFunctionDescriptor = fragmentShader;
        desc.vertexDescriptor = vertexAttribute;
        desc.colorAttachments[0] = blendState;
        pipeline->pipeline = [compiler newRenderPipelineStateWithDescriptor:desc
                                                        compilerTaskOptions:nil
                                                                      error:&error];
    }

    if (pipeline->pipeline == nil)
    {
        xxLog("xxGraphic", "%s", error.localizedDescription.UTF8String);
        return 0;
    }

    pipeline->depthStencil = depthStencilState;
    pipeline->cullMode = (rasterizerState & 0b001) ? MTLCullModeBack : MTLCullModeNone;
    pipeline->fillMode = (rasterizerState & 0b010) ? MTLTriangleFillModeFill : MTLTriangleFillModeLines;
    pipeline->scissorEnable = (rasterizerState & 0b100) != 0;

    return pipeline;
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportMetal4(MTLSWAPCHAIN* swapchain, int x, int y, int width, int height, float minZ, float maxZ)
{
    xxSetViewportMetal(swapchain->commandEncoder, x, y, width, height, minZ, maxZ);
}
//------------------------------------------------------------------------------
void xxSetScissorMetal4(MTLSWAPCHAIN* swapchain, int x, int y, int width, int height)
{
    xxSetScissorMetal(swapchain->commandEncoder, x, y, width, height);
}
//------------------------------------------------------------------------------
void xxSetPipelineMetal4(MTLSWAPCHAIN* swapchain, MTLPIPELINE* pipeline)
{
    xxSetPipelineMetal(swapchain->commandEncoder, pipeline);
}
//------------------------------------------------------------------------------
void xxSetMeshBuffersMetal4(MTLSWAPCHAIN* swapchain, int count, id <MTLBuffer> __unsafe_unretained* buffers)
{
    for (int i = 0; i < count; ++i)
    {
        id <MTLBuffer> __unsafe_unretained buffer = buffers[i];
        if (buffer == nil)
            continue;
        [swapchain->argumentTable setAddress:buffer.gpuAddress
                                     atIndex:VERTEX_TEXTURE + i];
    }
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersMetal4(MTLSWAPCHAIN* swapchain, int count, id <MTLBuffer> __unsafe_unretained* buffers, MTLVertexDescriptor* __unsafe_unretained vertexAttribute)
{
    for (int i = 0; i < count; ++i)
    {
        id <MTLBuffer> __unsafe_unretained buffer = buffers[i];
        if (buffer == nil)
            continue;
        [swapchain->argumentTable setAddress:buffer.gpuAddress
                                     atIndex:VERTEX_BUFFER + i];
    }
}
//------------------------------------------------------------------------------
void xxSetMeshTexturesMetal4(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures)
{
    for (int i = 0; i < count; ++i)
    {
        id <MTLTexture> __unsafe_unretained texture = textures[i] ? textures[i]->texture : nil;
        if (texture == nil)
            continue;
        [swapchain->argumentTable setTexture:texture.gpuResourceID
                                     atIndex:VERTEX_TEXTURE + i];
    }
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesMetal4(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures)
{
    for (int i = 0; i < count; ++i)
    {
        id <MTLTexture> __unsafe_unretained texture = textures[i] ? textures[i]->texture : nil;
        if (texture == nil)
            continue;
        [swapchain->argumentTable setTexture:texture.gpuResourceID
                                     atIndex:VERTEX_TEXTURE + i];
    }
}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesMetal4(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures)
{
    for (int i = 0; i < count; ++i)
    {
        id <MTLTexture> __unsafe_unretained texture = textures[i] ? textures[i]->texture : nil;
        if (texture == nil)
            continue;
        [swapchain->argumentTable setTexture:texture.gpuResourceID
                                     atIndex:FRAGMENT_TEXTURE + i];
    }
}
//------------------------------------------------------------------------------
void xxSetMeshSamplersMetal4(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers)
{
    for (int i = 0; i < count; ++i)
    {
        id <MTLSamplerState> __unsafe_unretained sampler = samplers[i];
        if (sampler == nil)
            continue;
        [swapchain->argumentTable setSamplerState:sampler.gpuResourceID
                                          atIndex:VERTEX_SAMPLER + i];
    }
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersMetal4(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers)
{
    for (int i = 0; i < count; ++i)
    {
        id <MTLSamplerState> __unsafe_unretained sampler = samplers[i];
        if (sampler == nil)
            continue;
        [swapchain->argumentTable setSamplerState:sampler.gpuResourceID
                                          atIndex:VERTEX_SAMPLER + i];
    }
}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersMetal4(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers)
{
    for (int i = 0; i < count; ++i)
    {
        id <MTLSamplerState> __unsafe_unretained sampler = samplers[i];
        if (sampler == nil)
            continue;
        [swapchain->argumentTable setSamplerState:sampler.gpuResourceID
                                          atIndex:FRAGMENT_SAMPLER + i];
    }
}
//------------------------------------------------------------------------------
void xxSetMeshConstantBufferMetal4(MTLSWAPCHAIN* swapchain, id <MTLBuffer> __unsafe_unretained buffer, int size)
{
    [swapchain->argumentTable setAddress:buffer.gpuAddress
                                 atIndex:MESH_UNIFORM];
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferMetal4(MTLSWAPCHAIN* swapchain, id <MTLBuffer> __unsafe_unretained buffer, int size)
{
    [swapchain->argumentTable setAddress:buffer.gpuAddress
                                 atIndex:VERTEX_UNIFORM];
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferMetal4(MTLSWAPCHAIN* swapchain, id <MTLBuffer> __unsafe_unretained buffer, int size)
{
    [swapchain->argumentTable setAddress:buffer.gpuAddress
                                 atIndex:FRAGMENT_UNIFORM];
}
//------------------------------------------------------------------------------
void xxDrawMetal4(MTLSWAPCHAIN* swapchain, int vertexCount, int instanceCount, int firstVertex, int firstInstance)
{
    [swapchain->commandEncoder4 drawPrimitives:MTLPrimitiveTypeTriangle
                                   vertexStart:firstVertex
                                   vertexCount:vertexCount
                                 instanceCount:instanceCount
                                  baseInstance:firstInstance];
}
//------------------------------------------------------------------------------
void xxDrawMeshedMetal4(MTLSWAPCHAIN* swapchain, int x, int y, int z)
{
    [swapchain->commandEncoder4 drawMeshThreadgroups:MTLSize(x, y, z)
                         threadsPerObjectThreadgroup:MTLSize(0, 0, 0)
                           threadsPerMeshThreadgroup:MTLSize(128, 1, 1)];
}
//------------------------------------------------------------------------------
void xxDrawIndexedMetal4(MTLSWAPCHAIN* swapchain, id <MTLBuffer> __unsafe_unretained indexBuffer, int indexCount, int vertexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    MTLIndexType indexType = vertexCount < 65536 ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
    [swapchain->commandEncoder4 drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                           indexCount:indexCount
                                            indexType:indexType
                                          indexBuffer:indexBuffer.gpuAddress + firstIndex * (vertexCount < 65536 ? 2 : 4)
                                    indexBufferLength:indexBuffer.length
                                        instanceCount:instanceCount
                                           baseVertex:vertexOffset
                                         baseInstance:firstInstance];
}
//------------------------------------------------------------------------------
