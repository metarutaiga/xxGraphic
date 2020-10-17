//==============================================================================
// xxGraphic : Graphic Source
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
uint64_t    (*xxCreateInstance)();
void        (*xxDestroyInstance)(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
uint64_t    (*xxCreateDevice)(uint64_t instance);
void        (*xxDestroyDevice)(uint64_t device);
bool        (*xxResetDevice)(uint64_t device);
bool        (*xxTestDevice)(uint64_t device);
const char* (*xxGetDeviceName)();
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t    (*xxCreateSwapchain)(uint64_t device, uint64_t renderPass, void* view, unsigned int width, unsigned int height, uint64_t oldSwapchain);
void        (*xxDestroySwapchain)(uint64_t swapchain);
void        (*xxPresentSwapchain)(uint64_t swapchain);
uint64_t    (*xxGetCommandBuffer)(uint64_t device, uint64_t swapchain);
uint64_t    (*xxGetFramebuffer)(uint64_t device, uint64_t swapchain);
//==============================================================================
//  Command Buffer
//==============================================================================
bool        (*xxBeginCommandBuffer)(uint64_t commandBuffer);
void        (*xxEndCommandBuffer)(uint64_t commandBuffer);
void        (*xxSubmitCommandBuffer)(uint64_t commandBuffer, uint64_t swapchain);
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t    (*xxCreateRenderPass)(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil);
void        (*xxDestroyRenderPass)(uint64_t renderPass);
uint64_t    (*xxBeginRenderPass)(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil);
void        (*xxEndRenderPass)(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass);
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t    (*xxCreateVertexAttribute)(uint64_t device, int count, int* attribute);
void        (*xxDestroyVertexAttribute)(uint64_t vertexAttribute);
//==============================================================================
//  Buffer
//==============================================================================
uint64_t    (*xxCreateConstantBuffer)(uint64_t device, unsigned int size);
uint64_t    (*xxCreateIndexBuffer)(uint64_t device, unsigned int size);
uint64_t    (*xxCreateVertexBuffer)(uint64_t device, unsigned int size, uint64_t vertexAttribute);
void        (*xxDestroyBuffer)(uint64_t device, uint64_t buffer);
void*       (*xxMapBuffer)(uint64_t device, uint64_t buffer);
void        (*xxUnmapBuffer)(uint64_t device, uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
uint64_t    (*xxCreateTexture)(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array, const void* external);
void        (*xxDestroyTexture)(uint64_t texture);
void*       (*xxMapTexture)(uint64_t device, uint64_t texture, unsigned int* stride, unsigned int level, unsigned int array);
void        (*xxUnmapTexture)(uint64_t device, uint64_t texture, unsigned int level, unsigned int array);
//==============================================================================
//  Sampler
//==============================================================================
uint64_t    (*xxCreateSampler)(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy);
void        (*xxDestroySampler)(uint64_t sampler);
//==============================================================================
//  Shader
//==============================================================================
uint64_t    (*xxCreateVertexShader)(uint64_t device, const char* shader, uint64_t vertexAttribute);
uint64_t    (*xxCreateFragmentShader)(uint64_t device, const char* shader);
void        (*xxDestroyShader)(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t    (*xxCreateBlendState)(uint64_t device, bool blending);
uint64_t    (*xxCreateDepthStencilState)(uint64_t device, bool depthTest, bool depthWrite);
uint64_t    (*xxCreateRasterizerState)(uint64_t device, bool cull, bool scissor);
uint64_t    (*xxCreatePipeline)(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
void        (*xxDestroyBlendState)(uint64_t blendState);
void        (*xxDestroyDepthStencilState)(uint64_t depthStencilState);
void        (*xxDestroyRasterizerState)(uint64_t rasterizerState);
void        (*xxDestroyPipeline)(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
void        (*xxSetViewport)(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ);
void        (*xxSetScissor)(uint64_t commandEncoder, int x, int y, int width, int height);
void        (*xxSetPipeline)(uint64_t commandEncoder, uint64_t pipeline);
void        (*xxSetIndexBuffer)(uint64_t commandEncoder, uint64_t buffer);
void        (*xxSetVertexBuffers)(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
void        (*xxSetVertexTextures)(uint64_t commandEncoder, int count, const uint64_t* textures);
void        (*xxSetFragmentTextures)(uint64_t commandEncoder, int count, const uint64_t* textures);
void        (*xxSetVertexSamplers)(uint64_t commandEncoder, int count, const uint64_t* samplers);
void        (*xxSetFragmentSamplers)(uint64_t commandEncoder, int count, const uint64_t* samplers);
void        (*xxSetVertexConstantBuffer)(uint64_t commandEncoder, uint64_t buffer, unsigned int size);
void        (*xxSetFragmentConstantBuffer)(uint64_t commandEncoder, uint64_t buffer, unsigned int size);
void        (*xxDrawIndexed)(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
//  Fixed-Function
//==============================================================================
void        (*xxSetTransform)(uint64_t commandEncoder, const float* world, const float* view, const float* projection);
