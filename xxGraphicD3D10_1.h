#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceD3D10_1();
xxAPI void          xxDestroyInstanceD3D10_1(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t      xxCreateDeviceD3D10_1(uint64_t instance);
xxAPI const char*   xxGetDeviceStringD3D10_1(uint64_t device);
