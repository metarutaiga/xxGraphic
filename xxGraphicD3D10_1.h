//==============================================================================
// xxGraphic : Direct3D 10.1 Header
//
// Copyright (c) 2019-2026 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceD3D10_1();
xxInline char const*    xxGetInstanceNameD3D10_1() { return "Direct3D 10.1"; }

#ifdef __d3d10_1_h__

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceD3D10_1();
void            xxDestroyInstanceD3D10_1(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
uint64_t        xxCreateDeviceD3D10_1(uint64_t instance);
//==============================================================================

#endif
