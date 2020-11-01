//==============================================================================
// xxGraphic : OpenGL WGL Header
//
// Copyright (c) 2019-2020 TAiGA
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
const void* xxGetDeviceFromDirect3DTexture(const void* texture);
const void* xxCreateImageFromDirect3DTexture(const void* device, const void* texture, int id);
void xxDestroyImage(const void* device, const void* image);
