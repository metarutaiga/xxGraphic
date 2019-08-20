#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t   xxCreateInstanceD3D8();
xxAPI void       xxDestroyInstanceD3D8(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t   xxCreateDeviceD3D8(uint64_t instance);
xxAPI void       xxDestroyDeviceD3D8(uint64_t device);
xxAPI void       xxResetDeviceD3D8(uint64_t device);
xxAPI bool       xxTestDeviceD3D8(uint64_t device);
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI uint64_t   xxCreateSwapchainD3D8(uint64_t device, void* view, unsigned int width, unsigned int height);
xxAPI void       xxDestroySwapchainD3D8(uint64_t swapchain);
xxAPI void       xxPresentSwapchainD3D8(uint64_t swapchain, void* view);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI uint64_t   xxGetCommandBufferD3D8(uint64_t device, uint64_t swapchain);
xxAPI bool       xxBeginCommandBufferD3D8(uint64_t commandBuffer);
xxAPI void       xxEndCommandBufferD3D8(uint64_t commandBuffer);
xxAPI void       xxSubmitCommandBufferD3D8(uint64_t commandBuffer);
//==============================================================================
//  Render Pass
//==============================================================================
xxAPI uint64_t   xxCreateRenderPassD3D8(uint64_t device, float r, float g, float b, float a, float depth, unsigned char stencil);
xxAPI void       xxDestroyRenderPassD3D8(uint64_t renderPass);
xxAPI bool       xxBeginRenderPassD3D8(uint64_t commandBuffer, uint64_t renderPass);
xxAPI void       xxEndRenderPassD3D8(uint64_t commandBuffer, uint64_t renderPass);
//==============================================================================
//  Buffer
//==============================================================================
xxAPI uint64_t   xxCreateBufferD3D8(uint64_t device, unsigned int size, bool indexBuffer = false);
xxAPI void       xxDestroyBufferD3D8(uint64_t buffer);
xxAPI void*      xxMapBufferD3D8(uint64_t buffer);
xxAPI void       xxUnmapBufferD3D8(uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
xxAPI uint64_t   xxCreateTextureD3D8(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array);
xxAPI void       xxDestroyTextureD3D8(uint64_t texture);
xxAPI void*      xxMapTextureD3D8(uint64_t texture, unsigned int& stride, unsigned int mipmap, unsigned int array);
xxAPI void       xxUnmapTextureD3D8(uint64_t texture, unsigned int mipmap, unsigned int array);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t   xxCreateVertexAttributeD3D8(uint64_t device, int count, ...);
xxAPI void       xxDestroyVertexAttributeD3D8(uint64_t vertexAttribute);
//==============================================================================
//  Command
//==============================================================================
xxAPI void       xxSetViewportD3D8(uint64_t commandBuffer, int x, int y, int width, int height, float minZ, float maxZ);
xxAPI void       xxSetScissorD3D8(uint64_t commandBuffer, int x, int y, int width, int height);
xxAPI void       xxSetIndexBufferD3D8(uint64_t commandBuffer, uint64_t buffer);
xxAPI void       xxSetVertexBuffersD3D8(uint64_t commandBuffer, int count, const uint64_t* buffers);
xxAPI void       xxSetFragmentBuffersD3D8(uint64_t commandBuffer, int count, const uint64_t* buffers);
xxAPI void       xxSetVertexTexturesD3D8(uint64_t commandBuffer, int count, const uint64_t* textures);
xxAPI void       xxSetFragmentTexturesD3D8(uint64_t commandBuffer, int count, const uint64_t* textures);
xxAPI void       xxSetVertexAttributeD3D8(uint64_t commandBuffer, uint64_t vertexAttribute);
xxAPI void       xxDrawIndexedD3D8(uint64_t commandBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
//  Fixed-Function
//==============================================================================
xxAPI void       xxSetOrthographicTransformD3D8(uint64_t commandBuffer, float left, float right, float top, float bottom);
//==============================================================================
//  Structure
//==============================================================================
union D3DVERTEXATTRIBUTE8
{
    uint64_t value;
    struct
    {
        DWORD fvf;
        int stride;
    };
};
