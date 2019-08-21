#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t   xxCreateInstanceD3D9();
xxAPI void       xxDestroyInstanceD3D9(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t   xxCreateDeviceD3D9(uint64_t instance);
xxAPI void       xxDestroyDeviceD3D9(uint64_t device);
xxAPI void       xxResetDeviceD3D9(uint64_t device);
xxAPI bool       xxTestDeviceD3D9(uint64_t device);
xxAPI const char*xxGetDeviceStringD3D9(uint64_t device);
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI uint64_t   xxCreateSwapchainD3D9(uint64_t device, void* view, unsigned int width, unsigned int height);
xxAPI void       xxDestroySwapchainD3D9(uint64_t swapchain);
xxAPI void       xxPresentSwapchainD3D9(uint64_t swapchain, void* view);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI uint64_t   xxGetCommandBufferD3D9(uint64_t device, uint64_t swapchain);
xxAPI bool       xxBeginCommandBufferD3D9(uint64_t commandBuffer);
xxAPI void       xxEndCommandBufferD3D9(uint64_t commandBuffer);
xxAPI void       xxSubmitCommandBufferD3D9(uint64_t commandBuffer);
//==============================================================================
//  Render Pass
//==============================================================================
xxAPI uint64_t   xxCreateRenderPassD3D9(uint64_t device, float r, float g, float b, float a, float depth, unsigned char stencil);
xxAPI void       xxDestroyRenderPassD3D9(uint64_t renderPass);
xxAPI bool       xxBeginRenderPassD3D9(uint64_t commandBuffer, uint64_t renderPass);
xxAPI void       xxEndRenderPassD3D9(uint64_t commandBuffer, uint64_t renderPass);
//==============================================================================
//  Buffer
//==============================================================================
xxAPI uint64_t   xxCreateConstantBufferD3D9(uint64_t device, unsigned int size);
xxAPI uint64_t   xxCreateIndexBufferD3D9(uint64_t device, unsigned int size);
xxAPI uint64_t   xxCreateVertexBufferD3D9(uint64_t device, unsigned int size);
xxAPI void       xxDestroyBufferD3D9(uint64_t buffer);
xxAPI void*      xxMapBufferD3D9(uint64_t buffer);
xxAPI void       xxUnmapBufferD3D9(uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
xxAPI uint64_t   xxCreateTextureD3D9(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth = 1, unsigned int mipmap = 1, unsigned int array = 1);
xxAPI void       xxDestroyTextureD3D9(uint64_t texture);
xxAPI void*      xxMapTextureD3D9(uint64_t texture, unsigned int& stride, unsigned int mipmap, unsigned int array);
xxAPI void       xxUnmapTextureD3D9(uint64_t texture, unsigned int mipmap, unsigned int array);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t   xxCreateVertexAttributeD3D9(uint64_t device, int count, ...);
xxAPI void       xxDestroyVertexAttributeD3D9(uint64_t vertexAttribute);
//==============================================================================
//  Shader
//==============================================================================
xxAPI uint64_t   xxCreateVertexShaderD3D9(uint64_t device, const char* shader, uint64_t vertexAttribute);
xxAPI uint64_t   xxCreateFragmentShaderD3D9(uint64_t device, const char* shader);
xxAPI void       xxDestroyShaderD3D9(uint64_t device, uint64_t shader);
//==============================================================================
//  Command
//==============================================================================
xxAPI void       xxSetViewportD3D9(uint64_t commandBuffer, int x, int y, int width, int height, float minZ, float maxZ);
xxAPI void       xxSetScissorD3D9(uint64_t commandBuffer, int x, int y, int width, int height);
xxAPI void       xxSetIndexBufferD3D9(uint64_t commandBuffer, uint64_t buffer);
xxAPI void       xxSetVertexBuffersD3D9(uint64_t commandBuffer, int count, const uint64_t* buffers);
xxAPI void       xxSetFragmentBuffersD3D9(uint64_t commandBuffer, int count, const uint64_t* buffers);
xxAPI void       xxSetVertexTexturesD3D9(uint64_t commandBuffer, int count, const uint64_t* textures);
xxAPI void       xxSetFragmentTexturesD3D9(uint64_t commandBuffer, int count, const uint64_t* textures);
xxAPI void       xxSetVertexAttributeD3D9(uint64_t commandBuffer, uint64_t vertexAttribute);
xxAPI void       xxSetVertexShaderD3D9(uint64_t commandBuffer, uint64_t shader);
xxAPI void       xxSetFragmentShaderD3D9(uint64_t commandBuffer, uint64_t shader);
xxAPI void       xxSetVertexConstantBufferD3D9(uint64_t commandBuffer, uint64_t buffer, unsigned int size);
xxAPI void       xxSetFragmentConstantBufferD3D9(uint64_t commandBuffer, uint64_t buffer, unsigned int size);
xxAPI void       xxDrawIndexedD3D9(uint64_t commandBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
//  Fixed-Function
//==============================================================================
xxAPI void       xxSetTransformD3D9(uint64_t commandBuffer, const float* world, const float* view, const float* projection);
