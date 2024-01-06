//==============================================================================
// xxGraphic : Graphic Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      (*xxCreateInstance)();
xxAPI void          (*xxDestroyInstance)(uint64_t instance);
xxAPI char const*   (*xxGetInstanceName)();
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t      (*xxCreateDevice)(uint64_t instance);
xxAPI void          (*xxDestroyDevice)(uint64_t device);
xxAPI bool          (*xxResetDevice)(uint64_t device);
xxAPI bool          (*xxTestDevice)(uint64_t device);
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI uint64_t      (*xxCreateSwapchain)(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain);
xxAPI void          (*xxDestroySwapchain)(uint64_t swapchain);
xxAPI void          (*xxPresentSwapchain)(uint64_t swapchain);
xxAPI uint64_t      (*xxGetCommandBuffer)(uint64_t device, uint64_t swapchain);
xxAPI uint64_t      (*xxGetFramebuffer)(uint64_t device, uint64_t swapchain, float* scale);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI bool          (*xxBeginCommandBuffer)(uint64_t commandBuffer);
xxAPI void          (*xxEndCommandBuffer)(uint64_t commandBuffer);
xxAPI void          (*xxSubmitCommandBuffer)(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
xxAPI uint64_t      (*xxCreateRenderPass)(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
xxAPI void          (*xxDestroyRenderPass)(uint64_t renderPass);
xxAPI uint64_t      (*xxBeginRenderPass)(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
xxAPI void          (*xxEndRenderPass)(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t      (*xxCreateVertexAttribute)(uint64_t device, int count, int* attribute);
xxAPI void          (*xxDestroyVertexAttribute)(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
xxAPI uint64_t      (*xxCreateConstantBuffer)(uint64_t device, int size);
xxAPI uint64_t      (*xxCreateIndexBuffer)(uint64_t device, int size);
xxAPI uint64_t      (*xxCreateVertexBuffer)(uint64_t device, int size, uint64_t vertexAttribute);
xxAPI void          (*xxDestroyBuffer)(uint64_t device, uint64_t buffer);
xxAPI void*         (*xxMapBuffer)(uint64_t device, uint64_t buffer);
xxAPI void          (*xxUnmapBuffer)(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
xxAPI uint64_t      (*xxCreateTexture)(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, void const* external);
xxAPI void          (*xxDestroyTexture)(uint64_t texture);
xxAPI void*         (*xxMapTexture)(uint64_t device, uint64_t texture, int* stride, int level, int array);
xxAPI void          (*xxUnmapTexture)(uint64_t device, uint64_t texture, int level, int array);
//==============================================================================
//  Sampler
//==============================================================================
xxAPI uint64_t      (*xxCreateSampler)(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
xxAPI void          (*xxDestroySampler)(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
xxAPI uint64_t      (*xxCreateVertexShader)(uint64_t device, char const* shader, uint64_t vertexAttribute);
xxAPI uint64_t      (*xxCreateFragmentShader)(uint64_t device, char const* shader);
xxAPI void          (*xxDestroyShader)(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
xxAPI uint64_t      (*xxCreateBlendState)(uint64_t device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha);
xxAPI uint64_t      (*xxCreateDepthStencilState)(uint64_t device, char const* depthTest, bool depthWrite);
xxAPI uint64_t      (*xxCreateRasterizerState)(uint64_t device, bool cull, bool scissor);
xxAPI uint64_t      (*xxCreatePipeline)(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
xxAPI void          (*xxDestroyBlendState)(uint64_t blendState);
xxAPI void          (*xxDestroyDepthStencilState)(uint64_t depthStencilState);
xxAPI void          (*xxDestroyRasterizerState)(uint64_t rasterizerState);
xxAPI void          (*xxDestroyPipeline)(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
xxAPI void          (*xxSetViewport)(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
xxAPI void          (*xxSetScissor)(uint64_t commandEncoder, int x, int y, int width, int height);
xxAPI void          (*xxSetPipeline)(uint64_t commandEncoder, uint64_t pipeline);
xxAPI void          (*xxSetVertexBuffers)(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
xxAPI void          (*xxSetVertexTextures)(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          (*xxSetFragmentTextures)(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          (*xxSetVertexSamplers)(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          (*xxSetFragmentSamplers)(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          (*xxSetVertexConstantBuffer)(uint64_t commandEncoder, uint64_t buffer, int size);
xxAPI void          (*xxSetFragmentConstantBuffer)(uint64_t commandEncoder, uint64_t buffer, int size);
xxAPI void          (*xxDraw)(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
xxAPI void          (*xxDrawIndexed)(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
