//==============================================================================
// xxGraphic : Direct3D 8.0 Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceD3D8();
xxInline char const*    xxGetInstanceNameD3D8() { return "Direct3D 8.0 Fixed Function"; }

#ifdef _D3D8_H_

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceD3D8();
void            xxDestroyInstanceD3D8(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
uint64_t        xxCreateDeviceD3D8(uint64_t instance);
void            xxDestroyDeviceD3D8(uint64_t device);
bool            xxResetDeviceD3D8(uint64_t device);
bool            xxTestDeviceD3D8(uint64_t device);
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t        xxCreateSwapchainD3D8(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain);
void            xxDestroySwapchainD3D8(uint64_t swapchain);
void            xxPresentSwapchainD3D8(uint64_t swapchain);
//==============================================================================
//  Framebuffer
//==============================================================================
uint64_t        xxCreateFramebufferD3D8(uint64_t device, uint64_t texture);
void            xxDestroyFramebufferD3D8(uint64_t framebuffer);
uint64_t        xxGetFramebufferD3D8(uint64_t device, uint64_t swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
uint64_t        xxGetCommandBufferD3D8(uint64_t device, uint64_t swapchain);
bool            xxBeginCommandBufferD3D8(uint64_t commandBuffer);
void            xxEndCommandBufferD3D8(uint64_t commandBuffer);
void            xxSubmitCommandBufferD3D8(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t        xxCreateRenderPassD3D8(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
void            xxDestroyRenderPassD3D8(uint64_t renderPass);
uint64_t        xxBeginRenderPassD3D8(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
void            xxEndRenderPassD3D8(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t        xxCreateVertexAttributeD3D8(uint64_t device, int count, int* attribute);
void            xxDestroyVertexAttributeD3D8(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
uint64_t        xxCreateConstantBufferD3D8(uint64_t device, int size);
uint64_t        xxCreateIndexBufferD3D8(uint64_t device, int size, int bits);
uint64_t        xxCreateVertexBufferD3D8(uint64_t device, int size, uint64_t vertexAttribute);
uint64_t        xxCreateStorageBufferD3D8(uint64_t device, int size);
uint64_t        xxCreateInstanceBufferD3D8(uint64_t device, int size);
void            xxDestroyBufferD3D8(uint64_t device, uint64_t buffer);
void*           xxMapBufferD3D8(uint64_t device, uint64_t buffer);
void            xxUnmapBufferD3D8(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
uint64_t        xxCreateTextureD3D8(uint64_t device, uint64_t format, int width, int height, int depth, int mipmap, int array, void const* external);
void            xxDestroyTextureD3D8(uint64_t texture);
void*           xxMapTextureD3D8(uint64_t device, uint64_t texture, int* stride, int level, int array);
void            xxUnmapTextureD3D8(uint64_t device, uint64_t texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
uint64_t        xxCreateSamplerD3D8(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
void            xxDestroySamplerD3D8(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
uint64_t        xxCreateMeshShaderD3D8(uint64_t device, char const* shader);
uint64_t        xxCreateVertexShaderD3D8(uint64_t device, char const* shader, uint64_t vertexAttribute);
uint64_t        xxCreateFragmentShaderD3D8(uint64_t device, char const* shader);
void            xxDestroyShaderD3D8(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t        xxCreateBlendStateD3D8(uint64_t device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha);
uint64_t        xxCreateDepthStencilStateD3D8(uint64_t device, char const* depthTest, bool depthWrite);
uint64_t        xxCreateRasterizerStateD3D8(uint64_t device, bool cull, bool scissor);
uint64_t        xxCreatePipelineD3D8(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t meshShader, uint64_t vertexShader, uint64_t fragmentShader);
void            xxDestroyBlendStateD3D8(uint64_t blendState);
void            xxDestroyDepthStencilStateD3D8(uint64_t depthStencilState);
void            xxDestroyRasterizerStateD3D8(uint64_t rasterizerState);
void            xxDestroyPipelineD3D8(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
void            xxSetViewportD3D8(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
void            xxSetScissorD3D8(uint64_t commandEncoder, int x, int y, int width, int height);
void            xxSetPipelineD3D8(uint64_t commandEncoder, uint64_t pipeline);
void            xxSetMeshBuffersD3D8(uint64_t commandEncoder, int count, const uint64_t* buffers);
void            xxSetVertexBuffersD3D8(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
void            xxSetMeshTexturesD3D8(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetVertexTexturesD3D8(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetFragmentTexturesD3D8(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetMeshSamplersD3D8(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetVertexSamplersD3D8(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetFragmentSamplersD3D8(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetMeshConstantBufferD3D8(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxSetVertexConstantBufferD3D8(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxSetFragmentConstantBufferD3D8(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxDrawD3D8(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
void            xxDrawMeshedD3D8(uint64_t commandEncoder, int x, int y, int z);
void            xxDrawIndexedD3D8(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int vertexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
