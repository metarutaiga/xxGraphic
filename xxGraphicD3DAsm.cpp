//==============================================================================
// xxGraphic : Direct3D Assembly Shader Source
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "xxGraphicD3DAsm.h"

#if defined(_MSC_VER)
#   pragma const_seg(".rdata$shader")
#endif

#include "dxsdk/d3d9.h"
#include "dxsdk/d3d10TokenizedProgramFormat.hpp"

//==============================================================================
//  Direct3D 8.0 Assembly Shader
//==============================================================================
#define D3DSP_WRITEMASK_X       0x00010000
#define D3DSP_WRITEMASK_XY      0x00030000
#define D3DSP_WRITEMASK_XYZ     0x00070000
#define D3DSP_WRITEMASK_XYZW    0x000F0000
#define D3DDECLUSAGE_UNKNOWN    D3DSTT_UNKNOWN
#define D3DDECLUSAGE_2D         D3DSTT_2D
#define D3DDECLUSAGE_CUBE       D3DSTT_CUBE
#define D3DDECLUSAGE_VOLUME     D3DSTT_VOLUME
#define _SIO(sio, length)       (D3DSIO_ ## sio | (length << D3DSI_INSTLENGTH_SHIFT))
#define _TOKEN(mask, value)     ((value << D3DSP_ ## mask ## _SHIFT))                             | 0x80000000
#define _TOKEN1(mask, value)    ((value << D3DSP_ ## mask ## _SHIFT)  & D3DSP_ ## mask ## _MASK)  | 0x80000000
#define _TOKEN2(mask, value)    ((value << D3DSP_ ## mask ## _SHIFT2) & D3DSP_ ## mask ## _MASK2) | _TOKEN1(mask, value)
#define _DCL(dcl)               _TOKEN(DCL_USAGE, D3DDECLUSAGE_ ## dcl)
#define _SRC(reg, index)        _TOKEN2(REGTYPE, D3DSPR_ ## reg) | D3DVS_W_W     | D3DVS_Z_Z     | D3DVS_Y_Y     | D3DVS_X_X     | index
#define _SRCM(reg, index, m)    _TOKEN2(REGTYPE, D3DSPR_ ## reg) | D3DVS_W_ ## m | D3DVS_Z_ ## m | D3DVS_Y_ ## m | D3DVS_X_ ## m | index
#define _DST(reg, index)        _TOKEN2(REGTYPE, D3DSPR_ ## reg) | D3DSP_WRITEMASK_ALL   | index
#define _DSTM(reg, index, m)    _TOKEN2(REGTYPE, D3DSPR_ ## reg) | D3DSP_WRITEMASK_ ## m | index
//------------------------------------------------------------------------------
const DWORD vertexShaderCode10[] =
{ 
    D3DVS_VERSION(1, 0),                                                            // vs_1_0
    D3DSIO_MUL, _DST(TEMP, 0), _SRC(CONST, 0), _SRCM(INPUT, 0, X),                  // mul r0, c0, v0.x,
    D3DSIO_MAD, _DST(TEMP, 0), _SRC(CONST, 1), _SRCM(INPUT, 0, Y), _SRC(TEMP, 0),   // mad r0, c1, v0.y, r0
    D3DSIO_MAD, _DST(TEMP, 0), _SRC(CONST, 2), _SRCM(INPUT, 0, Z), _SRC(TEMP, 0),   // mad r0, c2, v0.z, r0
    D3DSIO_ADD, _DST(RASTOUT, 0), _SRC(TEMP, 0), _SRC(CONST, 3),                    // add oPos, r0, c3
    D3DSIO_MOV, _DST(ATTROUT, 0), _SRC(INPUT, 1),                                   // mov oD0, v1
    D3DSIO_MOV, _DSTM(TEXCRDOUT, 0, XY), _SRC(INPUT, 2),                            // mov oT0.xy, v2
    D3DSIO_END
};
//------------------------------------------------------------------------------
const DWORD vertexShaderCode11[] =
{ 
    D3DVS_VERSION(1, 1),                                                            // vs_1_1
    D3DSIO_DCL, _DCL(POSITION), _DST(INPUT, 0),                                     // dcl_position v0
    D3DSIO_DCL, _DCL(COLOR), _DST(INPUT, 1),                                        // dcl_color v1
    D3DSIO_DCL, _DCL(TEXCOORD), _DST(INPUT, 2),                                     // dcl_texcoord v2
    D3DSIO_MUL, _DST(TEMP, 0), _SRC(CONST, 0), _SRCM(INPUT, 0, X),                  // mul r0, c0, v0.x,
    D3DSIO_MAD, _DST(TEMP, 0), _SRC(CONST, 1), _SRCM(INPUT, 0, Y), _SRC(TEMP, 0),   // mad r0, c1, v0.y, r0
    D3DSIO_MAD, _DST(TEMP, 0), _SRC(CONST, 2), _SRCM(INPUT, 0, Z), _SRC(TEMP, 0),   // mad r0, c2, v0.z, r0
    D3DSIO_ADD, _DST(RASTOUT, 0), _SRC(TEMP, 0), _SRC(CONST, 3),                    // add oPos, r0, c3
    D3DSIO_MOV, _DST(ATTROUT, 0), _SRC(INPUT, 1),                                   // mov oD0, v1
    D3DSIO_MOV, _DSTM(TEXCRDOUT, 0, XY), _SRC(INPUT, 2),                            // mov oT0.xy, v2
    D3DSIO_END
};
//------------------------------------------------------------------------------
const DWORD pixelShaderCode11[] =
{
    D3DPS_VERSION(1, 1),                                                            // ps_1_1
    D3DSIO_TEX, _DST(TEXTURE, 0),                                                   // tex t0
    D3DSIO_MUL, _DST(TEMP, 0), _SRC(TEXTURE, 0), _SRC(INPUT, 0),                    // mul r0, t0, v0
    D3DSIO_END
};
//------------------------------------------------------------------------------
const DWORD pixelShaderCode20[] =
{
    D3DPS_VERSION(2, 0),                                                            // ps_2_0
    _SIO(DCL, 2), _DCL(UNKNOWN), _DST(INPUT, 0),                                    // dcl v0
    _SIO(DCL, 2), _DCL(UNKNOWN), _DST(TEXTURE, 0),                                  // dcl t0.xy
    _SIO(DCL, 2), _DCL(2D), _DST(SAMPLER, 0),                                       // dcl_2d s0
    _SIO(TEX, 3), _DST(TEMP, 0), _SRC(TEXTURE, 0), _SRC(SAMPLER, 0),                // texld r0, t0, s0
    _SIO(MUL, 3), _DST(TEMP, 0), _SRC(TEMP, 0), _SRC(INPUT, 0),                     // mul r0, r0, v0
    _SIO(MOV, 2), _DST(COLOROUT, 0), _SRC(TEMP, 0),                                 // mov oC0, r0
    _SIO(END, 0)
};
//==============================================================================
//  Direct3D 10.0 Assembly Shader
//==============================================================================
#define _DXBC                                   0x43425844
#define _ISGN                                   0x4e475349
#define _OSGN                                   0x4e47534f
#define _SHDR                                   0x52444853
#define _POSITION                               0x49534f50, 0x4e4f4954
#define _COLOR                                  0x4f4c4f43, 0x00000052
#define _TEXCOORD                               0x43584554, 0x44524f4f
#define _SV_POSITION                            0x505f5653, 0x5449534f, 0x004e4f49
#define _SV_TARGET                              0x545f5653, 0x45475241, 0x00000054
#define D3D10_SB_4_COMPONENT_XXXX               ((D3D10_SB_4_COMPONENT_X << 0) | (D3D10_SB_4_COMPONENT_X << 2) | (D3D10_SB_4_COMPONENT_X << 4) | (D3D10_SB_4_COMPONENT_X << 6))
#define D3D10_SB_4_COMPONENT_XYXX               ((D3D10_SB_4_COMPONENT_X << 0) | (D3D10_SB_4_COMPONENT_Y << 2) | (D3D10_SB_4_COMPONENT_X << 4) | (D3D10_SB_4_COMPONENT_X << 6))
#define D3D10_SB_4_COMPONENT_XYZW               ((D3D10_SB_4_COMPONENT_X << 0) | (D3D10_SB_4_COMPONENT_Y << 2) | (D3D10_SB_4_COMPONENT_Z << 4) | (D3D10_SB_4_COMPONENT_W << 6))
#define D3D10_SB_4_COMPONENT_YYYY               ((D3D10_SB_4_COMPONENT_Y << 0) | (D3D10_SB_4_COMPONENT_Y << 2) | (D3D10_SB_4_COMPONENT_Y << 4) | (D3D10_SB_4_COMPONENT_Y << 6))
#define D3D10_SB_4_COMPONENT_ZZZZ               ((D3D10_SB_4_COMPONENT_Z << 0) | (D3D10_SB_4_COMPONENT_Z << 2) | (D3D10_SB_4_COMPONENT_Z << 4) | (D3D10_SB_4_COMPONENT_Z << 6))
#define D3D10_SB_4_COMPONENT_WWWW               ((D3D10_SB_4_COMPONENT_W << 0) | (D3D10_SB_4_COMPONENT_W << 2) | (D3D10_SB_4_COMPONENT_W << 4) | (D3D10_SB_4_COMPONENT_W << 6))
#define D3D10_SB_OPERAND_4_COMPONENT_MASK_XY    (D3D10_SB_OPERAND_4_COMPONENT_MASK_X | D3D10_SB_OPERAND_4_COMPONENT_MASK_Y)
#define D3D10_SB_OPERAND_4_COMPONENT_MASK_XYZ   (D3D10_SB_OPERAND_4_COMPONENT_MASK_XY | D3D10_SB_OPERAND_4_COMPONENT_MASK_Z)
#define D3D10_SB_OPERAND_4_COMPONENT_MASK_XYZW  (D3D10_SB_OPERAND_4_COMPONENT_MASK_XYZ | D3D10_SB_OPERAND_4_COMPONENT_MASK_W)
#define D3D10_SB_RETURN_TYPE_FLOAT4             ((D3D10_SB_RETURN_TYPE_FLOAT << 0) | (D3D10_SB_RETURN_TYPE_FLOAT << 4) | (D3D10_SB_RETURN_TYPE_FLOAT << 8) | (D3D10_SB_RETURN_TYPE_FLOAT << 12))
#define _VER(type, major, minor)                ENCODE_D3D10_SB_TOKENIZED_PROGRAM_VERSION_TOKEN(D3D10_SB_ ## type, major, minor)
#define _OP(opcode, length)                     (D3D10_SB_OPCODE_ ## opcode | ENCODE_D3D10_SB_TOKENIZED_INSTRUCTION_LENGTH(length))
#define _OPDIM(opcode, length, dim)             (_OP(opcode, length) | ENCODE_D3D10_SB_RESOURCE_DIMENSION(D3D10_SB_RESOURCE_DIMENSION_ ## dim))
#define _OPINT(opcode, length, int)             (_OP(opcode, length) | ENCODE_D3D10_SB_INPUT_INTERPOLATION_MODE(D3D10_SB_INTERPOLATION_ ## int))
#define _0(index, reg)                          ENCODE_D3D10_SB_OPERAND_INDEX_DIMENSION(D3D10_SB_OPERAND_INDEX_ ## index) | \
                                                ENCODE_D3D10_SB_OPERAND_TYPE(D3D10_SB_OPERAND_TYPE_ ## reg) | \
                                                ENCODE_D3D10_SB_OPERAND_NUM_COMPONENTS(D3D10_SB_OPERAND_0_COMPONENT)
#define _4MM(index, reg, mask)                  ENCODE_D3D10_SB_OPERAND_INDEX_DIMENSION(D3D10_SB_OPERAND_INDEX_ ## index) | \
                                                ENCODE_D3D10_SB_OPERAND_TYPE(D3D10_SB_OPERAND_TYPE_ ## reg) | \
                                                D3D10_SB_OPERAND_4_COMPONENT_MASK_ ## mask | \
                                                ENCODE_D3D10_SB_OPERAND_4_COMPONENT_SELECTION_MODE(D3D10_SB_OPERAND_4_COMPONENT_MASK_MODE) | \
                                                ENCODE_D3D10_SB_OPERAND_NUM_COMPONENTS(D3D10_SB_OPERAND_4_COMPONENT)
#define _4M(index, reg)                         _4MM(index, reg, XYZW)
#define _4SS(index, reg, swizzle)               ENCODE_D3D10_SB_OPERAND_INDEX_DIMENSION(D3D10_SB_OPERAND_INDEX_ ## index) | \
                                                ENCODE_D3D10_SB_OPERAND_TYPE(D3D10_SB_OPERAND_TYPE_ ## reg) | \
                                                (D3D10_SB_4_COMPONENT_ ## swizzle << D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_SHIFT) | \
                                                ENCODE_D3D10_SB_OPERAND_4_COMPONENT_SELECTION_MODE(D3D10_SB_OPERAND_4_COMPONENT_SWIZZLE_MODE) | \
                                                ENCODE_D3D10_SB_OPERAND_NUM_COMPONENTS(D3D10_SB_OPERAND_4_COMPONENT)
#define _4S(index, reg)                         _4SS(index, reg, XYZW)
//------------------------------------------------------------------------------
const DWORD vertexShaderCode40[] =
{
    _DXBC, 0x97f66d1a, 0x57e3bff3, 0x64b9ebf9, 0x84763d6e,
    0x00000001, 0x00000258, 3,
    0x0000002c, 0x000000a8, 0x00000128,
    _ISGN, 0x00000074, 3, 0x00000008,
    0x00000050, 0, 0, 3, 0, 0x0707,
    0x0000005c, 0, 0, 3, 1, 0x0f0f,
    0x00000068, 0, 0, 3, 2, 0x0303,
    _POSITION, 0,
    _COLOR, 0,
    _TEXCOORD, 0,
    _OSGN, 0x00000078, 3, 0x00000008,
    0x00000050, 0, 1, 3, 0, 0x000f,
    0x00000060, 0, 0, 3, 1, 0x000f,
    0x0000006c, 0, 0, 3, 2, 0x0c03,
    _SV_POSITION, 0,
    _COLOR, 0,
    _TEXCOORD, 0,
    _SHDR, 0x00000128, _VER(VERTEX_SHADER, 4, 0), 0x0000004a,
    _OP(DCL_CONSTANT_BUFFER, 4), _4S(2D, CONSTANT_BUFFER), D3D10_SB_CONSTANT_BUFFER_IMMEDIATE_INDEXED, 4,       // dcl_constantbuffer CB0[4], immediateIndexed
    _OP(DCL_INPUT, 3), _4MM(1D, INPUT, XYZ), 0,                                                                 // dcl_input v0.xyz
    _OP(DCL_INPUT, 3), _4M(1D, INPUT), 1,                                                                       // dcl_input v1.xyzw
    _OP(DCL_INPUT, 3), _4MM(1D, INPUT, XY), 2,                                                                  // dcl_input v2.xy
    _OP(DCL_OUTPUT_SIV, 4), _4M(1D, OUTPUT), 0, 1,                                                              // dcl_output_siv o0.xyzw
    _OP(DCL_OUTPUT, 3), _4M(1D, OUTPUT), 1,                                                                     // dcl_output o1.xyzw
    _OP(DCL_OUTPUT, 3), _4MM(1D, OUTPUT, XY), 2,                                                                // dcl_output o2.xy
    _OP(DCL_TEMPS, 2), 1,                                                                                       // dcl_temps 1
    _OP(MUL, 8), _4M(1D, TEMP), 0, _4S(2D, CONSTANT_BUFFER), 0, 0, _4SS(1D, INPUT, XXXX), 0,                    // mul r0.xyzw, cb0[0].xyzw, v0.xxxx
    _OP(MAD, 10), _4M(1D, TEMP), 0, _4S(2D, CONSTANT_BUFFER), 0, 1, _4SS(1D, INPUT, YYYY), 0, _4S(1D, TEMP), 0, // mad r0.xyzw, cb0[1].xyzw, v0.yyyy, r0.xyzw
    _OP(MAD, 10), _4M(1D, TEMP), 0, _4S(2D, CONSTANT_BUFFER), 0, 2, _4SS(1D, INPUT, ZZZZ), 0, _4S(1D, TEMP), 0, // mad r0.xyzw, cb0[2].xyzw, v0.zzzz, r0.xyzw
    _OP(ADD, 8), _4M(1D, OUTPUT), 0, _4S(1D, TEMP), 0, _4S(2D, CONSTANT_BUFFER), 0, 3,                          // add o0.xyzw, r0.xyzw, cb0[3].xyzw
    _OP(MOV, 5), _4M(1D, OUTPUT), 1, _4S(1D, INPUT), 1,                                                         // mov o1.xyzw, v1.xyzw
    _OP(MOV, 5), _4MM(1D, OUTPUT, XY), 2, _4SS(1D, INPUT, XYXX), 2,                                             // mov o2.xy, v2.xyxx
    _OP(RET, 1)                                                                                                 // ret
};
//------------------------------------------------------------------------------
const DWORD pixelShaderCode40[] =
{
    _DXBC, 0x699f5ee8, 0xcbd8a667, 0xedaa31ef, 0x41715e6e,
    0x00000001, 0x00000180, 3,
    0x0000002c, 0x000000ac, 0x000000e4,
    _ISGN, 0x00000078, 3, 0x00000008,
    0x00000050, 0, 1, 3, 0, 0x000f,
    0x00000060, 0, 0, 3, 1, 0x0f0f,
    0x0000006c, 0, 0, 3, 2, 0x0303,
    _SV_POSITION, 0,
    _COLOR, 0,
    _TEXCOORD, 0,
    _OSGN, 0x00000030, 1, 0x00000008,
    0x00000020, 0, 0, 3, 0, 0x000f,
    _SV_TARGET, 0,
    _SHDR, 0x00000094, _VER(PIXEL_SHADER, 4, 0), 0x00000025,
    _OP(DCL_SAMPLER, 3), _0(1D, SAMPLER), D3D10_SB_SAMPLER_MODE_DEFAULT,                                        // dcl_sampler s0, mode_default
    _OPDIM(DCL_RESOURCE, 4, TEXTURE2D), _0(1D, RESOURCE), 0, D3D10_SB_RETURN_TYPE_FLOAT4,                       // dcl_resource_texture2d(float,float,float,float) t0
    _OPINT(DCL_INPUT_PS, 3, LINEAR), _4M(1D, INPUT), 1,                                                         // dcl_input_ps linear v1.xyzw
    _OPINT(DCL_INPUT_PS, 3, LINEAR), _4MM(1D, INPUT, XY), 2,                                                    // dcl_input_ps linear v2.xy
    _OP(DCL_OUTPUT, 3), _4M(1D, OUTPUT), 0,                                                                     // dcl_output o0.xyzw
    _OP(DCL_TEMPS, 2), 1,                                                                                       // dcl_temps 1
    _OP(SAMPLE, 9), _4M(1D, TEMP), 0, _4SS(1D, INPUT, XYXX), 2, _4S(1D, RESOURCE), 0, _0(1D, SAMPLER), 0,       // sample r0.xyzw, v2.xyxx, t0.xyzw, s0
    _OP(MUL, 7), _4M(1D, OUTPUT), 0, _4S(1D, TEMP), 0, _4S(1D, INPUT), 1,                                       // mul o0.xyzw, r0.xyzw, v1.xyzw
    _OP(RET, 1)                                                                                                 // ret
};
//------------------------------------------------------------------------------
