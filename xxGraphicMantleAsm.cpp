//==============================================================================
// xxGraphic : Mantle Assembly Shader Source
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "xxGraphicMantleAsm.h"

#if defined(_MSC_VER)
#   pragma const_seg(".rdata$shader")
#endif

#include "mantle/amdil.h"
#define IL_DCL_INPUT(usage, interp)     DWORD(IL_DCL_INPUT | (IL_IMPORTUSAGE_ ## usage << 16) | (IL_INTERPMODE_ ## interp << 21))
#define IL_DCL_OUTPUT(usage)            DWORD(IL_DCL_OUTPUT | (IL_IMPORTUSAGE_ ## usage << 16))
#define IL_DCL_RESOURCE(id, type)       DWORD(IL_DCL_RESOURCE | (id << 16) | (IL_USAGE_PIXTEX_ ## type << 24))
#define IL_DCL_RESOURCE_FMT(x,y,z,w)    DWORD((IL_ELEMENTFORMAT_ ## x << 20) | (IL_ELEMENTFORMAT_ ## y << 23) | (IL_ELEMENTFORMAT_ ## z << 26) | (IL_ELEMENTFORMAT_ ## w << 29))
#define IL_DST(type, id)                DWORD(id | (IL_REGTYPE_ ## type << 16))
#define IL_DSTMOD(type, id)             DWORD(id | (IL_REGTYPE_ ## type << 16) | (1 << 22))
#define IL_DSTIMM(type, id)             DWORD(id | (IL_REGTYPE_ ## type << 16) | (1 << 26))
#define IL_SRC(type, id)                DWORD(id | (IL_REGTYPE_ ## type << 16))
#define IL_SRCMOD(type, id)             DWORD(id | (IL_REGTYPE_ ## type << 16) | (1 << 22))
#define IL_SRCIMM(type, id)             DWORD(id | (IL_REGTYPE_ ## type << 16) | (1 << 26))

//==============================================================================
//  AMD IL Assembly Shader
//==============================================================================
const DWORD vertexShaderCodeIL[55] =
{
    IL_LANG_DX10_VS,
    IL_MINOR_VERSION | (IL_MAJOR_VERSION << 8) | (IL_SHADER_VERTEX << 16),                                  //  il_vs_2_0
    IL_DCL_CONST_BUFFER, IL_DSTIMM(CONST_BUFF, 0), 4,                                                       //  dcl_cb cb0[4]
    IL_DCL_INPUT(GENERIC, NOTUSED), IL_DSTMOD(INPUT, 0), 0x15,                                              //  dcl_input_generic v0.xyz_
    IL_DCL_INPUT(GENERIC, NOTUSED), IL_DST(INPUT, 1),                                                       //  dcl_input_generic v1
    IL_DCL_INPUT(GENERIC, NOTUSED), IL_DSTMOD(INPUT, 2), 0x05,                                              //  dcl_input_generic v2.xy__
    IL_DCL_OUTPUT(POS), IL_DST(OUTPUT, 0),                                                                  //  dcl_output_position o0
    IL_DCL_OUTPUT(GENERIC), IL_DST(OUTPUT, 1),                                                              //  dcl_output_generic o1
    IL_DCL_OUTPUT(GENERIC), IL_DSTMOD(OUTPUT, 2), 0x05,                                                     //  dcl_output_generic o2.xy__
    IL_OP_MUL, IL_DST(TEMP, 0), IL_SRCIMM(CONST_BUFF, 0), 0, IL_SRCMOD(INPUT, 0), 0x0000,                   //  mul r0, cb0[0], v0.x
    IL_OP_MAD, IL_DST(TEMP, 0), IL_SRCIMM(CONST_BUFF, 0), 1, IL_SRCMOD(INPUT, 0), 0x1111, IL_SRC(TEMP, 0),  //  mad r0, cb0[1], v0.y, r0
    IL_OP_MAD, IL_DST(TEMP, 0), IL_SRCIMM(CONST_BUFF, 0), 2, IL_SRCMOD(INPUT, 0), 0x2222, IL_SRC(TEMP, 0),  //  mad r0, cb0[2], v0.z, r0
    IL_OP_ADD, IL_DST(OUTPUT, 0), IL_SRC(TEMP, 0), IL_SRCIMM(CONST_BUFF, 0), 3,                             //  add o0, r0, cb0[3]
    IL_OP_MOV, IL_DST(OUTPUT, 1), IL_SRC(INPUT, 1),                                                         //  mov o1, v1
    IL_OP_MOV, IL_DSTMOD(OUTPUT, 2), 0x05, IL_SRCMOD(INPUT, 2), 0x0010,                                     //  mov o2.xy__, v2.xyxx
    IL_OP_RET_DYN,                                                                                          //  ret_dyn
    IL_OP_END                                                                                               //  end
};
//------------------------------------------------------------------------------
const DWORD pixelShaderCodeIL[27] =
{
    IL_LANG_DX10_PS,
    IL_MINOR_VERSION | (IL_MAJOR_VERSION << 8) | (IL_SHADER_PIXEL << 16),                                   // il_ps_2_0
    IL_DCL_RESOURCE(0, 2D), IL_DCL_RESOURCE_FMT(FLOAT, FLOAT, FLOAT, FLOAT),                                // dcl_resource_id(0)_type(2d)_fmtx(float)_fmty(float)_fmtz(float)_fmtw(float)
    IL_DCL_INPUT(GENERIC, LINEAR), IL_DST(INPUT, 1),                                                        // dcl_input_generic_interp(linear) v1
    IL_DCL_INPUT(GENERIC, LINEAR), IL_DSTMOD(INPUT, 2), 0x05,                                               // dcl_input_generic_interp(linear) v2.xy__
    IL_DCL_OUTPUT(GENERIC), IL_DST(OUTPUT, 0),                                                              // dcl_output_generic o0
    IL_OP_SAMPLE, IL_DST(TEMP, 4106), IL_SRCMOD(INPUT, 2), 0x0010,                                          // sample_resource(0)_sampler(0) r4106, v2.xyxx
    IL_OP_MOV, IL_DST(TEMP, 0), IL_SRC(TEMP, 4106),                                                         // mov r0, r4106
    IL_OP_MUL, IL_DST(TEMP, 4096), IL_SRC(TEMP, 0), IL_SRC(INPUT, 1),                                       // mul r4096, r0, v1
    IL_OP_MOV, IL_DST(OUTPUT, 0), IL_DST(TEMP, 4096),                                                       // mov o0, r4096
    IL_OP_RET_DYN,                                                                                          // ret_dyn
    IL_OP_END                                                                                               // end
};
//==============================================================================
