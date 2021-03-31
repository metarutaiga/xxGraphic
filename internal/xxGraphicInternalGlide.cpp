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
//  Glide to OpenGL
//==============================================================================
#if defined(xxMACOS)
#define GL_SILENCE_DEPRECATION
#import <AppKit/NSOpenGL.h>
#import <AppKit/NSOpenGLView.h>
#import <AppKit/NSWindow.h>
#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>
#import <OpenGL/OpenGL.h>
static void*            g_glLibrary = nullptr;
static NSOpenGLView*    g_rootView = nil;
static NSOpenGLContext* g_openGLContext[256] = {};
static FxU32            g_texMinAddress = 0;
static FxU8             g_vertexLayout[256] = {};
//------------------------------------------------------------------------------
static void (*gto_glBegin)(GLenum mode);
static void (*gto_glVertex2f)(GLfloat x, GLfloat y);
static void (*gto_glVertex3f)(GLfloat x, GLfloat y, GLfloat z);
static void (*gto_glVertex4f)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
static void (*gto_glColor4f)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
static void (*gto_glTexCoord2f)(GLfloat s, GLfloat t);
static void (*gto_glEnd)(void);
static void (*gto_glMatrixMode)(GLenum mode);
static void (*gto_glLoadIdentity)(void);
static void (*gto_glOrtho)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
static void (*gto_glDepthRange)(GLclampd zNear, GLclampd zFar);
static void (*gto_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
static void (*gto_glClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
static void (*gto_glClearDepth)(GLclampd depth);
static void (*gto_glClear)(GLbitfield mask);
//------------------------------------------------------------------------------
static void gto_grDrawTriangle(const void *a, const void *b, const void *c)
{
    gto_glBegin(GL_TRIANGLES);

    for (int i = 0; i < 3; ++i)
    {
        const void *p = (i != 2) ? (i != 1) ? a : b : c;
        if (g_vertexLayout[GR_PARAM_ST0] != 0xFF)
        {
            GLfloat *v = (GLfloat*)((char*)p + g_vertexLayout[GR_PARAM_ST0]);
            gto_glTexCoord2f(v[0], v[1]);
        }
        if (g_vertexLayout[GR_PARAM_PARGB] != 0xFF)
        {
            GLubyte *v = (GLubyte*)((char*)p + g_vertexLayout[GR_PARAM_PARGB]);
            gto_glColor4f(v[0] / 255.0f, v[1] / 255.0f, v[2] / 255.0f, v[3] / 255.0f);
        }
        if (g_vertexLayout[GR_PARAM_XY] != 0xFF)
        {
            GLfloat *v = (GLfloat*)((char*)p + g_vertexLayout[GR_PARAM_XY]);
            if (g_vertexLayout[GR_PARAM_Z] != 0xFF)
            {
                if (g_vertexLayout[GR_PARAM_W] != 0xFF)
                {
                    gto_glVertex4f(v[0], v[1], 65535.0f / v[2], 1.0f / v[3]);
                }
                else
                {
                    gto_glVertex3f(v[0], v[1], 65535.0f / v[2]);
                }
            }
            else
            {
                gto_glVertex2f(v[0], v[1]);
            }
        }
    }

    gto_glEnd();
}
//------------------------------------------------------------------------------
static void gto_grVertexLayout(FxU32 param, FxI32 offset, FxU32 mode)
{
    g_vertexLayout[param] = (mode == GR_PARAM_ENABLE) ? offset : 0xFF;
}
//------------------------------------------------------------------------------
static void gto_grBufferClear(GrColor_t color, GrAlpha_t alpha, FxU32 depth)
{
    GLclampf glRed = ((color >> 0) & 0xFF) / 255.0f;
    GLclampf glGreen = ((color >> 8) & 0xFF) / 255.0f;
    GLclampf glBlue = ((color >> 16) & 0xFF) / 255.0f;
    GLclampf glAlpha = alpha;
    GLclampd glDepth = (depth & 0xFFFF) / 65535.0;
    gto_glClearColor(glRed, glGreen, glBlue, glAlpha);
    gto_glClearDepth(glDepth);
    gto_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
//------------------------------------------------------------------------------
static void gto_grBufferSwap(FxU32 swap_interval)
{
    [[NSOpenGLContext currentContext] flushBuffer];
}
//------------------------------------------------------------------------------
static GrContext_t gto_grSstWinOpen(void *hWnd, GrScreenResolution_t screen_resolution, GrScreenRefresh_t refresh_rate, GrColorFormat_t color_format, GrOriginLocation_t origin_location, int nColBuffers, int nAuxBuffers)
{
    NSOpenGLContext* nsContext = [[NSOpenGLContext alloc] initWithFormat:[g_rootView pixelFormat]
                                                            shareContext:[g_rootView openGLContext]];
    [nsContext setView:[[(__bridge NSWindow*)hWnd contentViewController] view]];
    int swapInterval = 0;
    [nsContext setValues:&swapInterval
            forParameter:NSOpenGLContextParameterSwapInterval];
    for (int i = 1; i < 256; ++i)
    {
        if (g_openGLContext[i] == nil)
        {
            g_openGLContext[i] = nsContext;
            return static_cast<GrContext_t>(i);
        }
    }
    return 0;
}
//------------------------------------------------------------------------------
static FxBool gto_grSstWinClose(GrContext_t context)
{
    if (context == 0)
        return FXFALSE;
    NSOpenGLContext* nsContext = g_openGLContext[context % 256];
    [nsContext makeCurrentContext];
    [nsContext clearDrawable];
    [NSOpenGLContext clearCurrentContext];
    g_openGLContext[context % 256] = nil;
    return FXTRUE;
}
//------------------------------------------------------------------------------
static FxBool gto_grSelectContext(GrContext_t context)
{
    if (context == 0)
        return FXFALSE;
    NSOpenGLContext* nsContext = g_openGLContext[context % 256];
    [nsContext makeCurrentContext];
    return FXTRUE;
}
//------------------------------------------------------------------------------
static const char * gto_grGetString(FxU32 pname)
{
    switch (pname)
    {
    case GR_HARDWARE:   return "Voodoo";
    case GR_VENDOR:     return "3Dfx Interactive";
    case GR_RENDERER:   return "Glide";
    case GR_VERSION:    return "3.0";
    }
    return nullptr;
}
//------------------------------------------------------------------------------
static void gto_grDepthRange(FxFloat n, FxFloat f)
{
    gto_glDepthRange(n, f);
}
//------------------------------------------------------------------------------
static void gto_grViewport(FxI32 x, FxI32 y, FxI32 width, FxI32 height)
{
    gto_glMatrixMode(GL_PROJECTION);
    gto_glLoadIdentity();
    gto_glOrtho(0, width, 0, height, -1, 1);
    gto_glViewport(x, y, width, height);
}
//------------------------------------------------------------------------------
static FxU32 gto_grTexMinAddress(GrChipID_t tmu)
{
    return g_texMinAddress;
}
//------------------------------------------------------------------------------
static void gto_grGlideInit(void)
{
    if (g_glLibrary == nullptr)
    {
        g_glLibrary = xxLoadLibrary("/System/Library/Frameworks/OpenGL.framework/OpenGL");
    }
    (void*&)gto_glBegin = xxGetProcAddress(g_glLibrary, "glBegin");
    (void*&)gto_glVertex2f = xxGetProcAddress(g_glLibrary, "glVertex2f");
    (void*&)gto_glVertex3f = xxGetProcAddress(g_glLibrary, "glVertex3f");
    (void*&)gto_glVertex4f = xxGetProcAddress(g_glLibrary, "glVertex4f");
    (void*&)gto_glColor4f = xxGetProcAddress(g_glLibrary, "glColor4f");
    (void*&)gto_glTexCoord2f = xxGetProcAddress(g_glLibrary, "glTexCoord2f");
    (void*&)gto_glEnd = xxGetProcAddress(g_glLibrary, "glEnd");
    (void*&)gto_glMatrixMode = xxGetProcAddress(g_glLibrary, "glMatrixMode");
    (void*&)gto_glLoadIdentity = xxGetProcAddress(g_glLibrary, "glLoadIdentity");
    (void*&)gto_glOrtho = xxGetProcAddress(g_glLibrary, "glOrtho");
    (void*&)gto_glDepthRange = xxGetProcAddress(g_glLibrary, "glDepthRange");
    (void*&)gto_glViewport = xxGetProcAddress(g_glLibrary, "glViewport");
    (void*&)gto_glClearColor = xxGetProcAddress(g_glLibrary, "glClearColor");
    (void*&)gto_glClearDepth = xxGetProcAddress(g_glLibrary, "glClearDepth");
    (void*&)gto_glClear = xxGetProcAddress(g_glLibrary, "glClear");

    NSOpenGLPixelFormatAttribute attributes[2] = { NSOpenGLPFADoubleBuffer };
    g_rootView = [[NSOpenGLView alloc] initWithFrame:CGRectMake(0, 0, 1, 1)
                                         pixelFormat:[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes]];

    memset(g_vertexLayout, 0xFF, sizeof(g_vertexLayout));
}
//------------------------------------------------------------------------------
static void gto_grGlideShutdown(void)
{
    if (g_glLibrary)
    {
        xxFreeLibrary(g_glLibrary);
        g_glLibrary = nullptr;
    }
    g_rootView = nil;
    g_texMinAddress = 0;
    gto_glBegin = nullptr;
    gto_glVertex2f = nullptr;
    gto_glVertex3f = nullptr;
    gto_glVertex4f = nullptr;
    gto_glColor4f = nullptr;
    gto_glTexCoord2f = nullptr;
    gto_glEnd = nullptr;
    gto_glMatrixMode = nullptr;
    gto_glLoadIdentity = nullptr;
    gto_glOrtho = nullptr;
    gto_glDepthRange = nullptr;
    gto_glViewport = nullptr;
    gto_glClearColor = nullptr;
    gto_glClearDepth = nullptr;
    gto_glClear = nullptr;
}
//------------------------------------------------------------------------------
GrProc FX_CALL gto_grGetProcAddress(char* name)
{
    switch (xxHash(name))
    {
    case xxHash("grDrawTriangle"):      return (GrProc)gto_grDrawTriangle;
    case xxHash("grVertexLayout"):      return (GrProc)gto_grVertexLayout;
    case xxHash("grBufferClear"):       return (GrProc)gto_grBufferClear;
    case xxHash("grBufferSwap"):        return (GrProc)gto_grBufferSwap;
    case xxHash("grSstWinOpen"):        return (GrProc)gto_grSstWinOpen;
    case xxHash("grSstWinClose"):       return (GrProc)gto_grSstWinClose;
    case xxHash("grSelectContext"):     return (GrProc)gto_grSelectContext;
    case xxHash("grGetString"):         return (GrProc)gto_grGetString;
    case xxHash("grDepthRange"):        return (GrProc)gto_grDepthRange;
    case xxHash("grViewport"):          return (GrProc)gto_grViewport;
    case xxHash("grTexMinAddress"):     return (GrProc)gto_grTexMinAddress;
    case xxHash("grGlideInit"):         return (GrProc)gto_grGlideInit;
    case xxHash("grGlideShutdown"):     return (GrProc)gto_grGlideShutdown;
    }
    return nullptr;
}
#endif
//==============================================================================
