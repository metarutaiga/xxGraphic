//==============================================================================
// xxGraphic : Metal 4 Header
//
// Copyright (c) 2019-2026 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceMetal4();
xxInline char const*    xxGetInstanceNameMetal4() { return "Metal 4"; }

#ifdef MTL_INTERN
#undef xxCreateInstanceMetal4

//==============================================================================
//  Instance
//==============================================================================
xxAPI id        xxCreateInstanceMetal4();
//==============================================================================
//  Swapchain
//==============================================================================
MTLSWAPCHAIN*   xxCreateSwapchainMetal4(id <MTLDevice> device, MTLRenderPassDescriptor* renderPass, void* view, int width, int height, MTLSWAPCHAIN* oldSwapchain);
void            xxPresentSwapchainMetal4(MTLSWAPCHAIN* swapchain);
//==============================================================================
//  Command Buffer
//==============================================================================
MTLSWAPCHAIN*   xxGetCommandBufferMetal4(id <MTLDevice> device, MTLSWAPCHAIN* swapchain);
bool            xxBeginCommandBufferMetal4(MTLSWAPCHAIN* swapchain);
void            xxEndCommandBufferMetal4(MTLSWAPCHAIN* swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
id              xxCreateRenderPassMetal4(id <MTLDevice> device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
MTLSWAPCHAIN*   xxBeginRenderPassMetal4(MTLSWAPCHAIN* swapchain, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
void            xxEndRenderPassMetal4(MTLSWAPCHAIN* swapchain, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* renderPass);
//==============================================================================
//  Buffer
//==============================================================================
id              xxCreateConstantBufferMetal4(id <MTLDevice> device, int size);
id              xxCreateIndexBufferMetal4(id <MTLDevice> device, int size, int bits);
id              xxCreateVertexBufferMetal4(id <MTLDevice> device, int size, MTLVertexDescriptor* vertexAttribute);
id              xxCreateStorageBufferMetal4(id <MTLDevice> device, int size);
id              xxCreateInstanceBufferMetal4(id <MTLDevice> device, int size);
void            xxDestroyBufferMetal4(id <MTLDevice> device, id <MTLBuffer> buffer);
//==============================================================================
//  Texture
//==============================================================================
MTLTEXTURE*     xxCreateTextureMetal4(id <MTLDevice> device, uint64_t format, int width, int height, int depth, int mipmap, int array, void const* external);
void            xxDestroyTextureMetal4(MTLTEXTURE* texture);
//==============================================================================
//  Shader
//==============================================================================
id              xxCreateMeshShaderMetal4(id <MTLDevice> device, char const* shader);
id              xxCreateVertexShaderMetal4(id <MTLDevice> device, char const* shader, MTLVertexDescriptor* vertexAttribute);
id              xxCreateFragmentShaderMetal4(id <MTLDevice> device, char const* shader);
//==============================================================================
//  Pipeline
//==============================================================================
id              xxCreateBlendStateMetal4(id <MTLDevice> device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha);
MTLPIPELINE*    xxCreatePipelineMetal4(id <MTLDevice> device, MTLRenderPassDescriptor* renderPass, MTL4RenderPipelineColorAttachmentDescriptor* blendState, id <MTLDepthStencilState> depthStencilState, uint64_t rasterizerState, MTLVertexDescriptor* vertexAttribute, MTL4FunctionDescriptor* meshShader, MTL4FunctionDescriptor* vertexShader, MTL4FunctionDescriptor* fragmentShader);
//==============================================================================
//  Command
//==============================================================================
void            xxSetViewportMetal4(MTLSWAPCHAIN* swapchain, int x, int y, int width, int height, float minZ, float maxZ);
void            xxSetScissorMetal4(MTLSWAPCHAIN* swapchain, int x, int y, int width, int height);
void            xxSetPipelineMetal4(MTLSWAPCHAIN* swapchain, MTLPIPELINE* pipeline);
void            xxSetMeshBuffersMetal4(MTLSWAPCHAIN* swapchain, int count, id <MTLBuffer> __unsafe_unretained* buffers);
void            xxSetVertexBuffersMetal4(MTLSWAPCHAIN* swapchain, int count, id <MTLBuffer> __unsafe_unretained* buffers, MTLVertexDescriptor* vertexAttribute);
void            xxSetMeshTexturesMetal4(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures);
void            xxSetVertexTexturesMetal4(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures);
void            xxSetFragmentTexturesMetal4(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures);
void            xxSetMeshSamplersMetal4(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers);
void            xxSetVertexSamplersMetal4(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers);
void            xxSetFragmentSamplersMetal4(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers);
void            xxSetMeshConstantBufferMetal4(MTLSWAPCHAIN* swapchain, id <MTLBuffer> __unsafe_unretained buffer, int size);
void            xxSetVertexConstantBufferMetal4(MTLSWAPCHAIN* swapchain, id <MTLBuffer> __unsafe_unretained buffer, int size);
void            xxSetFragmentConstantBufferMetal4(MTLSWAPCHAIN* swapchain, id <MTLBuffer> __unsafe_unretained buffer, int size);
void            xxDrawMetal4(MTLSWAPCHAIN* swapchain, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
void            xxDrawMeshedMetal4(MTLSWAPCHAIN* swapchain, int x, int y, int z);
void            xxDrawIndexedMetal4(MTLSWAPCHAIN* swapchain, id <MTLBuffer> __unsafe_unretained indexBuffer, int indexCount, int vertexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
