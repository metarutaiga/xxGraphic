//==============================================================================
// xxGraphic : Direct3D 9.0 Programmable Shader Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphicD3D9.h"

xxAPI    uint64_t       xxCreateInstanceD3D9PS();
xxInline char const*    xxGetInstanceNameD3D9PS() { return "Direct3D 9.0 Programmable Shader"; }

#ifdef _D3D9_H_

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceD3D9PS();
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t        xxCreateVertexAttributeD3D9PS(uint64_t device, int count, int* attribute);
void            xxDestroyVertexAttributeD3D9PS(uint64_t vertexAttribute);
//==============================================================================
//  Shader
//==============================================================================
uint64_t        xxCreateVertexShaderD3D9PS(uint64_t device, char const* shader, uint64_t vertexAttribute);
uint64_t        xxCreateFragmentShaderD3D9PS(uint64_t device, char const* shader);
void            xxDestroyShaderD3D9PS(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t        xxCreatePipelineD3D9PS(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader);
//==============================================================================
//  Command
//==============================================================================
void            xxSetVertexBuffersD3D9PS(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
void            xxSetVertexConstantBufferD3D9PS(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxSetFragmentConstantBufferD3D9PS(uint64_t commandEncoder, uint64_t buffer, int size);
//==============================================================================

#endif
