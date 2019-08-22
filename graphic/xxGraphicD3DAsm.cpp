#include "xxGraphicD3DAsm.h"

#include "dxsdk/d3d9.h"

//==============================================================================
//  Direct3D Assembly Shader
//==============================================================================
#define D3DSP_WRITEMASK_X           0x00010000
#define D3DSP_WRITEMASK_XY          0x00030000
#define D3DSP_WRITEMASK_XYZ         0x00070000
#define D3DSP_WRITEMASK_XYZW        0x000F0000
#define D3DDECLUSAGE_UNKNOWN        D3DSTT_UNKNOWN
#define D3DDECLUSAGE_2D             D3DSTT_2D
#define D3DDECLUSAGE_CUBE           D3DSTT_CUBE
#define D3DDECLUSAGE_VOLUME         D3DSTT_VOLUME
#define _SIO(sio, length)           D3DSIO_ ## sio | ((length << D3DSI_INSTLENGTH_SHIFT) & D3DSI_INSTLENGTH_MASK)
#define _TOKEN(mask, value)         ((value << D3DSP_ ## mask ## _SHIFT))                             | 0x80000000
#define _TOKEN1(mask, value)        ((value << D3DSP_ ## mask ## _SHIFT)  & D3DSP_ ## mask ## _MASK)  | 0x80000000
#define _TOKEN2(mask, value)        ((value << D3DSP_ ## mask ## _SHIFT2) & D3DSP_ ## mask ## _MASK2) | _TOKEN1(mask, value)
#define _DCL(dcl)                   _TOKEN(DCL_USAGE, D3DDECLUSAGE_ ## dcl)
#define _SRC(reg, index)            _TOKEN2(REGTYPE, D3DSPR_ ## reg) | D3DVS_W_W     | D3DVS_Z_Z     | D3DVS_Y_Y     | D3DVS_X_X     | index
#define _SRCM(reg, index, m)        _TOKEN2(REGTYPE, D3DSPR_ ## reg) | D3DVS_W_ ## m | D3DVS_Z_ ## m | D3DVS_Y_ ## m | D3DVS_X_ ## m | index
#define _DST(reg, index)            _TOKEN2(REGTYPE, D3DSPR_ ## reg) | D3DSP_WRITEMASK_ALL   | index
#define _DSTM(reg, index, m)        _TOKEN2(REGTYPE, D3DSPR_ ## reg) | D3DSP_WRITEMASK_ ## m | index
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
//------------------------------------------------------------------------------
const DWORD vertexShaderCode40[] =
{
    0x43425844, 0x1b72f980, 0xf6ba72d7, 0xb4c95127,
    0x8b3cb55e, 0x00000001, 0x00000390, 0x00000005,
    0x00000034, 0x00000100, 0x00000170, 0x000001e4,
    0x00000314, 0x46454452, 0x000000c4, 0x00000001,
    0x00000048, 0x00000001, 0x0000001c, 0xfffe0400,
    0x00000100, 0x0000009c, 0x0000003c, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000001, 0x00000000, 0x6f6c4724, 0x736c6162,
    0xababab00, 0x0000003c, 0x00000001, 0x00000060,
    0x00000040, 0x00000000, 0x00000000, 0x00000078,
    0x00000000, 0x00000040, 0x00000002, 0x0000008c,
    0x00000000, 0x6a6f7250, 0x69746365, 0x614d6e6f,
    0x78697274, 0xababab00, 0x00030003, 0x00040004,
    0x00000000, 0x00000000, 0x7263694d, 0x666f736f,
    0x52282074, 0x4c482029, 0x53204c53, 0x65646168,
    0x6f432072, 0x6c69706d, 0x31207265, 0x00312e30,
    0x4e475349, 0x00000068, 0x00000003, 0x00000008,
    0x00000050, 0x00000000, 0x00000000, 0x00000003,
    0x00000000, 0x00000707, 0x00000059, 0x00000000,
    0x00000000, 0x00000003, 0x00000001, 0x00000f0f,
    0x0000005f, 0x00000000, 0x00000000, 0x00000003,
    0x00000002, 0x00000303, 0x49534f50, 0x4e4f4954,
    0x4c4f4300, 0x5400524f, 0x4f435845, 0x0044524f,
    0x4e47534f, 0x0000006c, 0x00000003, 0x00000008,
    0x00000050, 0x00000000, 0x00000001, 0x00000003,
    0x00000000, 0x0000000f, 0x0000005c, 0x00000000,
    0x00000000, 0x00000003, 0x00000001, 0x0000000f,
    0x00000062, 0x00000000, 0x00000000, 0x00000003,
    0x00000002, 0x00000c03, 0x505f5653, 0x5449534f,
    0x004e4f49, 0x4f4c4f43, 0x45540052, 0x4f4f4358,
    0xab004452, 0x52444853, 0x00000128, 0x00010040,
    0x0000004a, 0x04000059, 0x00208e46, 0x00000000,
    0x00000004, 0x0300005f, 0x00101072, 0x00000000,
    0x0300005f, 0x001010f2, 0x00000001, 0x0300005f,
    0x00101032, 0x00000002, 0x04000067, 0x001020f2,
    0x00000000, 0x00000001, 0x03000065, 0x001020f2,
    0x00000001, 0x03000065, 0x00102032, 0x00000002,
    0x02000068, 0x00000001, 0x08000038, 0x001000f2,
    0x00000000, 0x00101556, 0x00000000, 0x00208e46,
    0x00000000, 0x00000001, 0x0a000032, 0x001000f2,
    0x00000000, 0x00208e46, 0x00000000, 0x00000000,
    0x00101006, 0x00000000, 0x00100e46, 0x00000000,
    0x0a000032, 0x001000f2, 0x00000000, 0x00208e46,
    0x00000000, 0x00000002, 0x00101aa6, 0x00000000,
    0x00100e46, 0x00000000, 0x08000000, 0x001020f2,
    0x00000000, 0x00100e46, 0x00000000, 0x00208e46,
    0x00000000, 0x00000003, 0x05000036, 0x001020f2,
    0x00000001, 0x00101e46, 0x00000001, 0x05000036,
    0x00102032, 0x00000002, 0x00101046, 0x00000002,
    0x0100003e, 0x54415453, 0x00000074, 0x00000007,
    0x00000001, 0x00000000, 0x00000006, 0x00000004,
    0x00000000, 0x00000000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000002, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
//------------------------------------------------------------------------------
const DWORD pixelShaderCode40[] =
{
    0x43425844, 0x7285ce9e, 0xb45297a7, 0xfa28c46d,
    0xc1d2c010, 0x00000001, 0x00000294, 0x00000005,
    0x00000034, 0x000000d4, 0x00000148, 0x0000017c,
    0x00000218, 0x46454452, 0x00000098, 0x00000000,
    0x00000000, 0x00000002, 0x0000001c, 0xffff0400,
    0x00000100, 0x0000006e, 0x0000005c, 0x00000003,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000001, 0x00000000, 0x00000065, 0x00000002,
    0x00000005, 0x00000004, 0xffffffff, 0x00000000,
    0x00000001, 0x0000000c, 0x706d6173, 0x3072656c,
    0x78657400, 0x65727574, 0x694d0030, 0x736f7263,
    0x2074666f, 0x20295228, 0x4c534c48, 0x61685320,
    0x20726564, 0x706d6f43, 0x72656c69, 0x2e303120,
    0xabab0031, 0x4e475349, 0x0000006c, 0x00000003,
    0x00000008, 0x00000050, 0x00000000, 0x00000001,
    0x00000003, 0x00000000, 0x0000000f, 0x0000005c,
    0x00000000, 0x00000000, 0x00000003, 0x00000001,
    0x00000f0f, 0x00000062, 0x00000000, 0x00000000,
    0x00000003, 0x00000002, 0x00000303, 0x505f5653,
    0x5449534f, 0x004e4f49, 0x4f4c4f43, 0x45540052,
    0x4f4f4358, 0xab004452, 0x4e47534f, 0x0000002c,
    0x00000001, 0x00000008, 0x00000020, 0x00000000,
    0x00000000, 0x00000003, 0x00000000, 0x0000000f,
    0x545f5653, 0x65677261, 0xabab0074, 0x52444853,
    0x00000094, 0x00000040, 0x00000025, 0x0300005a,
    0x00106000, 0x00000000, 0x04001858, 0x00107000,
    0x00000000, 0x00005555, 0x03001062, 0x001010f2,
    0x00000001, 0x03001062, 0x00101032, 0x00000002,
    0x03000065, 0x001020f2, 0x00000000, 0x02000068,
    0x00000001, 0x09000045, 0x001000f2, 0x00000000,
    0x00101046, 0x00000002, 0x00107e46, 0x00000000,
    0x00106000, 0x00000000, 0x07000038, 0x001020f2,
    0x00000000, 0x00100e46, 0x00000000, 0x00101e46,
    0x00000001, 0x0100003e, 0x54415453, 0x00000074,
    0x00000003, 0x00000001, 0x00000000, 0x00000003,
    0x00000001, 0x00000000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000
};
//------------------------------------------------------------------------------
