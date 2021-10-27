//==============================================================================
// xxGraphic : OpenGL Internal Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"
#include "xxGraphicInternal.h"

#define KHRONOS_STATIC
#define GL_GLES_PROTOTYPES 1
#include "gl/gl32.h"

#ifndef GL_APPLE_texture_format_BGRA8888
#define GL_APPLE_texture_format_BGRA8888 1
#define GL_BGRA_EXT                       0x80E1
#define GL_BGRA8_EXT                      0x93A1
#endif /* GL_APPLE_texture_format_BGRA8888 */

#ifndef GL_ARB_texture_rectangle
#define GL_ARB_texture_rectangle 1
#define GL_TEXTURE_RECTANGLE_ARB          0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE_ARB  0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE_ARB    0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB 0x84F8
#endif /* GL_EXT_texture_rectangle */

#ifndef GL_UNSIGNED_INT_8_8_8_8_REV
#define GL_UNSIGNED_INT_8_8_8_8_REV       0x8367
#endif /* GL_UNSIGNED_INT_8_8_8_8_REV */

//==============================================================================
//  GLSL Shader
//==============================================================================
extern const char* const    glDefaultShaderCode;
//==============================================================================
//  Blend Factor
//==============================================================================
inline GLenum glBlendFactor(const char* name)
{
    return xxTemplateBlendFactor<GLenum, GL_ZERO,
                                         GL_ONE,
                                         GL_SRC_COLOR,
                                         GL_ONE_MINUS_SRC_COLOR,
                                         GL_DST_COLOR,
                                         GL_ONE_MINUS_DST_COLOR,
                                         GL_SRC_ALPHA,
                                         GL_ONE_MINUS_SRC_ALPHA,
                                         GL_DST_ALPHA,
                                         GL_ONE_MINUS_DST_ALPHA>(name);
}
//==============================================================================
//  Blend Operation
//==============================================================================
inline GLenum glBlendOp(const char* name)
{
    return xxTemplateBlendOp<GLenum, GL_FUNC_ADD,
                                     GL_FUNC_SUBTRACT,
                                     GL_FUNC_REVERSE_SUBTRACT,
                                     GL_FUNC_ADD,
                                     GL_FUNC_ADD>(name);
}
//==============================================================================
//  Depth Comparison
//==============================================================================
inline GLenum glCompareOp(const char* name)
{
    return xxTemplateCompareOp<GLenum, GL_NEVER,
                                       GL_LESS,
                                       GL_EQUAL,
                                       GL_LEQUAL,
                                       GL_GREATER,
                                       GL_NOTEQUAL,
                                       GL_GEQUAL,
                                       GL_ALWAYS>(name);
}
//==============================================================================
//  Function
//==============================================================================
extern uint64_t             (*glCreateContext)(uint64_t device, void* view, void** display);
extern void                 (*glDestroyContext)(uint64_t context, void* view, void* display);
extern void*                (*glGetProcAddress)(const char* name);
extern float                (*glGetScaleContext)(uint64_t context, void* view);
extern void                 (*glMakeCurrentContext)(uint64_t context, void* display);
extern void                 (*glPresentContext)(uint64_t context, void* display);
//==============================================================================
inline void* getSymbolExtension(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed), const char* name, bool* failed)
{
    static const char* const tags[] = { "ARB", "OES", "EXT", "KHR", "AMD", "ARM", "IMG", "INTEL", "NV", "QCOM" };

    char extensionName[64];
    for (int i = 0; i < xxCountOf(tags); ++i)
    {
        snprintf(extensionName, 64, "%s%s", name, tags[i]);
        void* ptr = getSymbol(extensionName, failed);
        if (ptr != nullptr)
            return ptr;
    }

    return nullptr;
}
//==============================================================================
//  Swapchain
//==============================================================================
struct SWAPCHAINGL
{
    uint64_t    context;
    void*       view;
    void*       display;
    int         width;
    int         height;
    float       scale;
    uint64_t    pipeline;
    uint64_t    vertexBuffers[8];
    GLenum      textureTypes[8];
    bool        textureMipmaps[8];
};
//==============================================================================
//  Vertex Attribute
//==============================================================================
struct VERTEXATTRIBUTEGL
{
    struct Attribute
    {
        GLuint      index;
        GLint       size;
        GLenum      type;
        GLboolean   normalized;
        GLsizei     stride;
        const char* pointer;
        int         stream;
        const char* name;
    } attributes[16];
    int count;
};
//==============================================================================
//  Buffer
//==============================================================================
struct BUFFERGL
{
    GLenum  type;
    GLuint  buffer;
    void*   memory;
    GLsizei size;
};
//==============================================================================
//  Texture
//==============================================================================
struct TEXTUREGL
{
    GLuint          texture;
    GLenum          type;
    void*           memory;
    unsigned int    width;
    unsigned int    height;
    unsigned int    depth;
    unsigned int    mipmap;
    unsigned int    array;
    const void*     external;
    const void*     image;
    const void*     device;
};
//==============================================================================
//  Sampler
//==============================================================================
union SAMPLERGL
{
    uint64_t    value;
    struct
    {
        uint8_t addressU;
        uint8_t addressV;
        uint8_t addressW;
        uint8_t magFilter;
        uint8_t minFilter;
        uint8_t mipFilter;
        uint8_t anisotropy;
    };
};
//==============================================================================
//  Pipeline
//==============================================================================
struct BLENDGL
{
    GLboolean   blendEnable;
    GLenum      blendSourceColor;
    GLenum      blendFunctionColor;
    GLenum      blendDestinationColor;
    GLenum      blendSourceAlpha;
    GLenum      blendFunctionAlpha;
    GLenum      blendDestinationAlpha;
};
//------------------------------------------------------------------------------
union STATEGL
{
    uint64_t        value;
    struct
    {
        uint64_t    depthTest:16;
        uint64_t    depthWrite:1;
        uint64_t    cull:1;
        uint64_t    scissor:1;
    };
};
//------------------------------------------------------------------------------
struct PIPELINEGL
{
    GLuint              program;
    VERTEXATTRIBUTEGL*  vertexAttribute;
    GLint               texture;
    GLint               uniform;
    BLENDGL             blend;
    STATEGL             state;
};
//==============================================================================
//  Indirect Command
//==============================================================================
struct DrawArraysIndirectCommand
{
    GLuint  count;
    GLuint  primCount;
    GLuint  first;
    GLuint  baseInstance;
};
//------------------------------------------------------------------------------
struct DrawElementsIndirectCommand
{
    GLuint  count;
    GLuint  instanceCount;
    GLuint  firstIndex;
    GLuint  baseVertex;
    GLuint  baseInstance;
};
//==============================================================================
