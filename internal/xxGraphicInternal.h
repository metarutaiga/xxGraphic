//==============================================================================
// xxGraphic : Graphic Internal Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#define NUM_BACK_BUFFERS 3

enum xxGraphicDescriptor
{
    VERTEX_UNIFORM          = 0,
    FRAGMENT_UNIFORM        = 1,
    VERTEX_TEXTURE          = 2,
    FRAGMENT_TEXTURE        = 6,
    VERTEX_SAMPLER          = 14,
    FRAGMENT_SAMPLER        = 18,
    TOTAL                   = 26,

    VERTEX_UNIFORM_COUNT    = FRAGMENT_UNIFORM  - VERTEX_UNIFORM,
    FRAGMENT_UNIFORM_COUNT  = VERTEX_TEXTURE    - FRAGMENT_UNIFORM,
    VERTEX_TEXTURE_COUNT    = FRAGMENT_TEXTURE  - VERTEX_TEXTURE,
    FRAGMENT_TEXTURE_COUNT  = VERTEX_SAMPLER    - FRAGMENT_TEXTURE,
    VERTEX_SAMPLER_COUNT    = FRAGMENT_SAMPLER  - VERTEX_SAMPLER,
    FRAGMENT_SAMPLER_COUNT  = TOTAL             - FRAGMENT_SAMPLER,

    VERTEX_BUFFER           = TOTAL,
    VERTEX_BUFFER_COUNT     = 4,

    INDEX_BUFFER_WIDTH      = 2,
};

#define xxRegisterFunction(API) \
    xxCreateInstance = xxCreateInstance ## API; \
    xxDestroyInstance = xxDestroyInstance ## API; \
\
    xxCreateDevice = xxCreateDevice ## API; \
    xxDestroyDevice = xxDestroyDevice ## API; \
    xxResetDevice = xxResetDevice ## API; \
    xxTestDevice = xxTestDevice ## API; \
    xxGetDeviceName = xxGetDeviceName ## API; \
\
    xxGetFramebuffer = xxGetFramebuffer ## API; \
\
    xxCreateSwapchain = xxCreateSwapchain ## API; \
    xxDestroySwapchain = xxDestroySwapchain ## API; \
    xxPresentSwapchain = xxPresentSwapchain ## API; \
\
    xxCreateVertexAttribute = xxCreateVertexAttribute ## API; \
    xxDestroyVertexAttribute = xxDestroyVertexAttribute ## API; \
\
    xxGetCommandBuffer = xxGetCommandBuffer ## API; \
    xxBeginCommandBuffer = xxBeginCommandBuffer ## API; \
    xxEndCommandBuffer = xxEndCommandBuffer ## API; \
    xxSubmitCommandBuffer = xxSubmitCommandBuffer ## API; \
\
    xxCreateRenderPass = xxCreateRenderPass ## API; \
    xxDestroyRenderPass = xxDestroyRenderPass ## API; \
    xxBeginRenderPass = xxBeginRenderPass ## API; \
    xxEndRenderPass = xxEndRenderPass ## API; \
\
    xxCreateConstantBuffer = xxCreateConstantBuffer ## API; \
    xxCreateIndexBuffer = xxCreateIndexBuffer ## API; \
    xxCreateVertexBuffer = xxCreateVertexBuffer ## API; \
    xxDestroyBuffer = xxDestroyBuffer ## API; \
    xxMapBuffer = xxMapBuffer ## API; \
    xxUnmapBuffer = xxUnmapBuffer ## API; \
\
    xxCreateTexture = xxCreateTexture ## API; \
    xxDestroyTexture = xxDestroyTexture ## API; \
    xxMapTexture = xxMapTexture ## API; \
    xxUnmapTexture = xxUnmapTexture ## API; \
\
    xxCreateSampler = xxCreateSampler ## API; \
    xxDestroySampler = xxDestroySampler ## API; \
\
    xxCreateVertexShader = xxCreateVertexShader ## API; \
    xxCreateFragmentShader = xxCreateFragmentShader ## API; \
    xxDestroyShader = xxDestroyShader ## API; \
\
    xxCreateBlendState = xxCreateBlendState ## API; \
    xxCreateDepthStencilState = xxCreateDepthStencilState ## API; \
    xxCreateRasterizerState = xxCreateRasterizerState ## API; \
    xxCreatePipeline = xxCreatePipeline ## API; \
    xxDestroyBlendState = xxDestroyBlendState ## API; \
    xxDestroyDepthStencilState = xxDestroyDepthStencilState ## API; \
    xxDestroyRasterizerState = xxDestroyRasterizerState ## API; \
    xxDestroyPipeline = xxDestroyPipeline ## API; \
\
    xxSetViewport = xxSetViewport ## API; \
    xxSetScissor = xxSetScissor ## API; \
    xxSetPipeline = xxSetPipeline ## API; \
    xxSetVertexBuffers = xxSetVertexBuffers ## API; \
    xxSetVertexTextures = xxSetVertexTextures ## API; \
    xxSetFragmentTextures = xxSetFragmentTextures ## API; \
    xxSetVertexSamplers = xxSetVertexSamplers ## API; \
    xxSetFragmentSamplers = xxSetFragmentSamplers ## API; \
    xxSetVertexConstantBuffer = xxSetVertexConstantBuffer ## API; \
    xxSetFragmentConstantBuffer = xxSetFragmentConstantBuffer ## API; \
    xxDrawIndexed = xxDrawIndexed ## API;

