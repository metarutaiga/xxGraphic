//==============================================================================
// xxGraphic : NULL Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "xxGraphicNULL.h"

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceNULL()
{
    xxRegisterFunction(NULL);

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyInstanceNULL(uint64_t instance)
{
    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceNULL(uint64_t instance)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyDeviceNULL(uint64_t device)
{

}
//------------------------------------------------------------------------------
bool xxResetDeviceNULL(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
bool xxTestDeviceNULL(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameNULL()
{
    return "NULL";
}
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t xxCreateSwapchainNULL(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroySwapchainNULL(uint64_t swapchain)
{

}
//------------------------------------------------------------------------------
void xxPresentSwapchainNULL(uint64_t swapchain)
{

}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferNULL(uint64_t device, uint64_t swapchain)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferNULL(uint64_t device, uint64_t swapchain, float* scale)
{
    return 0;
}
//==============================================================================
//  Command Buffer
//==============================================================================
bool xxBeginCommandBufferNULL(uint64_t commandBuffer)
{
    return true;
}
//------------------------------------------------------------------------------
void xxEndCommandBufferNULL(uint64_t commandBuffer)
{

}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferNULL(uint64_t commandBuffer, uint64_t swapchain)
{
}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxCreateRenderPassNULL(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassNULL(uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassNULL(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxEndRenderPassNULL(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{

}
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t xxCreateVertexAttributeNULL(uint64_t device, int count, int* attribute)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeNULL(uint64_t vertexAttribute)
{

}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateConstantBufferNULL(uint64_t device, int size)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferNULL(uint64_t device, int size)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferNULL(uint64_t device, int size, uint64_t vertexAttribute)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyBufferNULL(uint64_t device, uint64_t buffer)
{

}
//------------------------------------------------------------------------------
void* xxMapBufferNULL(uint64_t device, uint64_t buffer)
{
    return nullptr;
}
//------------------------------------------------------------------------------
void xxUnmapBufferNULL(uint64_t device, uint64_t buffer)
{

}
//==============================================================================
//  Texture
//==============================================================================
uint64_t xxCreateTextureNULL(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, const void* external)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyTextureNULL(uint64_t texture)
{

}
//------------------------------------------------------------------------------
void* xxMapTextureNULL(uint64_t device, uint64_t texture, int* stride, int level, int array)
{
    return nullptr;
}
//------------------------------------------------------------------------------
void xxUnmapTextureNULL(uint64_t device, uint64_t texture, int level, int array)
{

}
//==============================================================================
//  Sampler
//==============================================================================
uint64_t xxCreateSamplerNULL(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroySamplerNULL(uint64_t sampler)
{

}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderNULL(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderNULL(uint64_t device, const char* shader)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderNULL(uint64_t device, uint64_t shader)
{

}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreateBlendStateNULL(uint64_t device, const char* sourceColor, const char* operationColor, const char* destinationColor, const char* sourceAlpha, const char* operationAlpha, const char* destinationAlpha)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateNULL(uint64_t device, bool depthTest, bool depthWrite)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateNULL(uint64_t device, bool cull, bool scissor)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineNULL(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateNULL(uint64_t blendState)
{

}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateNULL(uint64_t depthStencilState)
{

}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateNULL(uint64_t rasterizerState)
{

}
//------------------------------------------------------------------------------
void xxDestroyPipelineNULL(uint64_t pipeline)
{

}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportNULL(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{

}
//------------------------------------------------------------------------------
void xxSetScissorNULL(uint64_t commandEncoder, int x, int y, int width, int height)
{

}
//------------------------------------------------------------------------------
void xxSetPipelineNULL(uint64_t commandEncoder, uint64_t pipeline)
{

}
//------------------------------------------------------------------------------
void xxSetVertexBuffersNULL(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{

}
//------------------------------------------------------------------------------
void xxSetVertexTexturesNULL(uint64_t commandEncoder, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesNULL(uint64_t commandEncoder, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetVertexSamplersNULL(uint64_t commandEncoder, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersNULL(uint64_t commandEncoder, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferNULL(uint64_t commandEncoder, uint64_t buffer, int size)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferNULL(uint64_t commandEncoder, uint64_t buffer, int size)
{

}
//------------------------------------------------------------------------------
void xxDrawIndexedNULL(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{

}
//==============================================================================
