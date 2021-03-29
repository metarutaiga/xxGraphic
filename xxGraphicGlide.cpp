//==============================================================================
// xxGraphic : Glide Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalGlide.h"
#include "xxGraphicGlide.h"

static void*    g_glideLibrary = nullptr;
static FxU32    g_startAddress = 0;
static uint64_t g_vertexBuffer = 0;
static uint64_t g_vertexAttribute = 0;
static float    g_worldMatrix[4 * 4];
static float    g_viewMatrix[4 * 4];
static float    g_projectionMatrix[4 * 4];

//==============================================================================
//  Instance
//==============================================================================
GrProc FX_CALL grGetProcAddress(char* name)
{
#if defined(xxWINDOWS)
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
    return (GrProc)xxGetProcAddress(g_glideLibrary, name);
#endif
    return nullptr;
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
    grContext.context = grSstWinOpen(view, width | (height << 16), GR_REFRESH_NONE, GR_COLORFORMAT_ABGR, GR_ORIGIN_LOWER_LEFT, 2, 0);
    grContext.width = width;
    grContext.height = height;

    return grContext.value;
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

    return grContext.value;
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferGlide(uint64_t device, uint64_t swapchain, float* scale)
{
    GrContext grContext = { swapchain };

    if (scale)
    {
        (*scale) = 1.0f;
    }

    return grContext.value;
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

    return grVertexAttribute.value;
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

    GrAspectRatio_t aspect = GR_ASPECT_LOG2_1x1;
    switch ((int)(8.0 * (float)width / (float)height))
    {
    case 1:
        aspect = GR_ASPECT_LOG2_1x8;
        break;
    case 2:
        aspect = GR_ASPECT_LOG2_1x4;
        break;
    case 4:
        aspect = GR_ASPECT_LOG2_1x2;
        break;
    case 8:
        aspect = GR_ASPECT_LOG2_1x1;
        break;
    case 16:
        aspect = GR_ASPECT_LOG2_2x1;
        break;
    case 32:
        aspect = GR_ASPECT_LOG2_4x1;
        break;
    case 64:
        aspect = GR_ASPECT_LOG2_8x1;
        break;
    }

    int small = (width < height) ? width : height;
    int large = (width > height) ? width : height;
    info->smallLodLog2 = 31 - xxCountLeadingZeros(small);
    info->largeLodLog2 = 31 - xxCountLeadingZeros(large);
    info->aspectRatioLog2 = aspect;
    info->format = GR_TEXFMT_ARGB_4444;
    info->data = xxAlloc(uint32_t, width * height);
    info->startAddress = g_startAddress;
    g_startAddress = grTexCalcMemRequired(info->smallLodLog2, info->largeLodLog2, info->aspectRatioLog2, info->format);

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

    return info->data;
}
//------------------------------------------------------------------------------
void xxUnmapTextureGlide(uint64_t device, uint64_t texture, int level, int array)
{
    GrTexture* info = reinterpret_cast<GrTexture*>(texture);
    if (info == nullptr)
        return;

    uint32_t* tex8888 = (uint32_t*)info->data;
    uint16_t* tex4444 = (uint16_t*)info->data;
    int count = (1 << info->smallLodLog2) * (1 << info->largeLodLog2);
    for (int i = 0; i < count; ++i)
    {
        uint32_t pixel8888 = tex8888[i];
        uint16_t pixel4444 = 0;
        pixel4444 |= (pixel8888 >>  4) & 0x000F;
        pixel4444 |= (pixel8888 >>  8) & 0x00F0;
        pixel4444 |= (pixel8888 >> 12) & 0x0F00;
        pixel4444 |= (pixel8888 >> 16) & 0xF000;
        tex4444[i] = pixel4444;
    }

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

    return grSampler.value;
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
        grTexDownloadMipMap(GR_TMU0, info->startAddress, GR_MIPMAPLEVELMASK_BOTH, info);
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
        grTexClampMode(GR_TMU0, grSampler.addressU, grSampler.addressV);
        grTexFilterMode(GR_TMU0, grSampler.minFilter, grSampler.magFilter);
        grTexMipMapMode(GR_TMU0, grSampler.mipFilter, grSampler.mipFilter == GR_MIPMAP_NEAREST_DITHER ? FXTRUE : FXFALSE);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferGlide(uint64_t commandEncoder, uint64_t buffer, int size)
{
    float* grBuffer = reinterpret_cast<float*>(buffer);

    if (size >= sizeof(float[4 * 4]) * 1)
        memcpy(g_worldMatrix, grBuffer + 4 * 4 * 0, sizeof(float[4 * 4]));
    if (size >= sizeof(float[4 * 4]) * 2)
        memcpy(g_viewMatrix, grBuffer + 4 * 4 * 1, sizeof(float[4 * 4]));
    if (size >= sizeof(float[4 * 4]) * 3)
        memcpy(g_projectionMatrix, grBuffer + 4 * 4 * 2, sizeof(float[4 * 4]));
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferGlide(uint64_t commandEncoder, uint64_t buffer, int size)
{

}
//------------------------------------------------------------------------------
void xxDrawIndexedGlide(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    GrContext grContext = { commandEncoder };
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
            auto* p = g_projectionMatrix;

            t0.x = v0[0] * p[0] + v0[0] * p[4] + v0[0] * p[8] + p[12];
            t1.x = v1[0] * p[0] + v1[0] * p[4] + v1[0] * p[8] + p[12];
            t2.x = v2[0] * p[0] + v2[0] * p[4] + v2[0] * p[8] + p[12];

            t0.y = v0[1] * p[1] + v0[1] * p[5] + v0[1] * p[9] + p[13];
            t1.y = v1[1] * p[1] + v1[1] * p[5] + v1[1] * p[9] + p[13];
            t2.y = v2[1] * p[1] + v2[1] * p[5] + v2[1] * p[9] + p[13];

            t0.ooz = v0[2] * p[2] + v0[2] * p[6] + v0[3] * p[10] + p[14];
            t1.ooz = v1[2] * p[2] + v1[2] * p[6] + v1[3] * p[10] + p[14];
            t2.ooz = v2[2] * p[2] + v2[2] * p[6] + v2[3] * p[10] + p[14];

            t0.oow = p[3] + p[7] + p[11] + p[15];
            t1.oow = p[3] + p[7] + p[11] + p[15];
            t2.oow = p[3] + p[7] + p[11] + p[15];

            t0.x = grContext.width * (t0.x * 0.5f + 0.5f);
            t1.x = grContext.width * (t1.x * 0.5f + 0.5f);
            t2.x = grContext.width * (t2.x * 0.5f + 0.5f);

            t0.y = grContext.height * (t0.y * 0.5f + 0.5f);
            t1.y = grContext.height * (t1.y * 0.5f + 0.5f);
            t2.y = grContext.height * (t2.y * 0.5f + 0.5f);

            t0.ooz = 65535.0f / t0.ooz;
            t1.ooz = 65535.0f / t1.ooz;
            t2.ooz = 65535.0f / t2.ooz;

            t0.oow = 1.0f / t0.oow;
            t1.oow = 1.0f / t1.oow;
            t2.oow = 1.0f / t2.oow;

            v0 += 3;
            v1 += 3;
            v2 += 3;
        }
        if (grVertexAttribute.flags & GR_PARAM_PARGB)
        {
            t0.rgba = (int&)v0[0];  v0++;
            t1.rgba = (int&)v1[0];  v1++;
            t2.rgba = (int&)v2[0];  v2++;
        }
        if (grVertexAttribute.flags & GR_PARAM_ST0)
        {
            t0.sow = v0[0]; t0.tow = v0[1]; v0 += 2;
            t1.sow = v1[0]; t1.tow = v1[1]; v1 += 2;
            t2.sow = v2[0]; t2.tow = v2[1]; v2 += 2;
        }

        grDrawTriangle(&t0, &t1, &t2);
    }
}
//==============================================================================
