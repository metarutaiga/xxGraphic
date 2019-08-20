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
void        (*xxResetDevice)(uint64_t device);
bool        (*xxTestDevice)(uint64_t device);
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t    (*xxCreateSwapchain)(uint64_t device, void* view, unsigned int width, unsigned int height);
void        (*xxDestroySwapchain)(uint64_t swapchain);
void        (*xxPresentSwapchain)(uint64_t swapchain, void* view);
//==============================================================================
//  Command Buffer
//==============================================================================
uint64_t    (*xxGetCommandBuffer)(uint64_t device, uint64_t swapchain);
bool        (*xxBeginCommandBuffer)(uint64_t commandBuffer);
void        (*xxEndCommandBuffer)(uint64_t commandBuffer);
void        (*xxSubmitCommandBuffer)(uint64_t commandBuffer);
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t    (*xxCreateRenderPass)(uint64_t device, float r, float g, float b, float a, float depth, unsigned char stencil);
void        (*xxDestroyRenderPass)(uint64_t renderPass);
bool        (*xxBeginRenderPass)(uint64_t commandBuffer, uint64_t renderPass);
void        (*xxEndRenderPass)(uint64_t commandBuffer, uint64_t renderPass);
//==============================================================================
//  Buffer
//==============================================================================
uint64_t    (*xxCreateBuffer)(uint64_t device, unsigned int size, bool indexBuffer);
void        (*xxDestroyBuffer)(uint64_t buffer);
void*       (*xxMapBuffer)(uint64_t buffer);
void        (*xxUnmapBuffer)(uint64_t buffer);
//==============================================================================
//  Texture
//==============================================================================
uint64_t    (*xxCreateTexture)(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array);
void        (*xxDestroyTexture)(uint64_t texture);
void*       (*xxMapTexture)(uint64_t texture, unsigned int& stride, unsigned int mipmap, unsigned int array);
void        (*xxUnmapTexture)(uint64_t texture, unsigned int mipmap, unsigned int array);
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t    (*xxCreateVertexAttribute)(uint64_t device, int count, ...);
void        (*xxDestroyVertexAttribute)(uint64_t vertexAttribute);
//==============================================================================
//  Command
//==============================================================================
void        (*xxSetViewport)(uint64_t commandBuffer, int x, int y, int width, int height, float minZ, float maxZ);
void        (*xxSetScissor)(uint64_t commandBuffer, int x, int y, int width, int height);
void        (*xxSetIndexBuffer)(uint64_t commandBuffer, uint64_t buffer);
void        (*xxSetVertexBuffers)(uint64_t commandBuffer, const uint64_t* buffers, const int* offsets, const int* strides, int count);
void        (*xxSetFragmentBuffers)(uint64_t commandBuffer, const uint64_t* buffers, const int* offsets, const int* strides, int count);
void        (*xxSetVertexTextures)(uint64_t commandBuffer, const uint64_t* textures, int count);
void        (*xxSetFragmentTextures)(uint64_t commandBuffer, const uint64_t* textures, int count);
void        (*xxSetVertexAttribute)(uint64_t commandBuffer, uint64_t vertexAttribute);
void        (*xxDrawIndexed)(uint64_t commandBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================
//  Fixed-Function
//==============================================================================
void        (*xxSetOrthographicTransform)(uint64_t commandBuffer, float left, float right, float top, float bottom);
