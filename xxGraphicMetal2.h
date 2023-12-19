//==============================================================================
// xxGraphic : Metal 2 Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI uint64_t          xxCreateInstanceMetal2();
xxAPI char const*       xxGetDeviceNameMetal2();

#ifdef MTL_INTERN
#undef xxCreateInstanceMetal2

//==============================================================================
//  Instance
//==============================================================================
xxAPI id                xxCreateInstanceMetal2();
//==============================================================================
//  Device
//==============================================================================
xxAPI char const*       xxGetDeviceNameMetal2();
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI MTLSWAPCHAIN*     xxGetCommandBufferMetal2(id <MTLDevice> device, MTLSWAPCHAIN* swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
xxAPI MTLSWAPCHAIN*     xxBeginRenderPassMetal2(MTLSWAPCHAIN* swapchain, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
xxAPI void              xxEndRenderPassMetal2(MTLSWAPCHAIN* swapchain, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* renderPass);
//==============================================================================
//  Buffer
//==============================================================================
xxAPI MTLBUFFER*        xxCreateConstantBufferMetal2(id <MTLDevice> device, int size);
xxAPI MTLBUFFER*        xxCreateIndexBufferMetal2(id <MTLDevice> device, int size);
xxAPI MTLBUFFER*        xxCreateVertexBufferMetal2(id <MTLDevice> device, int size, MTLVertexDescriptor* vertexAttribute);
xxAPI void              xxDestroyBufferMetal2(id <MTLDevice> device, MTLBUFFER* buffer);
xxAPI void*             xxMapBufferMetal2(id <MTLDevice> device, MTLBUFFER* buffer);
xxAPI void              xxUnmapBufferMetal2(id <MTLDevice> device, MTLBUFFER* buffer);
//==============================================================================
//  Sampler
//==============================================================================
xxAPI id                xxCreateSamplerMetal2(id <MTLDevice> device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
//==============================================================================
//  Shader
//==============================================================================
xxAPI id                xxCreateVertexShaderMetal2(id <MTLDevice> device, char const* shader, MTLVertexDescriptor* vertexAttribute);
xxAPI id                xxCreateFragmentShaderMetal2(id <MTLDevice> device, char const* shader);
//==============================================================================
//  Pipeline
//==============================================================================
xxAPI MTLPIPELINE*      xxCreatePipelineMetal2(id <MTLDevice> device, MTLRenderPassDescriptor* renderPass, MTLRenderPipelineColorAttachmentDescriptor* blendState, id <MTLDepthStencilState> depthStencilState, uint64_t rasterizerState, MTLVertexDescriptor* vertexAttribute, id <MTLFunction> vertexShader, id <MTLFunction> fragmentShader);
//==============================================================================
//  Command
//==============================================================================
xxAPI void              xxSetViewportMetal2(MTLSWAPCHAIN* swapchain, int x, int y, int width, int height, float minZ, float maxZ);
xxAPI void              xxSetScissorMetal2(MTLSWAPCHAIN* swapchain, int x, int y, int width, int height);
xxAPI void              xxSetPipelineMetal2(MTLSWAPCHAIN* swapchain, MTLPIPELINE* pipeline);
xxAPI void              xxSetVertexBuffersMetal2(MTLSWAPCHAIN* swapchain, int count, MTLBUFFER** buffers, MTLVertexDescriptor* vertexAttribute);
xxAPI void              xxSetVertexTexturesMetal2(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures);
xxAPI void              xxSetFragmentTexturesMetal2(MTLSWAPCHAIN* swapchain, int count, MTLTEXTURE** textures);
xxAPI void              xxSetVertexSamplersMetal2(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers);
xxAPI void              xxSetFragmentSamplersMetal2(MTLSWAPCHAIN* swapchain, int count, id <MTLSamplerState> __unsafe_unretained* samplers);
xxAPI void              xxSetVertexConstantBufferMetal2(MTLSWAPCHAIN* swapchain, MTLBUFFER* buffer, int size);
xxAPI void              xxSetFragmentConstantBufferMetal2(MTLSWAPCHAIN* swapchain, MTLBUFFER* buffer, int size);
xxAPI void              xxDrawMetal2(MTLSWAPCHAIN* swapchain, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
xxAPI void              xxDrawIndexedMetal2(MTLSWAPCHAIN* swapchain, MTLBUFFER* indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
