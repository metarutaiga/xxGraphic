//==============================================================================
// xxGraphic : Direct3D 11On12 Header
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

//==============================================================================
//  Instance
//==============================================================================
xxAPI uint64_t      xxCreateInstanceD3D11On12();
xxAPI void          xxDestroyInstanceD3D11On12(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
xxAPI uint64_t      xxCreateDeviceD3D11On12(uint64_t instance);
xxAPI void          xxDestroyDeviceD3D11On12(uint64_t device);
xxAPI const char*   xxGetDeviceNameD3D11On12();
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI void          xxPresentSwapchainD3D11On12(uint64_t swapchain);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI uint64_t      xxGetCommandBufferD3D11On12(uint64_t device, uint64_t swapchain);
