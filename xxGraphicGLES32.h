//==============================================================================
// xxGraphic : OpenGL ES 3.2 Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceGLES32();
xxInline char const*    xxGetInstanceNameGLES32() { return "OpenGL ES 3.2"; }

#ifdef GL_ES_VERSION_3_2

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceGLES32();
//==============================================================================
//  Command
//==============================================================================
void            xxSetVertexBuffersGLES32(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
void            xxDrawGLES32(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance);
void            xxDrawIndexedGLES32(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance);
//==============================================================================

#endif
