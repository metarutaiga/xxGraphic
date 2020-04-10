//==============================================================================
// xxGraphic : OpenGL ES 3.0 Header
//
// Copyright (c) 2019-2020 TAiGA
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
xxAPI const char*   xxGetDeviceNameGLES3();
//==============================================================================
//  Buffer
//==============================================================================
xxAPI uint64_t      xxCreateIndexBufferGLES3(uint64_t device, unsigned int size);
xxAPI uint64_t      xxCreateVertexBufferGLES3(uint64_t device, unsigned int size, uint64_t vertexAttribute);
xxAPI void*         xxMapBufferGLES3(uint64_t device, uint64_t buffer);
xxAPI void          xxUnmapBufferGLES3(uint64_t device, uint64_t buffer);
