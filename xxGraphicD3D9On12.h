//==============================================================================
// xxGraphic : Direct3D 9On12 Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceD3D9On12();
xxAPI    uint64_t       xxCreateInstanceD3D9On12PS();
xxAPI    uint64_t       xxCreateInstanceD3D9On12Ex();
xxAPI    uint64_t       xxCreateInstanceD3D9On12ExPS();
xxInline char const*    xxGetInstanceNameD3D9On12() { return "Direct3D 9On12 Fixed Function"; }
xxInline char const*    xxGetInstanceNameD3D9On12PS() { return "Direct3D 9On12 Programmable Shader"; }
xxInline char const*    xxGetInstanceNameD3D9On12Ex() { return "Direct3D 9On12Ex Fixed Function"; }
xxInline char const*    xxGetInstanceNameD3D9On12ExPS() { return "Direct3D 9On12Ex Programmable Shader"; }

#ifdef _D3D9_H_

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceD3D9On12();
uint64_t        xxCreateInstanceD3D9On12PS();
uint64_t        xxCreateInstanceD3D9On12Ex();
uint64_t        xxCreateInstanceD3D9On12ExPS();
void            xxDestroyInstanceD3D9On12(uint64_t instance);
//==============================================================================

#endif
