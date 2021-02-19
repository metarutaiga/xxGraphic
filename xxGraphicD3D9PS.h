//==============================================================================
// xxGraphic : Direct3D 9.0 Programmable Shader Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphicD3D9.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceD3D9PS();
//==============================================================================
//  Device
//==============================================================================
xxAPI const char*   xxGetDeviceNameD3D9PS();
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t      xxCreateVertexAttributeD3D9PS(uint64_t device, int count, int* attribute);
xxAPI void          xxDestroyVertexAttributeD3D9PS(uint64_t vertexAttribute);
//==============================================================================
//  Shader
//==============================================================================
xxAPI uint64_t      xxCreateVertexShaderD3D9PS(uint64_t device, const char* shader, uint64_t vertexAttribute);
xxAPI uint64_t      xxCreateFragmentShaderD3D9PS(uint64_t device, const char* shader);
xxAPI void          xxDestroyShaderD3D9PS(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
xxAPI uint64_t      xxCreatePipelineD3D9PS(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
//==============================================================================
//  Command
//==============================================================================
xxAPI void          xxSetVertexBuffersD3D9PS(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
xxAPI void          xxSetVertexConstantBufferD3D9PS(uint64_t commandEncoder, uint64_t buffer, int size);
xxAPI void          xxSetFragmentConstantBufferD3D9PS(uint64_t commandEncoder, uint64_t buffer, int size);
//==============================================================================
//  Fixed-Function
//==============================================================================
xxAPI void          xxSetTransformD3D9PS(uint64_t commandEncoder, const float* world, const float* view, const float* projection);
