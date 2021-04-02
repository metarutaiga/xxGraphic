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
//  Glide to OpenGL prototype
//==============================================================================
#if defined(xxMACOS)
#define GL_SILENCE_DEPRECATION
#import <AppKit/NSOpenGL.h>
#import <AppKit/NSOpenGLView.h>
#import <AppKit/NSWindow.h>
#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>
#import <OpenGL/OpenGL.h>
#define GL_APIENTRY
//------------------------------------------------------------------------------
static void*                    g_glLibrary = nullptr;
static NSOpenGLView*            g_rootView = nil;
static NSOpenGLContext*         g_openGLContext[256] = {};
#elif defined(xxWINDOWS)
#include "../gl/gl.h"
#include "../gl/wgl.h"
typedef double GLdouble;
typedef double GLclampd;
static int                      (WINAPI* ChoosePixelFormat)(HDC, CONST PIXELFORMATDESCRIPTOR*);
static BOOL                     (WINAPI* SetPixelFormat)(HDC, int, CONST PIXELFORMATDESCRIPTOR*);
static BOOL                     (WINAPI* SwapBuffers)(HDC);
static PFNWGLCREATECONTEXTPROC  wglCreateContext;
static PFNWGLDELETECONTEXTPROC  wglDeleteContext;
static PFNWGLGETCURRENTDCPROC   wglGetCurrentDC;
static PFNWGLMAKECURRENTPROC    wglMakeCurrent;
static PFNWGLSHARELISTSPROC     wglShareLists;
//------------------------------------------------------------------------------
static void*                    g_gdiLibrary = nullptr;
static void*                    g_glLibrary = nullptr;
static HWND                     g_dummyWindow = nullptr;
static PIXELFORMATDESCRIPTOR    g_desc = {};
static HGLRC                    g_instance = nullptr;
static HGLRC                    g_openGLContext[256] = {};
static HDC                      g_openGLDC[256] = {};
static HWND                     g_openGLHWND[256] = {};
#endif
#if defined(xxMACOS) || defined(xxWINDOWS)
static FxU32                    g_width = 0;
static FxU32                    g_height = 0;
static FxU32                    g_baseTexture = 0;
static FxU8                     g_vertexLayout[256] = {};
//------------------------------------------------------------------------------
static void (GL_APIENTRY* gto_glEnable)(GLenum cap);
static void (GL_APIENTRY* gto_glBlendFunc)(GLenum sfactor, GLenum dfactor);
static void (GL_APIENTRY* gto_glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
static void (GL_APIENTRY* gto_glBegin)(GLenum mode);
static void (GL_APIENTRY* gto_glVertex4f)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
static void (GL_APIENTRY* gto_glColor4f)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
static void (GL_APIENTRY* gto_glTexCoord4f)(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
static void (GL_APIENTRY* gto_glEnd)(void);
static void (GL_APIENTRY* gto_glGenTextures)(GLsizei n, GLuint* textures);
static void (GL_APIENTRY* gto_glBindTexture)(GLenum target, GLuint texture);
static void (GL_APIENTRY* gto_glDeleteTextures)(GLsizei n, const GLuint* textures);
static void (GL_APIENTRY* gto_glTexParameteri)(GLenum target, GLenum pname, GLint param);
static void (GL_APIENTRY* gto_glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
static void (GL_APIENTRY* gto_glMatrixMode)(GLenum mode);
static void (GL_APIENTRY* gto_glLoadIdentity)(void);
static void (GL_APIENTRY* gto_glOrtho)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
static void (GL_APIENTRY* gto_glDepthRange)(GLclampd zNear, GLclampd zFar);
static void (GL_APIENTRY* gto_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
static void (GL_APIENTRY* gto_glScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
static void (GL_APIENTRY* gto_glClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
static void (GL_APIENTRY* gto_glClearDepth)(GLclampd depth);
static void (GL_APIENTRY* gto_glClear)(GLbitfield mask);
//------------------------------------------------------------------------------
template<int R, int G, int B, int A>
static void FX_CALL gto_grDrawTriangle(const void *a, const void *b, const void *c)
{
    gto_glBegin(GL_TRIANGLES);

    for (int i = 0; i < 3; ++i)
    {
        const void *p = (i != 2) ? (i != 1) ? a : b : c;
        if (g_vertexLayout[GR_PARAM_ST0] != 0xFF)
        {
            GLfloat *v = (GLfloat*)((char*)p + g_vertexLayout[GR_PARAM_ST0]);
            gto_glTexCoord4f(v[0], v[1], 0.0f, 1.0f);
        }
        if (g_vertexLayout[GR_PARAM_PARGB] != 0xFF)
        {
            GLubyte *v = (GLubyte*)((char*)p + g_vertexLayout[GR_PARAM_PARGB]);
            gto_glColor4f(v[R] / 255.0f, v[G] / 255.0f, v[B] / 255.0f, v[A] / 255.0f);
        }
        if (g_vertexLayout[GR_PARAM_XY] != 0xFF)
        {
            GLfloat *v = (GLfloat*)((char*)p + g_vertexLayout[GR_PARAM_XY]);
            gto_glVertex4f(v[0], v[1], 0.0f, 1.0f);
        }
    }

    gto_glEnd();
}
//------------------------------------------------------------------------------
static void FX_CALL gto_grVertexLayout(FxU32 param, FxI32 offset, FxU32 mode)
{
    g_vertexLayout[param] = (mode == GR_PARAM_ENABLE) ? offset : 0xFF;
}
//------------------------------------------------------------------------------
static void FX_CALL gto_grBufferClear(GrColor_t color, GrAlpha_t alpha, FxU32 depth)
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
static void FX_CALL gto_grBufferSwap(FxU32 swap_interval)
{
#if defined(xxMACOS)
    [[NSOpenGLContext currentContext] flushBuffer];
#elif defined(xxWINDOWS)
    SwapBuffers(wglGetCurrentDC());
#endif
}
//------------------------------------------------------------------------------
static GrContext_t FX_CALL gto_grSstWinOpen(void *hWnd, GrScreenResolution_t screen_resolution, GrScreenRefresh_t refresh_rate, GrColorFormat_t color_format, GrOriginLocation_t origin_location, int nColBuffers, int nAuxBuffers)
{
    switch (color_format)
    {
    case GR_COLORFORMAT_ARGB:
        grDrawTriangleEntry = gto_grDrawTriangle<2, 1, 0, 3>;
        break;
    case GR_COLORFORMAT_ABGR:
        grDrawTriangleEntry = gto_grDrawTriangle<0, 1, 2, 3>;
        break;
    }
#if defined(xxMACOS)
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
#elif defined(xxWINDOWS)
    HDC hDC = GetDC((HWND)hWnd);
    SetPixelFormat(hDC, ChoosePixelFormat(hDC, &g_desc), &g_desc);
    HGLRC hGLRC = wglCreateContext(hDC);
    wglShareLists(g_instance, hGLRC);
    for (int i = 1; i < 256; ++i)
    {
        if (g_openGLContext[i] == nullptr)
        {
            g_openGLContext[i] = hGLRC;
            g_openGLDC[i] = hDC;
            g_openGLHWND[i] = (HWND)hWnd;
            return static_cast<GrContext_t>(i);
        }
    }
#endif
    return 0;
}
//------------------------------------------------------------------------------
static FxBool FX_CALL gto_grSstWinClose(GrContext_t context)
{
    if (context == 0)
        return FXFALSE;
#if defined(xxMACOS)
    NSOpenGLContext* nsContext = g_openGLContext[context % 256];
    [nsContext makeCurrentContext];
    [nsContext clearDrawable];
    [NSOpenGLContext clearCurrentContext];
    g_openGLContext[context % 256] = nil;
#elif defined(xxWINDOWS)
    HGLRC hGLRC = g_openGLContext[context % 256];
    HDC hDC = g_openGLDC[context % 256];
    HWND hWnd = g_openGLHWND[context % 256];
    wglMakeCurrent(hDC, nullptr);
    wglDeleteContext(hGLRC);
    ReleaseDC(hWnd, hDC);
    g_openGLContext[context % 256] = nullptr;
    g_openGLDC[context % 256] = nullptr;
    g_openGLHWND[context % 256] = nullptr;
#endif
    return FXTRUE;
}
//------------------------------------------------------------------------------
static FxBool FX_CALL gto_grSelectContext(GrContext_t context)
{
    if (context == 0)
        return FXFALSE;
#if defined(xxMACOS)
    NSOpenGLContext* nsContext = g_openGLContext[context % 256];
    [nsContext makeCurrentContext];
    NSView* nsView = [nsContext view];
    NSRect frame = [nsView convertRectToBacking:[nsView frame]];
    g_width = frame.size.width;
    g_height = frame.size.height;
#elif defined(xxWINDOWS)
    HGLRC hGLRC = g_openGLContext[context % 256];
    HDC hDC = g_openGLDC[context % 256];
    HWND hWnd = g_openGLHWND[context % 256];
    wglMakeCurrent(hDC, hGLRC);
    RECT rect = {};
    GetClientRect(hWnd, &rect);
    g_width = rect.right - rect.left;
    g_height = rect.bottom - rect.top;
#endif
    return FXTRUE;
}
//------------------------------------------------------------------------------
static void FX_CALL gto_grAlphaBlendFunction(GrAlphaBlendFnc_t rgb_sf, GrAlphaBlendFnc_t rgb_df, GrAlphaBlendFnc_t alpha_sf, GrAlphaBlendFnc_t alpha_df)
{
    static const GLenum glAlphaBlend[8] =
    {
        GL_ZERO,
        GL_SRC_ALPHA,
        GL_SRC_COLOR,
        GL_DST_ALPHA,
        GL_ONE,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_ONE_MINUS_SRC_COLOR,
        GL_ONE_MINUS_DST_ALPHA,
    };
    gto_glEnable(GL_BLEND);
    if (gto_glBlendFuncSeparate)
        gto_glBlendFuncSeparate(glAlphaBlend[rgb_sf], glAlphaBlend[rgb_df], glAlphaBlend[alpha_sf], glAlphaBlend[alpha_df]);
    else
        gto_glBlendFunc(glAlphaBlend[rgb_sf], glAlphaBlend[rgb_df]);
}
//------------------------------------------------------------------------------
static void FX_CALL gto_grClipWindow(FxU32 minx, FxU32 miny, FxU32 maxx, FxU32 maxy)
{
    gto_glEnable(GL_SCISSOR_TEST);
    gto_glScissor(minx, g_height - maxy, maxx - minx, maxy - miny);
}
//------------------------------------------------------------------------------
static const char * FX_CALL gto_grGetString(FxU32 pname)
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
static void FX_CALL gto_grDepthRange(FxFloat n, FxFloat f)
{
    gto_glDepthRange(n, f);
}
//------------------------------------------------------------------------------
static void FX_CALL gto_grViewport(FxI32 x, FxI32 y, FxI32 width, FxI32 height)
{
    gto_glMatrixMode(GL_PROJECTION);
    gto_glLoadIdentity();
    gto_glOrtho(0, width, 0, height, -1, 1);
    gto_glViewport(x, y, width, height);
}
//------------------------------------------------------------------------------
static FxU32 FX_CALL gto_grTexCalcMemRequired(GrLOD_t lodmin, GrLOD_t lodmax, GrAspectRatio_t aspect, GrTextureFormat_t fmt)
{
    GLuint texture = 0;
    gto_glGenTextures(1, &texture);
    return texture - g_baseTexture;
}
//------------------------------------------------------------------------------
static FxU32 FX_CALL gto_grTexMinAddress(GrChipID_t tmu)
{
    return g_baseTexture;
}
//------------------------------------------------------------------------------
static void FX_CALL gto_grTexSource(GrChipID_t tmu, FxU32 startAddress, FxU32 evenOdd, GrTexInfo *info)
{
    GLuint texture = startAddress;
    gto_glEnable(GL_TEXTURE_2D);
    gto_glBindTexture(GL_TEXTURE_2D, texture);
}
//------------------------------------------------------------------------------
static void FX_CALL gto_grTexDownloadMipMap(GrChipID_t tmu, FxU32 startAddress, FxU32 evenOdd, GrTexInfo *info)
{
    GLuint texture = startAddress;
    gto_glBindTexture(GL_TEXTURE_2D, texture);
    gto_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gto_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gto_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gto_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLsizei width = 1;
    GLsizei height = 1;
    if (info->aspectRatioLog2 < 0)
    {
        width = (1 << (info->largeLodLog2 + info->aspectRatioLog2));
        height = (1 << info->largeLodLog2);
    }
    else
    {
        width = (1 << info->largeLodLog2);
        height = (1 << (info->largeLodLog2 - info->aspectRatioLog2));
    }
    gto_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, info->data);
    gto_glBindTexture(GL_TEXTURE_2D, 0);
}
//------------------------------------------------------------------------------
static void FX_CALL gto_grGlideInit(void)
{
    if (g_glLibrary == nullptr)
    {
#if defined(xxMACOS)
        g_glLibrary = xxLoadLibrary("/System/Library/Frameworks/OpenGL.framework/OpenGL");
#elif defined(xxWINDOWS)
        g_glLibrary = xxLoadLibrary("opengl32.dll");
#endif
    }
    (void*&)gto_glEnable = xxGetProcAddress(g_glLibrary, "glEnable");
    (void*&)gto_glBlendFunc = xxGetProcAddress(g_glLibrary, "glBlendFunc");
    (void*&)gto_glBlendFuncSeparate = xxGetProcAddress(g_glLibrary, "glBlendFuncSeparate");
    (void*&)gto_glBegin = xxGetProcAddress(g_glLibrary, "glBegin");
    (void*&)gto_glVertex4f = xxGetProcAddress(g_glLibrary, "glVertex4f");
    (void*&)gto_glColor4f = xxGetProcAddress(g_glLibrary, "glColor4f");
    (void*&)gto_glTexCoord4f = xxGetProcAddress(g_glLibrary, "glTexCoord4f");
    (void*&)gto_glEnd = xxGetProcAddress(g_glLibrary, "glEnd");
    (void*&)gto_glGenTextures = xxGetProcAddress(g_glLibrary, "glGenTextures");
    (void*&)gto_glBindTexture = xxGetProcAddress(g_glLibrary, "glBindTexture");
    (void*&)gto_glDeleteTextures = xxGetProcAddress(g_glLibrary, "glDeleteTextures");
    (void*&)gto_glTexParameteri = xxGetProcAddress(g_glLibrary, "glTexParameteri");
    (void*&)gto_glTexImage2D = xxGetProcAddress(g_glLibrary, "glTexImage2D");
    (void*&)gto_glMatrixMode = xxGetProcAddress(g_glLibrary, "glMatrixMode");
    (void*&)gto_glLoadIdentity = xxGetProcAddress(g_glLibrary, "glLoadIdentity");
    (void*&)gto_glOrtho = xxGetProcAddress(g_glLibrary, "glOrtho");
    (void*&)gto_glDepthRange = xxGetProcAddress(g_glLibrary, "glDepthRange");
    (void*&)gto_glViewport = xxGetProcAddress(g_glLibrary, "glViewport");
    (void*&)gto_glScissor = xxGetProcAddress(g_glLibrary, "glScissor");
    (void*&)gto_glClearColor = xxGetProcAddress(g_glLibrary, "glClearColor");
    (void*&)gto_glClearDepth = xxGetProcAddress(g_glLibrary, "glClearDepth");
    (void*&)gto_glClear = xxGetProcAddress(g_glLibrary, "glClear");
#if defined(xxMACOS)
    NSOpenGLPixelFormatAttribute attributes[2] = { NSOpenGLPFADoubleBuffer };
    g_rootView = [[NSOpenGLView alloc] initWithFrame:CGRectMake(0, 0, 1, 1)
                                         pixelFormat:[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes]];
    [[g_rootView openGLContext] makeCurrentContext];
#elif defined(xxWINDOWS)
    (void*&)wglCreateContext = xxGetProcAddress(g_glLibrary, "wglCreateContext");
    (void*&)wglDeleteContext = xxGetProcAddress(g_glLibrary, "wglDeleteContext");
    (void*&)wglGetCurrentDC = xxGetProcAddress(g_glLibrary, "wglGetCurrentDC");
    (void*&)wglMakeCurrent = xxGetProcAddress(g_glLibrary, "wglMakeCurrent");
    (void*&)wglShareLists = xxGetProcAddress(g_glLibrary, "wglShareLists");
    if (g_gdiLibrary == nullptr)
    {
        g_gdiLibrary = xxLoadLibrary("gdi32.dll");
    }
    (void*&)ChoosePixelFormat = xxGetProcAddress(g_gdiLibrary, "ChoosePixelFormat");
    (void*&)SetPixelFormat = xxGetProcAddress(g_gdiLibrary, "SetPixelFormat");
    (void*&)SwapBuffers = xxGetProcAddress(g_gdiLibrary, "SwapBuffers");
    WNDCLASSEXA wc = { sizeof(WNDCLASSEXA), CS_OWNDC, ::DefWindowProcA, 0, 0, ::GetModuleHandleA(nullptr), nullptr, nullptr, nullptr, nullptr, "Glide", nullptr };
    ::RegisterClassExA(&wc);
    g_dummyWindow = ::CreateWindowA(wc.lpszClassName, "Glide", WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
    HDC hDC = GetDC(g_dummyWindow);
    g_desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    g_desc.nVersion = 1;
    g_desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    g_desc.iPixelType = PFD_TYPE_RGBA;
    g_desc.cColorBits = 32;
    g_desc.cDepthBits = 24;
    g_desc.cStencilBits = 8;
    g_desc.iLayerType = PFD_MAIN_PLANE;
    SetPixelFormat(hDC, ChoosePixelFormat(hDC, &g_desc), &g_desc);
    g_instance = wglCreateContext(hDC);
    wglMakeCurrent(hDC, g_instance);
    ReleaseDC(g_dummyWindow, hDC);
#endif
    gto_glGenTextures(1, &g_baseTexture);
    memset(g_vertexLayout, 0xFF, sizeof(g_vertexLayout));
}
//------------------------------------------------------------------------------
static void FX_CALL gto_grGlideShutdown(void)
{
    if (g_baseTexture)
    {
        gto_glDeleteTextures(1, &g_baseTexture);
        g_baseTexture = 0;
    }
#if defined(xxMACOS)
    g_rootView = nil;
#elif defined(xxWINDOWS)
    if (g_instance)
    {
        wglDeleteContext(g_instance);
        g_instance = nullptr;
    }
    if (g_gdiLibrary)
    {
        xxFreeLibrary(g_gdiLibrary);
        g_gdiLibrary = nullptr;
    }
    if (g_dummyWindow)
    {
        CloseWindow(g_dummyWindow);
        g_dummyWindow = nullptr;
    }
#endif
    if (g_glLibrary)
    {
        xxFreeLibrary(g_glLibrary);
        g_glLibrary = nullptr;
    }
    gto_glEnable = nullptr;
    gto_glBlendFuncSeparate = nullptr;
    gto_glBegin = nullptr;
    gto_glVertex4f = nullptr;
    gto_glColor4f = nullptr;
    gto_glTexCoord4f = nullptr;
    gto_glEnd = nullptr;
    gto_glGenTextures = nullptr;
    gto_glBindTexture = nullptr;
    gto_glDeleteTextures = nullptr;
    gto_glTexParameteri = nullptr;
    gto_glTexImage2D = nullptr;
    gto_glMatrixMode = nullptr;
    gto_glLoadIdentity = nullptr;
    gto_glOrtho = nullptr;
    gto_glDepthRange = nullptr;
    gto_glViewport = nullptr;
    gto_glScissor = nullptr;
    gto_glClearColor = nullptr;
    gto_glClearDepth = nullptr;
    gto_glClear = nullptr;
}
//------------------------------------------------------------------------------
GrProc FX_CALL gto_grGetProcAddress(char* name)
{
    switch (xxHash(name))
    {
    case xxHash("grDrawTriangle"):          return (GrProc)gto_grDrawTriangle<0, 1, 2, 3>;
    case xxHash("grVertexLayout"):          return (GrProc)gto_grVertexLayout;
    case xxHash("grBufferClear"):           return (GrProc)gto_grBufferClear;
    case xxHash("grBufferSwap"):            return (GrProc)gto_grBufferSwap;
    case xxHash("grSstWinOpen"):            return (GrProc)gto_grSstWinOpen;
    case xxHash("grSstWinClose"):           return (GrProc)gto_grSstWinClose;
    case xxHash("grSelectContext"):         return (GrProc)gto_grSelectContext;
    case xxHash("grAlphaBlendFunction"):    return (GrProc)gto_grAlphaBlendFunction;
    case xxHash("grClipWindow"):            return (GrProc)gto_grClipWindow;
    case xxHash("grGetString"):             return (GrProc)gto_grGetString;
    case xxHash("grDepthRange"):            return (GrProc)gto_grDepthRange;
    case xxHash("grViewport"):              return (GrProc)gto_grViewport;
    case xxHash("grTexCalcMemRequired"):    return (GrProc)gto_grTexCalcMemRequired;
    case xxHash("grTexMinAddress"):         return (GrProc)gto_grTexMinAddress;
    case xxHash("grTexSource"):             return (GrProc)gto_grTexSource;
    case xxHash("grTexDownloadMipMap"):     return (GrProc)gto_grTexDownloadMipMap;
    case xxHash("grGlideInit"):             return (GrProc)gto_grGlideInit;
    case xxHash("grGlideShutdown"):         return (GrProc)gto_grGlideShutdown;
    }
    return nullptr;
}
#endif
//==============================================================================
