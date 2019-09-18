//==============================================================================
// xxGraphic : Direct3D 9Ex Header
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceD3D9Ex();
xxAPI uint64_t      xxCreateInstanceD3D9ExPS();
xxAPI void          xxDestroyInstanceD3D9Ex(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI bool          xxTestDeviceD3D9Ex(uint64_t device);
xxAPI const char*   xxGetDeviceStringD3D9Ex(uint64_t device);
xxAPI const char*   xxGetDeviceStringD3D9ExPS(uint64_t device);
