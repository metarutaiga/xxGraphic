#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceMetal();
xxAPI void          xxDestroyInstanceMetal(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t      xxCreateDeviceMetal(uint64_t instance);
xxAPI void          xxDestroyDeviceMetal(uint64_t device);
xxAPI void          xxResetDeviceMetal(uint64_t device);
xxAPI bool          xxTestDeviceMetal(uint64_t device);
xxAPI const char*   xxGetDeviceStringMetal(uint64_t device);
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI uint64_t      xxCreateSwapchainMetal(uint64_t device, uint64_t renderPass, void* view, unsigned int width, unsigned int height);
xxAPI void          xxDestroySwapchainMetal(uint64_t swapchain);
xxAPI void          xxPresentSwapchainMetal(uint64_t swapchain);
xxAPI uint64_t      xxGetCommandBufferMetal(uint64_t device, uint64_t swapchain);
xxAPI uint64_t      xxGetFramebufferMetal(uint64_t device, uint64_t swapchain);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI bool          xxBeginCommandBufferMetal(uint64_t commandBuffer);
xxAPI void          xxEndCommandBufferMetal(uint64_t commandBuffer);
xxAPI void          xxSubmitCommandBufferMetal(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
xxAPI uint64_t      xxCreateRenderPassMetal(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
xxAPI void          xxDestroyRenderPassMetal(uint64_t renderPass);
xxAPI uint64_t      xxBeginRenderPassMetal(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float r, float g, float b, float a, float depth, unsigned char stencil);
xxAPI void          xxEndRenderPassMetal(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Buffer
//==============================================================================
xxAPI uint64_t      xxCreateConstantBufferMetal(uint64_t device, unsigned int size);
xxAPI uint64_t      xxCreateIndexBufferMetal(uint64_t device, unsigned int size);
xxAPI uint64_t      xxCreateVertexBufferMetal(uint64_t device, unsigned int size);
xxAPI void          xxDestroyBufferMetal(uint64_t device, uint64_t buffer);
xxAPI void*         xxMapBufferMetal(uint64_t device, uint64_t buffer);
xxAPI void          xxUnmapBufferMetal(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
xxAPI uint64_t      xxCreateTextureMetal(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array);
xxAPI void          xxDestroyTextureMetal(uint64_t texture);
xxAPI void*         xxMapTextureMetal(uint64_t device, uint64_t texture, unsigned int& stride, unsigned int level, unsigned int array, unsigned int mipmap);
xxAPI void          xxUnmapTextureMetal(uint64_t device, uint64_t texture, unsigned int level, unsigned int array, unsigned int mipmap);
//==============================================================================
//  Sampler
//==============================================================================
xxAPI uint64_t      xxCreateSamplerMetal(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
xxAPI void          xxDestroySamplerMetal(uint64_t sampler);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t      xxCreateVertexAttributeMetal(uint64_t device, int count, ...);
xxAPI void          xxDestroyVertexAttributeMetal(uint64_t vertexAttribute);
//==============================================================================
//  Shader
//==============================================================================
xxAPI uint64_t      xxCreateVertexShaderMetal(uint64_t device, const char* shader, uint64_t vertexAttribute);
xxAPI uint64_t      xxCreateFragmentShaderMetal(uint64_t device, const char* shader);
xxAPI void          xxDestroyShaderMetal(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
xxAPI uint64_t      xxCreateBlendStateMetal(uint64_t device, bool blending);
xxAPI uint64_t      xxCreateDepthStencilStateMetal(uint64_t device, bool depthTest, bool depthWrite);
xxAPI uint64_t      xxCreateRasterizerStateMetal(uint64_t device, bool cull, bool scissor);
xxAPI uint64_t      xxCreatePipelineMetal(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
xxAPI void          xxDestroyBlendStateMetal(uint64_t blendState);
xxAPI void          xxDestroyDepthStencilStateMetal(uint64_t depthStencilState);
xxAPI void          xxDestroyRasterizerStateMetal(uint64_t rasterizerState);
xxAPI void          xxDestroyPipelineMetal(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
xxAPI void          xxSetViewportMetal(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
xxAPI void          xxSetScissorMetal(uint64_t commandEncoder, int x, int y, int width, int height);
xxAPI void          xxSetPipelineMetal(uint64_t commandEncoder, uint64_t pipeline);
xxAPI void          xxSetIndexBufferMetal(uint64_t commandEncoder, uint64_t buffer);
xxAPI void          xxSetVertexBuffersMetal(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
xxAPI void          xxSetVertexTexturesMetal(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetFragmentTexturesMetal(uint64_t commandEncoder, int count, const uint64_t* textures);
xxAPI void          xxSetVertexSamplersMetal(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetFragmentSamplersMetal(uint64_t commandEncoder, int count, const uint64_t* samplers);
xxAPI void          xxSetVertexConstantBufferMetal(uint64_t commandEncoder, uint64_t buffer, unsigned int size);
xxAPI void          xxSetFragmentConstantBufferMetal(uint64_t commandEncoder, uint64_t buffer, unsigned int size);
xxAPI void          xxDrawIndexedMetal(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
//  Fixed-Function
//==============================================================================
xxAPI void          xxSetTransformMetal(uint64_t commandEncoder, const float* world, const float* view, const float* projection);