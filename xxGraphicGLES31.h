//==============================================================================
// xxGraphic : OpenGL ES 3.1 Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceGLES31();
xxInline char const*    xxGetInstanceNameGLES31() { return "OpenGL ES 3.1"; }

#ifdef GL_ES_VERSION_3_1

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceGLES31();
void            xxDestroyInstanceGLES31(uint64_t instance);
//==============================================================================
//  Swapchain
//==============================================================================
void            xxPresentSwapchainGLES31(uint64_t swapchain);
uint64_t        xxGetCommandBufferGLES31(uint64_t device, uint64_t swapchain);
//==============================================================================
//  Command
//==============================================================================
void            xxSetVertexBuffersGLES31(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
void            xxDrawGLES31(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
void            xxDrawIndexedGLES31(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
