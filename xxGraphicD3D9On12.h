//==============================================================================
// xxGraphic : Direct3D 9On12 Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceD3D9On12();
xxAPI uint64_t      xxCreateInstanceD3D9On12PS();
xxAPI uint64_t      xxCreateInstanceD3D9On12Ex();
xxAPI uint64_t      xxCreateInstanceD3D9On12ExPS();
xxAPI void          xxDestroyInstanceD3D9On12(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI bool          xxTestDeviceD3D9On12Ex(uint64_t device);
xxAPI char const*   xxGetDeviceNameD3D9On12();
xxAPI char const*   xxGetDeviceNameD3D9On12PS();
xxAPI char const*   xxGetDeviceNameD3D9On12Ex();
xxAPI char const*   xxGetDeviceNameD3D9On12ExPS();
