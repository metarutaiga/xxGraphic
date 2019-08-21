#pragma once

#include "xxGraphicD3D8.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t   xxCreateInstanceD3D8S();
//==============================================================================
//  Device
//==============================================================================
xxAPI const char*xxGetDeviceStringD3D8S(uint64_t device);
//==============================================================================
//  Vertex Attribute
//==============================================================================
xxAPI uint64_t   xxCreateVertexAttributeD3D8S(uint64_t device, int count, ...);
xxAPI void       xxDestroyVertexAttributeD3D8S(uint64_t vertexAttribute);
//==============================================================================
//  Shader
//==============================================================================
xxAPI uint64_t   xxCreateVertexShaderD3D8S(uint64_t device, const char* shader, uint64_t vertexAttribute);
xxAPI uint64_t   xxCreateFragmentShaderD3D8S(uint64_t device, const char* shader);
xxAPI void       xxDestroyShaderD3D8S(uint64_t device, uint64_t shader);
//==============================================================================
//  Command
//==============================================================================
xxAPI void       xxSetScissorD3D8S(uint64_t commandBuffer, int x, int y, int width, int height);
xxAPI void       xxSetVertexAttributeD3D8S(uint64_t commandBuffer, uint64_t vertexAttribute);
xxAPI void       xxSetVertexShaderD3D8S(uint64_t commandBuffer, uint64_t shader);
xxAPI void       xxSetFragmentShaderD3D8S(uint64_t commandBuffer, uint64_t shader);
xxAPI void       xxSetVertexConstantBufferD3D8S(uint64_t commandBuffer, uint64_t buffer, unsigned int size);
xxAPI void       xxSetFragmentConstantBufferD3D8S(uint64_t commandBuffer, uint64_t buffer, unsigned int size);
