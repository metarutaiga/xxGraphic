//==============================================================================
// xxGraphic : Mantle Internal Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"
#include "xxGraphicInternal.h"

#define GR_PROTOTYPES 1
#include "mantle/mantle.h"
#include "mantle/mantleWsiWinExt.h"

extern void* grGetProcAddress(const char* name);
//==============================================================================
//  Blend Factor
//==============================================================================
inline GR_BLEND grBlendFactor(const char* name)
{
    return xxTemplateBlendFactor<GR_BLEND, GR_BLEND_ZERO,
                                           GR_BLEND_ONE,
                                           GR_BLEND_SRC_COLOR,
                                           GR_BLEND_ONE_MINUS_SRC_COLOR,
                                           GR_BLEND_DEST_COLOR,
                                           GR_BLEND_ONE_MINUS_DEST_COLOR,
                                           GR_BLEND_SRC_ALPHA,
                                           GR_BLEND_ONE_MINUS_SRC_ALPHA,
                                           GR_BLEND_DEST_ALPHA,
                                           GR_BLEND_ONE_MINUS_DEST_ALPHA>(name);
}
//==============================================================================
//  Blend Operation
//==============================================================================
inline GR_BLEND_FUNC grBlendOp(const char* name)
{
    return xxTemplateBlendOp<GR_BLEND_FUNC, GR_BLEND_FUNC_ADD,
                                            GR_BLEND_FUNC_SUBTRACT,
                                            GR_BLEND_FUNC_REVERSE_SUBTRACT,
                                            GR_BLEND_FUNC_MIN,
                                            GR_BLEND_FUNC_MAX>(name);
}
//==============================================================================
//  Depth Comparison
//==============================================================================
inline GR_COMPARE_FUNC grCompareOp(const char* name)
{
    return xxTemplateCompareOp<GR_COMPARE_FUNC, GR_COMPARE_NEVER,
                                                GR_COMPARE_LESS,
                                                GR_COMPARE_EQUAL,
                                                GR_COMPARE_LESS_EQUAL,
                                                GR_COMPARE_GREATER,
                                                GR_COMPARE_NOT_EQUAL,
                                                GR_COMPARE_GREATER_EQUAL,
                                                GR_COMPARE_ALWAYS>(name);
}
//==============================================================================
