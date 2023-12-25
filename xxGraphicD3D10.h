//==============================================================================
// xxGraphic : Direct3D 10.0 Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceD3D10();
xxInline char const*    xxGetInstanceNameD3D10() { return "Direct3D 10.0"; }

#ifdef __d3d10_h__

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceD3D10();
void            xxDestroyInstanceD3D10(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
uint64_t        xxCreateDeviceD3D10(uint64_t instance);
void            xxDestroyDeviceD3D10(uint64_t device);
bool            xxResetDeviceD3D10(uint64_t device);
bool            xxTestDeviceD3D10(uint64_t device);
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t        xxCreateSwapchainD3D10(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain);
void            xxDestroySwapchainD3D10(uint64_t swapchain);
void            xxPresentSwapchainD3D10(uint64_t swapchain);
uint64_t        xxGetCommandBufferD3D10(uint64_t device, uint64_t swapchain);
uint64_t        xxGetFramebufferD3D10(uint64_t device, uint64_t swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
bool            xxBeginCommandBufferD3D10(uint64_t commandBuffer);
void            xxEndCommandBufferD3D10(uint64_t commandBuffer);
void            xxSubmitCommandBufferD3D10(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t        xxCreateRenderPassD3D10(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
void            xxDestroyRenderPassD3D10(uint64_t renderPass);
uint64_t        xxBeginRenderPassD3D10(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
void            xxEndRenderPassD3D10(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t        xxCreateVertexAttributeD3D10(uint64_t device, int count, int* attribute);
void            xxDestroyVertexAttributeD3D10(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
uint64_t        xxCreateConstantBufferD3D10(uint64_t device, int size);
uint64_t        xxCreateIndexBufferD3D10(uint64_t device, int size);
uint64_t        xxCreateVertexBufferD3D10(uint64_t device, int size, uint64_t vertexAttribute);
void            xxDestroyBufferD3D10(uint64_t device, uint64_t buffer);
void*           xxMapBufferD3D10(uint64_t device, uint64_t buffer);
void            xxUnmapBufferD3D10(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
uint64_t        xxCreateTextureD3D10(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, void const* external);
void            xxDestroyTextureD3D10(uint64_t texture);
void*           xxMapTextureD3D10(uint64_t device, uint64_t texture, int* stride, int level, int array);
void            xxUnmapTextureD3D10(uint64_t device, uint64_t texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
uint64_t        xxCreateSamplerD3D10(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
void            xxDestroySamplerD3D10(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
uint64_t        xxCreateVertexShaderD3D10(uint64_t device, char const* shader, uint64_t vertexAttribute);
uint64_t        xxCreateFragmentShaderD3D10(uint64_t device, char const* shader);
void            xxDestroyShaderD3D10(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t        xxCreateBlendStateD3D10(uint64_t device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha);
uint64_t        xxCreateDepthStencilStateD3D10(uint64_t device, char const* depthTest, bool depthWrite);
uint64_t        xxCreateRasterizerStateD3D10(uint64_t device, bool cull, bool scissor);
uint64_t        xxCreatePipelineD3D10(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
void            xxDestroyBlendStateD3D10(uint64_t blendState);
void            xxDestroyDepthStencilStateD3D10(uint64_t depthStencilState);
void            xxDestroyRasterizerStateD3D10(uint64_t rasterizerState);
void            xxDestroyPipelineD3D10(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
void            xxSetViewportD3D10(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
void            xxSetScissorD3D10(uint64_t commandEncoder, int x, int y, int width, int height);
void            xxSetPipelineD3D10(uint64_t commandEncoder, uint64_t pipeline);
void            xxSetVertexBuffersD3D10(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
void            xxSetVertexTexturesD3D10(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetFragmentTexturesD3D10(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetVertexSamplersD3D10(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetFragmentSamplersD3D10(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetVertexConstantBufferD3D10(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxSetFragmentConstantBufferD3D10(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxDrawD3D10(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
void            xxDrawIndexedD3D10(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
