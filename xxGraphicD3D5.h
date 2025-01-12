//==============================================================================
// xxGraphic : Direct3D 5.0 Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceD3D5();
xxInline char const*    xxGetInstanceNameD3D5() { return "Direct3D 5.0"; }

#ifdef _D3D_H_

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceD3D5();
void            xxDestroyInstanceD3D5(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
uint64_t        xxCreateDeviceD3D5(uint64_t instance);
void            xxDestroyDeviceD3D5(uint64_t device);
bool            xxResetDeviceD3D5(uint64_t device);
bool            xxTestDeviceD3D5(uint64_t device);
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t        xxCreateSwapchainD3D5(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain);
void            xxDestroySwapchainD3D5(uint64_t swapchain);
void            xxPresentSwapchainD3D5(uint64_t swapchain);
//==============================================================================
//  Framebuffer
//==============================================================================
uint64_t        xxCreateFramebufferD3D5(uint64_t device, uint64_t texture);
void            xxDestroyFramebufferD3D5(uint64_t framebuffer);
uint64_t        xxGetFramebufferD3D5(uint64_t device, uint64_t swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
uint64_t        xxGetCommandBufferD3D5(uint64_t device, uint64_t swapchain);
bool            xxBeginCommandBufferD3D5(uint64_t commandBuffer);
void            xxEndCommandBufferD3D5(uint64_t commandBuffer);
void            xxSubmitCommandBufferD3D5(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t        xxCreateRenderPassD3D5(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
void            xxDestroyRenderPassD3D5(uint64_t renderPass);
uint64_t        xxBeginRenderPassD3D5(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
void            xxEndRenderPassD3D5(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t        xxCreateVertexAttributeD3D5(uint64_t device, int count, int* attribute);
void            xxDestroyVertexAttributeD3D5(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
uint64_t        xxCreateConstantBufferD3D5(uint64_t device, int size);
uint64_t        xxCreateIndexBufferD3D5(uint64_t device, int size, int bits);
uint64_t        xxCreateVertexBufferD3D5(uint64_t device, int size, uint64_t vertexAttribute);
uint64_t        xxCreateStorageBufferD3D5(uint64_t device, int size);
uint64_t        xxCreateInstanceBufferD3D5(uint64_t device, int size);
void            xxDestroyBufferD3D5(uint64_t device, uint64_t buffer);
void*           xxMapBufferD3D5(uint64_t device, uint64_t buffer);
void            xxUnmapBufferD3D5(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
uint64_t        xxCreateTextureD3D5(uint64_t device, uint64_t format, int width, int height, int depth, int mipmap, int array, void const* external);
void            xxDestroyTextureD3D5(uint64_t texture);
void*           xxMapTextureD3D5(uint64_t device, uint64_t texture, int* stride, int level, int array);
void            xxUnmapTextureD3D5(uint64_t device, uint64_t texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
uint64_t        xxCreateSamplerD3D5(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
void            xxDestroySamplerD3D5(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
uint64_t        xxCreateMeshShaderD3D5(uint64_t device, char const* shader);
uint64_t        xxCreateVertexShaderD3D5(uint64_t device, char const* shader, uint64_t vertexAttribute);
uint64_t        xxCreateFragmentShaderD3D5(uint64_t device, char const* shader);
void            xxDestroyShaderD3D5(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t        xxCreateBlendStateD3D5(uint64_t device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha);
uint64_t        xxCreateDepthStencilStateD3D5(uint64_t device, char const* depthTest, bool depthWrite);
uint64_t        xxCreateRasterizerStateD3D5(uint64_t device, bool cull, bool scissor);
uint64_t        xxCreatePipelineD3D5(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t meshShader, uint64_t vertexShader, uint64_t fragmentShader);
void            xxDestroyBlendStateD3D5(uint64_t blendState);
void            xxDestroyDepthStencilStateD3D5(uint64_t depthStencilState);
void            xxDestroyRasterizerStateD3D5(uint64_t rasterizerState);
void            xxDestroyPipelineD3D5(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
void            xxSetViewportD3D5(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
void            xxSetScissorD3D5(uint64_t commandEncoder, int x, int y, int width, int height);
void            xxSetPipelineD3D5(uint64_t commandEncoder, uint64_t pipeline);
void            xxSetMeshBuffersD3D5(uint64_t commandEncoder, int count, const uint64_t* buffers);
void            xxSetVertexBuffersD3D5(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
void            xxSetMeshTexturesD3D5(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetVertexTexturesD3D5(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetFragmentTexturesD3D5(uint64_t commandEncoder, int count, const uint64_t* textures);
void            xxSetMeshSamplersD3D5(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetVertexSamplersD3D5(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetFragmentSamplersD3D5(uint64_t commandEncoder, int count, const uint64_t* samplers);
void            xxSetMeshConstantBufferD3D5(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxSetVertexConstantBufferD3D5(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxSetFragmentConstantBufferD3D5(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxDrawD3D5(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
void            xxDrawMeshedD3D5(uint64_t commandEncoder, int x, int y, int z);
void            xxDrawIndexedD3D5(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int vertexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
