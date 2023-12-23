//==============================================================================
// xxGraphic : Metal 2 Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceMetal2();
xxInline char const*    xxGetInstanceNameMetal2() { return "Metal 2"; }

#ifdef MTL_INTERN
#undef xxCreateInstanceMetal2

//==============================================================================
//  Instance
//==============================================================================
xxAPI id        xxCreateInstanceMetal2();
//==============================================================================
//  Swapchain
//==============================================================================
MTLSWAPCHAIN*   xxGetCommandBufferMetal2(id <MTLDevice> device, MTLSWAPCHAIN* swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
MTLSWAPCHAIN*   xxBeginRenderPassMetal2(MTLSWAPCHAIN* swapchain, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
void            xxEndRenderPassMetal2(MTLSWAPCHAIN* swapchain, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* renderPass);
//==============================================================================
//  Buffer
//==============================================================================
MTLBUFFER*      xxCreateConstantBufferMetal2(id <MTLDevice> device, int size);
MTLBUFFER*      xxCreateIndexBufferMetal2(id <MTLDevice> device, int size);
MTLBUFFER*      xxCreateVertexBufferMetal2(id <MTLDevice> device, int size, MTLVertexDescriptor* vertexAttribute);
void            xxDestroyBufferMetal2(id <MTLDevice> device, MTLBUFFER* buffer);
void*           xxMapBufferMetal2(id <MTLDevice> device, MTLBUFFER* buffer);
void            xxUnmapBufferMetal2(id <MTLDevice> device, MTLBUFFER* buffer);
//==============================================================================
//  Sampler
//==============================================================================
id              xxCreateSamplerMetal2(id <MTLDevice> device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
//==============================================================================
//  Shader
//==============================================================================
id              xxCreateVertexShaderMetal2(id <MTLDevice> device, char const* shader, MTLVertexDescriptor* vertexAttribute);
id              xxCreateFragmentShaderMetal2(id <MTLDevice> device, char const* shader);
//==============================================================================
//  Pipeline
//==============================================================================
MTLPIPELINE*    xxCreatePipelineMetal2(id <MTLDevice> device, MTLRenderPassDescriptor* renderPass, MTLRenderPipelineColorAttachmentDescriptor* blendState, id <MTLDepthStencilState> depthStencilState, uint64_t rasterizerState, MTLVertexDescriptor* vertexAttribute, id <MTLFunction> vertexShader, id <MTLFunction> fragmentShader);
//==============================================================================
//  Command
//==============================================================================
void            xxSetViewportMetal2(MTLSWAPCHAIN* swapchain, int x, int y, int width, int height, float minZ, float maxZ);
void            xxSetScissorMetal2(MTLSWAPCHAIN* swapchain, int x, int y, int width, int height);
void            xxSetPipelineMetal2(MTLSWAPCHAIN* swapchain, MTLPIPELINE* pipeline);
void            xxSetVertexBuffersMetal2(MTLSWAPCHAIN* swapchain, int count, MTLBUFFER** buffers, MTLVertexDescriptor* vertexAttribute);
void            xxSetVertexTexturesMetal2(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures);
void            xxSetFragmentTexturesMetal2(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures);
void            xxSetVertexSamplersMetal2(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers);
void            xxSetFragmentSamplersMetal2(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers);
void            xxSetVertexConstantBufferMetal2(MTLSWAPCHAIN* swapchain, MTLBUFFER* buffer, int size);
void            xxSetFragmentConstantBufferMetal2(MTLSWAPCHAIN* swapchain, MTLBUFFER* buffer, int size);
void            xxDrawMetal2(MTLSWAPCHAIN* swapchain, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
void            xxDrawIndexedMetal2(MTLSWAPCHAIN* swapchain, MTLBUFFER* indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
