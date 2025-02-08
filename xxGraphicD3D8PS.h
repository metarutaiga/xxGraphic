//==============================================================================
// xxGraphic : Direct3D 8.0 Programmable Shader Header
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphicD3D8.h"

xxAPI    uint64_t       xxCreateInstanceD3D8PS();
xxInline char const*    xxGetInstanceNameD3D8PS() { return "Direct3D 8.0 Programmable Shader"; }

#ifdef _D3D8_H_

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceD3D8PS();
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t        xxCreateVertexAttributeD3D8PS(uint64_t device, int count, int* attribute);
void            xxDestroyVertexAttributeD3D8PS(uint64_t vertexAttribute);
//==============================================================================
//  Shader
//==============================================================================
uint64_t        xxCreateVertexShaderD3D8PS(uint64_t device, char const* shader, uint64_t vertexAttribute);
uint64_t        xxCreateFragmentShaderD3D8PS(uint64_t device, char const* shader);
void            xxDestroyShaderD3D8PS(uint64_t device, uint64_t shader);
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t        xxCreatePipelineD3D8PS(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t meshShader, uint64_t vertexShader, uint64_t fragmentShader);
void            xxDestroyPipelineD3D8PS(uint64_t pipeline);
//==============================================================================
//  Command
//==============================================================================
void            xxSetScissorD3D8PS(uint64_t commandEncoder, int x, int y, int width, int height);
void            xxSetVertexBuffersD3D8PS(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute);
void            xxSetVertexConstantBufferD3D8PS(uint64_t commandEncoder, uint64_t buffer, int size);
void            xxSetFragmentConstantBufferD3D8PS(uint64_t commandEncoder, uint64_t buffer, int size);
//==============================================================================

#endif