#define xxUnregisterFunction() \
    xxCreateInstance = nullptr; \
    xxDestroyInstance = nullptr; \
\
    xxCreateDevice = nullptr; \
    xxDestroyDevice = nullptr; \
    xxResetDevice = nullptr; \
    xxTestDevice = nullptr; \
    xxGetDeviceName = nullptr; \
\
    xxGetFramebuffer = nullptr; \
\
    xxCreateSwapchain = nullptr; \
    xxDestroySwapchain = nullptr; \
    xxPresentSwapchain = nullptr; \
\
    xxGetCommandBuffer = nullptr; \
    xxBeginCommandBuffer = nullptr; \
    xxEndCommandBuffer = nullptr; \
    xxSubmitCommandBuffer = nullptr; \
\
    xxCreateRenderPass = nullptr; \
    xxDestroyRenderPass = nullptr; \
    xxBeginRenderPass = nullptr; \
    xxEndRenderPass = nullptr; \
\
    xxCreateVertexAttribute = nullptr; \
    xxDestroyVertexAttribute = nullptr; \
\
    xxCreateConstantBuffer = nullptr; \
    xxCreateIndexBuffer = nullptr; \
    xxCreateVertexBuffer = nullptr; \
    xxDestroyBuffer = nullptr; \
    xxMapBuffer = nullptr; \
    xxUnmapBuffer = nullptr; \
\
    xxCreateTexture = nullptr; \
    xxDestroyTexture = nullptr; \
    xxMapTexture = nullptr; \
    xxUnmapTexture = nullptr; \
\
    xxCreateSampler = nullptr; \
    xxDestroySampler = nullptr; \
\
    xxCreateVertexShader = nullptr; \
    xxCreateFragmentShader = nullptr; \
    xxDestroyShader = nullptr; \
\
    xxCreateBlendState = nullptr; \
    xxCreateDepthStencilState = nullptr; \
    xxCreateRasterizerState = nullptr; \
    xxCreatePipeline = nullptr; \
    xxDestroyBlendState = nullptr; \
    xxDestroyDepthStencilState = nullptr; \
    xxDestroyRasterizerState = nullptr; \
    xxDestroyPipeline = nullptr; \
\
    xxSetViewport = nullptr; \
    xxSetScissor = nullptr; \
    xxSetPipeline = nullptr; \
    xxSetVertexBuffers = nullptr; \
    xxSetVertexTextures = nullptr; \
    xxSetFragmentTextures = nullptr; \
    xxSetVertexSamplers = nullptr; \
    xxSetFragmentSamplers = nullptr; \
    xxSetVertexConstantBuffer = nullptr; \
    xxSetFragmentConstantBuffer = nullptr; \
    xxDrawIndexed = nullptr;

template <typename T, T ZERO,
                      T ONE,
                      T SRC_COLOR,
                      T ONE_MINUS_SRC_COLOR,
                      T DST_COLOR,
                      T ONE_MINUS_DST_COLOR,
                      T SRC_ALPHA,
                      T ONE_MINUS_SRC_ALPHA,
                      T DST_ALPHA,
                      T ONE_MINUS_DST_ALPHA>
inline T xxTemplateBlendFactor(const char* name)
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
            return ZERO;
        }
    }
    if (d)
    {
        if (c) return o ? ONE_MINUS_DST_COLOR : DST_COLOR;
        if (a) return o ? ONE_MINUS_DST_ALPHA : DST_ALPHA;
    }
    if (s)
    {
        if (c) return o ? ONE_MINUS_SRC_COLOR : SRC_COLOR;
        if (a) return o ? ONE_MINUS_SRC_ALPHA : SRC_ALPHA;
    }
    return o ? ONE : ZERO;
}

template <typename T, T ADD,
                      T SUBTRACT,
                      T REVERSE_SUBTRACT,
                      T MIN,
                      T MAX>
inline T xxTemplateBlendOp(const char* name)
{
    for (char x; (x = (*name)); name++)
    {
        switch (x)
        {
        case 'A':
        case 'a':
        case '+':
            return ADD;
        case 'S':
        case 's':
        case '-':
            return SUBTRACT;
        case 'I':
        case 'i':
        case 'R':
        case 'r':
            return REVERSE_SUBTRACT;
        case 'M':
        case 'm':
            name++;
            x = (*name);
            return (x == 'I' || x == 'i') ? MIN : MAX;
        }
    }
    return ADD;
}

template <typename T, T NEVER,
                      T LESS,
                      T EQUAL,
                      T LESS_OR_EQUAL,
                      T GREATER,
                      T NOT_EQUAL,
                      T GREATER_OR_EQUAL,
                      T ALWAYS>
inline T xxTemplateCompareOp(const char* name)
{
    bool e = false;
    bool g = false;
    bool l = false;
    for (char x; (x = (*name)); name++)
    {
        switch (x)
        {
        case 'V':
        case 'v':
            return NEVER;
        case 'W':
        case 'w':
            return ALWAYS;
        case 'N':
        case 'n':
        case '!':
            return NOT_EQUAL;
        case 'Q':
        case 'q':
        case '=':
            e = true;
            break;
        case 'G':
        case 'g':
        case '>':
            if (e) break;
            g = true;
            break;
        case 'L':
        case 'l':
        case '<':
            if (e) break;
            l = true;
            break;
        }
    }
    if (e)
    {
        if (l) return LESS_OR_EQUAL;
        if (g) return GREATER_OR_EQUAL;
        return EQUAL;
    }
    if (l) return LESS;
    if (g) return GREATER;
    return ALWAYS;
}
