//==============================================================================
// xxGraphic : OpenGL EAGL Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

//==============================================================================
//  Initialize
//==============================================================================
uint64_t xxGraphicCreateEAGL(int version);
void xxGraphicDestroyEAGL(uint64_t context);
//==============================================================================
//  Extension
//==============================================================================
void xxBindTextureWithSurface(const void* surface);

