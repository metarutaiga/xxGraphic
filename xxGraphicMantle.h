//==============================================================================
// xxGraphic : Mantle Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceMantle();
xxAPI void          xxDestroyInstanceMantle(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t      xxCreateDeviceMantle(uint64_t instance);
xxAPI void          xxDestroyDeviceMantle(uint64_t device);
xxAPI bool          xxResetDeviceMantle(uint64_t device);
xxAPI bool          xxTestDeviceMantle(uint64_t device);
xxAPI const char*   xxGetDeviceNameMantle();
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI uint64_t      xxCreateSwapchainMantle(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain);
xxAPI void          xxDestroySwapchainMantle(uint64_t swapchain);
xxAPI void          xxPresentSwapchainMantle(uint64_t swapchain);
xxAPI uint64_t      xxGetCommandBufferMantle(uint64_t device, uint64_t swapchain);
xxAPI uint64_t      xxGetFramebufferMantle(uint64_t device, uint64_t swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI bool          xxBeginCommandBufferMantle(uint64_t commandBuffer);
xxAPI void          xxEndCommandBufferMantle(uint64_t commandBuffer);
xxAPI void          xxSubmitCommandBufferMantle(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
xxAPI uint64_t      xxCreateRenderPassMantle(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
xxAPI void          xxDestroyRenderPassMantle(uint64_t renderPass);
xxAPI uint64_t      xxBeginRenderPassMantle(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
xxAPI void          xxEndRenderPassMantle(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t      xxCreateVertexAttributeMantle(uint64_t device, int count, int* attribute);
xxAPI void          xxDestroyVertexAttributeMantle(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
xxAPI uint64_t      xxCreateConstantBufferMantle(uint64_t device, int size);
xxAPI uint64_t      xxCreateIndexBufferMantle(uint64_t device, int size);
xxAPI uint64_t      xxCreateVertexBufferMantle(uint64_t device, int size, uint64_t vertexAttribute);
xxAPI void          xxDestroyBufferMantle(uint64_t device, uint64_t buffer);
xxAPI void*         xxMapBufferMantle(uint64_t device, uint64_t buffer);
xxAPI void          xxUnmapBufferMantle(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
xxAPI uint64_t      xxCreateTextureMantle(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, const void* external);
xxAPI void          xxDestroyTextureMantle(uint64_t texture);
xxAPI void*         xxMapTextureMantle(uint64_t device, uint64_t texture, int* stride, int level, int array);
xxAPI void          xxUnmapTextureMantle(uint64_t device, uint64_t texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
xxAPI uint64_t      xxCreateSamplerMantle(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
xxAPI void          xxDestroySamplerMantle(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
xxAPI uint64_t      xxCreateVertexShaderMantle(uint64_t device, const char* shader, uint64_t vertexAttribute);
xxAPI uint64_t      xxCreateFragmentShaderMantle(uint64_t device, const char* shader);
xxAPI void          xxDestroyShaderMantle(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
xxAPI uint64_t      xxCreateBlendStateMantle(uint64_t device, bool blending);
xxAPI uint64_t      xxCreateDepthStencilStateMantle(uint64_t device, bool depthTest, bool depthWrite);
xxAPI uint64_t      xxCreateRasterizerStateMantle(uint64_t device, bool cull, bool scissor);
xxAPI uint64_t      xxCreatePipelineMantle(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
xxAPI void          xxDestroyBlendStateMantle(uint64_t blendState);
xxAPI void          xxDestroyDepthStencilStateMantle(uint64_t depthStencilState);
xxAPI void          xxDestroyRasterizerStateMantle(uint64_t rasterizerState);
xxAPI void          xxDestroyPipelineMantle(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
xxAPI void          xxSetViewportMantle(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
xxAPI void          xxSetScissorMantle(uint64_t commandEncoder, int x, int y, int width, int height);
xxAPI void          xxSetPipelineMantle(uint64_t commandEncoder, uint64_t pipeline);
xxAPI void          xxSetIndexBufferMantle(uint64_t commandEncoder, uint64_t buffer);
xxAPI void          xxSetVertexBuffersMantle(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
xxAPI void          xxSetVertexTexturesMantle(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetFragmentTexturesMantle(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetVertexSamplersMantle(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetFragmentSamplersMantle(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetVertexConstantBufferMantle(uint64_t commandEncoder, uint64_t buffer, int size);
xxAPI void          xxSetFragmentConstantBufferMantle(uint64_t commandEncoder, uint64_t buffer, int size);
xxAPI void          xxDrawIndexedMantle(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
//  Fixed-Function
//==============================================================================
xxAPI void          xxSetTransformMantle(uint64_t commandEncoder, const float* world, const float* view, const float* projection);
