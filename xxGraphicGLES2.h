//==============================================================================
// xxGraphic : OpenGL ES 2.0 Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceGLES2();
xxInline char const*    xxGetInstanceNameGLES2() { return "OpenGL ES 2.0"; }

#ifdef GL_ES_VERSION_2_0

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceGLES2();
void            xxDestroyInstanceGLES2(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
uint64_t        xxCreateDeviceGLES2(uint64_t instance);
void            xxDestroyDeviceGLES2(uint64_t device);
bool            xxResetDeviceGLES2(uint64_t device);
bool            xxTestDeviceGLES2(uint64_t device);
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t        xxCreateSwapchainGLES2(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain);
void            xxDestroySwapchainGLES2(uint64_t swapchain);
void            xxPresentSwapchainGLES2(uint64_t swapchain);
uint64_t        xxGetCommandBufferGLES2(uint64_t device, uint64_t swapchain);
uint64_t        xxGetFramebufferGLES2(uint64_t device, uint64_t swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
bool            xxBeginCommandBufferGLES2(uint64_t commandBuffer);
void            xxEndCommandBufferGLES2(uint64_t commandBuffer);
void            xxSubmitCommandBufferGLES2(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t        xxCreateRenderPassGLES2(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
void            xxDestroyRenderPassGLES2(uint64_t renderPass);
uint64_t        xxBeginRenderPassGLES2(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
void            xxEndRenderPassGLES2(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t        xxCreateVertexAttributeGLES2(uint64_t device, int count, int* attribute);
void            xxDestroyVertexAttributeGLES2(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
uint64_t        xxCreateConstantBufferGLES2(uint64_t device, int size);
uint64_t        xxCreateIndexBufferGLES2(uint64_t device, int size);
uint64_t        xxCreateVertexBufferGLES2(uint64_t device, int size, uint64_t vertexAttribute);
void            xxDestroyBufferGLES2(uint64_t device, uint64_t buffer);
void*           xxMapBufferGLES2(uint64_t device, uint64_t buffer);
void            xxUnmapBufferGLES2(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
uint64_t        xxCreateTextureGLES2(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, void const* external);
void            xxDestroyTextureGLES2(uint64_t texture);
void*           xxMapTextureGLES2(uint64_t device, uint64_t texture, int* stride, int level, int array);
void            xxUnmapTextureGLES2(uint64_t device, uint64_t texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
uint64_t        xxCreateSamplerGLES2(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
void            xxDestroySamplerGLES2(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
uint64_t        xxCreateVertexShaderGLES2(uint64_t device, char const* shader, uint64_t vertexAttribute);
uint64_t        xxCreateFragmentShaderGLES2(uint64_t device, char const* shader);
void            xxDestroyShaderGLES2(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t        xxCreateBlendStateGLES2(uint64_t device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha);
uint64_t        xxCreateDepthStencilStateGLES2(uint64_t device, char const* depthTest, bool depthWrite);
uint64_t        xxCreateRasterizerStateGLES2(uint64_t device, bool cull, bool scissor);
uint64_t        xxCreatePipelineGLES2(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
void            xxDestroyBlendStateGLES2(uint64_t blendState);
void            xxDestroyDepthStencilStateGLES2(uint64_t depthStencilState);
void            xxDestroyRasterizerStateGLES2(uint64_t rasterizerState);
void            xxDestroyPipelineGLES2(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
void            xxSetViewportGLES2(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
void            xxSetScissorGLES2(uint64_t commandEncoder, int x, int y, int width, int height);
void            xxSetPipelineGLES2(uint64_t commandEncoder, uint64_t pipeline);
void            xxSetVertexBuffersGLES2(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
void            xxSetVertexTexturesGLES2(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetFragmentTexturesGLES2(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetVertexSamplersGLES2(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetFragmentSamplersGLES2(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetVertexConstantBufferGLES2(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxSetFragmentConstantBufferGLES2(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxDrawGLES2(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
void            xxDrawIndexedGLES2(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
