//==============================================================================
// xxGraphic : OpenGL ES 3.2 Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceGLES32();
//==============================================================================
//  Device
//==============================================================================
xxAPI const char*   xxGetDeviceNameGLES32();
//==============================================================================
//  Command
//==============================================================================
xxAPI void          xxSetVertexBuffersGLES32(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
xxAPI void          xxDrawGLES32(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
xxAPI void          xxDrawIndexedGLES32(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
