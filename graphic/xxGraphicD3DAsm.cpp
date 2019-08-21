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
