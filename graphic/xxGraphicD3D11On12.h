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
xxAPI const char*   xxGetDeviceStringD3D11On12(uint64_t device);
//==============================================================================
//  Swapchain
//==============================================================================
xxAPI void          xxPresentSwapchainD3D11On12(uint64_t swapchain, void* view);
//==============================================================================
//  Command Buffer
//==============================================================================
xxAPI uint64_t      xxGetCommandBufferD3D11On12(uint64_t device, uint64_t swapchain);
