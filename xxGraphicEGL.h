//==============================================================================
// xxGraphic : OpenGL EGL Header
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

//==============================================================================
//  Initialize
//==============================================================================
uint64_t xxGraphicCreateEGL(int version);
void xxGraphicDestroyEGL(uint64_t context);
