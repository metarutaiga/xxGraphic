#pragma once

#include "xxGraphicD3D9.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t   xxCreateInstanceD3D9S();
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t   xxCreateDeviceD3D9S(uint64_t instance);
xxAPI void       xxDestroyDeviceD3D9S(uint64_t device);
//==============================================================================
//  Command
//==============================================================================
xxAPI void       xxSetVertexAttributeD3D9S(uint64_t commandBuffer, uint64_t vertexAttribute);
//==============================================================================
//  Fixed-Function
//==============================================================================
xxAPI void       xxSetOrthographicTransformD3D9S(uint64_t commandBuffer, float left, float right, float top, float bottom);
