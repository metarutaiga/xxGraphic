//==============================================================================
// xxGraphic : OpenGL ES 3.0 Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceGLES3();
xxInline char const*    xxGetInstanceNameGLES3() { return "OpenGL ES 3.0"; }

#ifdef GL_ES_VERSION_3_0

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceGLES3();
//==============================================================================
//  Buffer
//==============================================================================
uint64_t        xxCreateIndexBufferGLES3(uint64_t device, int size);
uint64_t        xxCreateVertexBufferGLES3(uint64_t device, int size, uint64_t vertexAttribute);
void*           xxMapBufferGLES3(uint64_t device, uint64_t buffer);
void            xxUnmapBufferGLES3(uint64_t device, uint64_t buffer);
//==============================================================================

#endif
