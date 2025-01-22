//==============================================================================
// xxGraphic : OpenGL EGL Header
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

//==============================================================================
//  Initialize
//==============================================================================
uint64_t xxGraphicCreateEGL(int version);
void xxGraphicDestroyEGL(uint64_t context);
//==============================================================================
//  Extension
//==============================================================================
void const* xxCreateImageFromHardwareBuffer(void const* hardwareBuffer);
void xxBindTextureWithImage(void const* image);
void xxDestroyImage(void const* image);
