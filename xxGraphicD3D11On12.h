//==============================================================================
// xxGraphic : Direct3D 11On12 Header
//
// Copyright (c) 2019-2026 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

xxAPI    uint64_t       xxCreateInstanceD3D11On12();
xxInline char const*    xxGetInstanceNameD3D11On12() { return "Direct3D 11On12"; }

#ifdef __d3d11on12_h__

//==============================================================================
//  Instance
//==============================================================================
uint64_t        xxCreateInstanceD3D11On12();
void            xxDestroyInstanceD3D11On12(uint64_t instance);
//==============================================================================
//  Device
//==============================================================================
uint64_t        xxCreateDeviceD3D11On12(uint64_t instance);
void            xxDestroyDeviceD3D11On12(uint64_t device);
//==============================================================================
//  Swapchain
//==============================================================================
void            xxPresentSwapchainD3D11On12(uint64_t swapchain);
//==============================================================================
//  Command Buffer
//==============================================================================
uint64_t        xxGetCommandBufferD3D11On12(uint64_t device, uint64_t swapchain);
//==============================================================================

#endif
