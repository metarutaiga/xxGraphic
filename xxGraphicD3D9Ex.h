//==============================================================================
// xxGraphic : Direct3D 9Ex Header
//
// Copyright (c) 2019-2026 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceD3D9Ex();
xxAPI    uint64_t       xxCreateInstanceD3D9ExPS();
xxInline char const*    xxGetInstanceNameD3D9Ex() { return "Direct3D 9Ex Fixed Function"; }
xxInline char const*    xxGetInstanceNameD3D9ExPS() { return "Direct3D 9Ex Programmable Shader"; }

#ifdef _D3D9_H_

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceD3D9Ex();
uint64_t        xxCreateInstanceD3D9ExPS();
void            xxDestroyInstanceD3D9Ex(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
bool            xxTestDeviceD3D9Ex(uint64_t device);
//==============================================================================

#endif
