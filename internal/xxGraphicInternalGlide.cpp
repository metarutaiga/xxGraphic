//==============================================================================
// xxGraphic : Glide Internal Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicInternalGlide.h"

//==============================================================================
//  Loader
//==============================================================================
#define FX_PROTOTYPE(type, prototype, parameter, ...) \
extern type (FX_CALL* prototype ## Entry) parameter; \
extern "C" type FX_CALL prototype parameter \
{ \
    return prototype ## Entry(__VA_ARGS__); \
} \
static type FX_CALL prototype ## Trunk parameter \
{ \
    (void*&)prototype ## Entry = (void*)grGetProcAddress((char*)#prototype); \
    if (prototype ## Entry == nullptr) \
    { \
        prototype ## Entry = [] parameter -> type \
        { \
            typedef type returnType; \
            return returnType(); \
        }; \
    } \
    return prototype ## Entry(__VA_ARGS__); \
} \
type (FX_CALL* prototype ## Entry) parameter = prototype ## Trunk;
//------------------------------------------------------------------------------
FX_PROTOTYPE(void, grDrawPoint, (const void *pt), pt);
FX_PROTOTYPE(void, grDrawLine, (const void *v1, const void *v2), v1, v2);
FX_PROTOTYPE(void, grDrawTriangle, (const void *a, const void *b, const void *c), a, b, c);
FX_PROTOTYPE(void, grVertexLayout, (FxU32 param, FxI32 offset, FxU32 mode), param, offset, mode);
FX_PROTOTYPE(void, grDrawVertexArray, (FxU32 mode, FxU32 Count, void *pointers), mode, Count, pointers);
FX_PROTOTYPE(void, grDrawVertexArrayContiguous, (FxU32 mode, FxU32 Count, void *pointers, FxU32 stride), mode, Count, pointers, stride);
//------------------------------------------------------------------------------
FX_PROTOTYPE(void, grAADrawTriangle, (const void *a, const void *b, const void *c, FxBool ab_antialias, FxBool bc_antialias, FxBool ca_antialias), a, b, c, ab_antialias, bc_antialias, ca_antialias);
//------------------------------------------------------------------------------
FX_PROTOTYPE(void, grBufferClear, (GrColor_t color, GrAlpha_t alpha, FxU32 depth), color, alpha, depth);
FX_PROTOTYPE(void, grBufferSwap, (FxU32 swap_interval), swap_interval);
FX_PROTOTYPE(void, grRenderBuffer, (GrBuffer_t buffer), buffer);
//------------------------------------------------------------------------------
FX_PROTOTYPE(void, grErrorSetCallback, (GrErrorCallbackFnc_t fnc), fnc);
//------------------------------------------------------------------------------
FX_PROTOTYPE(void, grFinish, (void));
FX_PROTOTYPE(void, grFlush, (void));
FX_PROTOTYPE(GrContext_t, grSstWinOpen, (void *hWnd, GrScreenResolution_t screen_resolution, GrScreenRefresh_t refresh_rate, GrColorFormat_t color_format, GrOriginLocation_t origin_location, int nColBuffers, int nAuxBuffers), hWnd, screen_resolution, refresh_rate, color_format, origin_location, nColBuffers, nAuxBuffers);
FX_PROTOTYPE(FxBool, grSstWinClose, (GrContext_t context), context);
FX_PROTOTYPE(FxBool, grSelectContext, (GrContext_t context), context);
FX_PROTOTYPE(void, grSstOrigin, (GrOriginLocation_t origin), origin);
FX_PROTOTYPE(void, grSstSelect, (int which_sst), which_sst);
//------------------------------------------------------------------------------
FX_PROTOTYPE(void, grAlphaBlendFunction, (GrAlphaBlendFnc_t rgb_sf, GrAlphaBlendFnc_t rgb_df, GrAlphaBlendFnc_t alpha_sf, GrAlphaBlendFnc_t alpha_df), rgb_sf, rgb_df, alpha_sf, alpha_df);
FX_PROTOTYPE(void, grAlphaCombine, (GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert), function, factor, local, other, invert);
FX_PROTOTYPE(void, grAlphaControlsITRGBLighting, (FxBool enable), enable);
FX_PROTOTYPE(void, grAlphaTestFunction, (GrCmpFnc_t function), function);
FX_PROTOTYPE(void, grAlphaTestReferenceValue, (GrAlpha_t value), value);
FX_PROTOTYPE(void, grChromakeyMode, (GrChromakeyMode_t mode), mode);
FX_PROTOTYPE(void, grChromakeyValue, (GrColor_t value), value);
FX_PROTOTYPE(void, grClipWindow, (FxU32 minx, FxU32 miny, FxU32 maxx, FxU32 maxy), minx, miny, maxx, maxy);
FX_PROTOTYPE(void, grColorCombine, (GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert), function, factor, local, other, invert);
FX_PROTOTYPE(void, grColorMask, (FxBool rgb, FxBool a), rgb, a);
FX_PROTOTYPE(void, grCullMode, (GrCullMode_t mode), mode);
FX_PROTOTYPE(void, grConstantColorValue, (GrColor_t value), value);
FX_PROTOTYPE(void, grDepthBiasLevel, (FxI32 level), level);
FX_PROTOTYPE(void, grDepthBufferFunction, (GrCmpFnc_t function), function);
FX_PROTOTYPE(void, grDepthBufferMode, (GrDepthBufferMode_t mode), mode);
FX_PROTOTYPE(void, grDepthMask, (FxBool mask), mask);
FX_PROTOTYPE(void, grDisableAllEffects, (void));
FX_PROTOTYPE(void, grDitherMode, (GrDitherMode_t mode), mode);
FX_PROTOTYPE(void, grFogColorValue, (GrColor_t fogcolor), fogcolor);
FX_PROTOTYPE(void, grFogMode, (GrFogMode_t mode), mode);
FX_PROTOTYPE(void, grFogTable, (const GrFog_t ft[]), ft);
FX_PROTOTYPE(void, grLoadGammaTable, (FxU32 nentries, FxU32 *red, FxU32 *green, FxU32 *blue), nentries, red, green, blue);
FX_PROTOTYPE(void, grSplash, (float x, float y, float width, float height, FxU32 frame), x, y, width, height, frame);
FX_PROTOTYPE(FxU32, grGet, (FxU32 pname, FxU32 plength, FxI32 *params), pname, plength, params);
FX_PROTOTYPE(const char *, grGetString, (FxU32 pname), pname);
FX_PROTOTYPE(FxI32, grQueryResolutions, (const GrResolution *resTemplate, GrResolution *output), resTemplate, output);
FX_PROTOTYPE(FxBool, grReset, (FxU32 what), what);
//FX_PROTOTYPE(GrProc, grGetProcAddress, (char *procName), procName);
FX_PROTOTYPE(void, grEnable, (GrEnableMode_t mode), mode);
FX_PROTOTYPE(void, grDisable, (GrEnableMode_t mode), mode);
FX_PROTOTYPE(void, grCoordinateSpace, (GrCoordinateSpaceMode_t mode), mode);
FX_PROTOTYPE(void, grDepthRange, (FxFloat n, FxFloat f), n, f);
FX_PROTOTYPE(void, grViewport, (FxI32 x, FxI32 y, FxI32 width, FxI32 height), x, y, width, height);
//------------------------------------------------------------------------------
FX_PROTOTYPE(FxU32, grTexCalcMemRequired, (GrLOD_t lodmin, GrLOD_t lodmax, GrAspectRatio_t aspect, GrTextureFormat_t fmt), lodmin, lodmax, aspect, fmt);
FX_PROTOTYPE(FxU32, grTexTextureMemRequired, (FxU32 evenOdd, GrTexInfo *info), evenOdd, info);
FX_PROTOTYPE(FxU32, grTexMinAddress, (GrChipID_t tmu), tmu);
FX_PROTOTYPE(FxU32, grTexMaxAddress, (GrChipID_t tmu), tmu);
FX_PROTOTYPE(void, grTexNCCTable, (GrNCCTable_t table), table);
FX_PROTOTYPE(void, grTexSource, (GrChipID_t tmu, FxU32 startAddress, FxU32 evenOdd, GrTexInfo *info), tmu, startAddress, evenOdd, info);
FX_PROTOTYPE(void, grTexClampMode, (GrChipID_t tmu, GrTextureClampMode_t s_clampmode, GrTextureClampMode_t t_clampmode), tmu, s_clampmode, t_clampmode);
FX_PROTOTYPE(void, grTexCombine, (GrChipID_t tmu, GrCombineFunction_t rgb_function, GrCombineFactor_t rgb_factor, GrCombineFunction_t alpha_function,  GrCombineFactor_t alpha_factor, FxBool rgb_invert, FxBool alpha_invert), tmu, rgb_function, rgb_factor, alpha_function, alpha_factor, rgb_invert, alpha_invert);
FX_PROTOTYPE(void, grTexDetailControl, (GrChipID_t tmu, int lod_bias, FxU8 detail_scale, float detail_max), tmu, lod_bias, detail_scale, detail_max);
FX_PROTOTYPE(void, grTexFilterMode, (GrChipID_t tmu, GrTextureFilterMode_t minfilter_mode, GrTextureFilterMode_t magfilter_mode), tmu, minfilter_mode, magfilter_mode);
FX_PROTOTYPE(void, grTexLodBiasValue, (GrChipID_t tmu, float bias), tmu, bias);
FX_PROTOTYPE(void, grTexDownloadMipMap, (GrChipID_t tmu, FxU32 startAddress, FxU32 evenOdd, GrTexInfo *info), tmu, startAddress, evenOdd, info);
FX_PROTOTYPE(void, grTexDownloadMipMapLevel, (GrChipID_t tmu, FxU32 startAddress, GrLOD_t thisLod, GrLOD_t largeLod, GrAspectRatio_t aspectRatio, GrTextureFormat_t format, FxU32 evenOdd, void *data), tmu, startAddress, thisLod, largeLod, aspectRatio, format, evenOdd, data);
FX_PROTOTYPE(FxBool, grTexDownloadMipMapLevelPartial, (GrChipID_t tmu, FxU32 startAddress, GrLOD_t thisLod, GrLOD_t largeLod, GrAspectRatio_t aspectRatio, GrTextureFormat_t format, FxU32 evenOdd, void *data, int start, int end), tmu, startAddress, thisLod, largeLod, aspectRatio, format, evenOdd, data, start, end);
FX_PROTOTYPE(void, grTexDownloadTable, (GrTexTable_t type, void *data), type, data);
FX_PROTOTYPE(void, grTexDownloadTablePartial, (GrTexTable_t type, void *data, int start, int end), type, data, start, end);
FX_PROTOTYPE(void, grTexMipMapMode, (GrChipID_t tmu, GrMipMapMode_t mode, FxBool lodBlend), tmu, mode, lodBlend);
FX_PROTOTYPE(void, grTexMultibase, (GrChipID_t tmu, FxBool enable), tmu, enable);
FX_PROTOTYPE(void, grTexMultibaseAddress, (GrChipID_t tmu, GrTexBaseRange_t range, FxU32 startAddress, FxU32 evenOdd, GrTexInfo *info), tmu, range, startAddress, evenOdd, info);
//------------------------------------------------------------------------------
FX_PROTOTYPE(FxBool, grLfbLock, (GrLock_t type, GrBuffer_t buffer, GrLfbWriteMode_t writeMode, GrOriginLocation_t origin, FxBool pixelPipeline, GrLfbInfo_t *info), type, buffer, writeMode, origin, pixelPipeline, info);
FX_PROTOTYPE(FxBool, grLfbUnlock, (GrLock_t type, GrBuffer_t buffer), type, buffer);
FX_PROTOTYPE(void, grLfbConstantAlpha, (GrAlpha_t alpha), alpha);
FX_PROTOTYPE(void, grLfbConstantDepth, (FxU32 depth), depth);
FX_PROTOTYPE(void, grLfbWriteColorSwizzle, (FxBool swizzleBytes, FxBool swapWords), swizzleBytes, swapWords);
FX_PROTOTYPE(void, grLfbWriteColorFormat, (GrColorFormat_t colorFormat), colorFormat);
FX_PROTOTYPE(FxBool, grLfbWriteRegion, (GrBuffer_t dst_buffer, FxU32 dst_x, FxU32 dst_y, GrLfbSrcFmt_t src_format, FxU32 src_width, FxU32 src_height, FxBool pixelPipeline, FxI32 src_stride, void *src_data), dst_buffer, dst_x, dst_y, src_format, src_width, src_height, pixelPipeline, src_stride, src_data);
FX_PROTOTYPE(FxBool, grLfbReadRegion, (GrBuffer_t src_buffer, FxU32 src_x, FxU32 src_y, FxU32 src_width, FxU32 src_height, FxU32 dst_stride, void *dst_data), src_buffer, src_x, src_y, src_width, src_height, dst_stride, dst_data);
//------------------------------------------------------------------------------
FX_PROTOTYPE(void, grGlideInit, (void));
FX_PROTOTYPE(void, grGlideShutdown, (void));
FX_PROTOTYPE(void, grGlideGetState, (void *state), state);
FX_PROTOTYPE(void, grGlideSetState, (const void *state), state);
FX_PROTOTYPE(void, grGlideGetVertexLayout, (void *layout), layout);
FX_PROTOTYPE(void, grGlideSetVertexLayout, (const void *layout), layout);
//==============================================================================
