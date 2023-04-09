//==============================================================================
// xxGraphic : OpenGL EGL Header
//
// Copyright (c) 2019-2023 TAiGA
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
const void* xxCreateImageFromHardwareBuffer(const void* hardwareBuffer);
void xxBindTextureWithImage(const void* image);
void xxDestroyImage(const void* image);
