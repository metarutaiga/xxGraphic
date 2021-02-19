//==============================================================================
// xxGraphic : Direct3D 8.0 Programmable Shader Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphicD3D8.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceD3D8PS();
//==============================================================================
//  Device
//==============================================================================
xxAPI const char*   xxGetDeviceNameD3D8PS();
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t      xxCreateVertexAttributeD3D8PS(uint64_t device, int count, int* attribute);
xxAPI void          xxDestroyVertexAttributeD3D8PS(uint64_t vertexAttribute);
//==============================================================================
//  Shader
//==============================================================================
xxAPI uint64_t      xxCreateVertexShaderD3D8PS(uint64_t device, const char* shader, uint64_t vertexAttribute);
xxAPI uint64_t      xxCreateFragmentShaderD3D8PS(uint64_t device, const char* shader);
xxAPI void          xxDestroyShaderD3D8PS(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
xxAPI uint64_t      xxCreatePipelineD3D8PS(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
//==============================================================================
//  Command
//==============================================================================
xxAPI void          xxSetScissorD3D8PS(uint64_t commandEncoder, int x, int y, int width, int height);
xxAPI void          xxSetVertexBuffersD3D8PS(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
xxAPI void          xxSetVertexConstantBufferD3D8PS(uint64_t commandEncoder, uint64_t buffer, int size);
xxAPI void          xxSetFragmentConstantBufferD3D8PS(uint64_t commandEncoder, uint64_t buffer, int size);
