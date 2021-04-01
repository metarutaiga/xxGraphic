//==============================================================================
// xxGraphic : Glide Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalGlide.h"
#include "xxVector.h"
#include "xxGraphicGlide.h"

static void*    g_glideLibrary = nullptr;
static FxU32    g_startAddress = 0;
static uint64_t g_vertexBuffer = 0;
static uint64_t g_vertexAttribute = 0;
static v4sf     g_worldMatrix[4];
static v4sf     g_viewMatrix[4];
static v4sf     g_projectionMatrix[4];
static v4sf     g_worldViewProjectionScreenMatrix[4];

//==============================================================================
//  Instance
//==============================================================================
GrProc FX_CALL grGetProcAddress(char* name)
{
#if defined(xxMACOS)
    if (g_glideLibrary == nullptr)
        g_glideLibrary = xxLoadLibrary("glide3x.dylib");
    if (g_glideLibrary == nullptr)
        return gto_grGetProcAddress(name);
#elif defined(xxWINDOWS)
    if (g_glideLibrary == nullptr)
        g_glideLibrary = xxLoadLibrary("glide3x.dll");
    if (g_glideLibrary == nullptr)
        return nullptr;
#if defined(_M_IX86)
    for (int i = 0; i <= 40; i += 4)
    {
        char temp[64];
        snprintf(temp, 64, "_%s@%d", name, i);
        GrProc proc = (GrProc)xxGetProcAddress(g_glideLibrary, temp);
        if (proc == nullptr)
            continue;
        return proc;
    }
#endif
#endif
    return (GrProc)xxGetProcAddress(g_glideLibrary, name);
}
//------------------------------------------------------------------------------
uint64_t xxCreateInstanceGlide()
{
    grGlideInit();

    grVertexLayout(GR_PARAM_XY, xxOffsetOf(GrVertex, x), GR_PARAM_ENABLE);
    grVertexLayout(GR_PARAM_Z, xxOffsetOf(GrVertex, ooz), GR_PARAM_ENABLE);
    grVertexLayout(GR_PARAM_W, xxOffsetOf(GrVertex, oow), GR_PARAM_ENABLE);
    grVertexLayout(GR_PARAM_PARGB, xxOffsetOf(GrVertex, rgba), GR_PARAM_ENABLE);
    grVertexLayout(GR_PARAM_ST0, xxOffsetOf(GrVertex, sow), GR_PARAM_ENABLE);

    g_startAddress = grTexMinAddress(GR_TMU0);
    g_vertexBuffer = 0;
    g_vertexAttribute = 0;

    xxRegisterFunction(Glide);

    return reinterpret_cast<uint64_t>(grGetString(GR_VENDOR));
}
//------------------------------------------------------------------------------
void xxDestroyInstanceGlide(uint64_t instance)
{
    grGlideShutdown();

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceGlide(uint64_t instance)
{
    return reinterpret_cast<uint64_t>(grGetString(GR_HARDWARE));
}
//------------------------------------------------------------------------------
void xxDestroyDeviceGlide(uint64_t device)
{

}
//------------------------------------------------------------------------------
bool xxResetDeviceGlide(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
bool xxTestDeviceGlide(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameGlide()
{
    return "Glide";
}
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t xxCreateSwapchainGlide(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain)
{
    xxDestroySwapchainGlide(oldSwapchain);

    GrContext grContext = {};
    grContext.context = grSstWinOpen(view, width | (height << 16), GR_REFRESH_NONE, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, 2, 0);
#if defined(xxMACOS)
    grContext.width = width * 2.0f;
    grContext.height = height * 2.0f;
#else
    grContext.width = width;
    grContext.height = height;
#endif

    return static_cast<uint64_t>(grContext.value);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainGlide(uint64_t swapchain)
{
    GrContext grContext = { swapchain };

    grSstWinClose(grContext.context);
}
//------------------------------------------------------------------------------
void xxPresentSwapchainGlide(uint64_t swapchain)
{
    grBufferSwap(0);
}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferGlide(uint64_t device, uint64_t swapchain)
{
    GrContext grContext = { swapchain };

    grSelectContext(grContext.context);

    return static_cast<uint64_t>(grContext.value);
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferGlide(uint64_t device, uint64_t swapchain, float* scale)
{
    GrContext grContext = { swapchain };

    if (scale)
    {
#if defined(xxMACOS)
        (*scale) = 2.0f;
#else
        (*scale) = 1.0f;
#endif
    }

    return static_cast<uint64_t>(grContext.value);
}
//==============================================================================
//  Command Buffer
//==============================================================================
bool xxBeginCommandBufferGlide(uint64_t commandBuffer)
{
    return true;
}
//------------------------------------------------------------------------------
void xxEndCommandBufferGlide(uint64_t commandBuffer)
{

}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferGlide(uint64_t commandBuffer, uint64_t swapchain)
{
}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxCreateRenderPassGlide(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
{
    uint64_t renderPass = 0;

    if (clearColor)
        renderPass |= GR_PARAM_RGB | GR_PARAM_A;
    if (clearDepth)
        renderPass |= GR_PARAM_Z;

    return renderPass;
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassGlide(uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassGlide(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    GrColor_t grColor = 0;
    GrAlpha_t grAlpha = 0;

    grColor |= (int)(color[0] * 255);
    grColor |= (int)(color[1] * 255) << 8;
    grColor |= (int)(color[2] * 255) << 16;
    grAlpha |= (int)(color[3] * 255);

    grColorMask(renderPass & GR_PARAM_RGB ? FXTRUE : FXFALSE, renderPass & GR_PARAM_A ? FXTRUE : FXFALSE);
    grDepthMask(renderPass & GR_PARAM_Z ? FXTRUE : FXFALSE);
    grBufferClear(grColor, grAlpha, -1);
    grColorMask(FXTRUE, FXTRUE);
    grDepthMask(FXTRUE);

    return commandBuffer;
}
//------------------------------------------------------------------------------
void xxEndRenderPassGlide(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{

}
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t xxCreateVertexAttributeGlide(uint64_t device, int count, int* attribute)
{
    GrVertexAttribute grVertexAttribute = {};
    int stride = 0;

    for (int i = 0; i < count; ++i)
    {
        int stream = (*attribute++);
        int offset = (*attribute++);
        int element = (*attribute++);
        int size = (*attribute++);

        stride += size;

        if (offset == 0 && element == 3 && size == sizeof(float) * 3)
            grVertexAttribute.flags |= GR_PARAM_XY | GR_PARAM_Z;
        if (offset != 0 && element == 4 && size == sizeof(char) * 4)
            grVertexAttribute.flags |= GR_PARAM_PARGB;
        if (offset != 0 && element == 2 && size == sizeof(float) * 2)
            grVertexAttribute.flags += GR_PARAM_ST0;
    }

    grVertexAttribute.stride = stride;

    return static_cast<uint64_t>(grVertexAttribute.value);
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeGlide(uint64_t vertexAttribute)
{

}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateConstantBufferGlide(uint64_t device, int size)
{
    char* grBuffer = xxAlloc(char, size);

    return reinterpret_cast<uint64_t>(grBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferGlide(uint64_t device, int size)
{
    char* grBuffer = xxAlloc(char, size);

    return reinterpret_cast<uint64_t>(grBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferGlide(uint64_t device, int size, uint64_t vertexAttribute)
{
    char* grBuffer = xxAlloc(char, size);

    return reinterpret_cast<uint64_t>(grBuffer);
}
//------------------------------------------------------------------------------
void xxDestroyBufferGlide(uint64_t device, uint64_t buffer)
{
    char* grBuffer = reinterpret_cast<char*>(buffer);

    xxFree(grBuffer);
}
//------------------------------------------------------------------------------
void* xxMapBufferGlide(uint64_t device, uint64_t buffer)
{
    char* grBuffer = reinterpret_cast<char*>(buffer);

    return grBuffer;
}
//------------------------------------------------------------------------------
void xxUnmapBufferGlide(uint64_t device, uint64_t buffer)
{

}
//==============================================================================
//  Texture
//==============================================================================
uint64_t xxCreateTextureGlide(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, const void* external)
{
    GrTexture* info = xxAlloc(GrTexture);
    if (info == nullptr)
        return 0;

    int widthLog2 = 31 - xxCountLeadingZeros(width);
    int heightLog2 = 31 - xxCountLeadingZeros(height);
    int largeLodLog2 = widthLog2 > heightLog2 ? widthLog2 : heightLog2;
    GrAspectRatio_t aspectRatioLog2 = widthLog2 - heightLog2;

    info->smallLodLog2 = largeLodLog2 >> (mipmap - 1);
    info->largeLodLog2 = largeLodLog2;
    info->aspectRatioLog2 = aspectRatioLog2;
    info->format = GR_TEXFMT_ARGB_8888;
    info->data = xxAlloc(uint32_t, width * height);
    info->startAddress = g_startAddress;
    info->width = width;
    info->height = height;
    info->stride = width * sizeof(uint32_t);
    g_startAddress += grTexCalcMemRequired(info->smallLodLog2, info->largeLodLog2, info->aspectRatioLog2, info->format);

    return reinterpret_cast<uint64_t>(info);
}
//------------------------------------------------------------------------------
void xxDestroyTextureGlide(uint64_t texture)
{
    GrTexture* info = reinterpret_cast<GrTexture*>(texture);

    xxFree(info);
}
//------------------------------------------------------------------------------
void* xxMapTextureGlide(uint64_t device, uint64_t texture, int* stride, int level, int array)
{
    GrTexture* info = reinterpret_cast<GrTexture*>(texture);
    if (info == nullptr)
        return nullptr;

    if (stride)
    {
        (*stride) = info->stride;
    }

    return info->data;
}
//------------------------------------------------------------------------------
void xxUnmapTextureGlide(uint64_t device, uint64_t texture, int level, int array)
{
    GrTexture* info = reinterpret_cast<GrTexture*>(texture);
    if (info == nullptr)
        return;

    grTexDownloadMipMap(GR_TMU0, info->startAddress, GR_MIPMAPLEVELMASK_BOTH, info);
}
//==============================================================================
//  Sampler
//==============================================================================
uint64_t xxCreateSamplerGlide(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    GrSampler grSampler = {};

    grSampler.addressU = clampU ? GR_TEXTURECLAMP_CLAMP : GR_TEXTURECLAMP_WRAP;
    grSampler.addressV = clampV ? GR_TEXTURECLAMP_CLAMP : GR_TEXTURECLAMP_WRAP;
    grSampler.magFilter = linearMag ? GR_TEXTUREFILTER_BILINEAR : GR_TEXTUREFILTER_POINT_SAMPLED;
    grSampler.minFilter = linearMin ? GR_TEXTUREFILTER_BILINEAR : GR_TEXTUREFILTER_POINT_SAMPLED;
    grSampler.mipFilter = linearMip ? GR_MIPMAP_NEAREST_DITHER : GR_MIPMAP_NEAREST;

    return static_cast<uint64_t>(grSampler.value);
}
//------------------------------------------------------------------------------
void xxDestroySamplerGlide(uint64_t sampler)
{

}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderGlide(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderGlide(uint64_t device, const char* shader)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderGlide(uint64_t device, uint64_t shader)
{

}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreateBlendStateGlide(uint64_t device, bool blending)
{
    GrPipeline grPipeline = {};
    grPipeline.alphaBlending = blending;
    return static_cast<uint64_t>(grPipeline.value);
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateGlide(uint64_t device, bool depthTest, bool depthWrite)
{
    GrPipeline grPipeline = {};
    grPipeline.depthTest = depthTest;
    grPipeline.depthWrite = depthWrite;
    return static_cast<uint64_t>(grPipeline.value);
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateGlide(uint64_t device, bool cull, bool scissor)
{
    GrPipeline grPipeline = {};
    grPipeline.cull = cull;
    grPipeline.scissor = scissor;
    return static_cast<uint64_t>(grPipeline.value);
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineGlide(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    GrPipeline grBlendState = { blendState };
    GrPipeline grDepthStencilState = { depthStencilState };
    GrPipeline grRasterizerState = { rasterizerState };

    GrPipeline grPipeline = {};
    grPipeline.alphaBlending = grBlendState.alphaBlending;
    grPipeline.depthTest = grDepthStencilState.depthTest;
    grPipeline.depthWrite = grDepthStencilState.depthWrite;
    grPipeline.cull = grRasterizerState.cull;
    grPipeline.scissor = grRasterizerState.scissor;

    return static_cast<uint64_t>(grPipeline.value);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateGlide(uint64_t blendState)
{

}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateGlide(uint64_t depthStencilState)
{

}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateGlide(uint64_t rasterizerState)
{

}
//------------------------------------------------------------------------------
void xxDestroyPipelineGlide(uint64_t pipeline)
{

}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportGlide(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{
    grViewport(x, y, width, height);
    grDepthRange(minZ, maxZ);
}
//------------------------------------------------------------------------------
void xxSetScissorGlide(uint64_t commandEncoder, int x, int y, int width, int height)
{
    grClipWindow(x, y, x + width, y + height);
}
//------------------------------------------------------------------------------
void xxSetPipelineGlide(uint64_t commandEncoder, uint64_t pipeline)
{
    GrPipeline grPipeline = { pipeline };

    if (grPipeline.alphaBlending)
    {
        grAlphaBlendFunction(GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA, GR_BLEND_ONE, GR_BLEND_ZERO);
    }
    else
    {
        grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ZERO, GR_BLEND_ONE, GR_BLEND_ZERO);
    }
    grDepthBufferFunction(grPipeline.depthTest ? GR_CMP_LESS : GR_CMP_ALWAYS);
    grDepthMask(grPipeline.depthWrite ? FXTRUE : FXFALSE);
    grCullMode(grPipeline.cull ? GR_CULL_NEGATIVE : GR_CULL_DISABLE);
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersGlide(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    g_vertexBuffer = buffers[0];
    g_vertexAttribute = vertexAttribute;
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesGlide(uint64_t commandEncoder, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesGlide(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    for (int i = 0; i < count; ++i)
    {
        GrTexture* info = reinterpret_cast<GrTexture*>(textures[i]);
        grTexSource(GR_TMU0 + i, info->startAddress, GR_MIPMAPLEVELMASK_BOTH, info);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersGlide(uint64_t commandEncoder, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersGlide(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    for (int i = 0; i < count; ++i)
    {
        GrSampler grSampler = { samplers[i] };
        grTexClampMode(GR_TMU0 + i, grSampler.addressU, grSampler.addressV);
        grTexFilterMode(GR_TMU0 + i, grSampler.minFilter, grSampler.magFilter);
        grTexMipMapMode(GR_TMU0 + i, grSampler.mipFilter, grSampler.mipFilter == GR_MIPMAP_NEAREST_DITHER ? FXTRUE : FXFALSE);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferGlide(uint64_t commandEncoder, uint64_t buffer, int size)
{
    GrContext grContext = { commandEncoder };
    v4sf* grBuffer = reinterpret_cast<v4sf*>(buffer);

    if (size >= sizeof(g_worldMatrix))
    {
        size -= sizeof(g_worldMatrix);
        g_worldMatrix[0] = (*grBuffer++);
        g_worldMatrix[1] = (*grBuffer++);
        g_worldMatrix[2] = (*grBuffer++);
        g_worldMatrix[3] = (*grBuffer++);
    }
    if (size >= sizeof(g_viewMatrix))
    {
        size -= sizeof(g_viewMatrix);
        g_viewMatrix[0] = (*grBuffer++);
        g_viewMatrix[1] = (*grBuffer++);
        g_viewMatrix[2] = (*grBuffer++);
        g_viewMatrix[3] = (*grBuffer++);
    }
    if (size >= sizeof(g_projectionMatrix))
    {
        size -= sizeof(g_projectionMatrix);
        g_projectionMatrix[0] = (*grBuffer++);
        g_projectionMatrix[1] = (*grBuffer++);
        g_projectionMatrix[2] = (*grBuffer++);
        g_projectionMatrix[3] = (*grBuffer++);
    }

    v4sf screenMatrix[4] =
    {
        { grContext.width * 0.5f,                    0.0f, 0.0f, 0.0f },
        {                   0.0f, grContext.height * 0.5f, 0.0f, 0.0f },
        {                   0.0f,                    0.0f, 1.0f, 0.0f },
        { grContext.width * 0.5f, grContext.height * 0.5f, 0.0f, 1.0f },
    };
    g_worldViewProjectionScreenMatrix[0] = __builtin_multiplyvector(screenMatrix, __builtin_multiplyvector(g_projectionMatrix, __builtin_multiplyvector(g_viewMatrix, g_worldMatrix[0])));
    g_worldViewProjectionScreenMatrix[1] = __builtin_multiplyvector(screenMatrix, __builtin_multiplyvector(g_projectionMatrix, __builtin_multiplyvector(g_viewMatrix, g_worldMatrix[1])));
    g_worldViewProjectionScreenMatrix[2] = __builtin_multiplyvector(screenMatrix, __builtin_multiplyvector(g_projectionMatrix, __builtin_multiplyvector(g_viewMatrix, g_worldMatrix[2])));
    g_worldViewProjectionScreenMatrix[3] = __builtin_multiplyvector(screenMatrix, __builtin_multiplyvector(g_projectionMatrix, __builtin_multiplyvector(g_viewMatrix, g_worldMatrix[3])));
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferGlide(uint64_t commandEncoder, uint64_t buffer, int size)
{

}
//------------------------------------------------------------------------------
void xxDrawIndexedGlide(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    GrVertexAttribute grVertexAttribute = { g_vertexAttribute };
    uint16_t* grIndexBuffer = reinterpret_cast<uint16_t*>(indexBuffer) + firstIndex;
    for (int i = 0; i < indexCount; i += 3)
    {
        float* v0 = reinterpret_cast<float*>(g_vertexBuffer + (vertexOffset + grIndexBuffer[i + 0]) * grVertexAttribute.stride);
        float* v1 = reinterpret_cast<float*>(g_vertexBuffer + (vertexOffset + grIndexBuffer[i + 1]) * grVertexAttribute.stride);
        float* v2 = reinterpret_cast<float*>(g_vertexBuffer + (vertexOffset + grIndexBuffer[i + 2]) * grVertexAttribute.stride);
        GrVertex t0;
        GrVertex t1;
        GrVertex t2;

        if (grVertexAttribute.flags & GR_PARAM_XY)
        {
            v4sf p0 = __builtin_multiplyvector(g_worldViewProjectionScreenMatrix, v4sf{ v0[0], v0[1], v0[2], 1.0f });
            v4sf p1 = __builtin_multiplyvector(g_worldViewProjectionScreenMatrix, v4sf{ v1[0], v1[1], v1[2], 1.0f });
            v4sf p2 = __builtin_multiplyvector(g_worldViewProjectionScreenMatrix, v4sf{ v2[0], v2[1], v2[2], 1.0f });

            t0.x = p0[0];
            t1.x = p1[0];
            t2.x = p2[0];

            t0.y = p0[1];
            t1.y = p1[1];
            t2.y = p2[1];

            t0.ooz = 65535.0f / p0[2];
            t1.ooz = 65535.0f / p1[2];
            t2.ooz = 65535.0f / p2[2];

            t0.oow = 1.0f / p0[3];
            t1.oow = 1.0f / p1[3];
            t2.oow = 1.0f / p2[3];

            v0 += 3;
            v1 += 3;
            v2 += 3;
        }
        if (grVertexAttribute.flags & GR_PARAM_PARGB)
        {
            t0.rgba = (int&)v0[0];
            t1.rgba = (int&)v1[0];
            t2.rgba = (int&)v2[0];

            v0++;
            v1++;
            v2++;
        }
        if (grVertexAttribute.flags & GR_PARAM_ST0)
        {
            t0.sow = v0[0]; t0.tow = v0[1];
            t1.sow = v1[0]; t1.tow = v1[1];
            t2.sow = v2[0]; t2.tow = v2[1];

            v0 += 2;
            v1 += 2;
            v2 += 2;
        }

        grDrawTriangle(&t0, &t1, &t2);
    }
}
//==============================================================================
