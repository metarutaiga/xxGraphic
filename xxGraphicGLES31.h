//==============================================================================
// xxGraphic : OpenGL ES 3.1 Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceGLES31();
xxAPI void          xxDestroyInstanceGLES31(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI const char*   xxGetDeviceNameGLES31();
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI void          xxPresentSwapchainGLES31(uint64_t swapchain);
xxAPI uint64_t      xxGetCommandBufferGLES31(uint64_t device, uint64_t swapchain);
//==============================================================================
//  Command
//==============================================================================
xxAPI void          xxSetVertexBuffersGLES31(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
xxAPI void          xxDrawGLES31(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
xxAPI void          xxDrawIndexedGLES31(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
