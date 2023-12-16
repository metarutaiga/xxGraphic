//==============================================================================
// xxGraphic : Direct3D 10.1 Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
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
xxAPI char const*   xxGetDeviceNameD3D10_1();
