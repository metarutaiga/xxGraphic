//==============================================================================
// xxGraphic : Vulkan Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceVulkan();
xxInline char const*    xxGetInstanceNameVulkan() { return "Vulkan"; }

#ifdef VULKAN_H_

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceVulkan();
void            xxDestroyInstanceVulkan(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
uint64_t        xxCreateDeviceVulkan(uint64_t instance);
void            xxDestroyDeviceVulkan(uint64_t device);
bool            xxResetDeviceVulkan(uint64_t device);
bool            xxTestDeviceVulkan(uint64_t device);
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t        xxCreateSwapchainVulkan(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain);
void            xxDestroySwapchainVulkan(uint64_t swapchain);
void            xxPresentSwapchainVulkan(uint64_t swapchain);
uint64_t        xxGetCommandBufferVulkan(uint64_t device, uint64_t swapchain);
uint64_t        xxGetFramebufferVulkan(uint64_t device, uint64_t swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
bool            xxBeginCommandBufferVulkan(uint64_t commandBuffer);
void            xxEndCommandBufferVulkan(uint64_t commandBuffer);
void            xxSubmitCommandBufferVulkan(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t        xxCreateRenderPassVulkan(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
void            xxDestroyRenderPassVulkan(uint64_t renderPass);
uint64_t        xxBeginRenderPassVulkan(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
void            xxEndRenderPassVulkan(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t        xxCreateVertexAttributeVulkan(uint64_t device, int count, int* attribute);
void            xxDestroyVertexAttributeVulkan(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
uint64_t        xxCreateConstantBufferVulkan(uint64_t device, int size);
uint64_t        xxCreateIndexBufferVulkan(uint64_t device, int size);
uint64_t        xxCreateVertexBufferVulkan(uint64_t device, int size, uint64_t vertexAttribute);
void            xxDestroyBufferVulkan(uint64_t device, uint64_t buffer);
void*           xxMapBufferVulkan(uint64_t device, uint64_t buffer);
void            xxUnmapBufferVulkan(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
uint64_t        xxCreateTextureVulkan(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, void const* external);
void            xxDestroyTextureVulkan(uint64_t texture);
void*           xxMapTextureVulkan(uint64_t device, uint64_t texture, int* stride, int level, int array);
void            xxUnmapTextureVulkan(uint64_t device, uint64_t texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
uint64_t        xxCreateSamplerVulkan(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
void            xxDestroySamplerVulkan(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
uint64_t        xxCreateVertexShaderVulkan(uint64_t device, char const* shader, uint64_t vertexAttribute);
uint64_t        xxCreateFragmentShaderVulkan(uint64_t device, char const* shader);
void            xxDestroyShaderVulkan(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t        xxCreateBlendStateVulkan(uint64_t device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha);
uint64_t        xxCreateDepthStencilStateVulkan(uint64_t device, char const* depthTest, bool depthWrite);
uint64_t        xxCreateRasterizerStateVulkan(uint64_t device, bool cull, bool scissor);
uint64_t        xxCreatePipelineVulkan(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
void            xxDestroyBlendStateVulkan(uint64_t blendState);
void            xxDestroyDepthStencilStateVulkan(uint64_t depthStencilState);
void            xxDestroyRasterizerStateVulkan(uint64_t rasterizerState);
void            xxDestroyPipelineVulkan(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
void            xxSetViewportVulkan(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
void            xxSetScissorVulkan(uint64_t commandEncoder, int x, int y, int width, int height);
void            xxSetPipelineVulkan(uint64_t commandEncoder, uint64_t pipeline);
void            xxSetVertexBuffersVulkan(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
void            xxSetVertexTexturesVulkan(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetFragmentTexturesVulkan(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetVertexSamplersVulkan(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetFragmentSamplersVulkan(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetVertexConstantBufferVulkan(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxSetFragmentConstantBufferVulkan(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxDrawVulkan(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
void            xxDrawIndexedVulkan(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
