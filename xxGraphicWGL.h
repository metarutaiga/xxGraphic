//==============================================================================
// xxGraphic : OpenGL WGL Header
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

//==============================================================================
//  Initialize
//==============================================================================
uint64_t xxGraphicCreateWGL(int version);
void xxGraphicDestroyWGL(uint64_t context);
//==============================================================================
//  Extension
//==============================================================================
void const* xxGetDeviceFromDirect3DTexture(void const* texture);
void const* xxCreateImageFromDirect3DTexture(void const* device, void const* texture, int id);
void xxDestroyImage(void const* device, void const* image);
