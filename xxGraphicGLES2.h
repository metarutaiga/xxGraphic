//==============================================================================
// xxGraphic : OpenGL ES 2.0 Header
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceGLES2();
xxAPI void          xxDestroyInstanceGLES2(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t      xxCreateDeviceGLES2(uint64_t instance);
xxAPI void          xxDestroyDeviceGLES2(uint64_t device);
xxAPI bool          xxResetDeviceGLES2(uint64_t device);
xxAPI bool          xxTestDeviceGLES2(uint64_t device);
xxAPI const char*   xxGetDeviceNameGLES2();
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI uint64_t      xxCreateSwapchainGLES2(uint64_t device, uint64_t renderPass, void* view, unsigned int width, unsigned int height, uint64_t oldSwapchain);
xxAPI void          xxDestroySwapchainGLES2(uint64_t swapchain);
xxAPI void          xxPresentSwapchainGLES2(uint64_t swapchain);
xxAPI uint64_t      xxGetCommandBufferGLES2(uint64_t device, uint64_t swapchain);
xxAPI uint64_t      xxGetFramebufferGLES2(uint64_t device, uint64_t swapchain);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI bool          xxBeginCommandBufferGLES2(uint64_t commandBuffer);
xxAPI void          xxEndCommandBufferGLES2(uint64_t commandBuffer);
xxAPI void          xxSubmitCommandBufferGLES2(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
xxAPI uint64_t      xxCreateRenderPassGLES2(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
xxAPI void          xxDestroyRenderPassGLES2(uint64_t renderPass);
xxAPI uint64_t      xxBeginRenderPassGLES2(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float r, float g, float b, float a, float depth, unsigned char stencil);
xxAPI void          xxEndRenderPassGLES2(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Buffer
//==============================================================================
xxAPI uint64_t      xxCreateConstantBufferGLES2(uint64_t device, unsigned int size);
xxAPI uint64_t      xxCreateIndexBufferGLES2(uint64_t device, unsigned int size);
xxAPI uint64_t      xxCreateVertexBufferGLES2(uint64_t device, unsigned int size);
xxAPI void          xxDestroyBufferGLES2(uint64_t device, uint64_t buffer);
xxAPI void*         xxMapBufferGLES2(uint64_t device, uint64_t buffer);
xxAPI void          xxUnmapBufferGLES2(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
xxAPI uint64_t      xxCreateTextureGLES2(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array);
xxAPI void          xxDestroyTextureGLES2(uint64_t texture);
xxAPI void*         xxMapTextureGLES2(uint64_t device, uint64_t texture, unsigned int* stride, unsigned int level, unsigned int array, unsigned int mipmap);
xxAPI void          xxUnmapTextureGLES2(uint64_t device, uint64_t texture, unsigned int level, unsigned int array, unsigned int mipmap);
//==============================================================================
//  Sampler
//==============================================================================
xxAPI uint64_t      xxCreateSamplerGLES2(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
xxAPI void          xxDestroySamplerGLES2(uint64_t sampler);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t      xxCreateVertexAttributeGLES2(uint64_t device, int count, ...);
xxAPI void          xxDestroyVertexAttributeGLES2(uint64_t vertexAttribute);
//==============================================================================
//  Shader
//==============================================================================
xxAPI uint64_t      xxCreateVertexShaderGLES2(uint64_t device, const char* shader, uint64_t vertexAttribute);
xxAPI uint64_t      xxCreateFragmentShaderGLES2(uint64_t device, const char* shader);
xxAPI void          xxDestroyShaderGLES2(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
xxAPI uint64_t      xxCreateBlendStateGLES2(uint64_t device, bool blending);
xxAPI uint64_t      xxCreateDepthStencilStateGLES2(uint64_t device, bool depthTest, bool depthWrite);
xxAPI uint64_t      xxCreateRasterizerStateGLES2(uint64_t device, bool cull, bool scissor);
xxAPI uint64_t      xxCreatePipelineGLES2(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
xxAPI void          xxDestroyBlendStateGLES2(uint64_t blendState);
xxAPI void          xxDestroyDepthStencilStateGLES2(uint64_t depthStencilState);
xxAPI void          xxDestroyRasterizerStateGLES2(uint64_t rasterizerState);
xxAPI void          xxDestroyPipelineGLES2(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
xxAPI void          xxSetViewportGLES2(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
xxAPI void          xxSetScissorGLES2(uint64_t commandEncoder, int x, int y, int width, int height);
xxAPI void          xxSetPipelineGLES2(uint64_t commandEncoder, uint64_t pipeline);
xxAPI void          xxSetIndexBufferGLES2(uint64_t commandEncoder, uint64_t buffer);
xxAPI void          xxSetVertexBuffersGLES2(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
xxAPI void          xxSetVertexTexturesGLES2(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetFragmentTexturesGLES2(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetVertexSamplersGLES2(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetFragmentSamplersGLES2(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetVertexConstantBufferGLES2(uint64_t commandEncoder, uint64_t buffer, unsigned int size);
xxAPI void          xxSetFragmentConstantBufferGLES2(uint64_t commandEncoder, uint64_t buffer, unsigned int size);
xxAPI void          xxDrawIndexedGLES2(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
//  Fixed-Function
//==============================================================================
xxAPI void          xxSetTransformGLES2(uint64_t commandEncoder, const float* world, const float* view, const float* projection);
