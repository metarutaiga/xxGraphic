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
inline GR_BLEND grBlendFactor(const char* name)
{
    bool a = false;
    bool c = false;
    bool d = false;
    bool o = false;
    bool s = false;
    for (char x; (x = (*name)); name++)
    {
        switch (x)
        {
        case 'A':
        case 'a':
            a = true;
            c = false;
            break;
        case 'C':
        case 'c':
            a = false;
            c = true;
            break;
        case 'D':
        case 'd':
            if (s) break;
            d = true;
            break;
        case 'O':
        case 'o':
        case '1':
            o = true;
            break;
        case 'S':
        case 's':
            if (s) break;
            s = true;
            break;
        case 'Z':
        case 'z':
        case '0':
            return GR_BLEND_ZERO;
        }
    }
    if (d)
    {
        if (c) return o ? GR_BLEND_ONE_MINUS_DEST_COLOR : GR_BLEND_DEST_COLOR;
        if (a) return o ? GR_BLEND_ONE_MINUS_DEST_ALPHA : GR_BLEND_DEST_ALPHA;
    }
    if (s)
    {
        if (c) return o ? GR_BLEND_ONE_MINUS_SRC_COLOR : GR_BLEND_SRC_COLOR;
        if (a) return o ? GR_BLEND_ONE_MINUS_SRC_ALPHA : GR_BLEND_SRC_ALPHA;
    }
    return o ? GR_BLEND_ONE : GR_BLEND_ZERO;
}
//==============================================================================
//  Blend Operation
//==============================================================================
inline GR_BLEND_FUNC grBlendOp(const char* name)
{
    for (char x; (x = (*name)); name++)
    {
        switch (x)
        {
        case 'A':
        case 'a':
        case '+':
            return GR_BLEND_FUNC_ADD;
        case 'S':
        case 's':
        case '-':
            return GR_BLEND_FUNC_SUBTRACT;
        case 'I':
        case 'i':
        case 'R':
        case 'r':
            return GR_BLEND_FUNC_REVERSE_SUBTRACT;
        case 'M':
        case 'm':
            name++;
            x = (*name);
            return (x == 'I' || x == 'i') ? GR_BLEND_FUNC_MIN : GR_BLEND_FUNC_MAX;
        }
    }
    return GR_BLEND_FUNC_ADD;
}
//==============================================================================
