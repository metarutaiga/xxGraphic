//==============================================================================
// xxGraphic : Mantle Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceMantle();
xxInline char const*    xxGetInstanceNameMantle() { return "Mantle"; }

#ifdef _MANTLE_H_

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceMantle();
void            xxDestroyInstanceMantle(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
uint64_t        xxCreateDeviceMantle(uint64_t instance);
void            xxDestroyDeviceMantle(uint64_t device);
bool            xxResetDeviceMantle(uint64_t device);
bool            xxTestDeviceMantle(uint64_t device);
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t        xxCreateSwapchainMantle(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain);
void            xxDestroySwapchainMantle(uint64_t swapchain);
void            xxPresentSwapchainMantle(uint64_t swapchain);
uint64_t        xxGetCommandBufferMantle(uint64_t device, uint64_t swapchain);
uint64_t        xxGetFramebufferMantle(uint64_t device, uint64_t swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
bool            xxBeginCommandBufferMantle(uint64_t commandBuffer);
void            xxEndCommandBufferMantle(uint64_t commandBuffer);
void            xxSubmitCommandBufferMantle(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t        xxCreateRenderPassMantle(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
void            xxDestroyRenderPassMantle(uint64_t renderPass);
uint64_t        xxBeginRenderPassMantle(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
void            xxEndRenderPassMantle(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t        xxCreateVertexAttributeMantle(uint64_t device, int count, int* attribute);
void            xxDestroyVertexAttributeMantle(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
uint64_t        xxCreateConstantBufferMantle(uint64_t device, int size);
uint64_t        xxCreateIndexBufferMantle(uint64_t device, int size);
uint64_t        xxCreateVertexBufferMantle(uint64_t device, int size, uint64_t vertexAttribute);
void            xxDestroyBufferMantle(uint64_t device, uint64_t buffer);
void*           xxMapBufferMantle(uint64_t device, uint64_t buffer);
void            xxUnmapBufferMantle(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
uint64_t        xxCreateTextureMantle(uint64_t device, uint64_t format, int width, int height, int depth, int mipmap, int array, void const* external);
void            xxDestroyTextureMantle(uint64_t texture);
void*           xxMapTextureMantle(uint64_t device, uint64_t texture, int* stride, int level, int array);
void            xxUnmapTextureMantle(uint64_t device, uint64_t texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
uint64_t        xxCreateSamplerMantle(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
void            xxDestroySamplerMantle(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
uint64_t        xxCreateVertexShaderMantle(uint64_t device, char const* shader, uint64_t vertexAttribute);
uint64_t        xxCreateFragmentShaderMantle(uint64_t device, char const* shader);
void            xxDestroyShaderMantle(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t        xxCreateBlendStateMantle(uint64_t device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha);
uint64_t        xxCreateDepthStencilStateMantle(uint64_t device, char const* depthTest, bool depthWrite);
uint64_t        xxCreateRasterizerStateMantle(uint64_t device, bool cull, bool scissor);
uint64_t        xxCreatePipelineMantle(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
void            xxDestroyBlendStateMantle(uint64_t blendState);
void            xxDestroyDepthStencilStateMantle(uint64_t depthStencilState);
void            xxDestroyRasterizerStateMantle(uint64_t rasterizerState);
void            xxDestroyPipelineMantle(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
void            xxSetViewportMantle(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
void            xxSetScissorMantle(uint64_t commandEncoder, int x, int y, int width, int height);
void            xxSetPipelineMantle(uint64_t commandEncoder, uint64_t pipeline);
void            xxSetVertexBuffersMantle(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
void            xxSetVertexTexturesMantle(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetFragmentTexturesMantle(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetVertexSamplersMantle(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetFragmentSamplersMantle(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetVertexConstantBufferMantle(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxSetFragmentConstantBufferMantle(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxDrawMantle(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
void            xxDrawIndexedMantle(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
