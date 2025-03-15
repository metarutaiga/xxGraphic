//==============================================================================
// xxGraphic : NULL Header
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceNULL();
xxInline char const*    xxGetInstanceNameNULL() { return "NULL"; }

#ifdef _NULL_H_

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceNULL();
void            xxDestroyInstanceNULL(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
uint64_t        xxCreateDeviceNULL(uint64_t instance);
void            xxDestroyDeviceNULL(uint64_t device);
bool            xxResetDeviceNULL(uint64_t device);
bool            xxTestDeviceNULL(uint64_t device);
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t        xxCreateSwapchainNULL(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain);
void            xxDestroySwapchainNULL(uint64_t swapchain);
void            xxPresentSwapchainNULL(uint64_t swapchain);
//==============================================================================
//  Framebuffer
//==============================================================================
uint64_t        xxCreateFramebufferNULL(uint64_t device, uint64_t texture);
void            xxDestroyFramebufferNULL(uint64_t framebuffer);
uint64_t        xxGetFramebufferNULL(uint64_t device, uint64_t swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
uint64_t        xxGetCommandBufferNULL(uint64_t device, uint64_t swapchain);
bool            xxBeginCommandBufferNULL(uint64_t commandBuffer);
void            xxEndCommandBufferNULL(uint64_t commandBuffer);
void            xxSubmitCommandBufferNULL(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t        xxCreateRenderPassNULL(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
void            xxDestroyRenderPassNULL(uint64_t renderPass);
uint64_t        xxBeginRenderPassNULL(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
void            xxEndRenderPassNULL(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t        xxCreateVertexAttributeNULL(uint64_t device, int count, int* attribute);
void            xxDestroyVertexAttributeNULL(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
uint64_t        xxCreateConstantBufferNULL(uint64_t device, int size);
uint64_t        xxCreateIndexBufferNULL(uint64_t device, int size, int bits);
uint64_t        xxCreateVertexBufferNULL(uint64_t device, int size, uint64_t vertexAttribute);
uint64_t        xxCreateStorageBufferNULL(uint64_t device, int size);
uint64_t        xxCreateInstanceBufferNULL(uint64_t device, int size);
void            xxDestroyBufferNULL(uint64_t device, uint64_t buffer);
void*           xxMapBufferNULL(uint64_t device, uint64_t buffer);
void            xxUnmapBufferNULL(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
uint64_t        xxCreateTextureNULL(uint64_t device, uint64_t format, int width, int height, int depth, int mipmap, int array, void const* external);
void            xxDestroyTextureNULL(uint64_t texture);
void*           xxMapTextureNULL(uint64_t device, uint64_t texture, int* stride, int level, int array);
void            xxUnmapTextureNULL(uint64_t device, uint64_t texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
uint64_t        xxCreateSamplerNULL(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
void            xxDestroySamplerNULL(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
uint64_t        xxCreateMeshShaderNULL(uint64_t device, char const* shader);
uint64_t        xxCreateVertexShaderNULL(uint64_t device, char const* shader, uint64_t vertexAttribute);
uint64_t        xxCreateFragmentShaderNULL(uint64_t device, char const* shader);
void            xxDestroyShaderNULL(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t        xxCreateBlendStateNULL(uint64_t device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha);
uint64_t        xxCreateDepthStencilStateNULL(uint64_t device, char const* depthTest, bool depthWrite);
uint64_t        xxCreateRasterizerStateNULL(uint64_t device, bool cull, bool fill, bool scissor);
uint64_t        xxCreatePipelineNULL(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t meshShader, uint64_t vertexShader, uint64_t fragmentShader);
void            xxDestroyBlendStateNULL(uint64_t blendState);
void            xxDestroyDepthStencilStateNULL(uint64_t depthStencilState);
void            xxDestroyRasterizerStateNULL(uint64_t rasterizerState);
void            xxDestroyPipelineNULL(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
void            xxSetViewportNULL(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
void            xxSetScissorNULL(uint64_t commandEncoder, int x, int y, int width, int height);
void            xxSetPipelineNULL(uint64_t commandEncoder, uint64_t pipeline);
void            xxSetMeshBuffersNULL(uint64_t commandEncoder, int count, const uint64_t* buffers);
void            xxSetVertexBuffersNULL(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
void            xxSetMeshTexturesNULL(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetVertexTexturesNULL(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetFragmentTexturesNULL(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetMeshSamplersNULL(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetVertexSamplersNULL(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetFragmentSamplersNULL(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetMeshConstantBufferNULL(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxSetVertexConstantBufferNULL(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxSetFragmentConstantBufferNULL(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxDrawNULL(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
void            xxDrawMeshedNULL(uint64_t commandEncoder, int x, int y, int z);
void            xxDrawIndexedNULL(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int vertexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
