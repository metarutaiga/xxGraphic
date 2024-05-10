//==============================================================================
// xxGraphic : Glide Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceGlide();
xxInline char const*    xxGetInstanceNameGlide() { return "Glide"; }

#ifdef __3DFX_H__

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceGlide();
void            xxDestroyInstanceGlide(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
uint64_t        xxCreateDeviceGlide(uint64_t instance);
void            xxDestroyDeviceGlide(uint64_t device);
bool            xxResetDeviceGlide(uint64_t device);
bool            xxTestDeviceGlide(uint64_t device);
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t        xxCreateSwapchainGlide(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain);
void            xxDestroySwapchainGlide(uint64_t swapchain);
void            xxPresentSwapchainGlide(uint64_t swapchain);
uint64_t        xxGetCommandBufferGlide(uint64_t device, uint64_t swapchain);
uint64_t        xxGetFramebufferGlide(uint64_t device, uint64_t swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
bool            xxBeginCommandBufferGlide(uint64_t commandBuffer);
void            xxEndCommandBufferGlide(uint64_t commandBuffer);
void            xxSubmitCommandBufferGlide(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t        xxCreateRenderPassGlide(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
void            xxDestroyRenderPassGlide(uint64_t renderPass);
uint64_t        xxBeginRenderPassGlide(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
void            xxEndRenderPassGlide(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t        xxCreateVertexAttributeGlide(uint64_t device, int count, int* attribute);
void            xxDestroyVertexAttributeGlide(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
uint64_t        xxCreateConstantBufferGlide(uint64_t device, int size);
uint64_t        xxCreateIndexBufferGlide(uint64_t device, int size);
uint64_t        xxCreateVertexBufferGlide(uint64_t device, int size, uint64_t vertexAttribute);
void            xxDestroyBufferGlide(uint64_t device, uint64_t buffer);
void*           xxMapBufferGlide(uint64_t device, uint64_t buffer);
void            xxUnmapBufferGlide(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
uint64_t        xxCreateTextureGlide(uint64_t device, uint64_t format, int width, int height, int depth, int mipmap, int array, void const* external);
void            xxDestroyTextureGlide(uint64_t texture);
void*           xxMapTextureGlide(uint64_t device, uint64_t texture, int* stride, int level, int array);
void            xxUnmapTextureGlide(uint64_t device, uint64_t texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
uint64_t        xxCreateSamplerGlide(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
void            xxDestroySamplerGlide(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
uint64_t        xxCreateVertexShaderGlide(uint64_t device, char const* shader, uint64_t vertexAttribute);
uint64_t        xxCreateFragmentShaderGlide(uint64_t device, char const* shader);
void            xxDestroyShaderGlide(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t        xxCreateBlendStateGlide(uint64_t device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha);
uint64_t        xxCreateDepthStencilStateGlide(uint64_t device, char const* depthTest, bool depthWrite);
uint64_t        xxCreateRasterizerStateGlide(uint64_t device, bool cull, bool scissor);
uint64_t        xxCreatePipelineGlide(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
void            xxDestroyBlendStateGlide(uint64_t blendState);
void            xxDestroyDepthStencilStateGlide(uint64_t depthStencilState);
void            xxDestroyRasterizerStateGlide(uint64_t rasterizerState);
void            xxDestroyPipelineGlide(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
void            xxSetViewportGlide(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
void            xxSetScissorGlide(uint64_t commandEncoder, int x, int y, int width, int height);
void            xxSetPipelineGlide(uint64_t commandEncoder, uint64_t pipeline);
void            xxSetVertexBuffersGlide(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
void            xxSetVertexTexturesGlide(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetFragmentTexturesGlide(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetVertexSamplersGlide(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetFragmentSamplersGlide(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetVertexConstantBufferGlide(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxSetFragmentConstantBufferGlide(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxDrawGlide(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
void            xxDrawIndexedGlide(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
