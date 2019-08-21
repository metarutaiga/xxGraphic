#pragma once

#include "xxGraphicD3D8.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t   xxCreateInstanceD3D8S();
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t   xxCreateDeviceD3D8S(uint64_t instance);
xxAPI void       xxDestroyDeviceD3D8S(uint64_t device);
xxAPI const char*xxGetDeviceStringD3D8S(uint64_t device);
//==============================================================================
//  Command
//==============================================================================
xxAPI void       xxSetScissorD3D8S(uint64_t commandBuffer, int x, int y, int width, int height);
xxAPI void       xxSetVertexAttributeD3D8S(uint64_t commandBuffer, uint64_t vertexAttribute);
//==============================================================================
//  Fixed-Function
//==============================================================================
xxAPI void       xxSetOrthographicTransformD3D8S(uint64_t commandBuffer, float left, float right, float top, float bottom);
