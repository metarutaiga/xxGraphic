//==============================================================================
// xxGraphic : OpenGL WGL Header
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

//==============================================================================
//  Initialize
//==============================================================================
uint64_t xxGraphicCreateWGL(int version);
void xxGraphicDestroyWGL(uint64_t context);
