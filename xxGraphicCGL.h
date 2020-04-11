//==============================================================================
// xxGraphic : OpenGL CGL Header
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

//==============================================================================
//  Initialize
//==============================================================================
uint64_t xxGraphicCreateCGL(int version);
void xxGraphicDestroyCGL(uint64_t context);
//==============================================================================
//  Extension
//==============================================================================
void xxBindTextureWithSurface(const void* surface);
void xxBindRectangleProgram();
