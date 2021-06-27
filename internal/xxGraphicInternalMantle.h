//==============================================================================
// xxGraphic : Mantle Internal Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

#define GR_PROTOTYPES 1
#include "mantle/mantle.h"
#include "mantle/mantleWsiWinExt.h"

extern void* grGetProcAddress(const char* name);
//==============================================================================
//  Blend Factor
//==============================================================================
inline GR_BLEND grBlendFactor(xxGraphicBlendFactor blend)
{
    switch (blend)
    {
    default:
    case BLEND_FACTOR_ZERO:                     return GR_BLEND_ZERO;
    case BLEND_FACTOR_ONE:                      return GR_BLEND_ONE;
    case BLEND_FACTOR_SRC_COLOR:                return GR_BLEND_SRC_COLOR;
    case BLEND_FACTOR_ONE_MINUS_SRC_COLOR:      return GR_BLEND_ONE_MINUS_SRC_COLOR;
    case BLEND_FACTOR_DST_COLOR:                return GR_BLEND_DEST_COLOR;
    case BLEND_FACTOR_ONE_MINUS_DST_COLOR:      return GR_BLEND_ONE_MINUS_DEST_COLOR;
    case BLEND_FACTOR_SRC_ALPHA:                return GR_BLEND_SRC_ALPHA;
    case BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:      return GR_BLEND_ONE_MINUS_SRC_ALPHA;
    case BLEND_FACTOR_DST_ALPHA:                return GR_BLEND_DEST_ALPHA;
    case BLEND_FACTOR_ONE_MINUS_DST_ALPHA:      return GR_BLEND_ONE_MINUS_DEST_ALPHA;
    case BLEND_FACTOR_CONSTANT_COLOR:           return GR_BLEND_CONSTANT_COLOR;
    case BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR: return GR_BLEND_ONE_MINUS_CONSTANT_COLOR;
    case BLEND_FACTOR_CONSTANT_ALPHA:           return GR_BLEND_CONSTANT_ALPHA;
    case BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA: return GR_BLEND_ONE_MINUS_CONSTANT_ALPHA;
    case BLEND_FACTOR_SRC_ALPHA_SATURATE:       return GR_BLEND_SRC_ALPHA_SATURATE;
    case BLEND_FACTOR_SRC1_COLOR:               return GR_BLEND_SRC1_COLOR;
    case BLEND_FACTOR_ONE_MINUS_SRC1_COLOR:     return GR_BLEND_ONE_MINUS_SRC1_COLOR;
    case BLEND_FACTOR_SRC1_ALPHA:               return GR_BLEND_SRC1_ALPHA;
    case BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA:     return GR_BLEND_ONE_MINUS_SRC1_ALPHA;
    }
}
//==============================================================================
