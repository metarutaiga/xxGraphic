//==============================================================================
// xxGraphic : Glide Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceGlide();
xxAPI void          xxDestroyInstanceGlide(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t      xxCreateDeviceGlide(uint64_t instance);
xxAPI void          xxDestroyDeviceGlide(uint64_t device);
xxAPI bool          xxResetDeviceGlide(uint64_t device);
xxAPI bool          xxTestDeviceGlide(uint64_t device);
xxAPI const char*   xxGetDeviceNameGlide();
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI uint64_t      xxCreateSwapchainGlide(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain);
xxAPI void          xxDestroySwapchainGlide(uint64_t swapchain);
xxAPI void          xxPresentSwapchainGlide(uint64_t swapchain);
xxAPI uint64_t      xxGetCommandBufferGlide(uint64_t device, uint64_t swapchain);
xxAPI uint64_t      xxGetFramebufferGlide(uint64_t device, uint64_t swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI bool          xxBeginCommandBufferGlide(uint64_t commandBuffer);
xxAPI void          xxEndCommandBufferGlide(uint64_t commandBuffer);
xxAPI void          xxSubmitCommandBufferGlide(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
xxAPI uint64_t      xxCreateRenderPassGlide(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
xxAPI void          xxDestroyRenderPassGlide(uint64_t renderPass);
xxAPI uint64_t      xxBeginRenderPassGlide(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
xxAPI void          xxEndRenderPassGlide(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t      xxCreateVertexAttributeGlide(uint64_t device, int count, int* attribute);
xxAPI void          xxDestroyVertexAttributeGlide(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
xxAPI uint64_t      xxCreateConstantBufferGlide(uint64_t device, int size);
xxAPI uint64_t      xxCreateIndexBufferGlide(uint64_t device, int size);
xxAPI uint64_t      xxCreateVertexBufferGlide(uint64_t device, int size, uint64_t vertexAttribute);
xxAPI void          xxDestroyBufferGlide(uint64_t device, uint64_t buffer);
xxAPI void*         xxMapBufferGlide(uint64_t device, uint64_t buffer);
xxAPI void          xxUnmapBufferGlide(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
xxAPI uint64_t      xxCreateTextureGlide(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, const void* external);
xxAPI void          xxDestroyTextureGlide(uint64_t texture);
xxAPI void*         xxMapTextureGlide(uint64_t device, uint64_t texture, int* stride, int level, int array);
xxAPI void          xxUnmapTextureGlide(uint64_t device, uint64_t texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
xxAPI uint64_t      xxCreateSamplerGlide(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
xxAPI void          xxDestroySamplerGlide(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
xxAPI uint64_t      xxCreateVertexShaderGlide(uint64_t device, const char* shader, uint64_t vertexAttribute);
xxAPI uint64_t      xxCreateFragmentShaderGlide(uint64_t device, const char* shader);
xxAPI void          xxDestroyShaderGlide(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
xxAPI uint64_t      xxCreateBlendStateGlide(uint64_t device, bool blending);
xxAPI uint64_t      xxCreateDepthStencilStateGlide(uint64_t device, bool depthTest, bool depthWrite);
xxAPI uint64_t      xxCreateRasterizerStateGlide(uint64_t device, bool cull, bool scissor);
xxAPI uint64_t      xxCreatePipelineGlide(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
xxAPI void          xxDestroyBlendStateGlide(uint64_t blendState);
xxAPI void          xxDestroyDepthStencilStateGlide(uint64_t depthStencilState);
xxAPI void          xxDestroyRasterizerStateGlide(uint64_t rasterizerState);
xxAPI void          xxDestroyPipelineGlide(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
xxAPI void          xxSetViewportGlide(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
xxAPI void          xxSetScissorGlide(uint64_t commandEncoder, int x, int y, int width, int height);
xxAPI void          xxSetPipelineGlide(uint64_t commandEncoder, uint64_t pipeline);
xxAPI void          xxSetVertexBuffersGlide(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
xxAPI void          xxSetVertexTexturesGlide(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetFragmentTexturesGlide(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetVertexSamplersGlide(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetFragmentSamplersGlide(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetVertexConstantBufferGlide(uint64_t commandEncoder, uint64_t buffer, int size);
xxAPI void          xxSetFragmentConstantBufferGlide(uint64_t commandEncoder, uint64_t buffer, int size);
xxAPI void          xxDrawIndexedGlide(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
