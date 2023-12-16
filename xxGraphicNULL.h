//==============================================================================
// xxGraphic : NULL Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceNULL();
xxAPI void          xxDestroyInstanceNULL(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t      xxCreateDeviceNULL(uint64_t instance);
xxAPI void          xxDestroyDeviceNULL(uint64_t device);
xxAPI bool          xxResetDeviceNULL(uint64_t device);
xxAPI bool          xxTestDeviceNULL(uint64_t device);
xxAPI char const*   xxGetDeviceNameNULL();
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI uint64_t      xxCreateSwapchainNULL(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain);
xxAPI void          xxDestroySwapchainNULL(uint64_t swapchain);
xxAPI void          xxPresentSwapchainNULL(uint64_t swapchain);
xxAPI uint64_t      xxGetCommandBufferNULL(uint64_t device, uint64_t swapchain);
xxAPI uint64_t      xxGetFramebufferNULL(uint64_t device, uint64_t swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI bool          xxBeginCommandBufferNULL(uint64_t commandBuffer);
xxAPI void          xxEndCommandBufferNULL(uint64_t commandBuffer);
xxAPI void          xxSubmitCommandBufferNULL(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
xxAPI uint64_t      xxCreateRenderPassNULL(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
xxAPI void          xxDestroyRenderPassNULL(uint64_t renderPass);
xxAPI uint64_t      xxBeginRenderPassNULL(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
xxAPI void          xxEndRenderPassNULL(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t      xxCreateVertexAttributeNULL(uint64_t device, int count, int* attribute);
xxAPI void          xxDestroyVertexAttributeNULL(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
xxAPI uint64_t      xxCreateConstantBufferNULL(uint64_t device, int size);
xxAPI uint64_t      xxCreateIndexBufferNULL(uint64_t device, int size);
xxAPI uint64_t      xxCreateVertexBufferNULL(uint64_t device, int size, uint64_t vertexAttribute);
xxAPI void          xxDestroyBufferNULL(uint64_t device, uint64_t buffer);
xxAPI void*         xxMapBufferNULL(uint64_t device, uint64_t buffer);
xxAPI void          xxUnmapBufferNULL(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
xxAPI uint64_t      xxCreateTextureNULL(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, void const* external);
xxAPI void          xxDestroyTextureNULL(uint64_t texture);
xxAPI void*         xxMapTextureNULL(uint64_t device, uint64_t texture, int* stride, int level, int array);
xxAPI void          xxUnmapTextureNULL(uint64_t device, uint64_t texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
xxAPI uint64_t      xxCreateSamplerNULL(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
xxAPI void          xxDestroySamplerNULL(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
xxAPI uint64_t      xxCreateVertexShaderNULL(uint64_t device, char const* shader, uint64_t vertexAttribute);
xxAPI uint64_t      xxCreateFragmentShaderNULL(uint64_t device, char const* shader);
xxAPI void          xxDestroyShaderNULL(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
xxAPI uint64_t      xxCreateBlendStateNULL(uint64_t device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha);
xxAPI uint64_t      xxCreateDepthStencilStateNULL(uint64_t device, char const* depthTest, bool depthWrite);
xxAPI uint64_t      xxCreateRasterizerStateNULL(uint64_t device, bool cull, bool scissor);
xxAPI uint64_t      xxCreatePipelineNULL(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
xxAPI void          xxDestroyBlendStateNULL(uint64_t blendState);
xxAPI void          xxDestroyDepthStencilStateNULL(uint64_t depthStencilState);
xxAPI void          xxDestroyRasterizerStateNULL(uint64_t rasterizerState);
xxAPI void          xxDestroyPipelineNULL(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
xxAPI void          xxSetViewportNULL(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
xxAPI void          xxSetScissorNULL(uint64_t commandEncoder, int x, int y, int width, int height);
xxAPI void          xxSetPipelineNULL(uint64_t commandEncoder, uint64_t pipeline);
xxAPI void          xxSetVertexBuffersNULL(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
xxAPI void          xxSetVertexTexturesNULL(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetFragmentTexturesNULL(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetVertexSamplersNULL(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetFragmentSamplersNULL(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetVertexConstantBufferNULL(uint64_t commandEncoder, uint64_t buffer, int size);
xxAPI void          xxSetFragmentConstantBufferNULL(uint64_t commandEncoder, uint64_t buffer, int size);
xxAPI void          xxDrawNULL(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
xxAPI void          xxDrawIndexedNULL(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
