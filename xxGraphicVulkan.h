//==============================================================================
// xxGraphic : Vulkan Header
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceVulkan();
xxAPI void          xxDestroyInstanceVulkan(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t      xxCreateDeviceVulkan(uint64_t instance);
xxAPI void          xxDestroyDeviceVulkan(uint64_t device);
xxAPI bool          xxResetDeviceVulkan(uint64_t device);
xxAPI bool          xxTestDeviceVulkan(uint64_t device);
xxAPI const char*   xxGetDeviceNameVulkan();
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI uint64_t      xxCreateSwapchainVulkan(uint64_t device, uint64_t renderPass, void* view, unsigned int width, unsigned int height, uint64_t oldSwapchain);
xxAPI void          xxDestroySwapchainVulkan(uint64_t swapchain);
xxAPI void          xxPresentSwapchainVulkan(uint64_t swapchain);
xxAPI uint64_t      xxGetCommandBufferVulkan(uint64_t device, uint64_t swapchain);
xxAPI uint64_t      xxGetFramebufferVulkan(uint64_t device, uint64_t swapchain);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI bool          xxBeginCommandBufferVulkan(uint64_t commandBuffer);
xxAPI void          xxEndCommandBufferVulkan(uint64_t commandBuffer);
xxAPI void          xxSubmitCommandBufferVulkan(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
xxAPI uint64_t      xxCreateRenderPassVulkan(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
xxAPI void          xxDestroyRenderPassVulkan(uint64_t renderPass);
xxAPI uint64_t      xxBeginRenderPassVulkan(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float r, float g, float b, float a, float depth, unsigned char stencil);
xxAPI void          xxEndRenderPassVulkan(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t      xxCreateVertexAttributeVulkan(uint64_t device, int count, ...);
xxAPI void          xxDestroyVertexAttributeVulkan(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
xxAPI uint64_t      xxCreateConstantBufferVulkan(uint64_t device, unsigned int size);
xxAPI uint64_t      xxCreateIndexBufferVulkan(uint64_t device, unsigned int size);
xxAPI uint64_t      xxCreateVertexBufferVulkan(uint64_t device, unsigned int size, uint64_t vertexAttribute);
xxAPI void          xxDestroyBufferVulkan(uint64_t device, uint64_t buffer);
xxAPI void*         xxMapBufferVulkan(uint64_t device, uint64_t buffer);
xxAPI void          xxUnmapBufferVulkan(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
xxAPI uint64_t      xxCreateTextureVulkan(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array);
xxAPI void          xxDestroyTextureVulkan(uint64_t texture);
xxAPI void*         xxMapTextureVulkan(uint64_t device, uint64_t texture, unsigned int* stride, unsigned int level, unsigned int array, unsigned int mipmap);
xxAPI void          xxUnmapTextureVulkan(uint64_t device, uint64_t texture, unsigned int level, unsigned int array, unsigned int mipmap);
//==============================================================================
//  Sampler
//==============================================================================
xxAPI uint64_t      xxCreateSamplerVulkan(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
xxAPI void          xxDestroySamplerVulkan(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
xxAPI uint64_t      xxCreateVertexShaderVulkan(uint64_t device, const char* shader, uint64_t vertexAttribute);
xxAPI uint64_t      xxCreateFragmentShaderVulkan(uint64_t device, const char* shader);
xxAPI void          xxDestroyShaderVulkan(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
xxAPI uint64_t      xxCreateBlendStateVulkan(uint64_t device, bool blending);
xxAPI uint64_t      xxCreateDepthStencilStateVulkan(uint64_t device, bool depthTest, bool depthWrite);
xxAPI uint64_t      xxCreateRasterizerStateVulkan(uint64_t device, bool cull, bool scissor);
xxAPI uint64_t      xxCreatePipelineVulkan(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
xxAPI void          xxDestroyBlendStateVulkan(uint64_t blendState);
xxAPI void          xxDestroyDepthStencilStateVulkan(uint64_t depthStencilState);
xxAPI void          xxDestroyRasterizerStateVulkan(uint64_t rasterizerState);
xxAPI void          xxDestroyPipelineVulkan(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
xxAPI void          xxSetViewportVulkan(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
xxAPI void          xxSetScissorVulkan(uint64_t commandEncoder, int x, int y, int width, int height);
xxAPI void          xxSetPipelineVulkan(uint64_t commandEncoder, uint64_t pipeline);
xxAPI void          xxSetIndexBufferVulkan(uint64_t commandEncoder, uint64_t buffer);
xxAPI void          xxSetVertexBuffersVulkan(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
xxAPI void          xxSetVertexTexturesVulkan(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetFragmentTexturesVulkan(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetVertexSamplersVulkan(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetFragmentSamplersVulkan(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetVertexConstantBufferVulkan(uint64_t commandEncoder, uint64_t buffer, unsigned int size);
xxAPI void          xxSetFragmentConstantBufferVulkan(uint64_t commandEncoder, uint64_t buffer, unsigned int size);
xxAPI void          xxDrawIndexedVulkan(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
//  Fixed-Function
//==============================================================================
xxAPI void          xxSetTransformVulkan(uint64_t commandEncoder, const float* world, const float* view, const float* projection);
