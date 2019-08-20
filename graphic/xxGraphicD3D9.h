#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxGL_API uint64_t   xxCreateInstanceD3D9();
xxGL_API void       xxDestroyInstanceD3D9(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxGL_API uint64_t   xxCreateDeviceD3D9(uint64_t instance);
xxGL_API void       xxDestroyDeviceD3D9(uint64_t device);
xxGL_API void       xxResetDeviceD3D9(uint64_t device);
xxGL_API bool       xxTestDeviceD3D9(uint64_t device);
//==============================================================================
//  Swapchain
//==============================================================================
xxGL_API uint64_t   xxCreateSwapchainD3D9(uint64_t device, void* view, unsigned int width = 0, unsigned int height = 0);
xxGL_API void       xxDestroySwapchainD3D9(uint64_t swapchain);
xxGL_API void       xxPresentSwapchainD3D9(uint64_t swapchain, void* view);
//==============================================================================
//  Command Buffer
//==============================================================================
xxGL_API uint64_t   xxGetCommandBufferD3D9(uint64_t device, uint64_t swapchain);
xxGL_API bool       xxBeginCommandBufferD3D9(uint64_t commandBuffer);
xxGL_API void       xxEndCommandBufferD3D9(uint64_t commandBuffer);
xxGL_API void       xxSubmitCommandBufferD3D9(uint64_t commandBuffer);
//==============================================================================
//  Render Pass
//==============================================================================
xxGL_API uint64_t   xxCreateRenderPassD3D9(uint64_t device, float r, float g, float b, float a, float depth, unsigned char stencil);
xxGL_API void       xxDestroyRenderPassD3D9(uint64_t renderPass);
xxGL_API bool       xxBeginRenderPassD3D9(uint64_t commandBuffer, uint64_t renderPass);
xxGL_API void       xxEndRenderPassD3D9(uint64_t commandBuffer, uint64_t renderPass);
//==============================================================================
//  Buffer
//==============================================================================
xxGL_API uint64_t   xxCreateBufferD3D9(uint64_t device, unsigned int size, bool indexBuffer = false);
xxGL_API void       xxDestroyBufferD3D9(uint64_t buffer);
xxGL_API void*      xxMapBufferD3D9(uint64_t buffer);
xxGL_API void       xxUnmapBufferD3D9(uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
xxGL_API uint64_t   xxCreateTextureD3D9(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth = 1, unsigned int mipmap = 1, unsigned int array = 1);
xxGL_API void       xxDestroyTextureD3D9(uint64_t texture);
xxGL_API void*      xxMapTextureD3D9(uint64_t texture, unsigned int& stride, unsigned int mipmap = 0, unsigned int array = 0);
xxGL_API void       xxUnmapTextureD3D9(uint64_t texture, unsigned int mipmap = 0, unsigned int array = 0);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxGL_API uint64_t   xxCreateVertexAttributeD3D9(uint64_t device, int count, ...);
xxGL_API void       xxDestroyVertexAttributeD3D9(uint64_t vertexAttribute);
//==============================================================================
//  Command
//==============================================================================
xxGL_API void       xxSetViewportD3D9(uint64_t commandBuffer, int x, int y, int width, int height, float minZ, float maxZ);
xxGL_API void       xxSetScissorD3D9(uint64_t commandBuffer, int x, int y, int width, int height);
xxGL_API void       xxSetIndexBufferD3D9(uint64_t commandBuffer, uint64_t buffer);
xxGL_API void       xxSetVertexBuffersD3D9(uint64_t commandBuffer, const uint64_t* buffers, const int* offsets, const int* strides, int count);
xxGL_API void       xxSetFragmentBuffersD3D9(uint64_t commandBuffer, const uint64_t* buffers, const int* offsets, const int* strides, int count);
xxGL_API void       xxSetVertexTexturesD3D9(uint64_t commandBuffer, const uint64_t* textures, int count);
xxGL_API void       xxSetFragmentTexturesD3D9(uint64_t commandBuffer, const uint64_t* textures, int count);
xxGL_API void       xxSetVertexAttributeD3D9(uint64_t commandBuffer, uint64_t vertexAttribute);
xxGL_API void       xxDrawIndexedD3D9(uint64_t commandBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
//  Fixed-Function
//==============================================================================
xxGL_API void       xxSetOrthographicTransformD3D9(uint64_t commandBuffer, float left, float right, float top, float bottom);
