//==============================================================================
// xxGraphic : OpenGL ES 3.0 Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceGLES3();
//==============================================================================
//  Device
//==============================================================================
xxAPI char const*   xxGetDeviceNameGLES3();
//==============================================================================
//  Buffer
//==============================================================================
xxAPI uint64_t      xxCreateIndexBufferGLES3(uint64_t device, int size);
xxAPI uint64_t      xxCreateVertexBufferGLES3(uint64_t device, int size, uint64_t vertexAttribute);
xxAPI void*         xxMapBufferGLES3(uint64_t device, uint64_t buffer);
xxAPI void          xxUnmapBufferGLES3(uint64_t device, uint64_t buffer);
