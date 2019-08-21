#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t   xxCreateInstanceNULL();
xxAPI void       xxDestroyInstanceNULL(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t   xxCreateDeviceNULL(uint64_t instance);
xxAPI void       xxDestroyDeviceNULL(uint64_t device);
xxAPI void       xxResetDeviceNULL(uint64_t device);
xxAPI bool       xxTestDeviceNULL(uint64_t device);
xxAPI const char*xxGetDeviceStringNULL(uint64_t device);
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI uint64_t   xxCreateSwapchainNULL(uint64_t device, void* view, unsigned int width, unsigned int height);
xxAPI void       xxDestroySwapchainNULL(uint64_t swapchain);
xxAPI void       xxPresentSwapchainNULL(uint64_t swapchain, void* view);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI uint64_t   xxGetCommandBufferNULL(uint64_t device, uint64_t swapchain);
xxAPI bool       xxBeginCommandBufferNULL(uint64_t commandBuffer);
xxAPI void       xxEndCommandBufferNULL(uint64_t commandBuffer);
xxAPI void       xxSubmitCommandBufferNULL(uint64_t commandBuffer);
//==============================================================================
//  Render Pass
//==============================================================================
xxAPI uint64_t   xxCreateRenderPassNULL(uint64_t device, float r, float g, float b, float a, float depth, unsigned char stencil);
xxAPI void       xxDestroyRenderPassNULL(uint64_t renderPass);
xxAPI bool       xxBeginRenderPassNULL(uint64_t commandBuffer, uint64_t renderPass);
xxAPI void       xxEndRenderPassNULL(uint64_t commandBuffer, uint64_t renderPass);
//==============================================================================
//  Buffer
//==============================================================================
xxAPI uint64_t   xxCreateConstantBufferNULL(uint64_t device, unsigned int size);
xxAPI uint64_t   xxCreateIndexBufferNULL(uint64_t device, unsigned int size);
xxAPI uint64_t   xxCreateVertexBufferNULL(uint64_t device, unsigned int size);
xxAPI void       xxDestroyBufferNULL(uint64_t buffer);
xxAPI void*      xxMapBufferNULL(uint64_t buffer);
xxAPI void       xxUnmapBufferNULL(uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
xxAPI uint64_t   xxCreateTextureNULL(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array);
xxAPI void       xxDestroyTextureNULL(uint64_t texture);
xxAPI void*      xxMapTextureNULL(uint64_t texture, unsigned int& stride, unsigned int mipmap, unsigned int array);
xxAPI void       xxUnmapTextureNULL(uint64_t texture, unsigned int mipmap, unsigned int array);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t   xxCreateVertexAttributeNULL(uint64_t device, int count, ...);
xxAPI void       xxDestroyVertexAttributeNULL(uint64_t vertexAttribute);
//==============================================================================
//  Shader
//==============================================================================
xxAPI uint64_t   xxCreateVertexShaderNULL(uint64_t device, const char* shader, uint64_t vertexAttribute);
xxAPI uint64_t   xxCreateFragmentShaderNULL(uint64_t device, const char* shader);
xxAPI void       xxDestroyShaderNULL(uint64_t device, uint64_t shader);
//==============================================================================
//  Command
//==============================================================================
xxAPI void       xxSetViewportNULL(uint64_t commandBuffer, int x, int y, int width, int height, float minZ, float maxZ);
xxAPI void       xxSetScissorNULL(uint64_t commandBuffer, int x, int y, int width, int height);
xxAPI void       xxSetIndexBufferNULL(uint64_t commandBuffer, uint64_t buffer);
xxAPI void       xxSetVertexBuffersNULL(uint64_t commandBuffer, int count, const uint64_t* buffers);
xxAPI void       xxSetFragmentBuffersNULL(uint64_t commandBuffer, int count, const uint64_t* buffers);
xxAPI void       xxSetVertexTexturesNULL(uint64_t commandBuffer, int count, const uint64_t* textures);
xxAPI void       xxSetFragmentTexturesNULL(uint64_t commandBuffer, int count, const uint64_t* textures);
xxAPI void       xxSetVertexAttributeNULL(uint64_t commandBuffer, uint64_t vertexAttribute);
xxAPI void       xxSetVertexShaderNULL(uint64_t commandBuffer, uint64_t shader);
xxAPI void       xxSetFragmentShaderNULL(uint64_t commandBuffer, uint64_t shader);
xxAPI void       xxSetVertexConstantBufferNULL(uint64_t commandBuffer, uint64_t buffer, unsigned int size);
xxAPI void       xxSetFragmentConstantBufferNULL(uint64_t commandBuffer, uint64_t buffer, unsigned int size);
xxAPI void       xxDrawIndexedNULL(uint64_t commandBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
//  Fixed-Function
//==============================================================================
xxAPI void       xxSetTransformNULL(uint64_t commandBuffer, const float* world, const float* view, const float* projection);
