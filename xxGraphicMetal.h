//==============================================================================
// xxGraphic : Metal Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI uint64_t          xxCreateInstanceMetal();
xxAPI char const*       xxGetDeviceNameMetal();

#ifdef MTL_INTERN
#undef xxCreateInstanceMetal

//==============================================================================
//  Instance
//==============================================================================
xxAPI id                xxCreateInstanceMetal();
xxAPI void              xxDestroyInstanceMetal(id instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI id                xxCreateDeviceMetal(id instance);
xxAPI void              xxDestroyDeviceMetal(id <MTLDevice> device);
xxAPI bool              xxResetDeviceMetal(id <MTLDevice> device);
xxAPI bool              xxTestDeviceMetal(id <MTLDevice> device);
xxAPI char const*       xxGetDeviceNameMetal();
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI MTLSWAPCHAIN*     xxCreateSwapchainMetal(id <MTLDevice> device, MTLRenderPassDescriptor* renderPass, void* view, int width, int height, MTLSWAPCHAIN* oldSwapchain);
xxAPI void              xxDestroySwapchainMetal(MTLSWAPCHAIN* swapchain);
xxAPI void              xxPresentSwapchainMetal(MTLSWAPCHAIN* swapchain);
xxAPI id                xxGetCommandBufferMetal(id <MTLDevice> device, MTLSWAPCHAIN* swapchain);
xxAPI MTLFRAMEBUFFER*   xxGetFramebufferMetal(id <MTLDevice> device, MTLSWAPCHAIN* swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI bool              xxBeginCommandBufferMetal(uint64_t commandBuffer);
xxAPI void              xxEndCommandBufferMetal(uint64_t commandBuffer);
xxAPI void              xxSubmitCommandBufferMetal(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
xxAPI id                xxCreateRenderPassMetal(id <MTLDevice> device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
xxAPI void              xxDestroyRenderPassMetal(MTLRenderPassDescriptor* renderPass);
xxAPI id                xxBeginRenderPassMetal(id <MTLCommandBuffer> commandBuffer, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
xxAPI void              xxEndRenderPassMetal(id <MTLRenderCommandEncoder> commandEncoder, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI id                xxCreateVertexAttributeMetal(id <MTLDevice> device, int count, int* attribute);
xxAPI void              xxDestroyVertexAttributeMetal(MTLVertexDescriptor* vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
xxAPI id                xxCreateConstantBufferMetal(id <MTLDevice> device, int size);
xxAPI id                xxCreateIndexBufferMetal(id <MTLDevice> device, int size);
xxAPI id                xxCreateVertexBufferMetal(id <MTLDevice> device, int size, MTLVertexDescriptor* vertexAttribute);
xxAPI void              xxDestroyBufferMetal(id <MTLDevice> device, id <MTLBuffer> buffer);
xxAPI void*             xxMapBufferMetal(id <MTLDevice> device, id <MTLBuffer> buffer);
xxAPI void              xxUnmapBufferMetal(id <MTLDevice> device, id <MTLBuffer> buffer);
//==============================================================================
//  Texture
//==============================================================================
xxAPI MTLTEXTURE*       xxCreateTextureMetal(id <MTLDevice> device, int format, int width, int height, int depth, int mipmap, int array, void const* external);
xxAPI void              xxDestroyTextureMetal(MTLTEXTURE* texture);
xxAPI void*             xxMapTextureMetal(id <MTLDevice> device, MTLTEXTURE* texture, int* stride, int level, int array);
xxAPI void              xxUnmapTextureMetal(id <MTLDevice> device, MTLTEXTURE* texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
xxAPI id                xxCreateSamplerMetal(id <MTLDevice> device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
xxAPI void              xxDestroySamplerMetal(id <MTLSamplerState> sampler);
//==============================================================================
//  Shader
//==============================================================================
xxAPI id                xxCreateVertexShaderMetal(id <MTLDevice> device, char const* shader, MTLVertexDescriptor* vertexAttribute);
xxAPI id                xxCreateFragmentShaderMetal(id <MTLDevice> device, char const* shader);
xxAPI void              xxDestroyShaderMetal(id <MTLDevice> device, id <MTLFunction> shader);
//==============================================================================
//  Pipeline
//==============================================================================
xxAPI id                xxCreateBlendStateMetal(id <MTLDevice> device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha);
xxAPI id                xxCreateDepthStencilStateMetal(id <MTLDevice> device, char const* depthTest, bool depthWrite);
xxAPI uint64_t          xxCreateRasterizerStateMetal(id <MTLDevice> device, bool cull, bool scissor);
xxAPI MTLPIPELINE*      xxCreatePipelineMetal(id <MTLDevice> device, MTLRenderPassDescriptor* renderPass, MTLRenderPipelineColorAttachmentDescriptor* blendState, id <MTLDepthStencilState> depthStencilState, uint64_t rasterizerState, MTLVertexDescriptor* vertexAttribute, id <MTLFunction> vertexShader, id <MTLFunction> fragmentShader);
xxAPI void              xxDestroyBlendStateMetal(MTLRenderPipelineColorAttachmentDescriptor* blendState);
xxAPI void              xxDestroyDepthStencilStateMetal(id <MTLDepthStencilState> depthStencilState);
xxAPI void              xxDestroyRasterizerStateMetal(uint64_t rasterizerState);
xxAPI void              xxDestroyPipelineMetal(MTLPIPELINE* pipeline);
//==============================================================================
//  Command
//==============================================================================
xxAPI void              xxSetViewportMetal(id <MTLRenderCommandEncoder> commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
xxAPI void              xxSetScissorMetal(id <MTLRenderCommandEncoder> commandEncoder, int x, int y, int width, int height);
xxAPI void              xxSetPipelineMetal(id <MTLRenderCommandEncoder> commandEncoder, MTLPIPELINE* pipeline);
xxAPI void              xxSetVertexBuffersMetal(id <MTLRenderCommandEncoder> commandEncoder, int count, id <MTLBuffer> __unsafe_unretained* buffers, MTLVertexDescriptor* vertexAttribute);
xxAPI void              xxSetVertexTexturesMetal(id <MTLRenderCommandEncoder> commandEncoder, int count, MTLTEXTURE** textures);
xxAPI void              xxSetFragmentTexturesMetal(id <MTLRenderCommandEncoder> commandEncoder, int count, MTLTEXTURE** textures);
xxAPI void              xxSetVertexSamplersMetal(id <MTLRenderCommandEncoder> commandEncoder, int count, id <MTLSamplerState> __unsafe_unretained* samplers);
xxAPI void              xxSetFragmentSamplersMetal(id <MTLRenderCommandEncoder> commandEncoder, int count, id <MTLSamplerState> __unsafe_unretained* samplers);
xxAPI void              xxSetVertexConstantBufferMetal(id <MTLRenderCommandEncoder> commandEncoder, id <MTLBuffer> buffer, int size);
xxAPI void              xxSetFragmentConstantBufferMetal(id <MTLRenderCommandEncoder> commandEncoder, id <MTLBuffer> buffer, int size);
xxAPI void              xxDrawMetal(id <MTLRenderCommandEncoder> commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
xxAPI void              xxDrawIndexedMetal(id <MTLRenderCommandEncoder> commandEncoder, id <MTLBuffer> indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
