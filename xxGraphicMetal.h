//==============================================================================
// xxGraphic : Metal Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceMetal();
xxInline char const*    xxGetInstanceNameMetal() { return "Metal"; }

#ifdef MTL_INTERN
#undef xxCreateInstanceMetal

//==============================================================================
//  Instance
//==============================================================================
xxAPI id        xxCreateInstanceMetal();
void            xxDestroyInstanceMetal(id instance);
//==============================================================================
//  Device
//==============================================================================
id              xxCreateDeviceMetal(id instance);
void            xxDestroyDeviceMetal(id <MTLDevice> device);
bool            xxResetDeviceMetal(id <MTLDevice> device);
bool            xxTestDeviceMetal(id <MTLDevice> device);
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
MTLSWAPCHAIN*   xxCreateSwapchainMetal(id <MTLDevice> device, MTLRenderPassDescriptor* renderPass, void* view, int width, int height, MTLSWAPCHAIN* oldSwapchain);
void            xxDestroySwapchainMetal(MTLSWAPCHAIN* swapchain);
void            xxPresentSwapchainMetal(MTLSWAPCHAIN* swapchain);
id              xxGetCommandBufferMetal(id <MTLDevice> device, MTLSWAPCHAIN* swapchain);
MTLFRAMEBUFFER* xxGetFramebufferMetal(id <MTLDevice> device, MTLSWAPCHAIN* swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
bool            xxBeginCommandBufferMetal(uint64_t commandBuffer);
void            xxEndCommandBufferMetal(uint64_t commandBuffer);
void            xxSubmitCommandBufferMetal(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
id              xxCreateRenderPassMetal(id <MTLDevice> device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
void            xxDestroyRenderPassMetal(MTLRenderPassDescriptor* renderPass);
id              xxBeginRenderPassMetal(id <MTLCommandBuffer> commandBuffer, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
void            xxEndRenderPassMetal(id <MTLRenderCommandEncoder> commandEncoder, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
id              xxCreateVertexAttributeMetal(id <MTLDevice> device, int count, int* attribute);
void            xxDestroyVertexAttributeMetal(MTLVertexDescriptor* vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
id              xxCreateConstantBufferMetal(id <MTLDevice> device, int size);
id              xxCreateIndexBufferMetal(id <MTLDevice> device, int size);
id              xxCreateVertexBufferMetal(id <MTLDevice> device, int size, MTLVertexDescriptor* vertexAttribute);
void            xxDestroyBufferMetal(id <MTLDevice> device, id <MTLBuffer> buffer);
void*           xxMapBufferMetal(id <MTLDevice> device, id <MTLBuffer> buffer);
void            xxUnmapBufferMetal(id <MTLDevice> device, id <MTLBuffer> buffer);
//==============================================================================
//  Texture
//==============================================================================
MTLTEXTURE*     xxCreateTextureMetal(id <MTLDevice> device, int format, int width, int height, int depth, int mipmap, int array, void const* external);
void            xxDestroyTextureMetal(MTLTEXTURE* texture);
void*           xxMapTextureMetal(id <MTLDevice> device, MTLTEXTURE* texture, int* stride, int level, int array);
void            xxUnmapTextureMetal(id <MTLDevice> device, MTLTEXTURE* texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
id              xxCreateSamplerMetal(id <MTLDevice> device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
void            xxDestroySamplerMetal(id <MTLSamplerState> sampler);
//==============================================================================
//  Shader
//==============================================================================
id              xxCreateVertexShaderMetal(id <MTLDevice> device, char const* shader, MTLVertexDescriptor* vertexAttribute);
id              xxCreateFragmentShaderMetal(id <MTLDevice> device, char const* shader);
void            xxDestroyShaderMetal(id <MTLDevice> device, id <MTLFunction> shader);
//==============================================================================
//  Pipeline
//==============================================================================
id              xxCreateBlendStateMetal(id <MTLDevice> device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha);
id              xxCreateDepthStencilStateMetal(id <MTLDevice> device, char const* depthTest, bool depthWrite);
uint64_t        xxCreateRasterizerStateMetal(id <MTLDevice> device, bool cull, bool scissor);
MTLPIPELINE*    xxCreatePipelineMetal(id <MTLDevice> device, MTLRenderPassDescriptor* renderPass, MTLRenderPipelineColorAttachmentDescriptor* blendState, id <MTLDepthStencilState> depthStencilState, uint64_t rasterizerState, MTLVertexDescriptor* vertexAttribute, id <MTLFunction> vertexShader, id <MTLFunction> fragmentShader);
void            xxDestroyBlendStateMetal(MTLRenderPipelineColorAttachmentDescriptor* blendState);
void            xxDestroyDepthStencilStateMetal(id <MTLDepthStencilState> depthStencilState);
void            xxDestroyRasterizerStateMetal(uint64_t rasterizerState);
void            xxDestroyPipelineMetal(MTLPIPELINE* pipeline);
//==============================================================================
//  Command
//==============================================================================
void            xxSetViewportMetal(id <MTLRenderCommandEncoder> commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
void            xxSetScissorMetal(id <MTLRenderCommandEncoder> commandEncoder, int x, int y, int width, int height);
void            xxSetPipelineMetal(id <MTLRenderCommandEncoder> commandEncoder, MTLPIPELINE* pipeline);
void            xxSetVertexBuffersMetal(id <MTLRenderCommandEncoder> commandEncoder, int count, id <MTLBuffer> __unsafe_unretained* buffers, MTLVertexDescriptor* vertexAttribute);
void            xxSetVertexTexturesMetal(id <MTLRenderCommandEncoder> commandEncoder, int count, MTLTEXTURE** textures);
void            xxSetFragmentTexturesMetal(id <MTLRenderCommandEncoder> commandEncoder, int count, MTLTEXTURE** textures);
void            xxSetVertexSamplersMetal(id <MTLRenderCommandEncoder> commandEncoder, int count, id <MTLSamplerState> __unsafe_unretained* samplers);
void            xxSetFragmentSamplersMetal(id <MTLRenderCommandEncoder> commandEncoder, int count, id <MTLSamplerState> __unsafe_unretained* samplers);
void            xxSetVertexConstantBufferMetal(id <MTLRenderCommandEncoder> commandEncoder, id <MTLBuffer> buffer, int size);
void            xxSetFragmentConstantBufferMetal(id <MTLRenderCommandEncoder> commandEncoder, id <MTLBuffer> buffer, int size);
void            xxDrawMetal(id <MTLRenderCommandEncoder> commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
void            xxDrawIndexedMetal(id <MTLRenderCommandEncoder> commandEncoder, id <MTLBuffer> indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
