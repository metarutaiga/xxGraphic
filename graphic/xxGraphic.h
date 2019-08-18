#pragma once

#include "xxSystem.h"

xxGL_API uint64_t xxCreateInstance();
xxGL_API void xxDestroyInstance(uint64_t instance);

xxGL_API uint64_t xxCreateDevice(uint64_t instance);
xxGL_API void xxDestroyDevice(uint64_t device);
xxGL_API void xxResetDevice(uint64_t device);
xxGL_API bool xxTestDevice(uint64_t device);

xxGL_API uint64_t xxCreateSwapchain(uint64_t device, void* view, unsigned int width = 0, unsigned int height = 0);
xxGL_API void xxDestroySwapchain(uint64_t swapchain);
xxGL_API void xxPresentSwapchain(uint64_t swapchain, void* view);

xxGL_API uint64_t xxGetCommandBuffer(uint64_t device, uint64_t swapchain);
xxGL_API bool xxBeginCommandBuffer(uint64_t commandBuffer);
xxGL_API void xxEndCommandBuffer(uint64_t commandBuffer);
xxGL_API void xxSubmitCommandBuffer(uint64_t commandBuffer);

xxGL_API uint64_t xxCreateRenderPass(uint64_t device, float r, float g, float b, float a, float depth, unsigned int stencil);
xxGL_API void xxDestroyRenderPass(uint64_t renderPass);
xxGL_API bool xxBeginRenderPass(uint64_t commandBuffer, uint64_t renderPass);
xxGL_API void xxEndRenderPass(uint64_t commandBuffer, uint64_t renderPass);

xxGL_API uint64_t xxCreateBuffer(uint64_t device, unsigned int size, bool indexBuffer = false);
xxGL_API void xxDestroyBuffer(uint64_t buffer);
xxGL_API void* xxMapBuffer(uint64_t buffer);
xxGL_API void xxUnmapBuffer(uint64_t buffer);

xxGL_API uint64_t xxCreateTexture(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth = 1, unsigned int mipmap = 1, unsigned int array = 1);
xxGL_API void xxDestroyTexture(uint64_t texture);
xxGL_API void* xxMapTexture(uint64_t texture, unsigned int& stride, unsigned int mipmap = 0, unsigned int array = 0);
xxGL_API void xxUnmapTexture(uint64_t texture, unsigned int mipmap = 0, unsigned int array = 0);

xxGL_API void xxSetViewport(uint64_t commandBuffer, int x, int y, int width, int height, float minZ, float maxZ);
xxGL_API void xxSetScissor(uint64_t commandBuffer, int x, int y, int width, int height);

xxGL_API void xxSetIndexBuffer(uint64_t commandBuffer, uint64_t buffer);
xxGL_API void xxSetVertexBuffers(uint64_t commandBuffer, const uint64_t* buffers, const int* offsets, const int* strides, int count);
xxGL_API void xxSetFragmentBuffers(uint64_t commandBuffer, const uint64_t* buffers, const int* offsets, const int* strides, int count);

xxGL_API void xxSetVertexTextures(uint64_t commandBuffer, const uint64_t* textures, int count);
xxGL_API void xxSetFragmentTextures(uint64_t commandBuffer, const uint64_t* textures, int count);

xxGL_API void xxDrawIndexed(uint64_t commandBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);

xxGL_API void xxSetOrthographicTransform(uint64_t commandBuffer, float left, float right, float top, float bottom);
