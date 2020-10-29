//==============================================================================
// xxGraphic : Direct3D 8.0 Header
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceD3D8();
xxAPI void          xxDestroyInstanceD3D8(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t      xxCreateDeviceD3D8(uint64_t instance);
xxAPI void          xxDestroyDeviceD3D8(uint64_t device);
xxAPI bool          xxResetDeviceD3D8(uint64_t device);
xxAPI bool          xxTestDeviceD3D8(uint64_t device);
xxAPI const char*   xxGetDeviceNameD3D8();
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI uint64_t      xxCreateSwapchainD3D8(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain);
xxAPI void          xxDestroySwapchainD3D8(uint64_t swapchain);
xxAPI void          xxPresentSwapchainD3D8(uint64_t swapchain);
xxAPI uint64_t      xxGetCommandBufferD3D8(uint64_t device, uint64_t swapchain);
xxAPI uint64_t      xxGetFramebufferD3D8(uint64_t device, uint64_t swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI bool          xxBeginCommandBufferD3D8(uint64_t commandBuffer);
xxAPI void          xxEndCommandBufferD3D8(uint64_t commandBuffer);
xxAPI void          xxSubmitCommandBufferD3D8(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
xxAPI uint64_t      xxCreateRenderPassD3D8(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
xxAPI void          xxDestroyRenderPassD3D8(uint64_t renderPass);
xxAPI uint64_t      xxBeginRenderPassD3D8(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
xxAPI void          xxEndRenderPassD3D8(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t      xxCreateVertexAttributeD3D8(uint64_t device, int count, int* attribute);
xxAPI void          xxDestroyVertexAttributeD3D8(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
xxAPI uint64_t      xxCreateConstantBufferD3D8(uint64_t device, int size);
xxAPI uint64_t      xxCreateIndexBufferD3D8(uint64_t device, int size);
xxAPI uint64_t      xxCreateVertexBufferD3D8(uint64_t device, int size, uint64_t vertexAttribute);
xxAPI void          xxDestroyBufferD3D8(uint64_t device, uint64_t buffer);
xxAPI void*         xxMapBufferD3D8(uint64_t device, uint64_t buffer);
xxAPI void          xxUnmapBufferD3D8(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
xxAPI uint64_t      xxCreateTextureD3D8(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, const void* external);
xxAPI void          xxDestroyTextureD3D8(uint64_t texture);
xxAPI void*         xxMapTextureD3D8(uint64_t device, uint64_t texture, int* stride, int level, int array);
xxAPI void          xxUnmapTextureD3D8(uint64_t device, uint64_t texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
xxAPI uint64_t      xxCreateSamplerD3D8(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
xxAPI void          xxDestroySamplerD3D8(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
xxAPI uint64_t      xxCreateVertexShaderD3D8(uint64_t device, const char* shader, uint64_t vertexAttribute);
xxAPI uint64_t      xxCreateFragmentShaderD3D8(uint64_t device, const char* shader);
xxAPI void          xxDestroyShaderD3D8(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
xxAPI uint64_t      xxCreateBlendStateD3D8(uint64_t device, bool blending);
xxAPI uint64_t      xxCreateDepthStencilStateD3D8(uint64_t device, bool depthTest, bool depthWrite);
xxAPI uint64_t      xxCreateRasterizerStateD3D8(uint64_t device, bool cull, bool scissor);
xxAPI uint64_t      xxCreatePipelineD3D8(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
xxAPI void          xxDestroyBlendStateD3D8(uint64_t blendState);
xxAPI void          xxDestroyDepthStencilStateD3D8(uint64_t depthStencilState);
xxAPI void          xxDestroyRasterizerStateD3D8(uint64_t rasterizerState);
xxAPI void          xxDestroyPipelineD3D8(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
xxAPI void          xxSetViewportD3D8(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
xxAPI void          xxSetScissorD3D8(uint64_t commandEncoder, int x, int y, int width, int height);
xxAPI void          xxSetPipelineD3D8(uint64_t commandEncoder, uint64_t pipeline);
xxAPI void          xxSetIndexBufferD3D8(uint64_t commandEncoder, uint64_t buffer);
xxAPI void          xxSetVertexBuffersD3D8(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
xxAPI void          xxSetVertexTexturesD3D8(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetFragmentTexturesD3D8(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetVertexSamplersD3D8(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetFragmentSamplersD3D8(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetVertexConstantBufferD3D8(uint64_t commandEncoder, uint64_t buffer, int size);
xxAPI void          xxSetFragmentConstantBufferD3D8(uint64_t commandEncoder, uint64_t buffer, int size);
xxAPI void          xxDrawIndexedD3D8(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
//  Fixed-Function
//==============================================================================
xxAPI void          xxSetTransformD3D8(uint64_t commandEncoder, const float* world, const float* view, const float* projection);
