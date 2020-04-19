//==============================================================================
// xxGraphic : OpenGL Internal Header
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

#define GL_GLES_PROTOTYPES 0
#include "gl/gl32.h"

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
    GLenum          type;
    GLuint          texture;
    void*           memory;
    unsigned int    width;
    unsigned int    height;
    unsigned int    depth;
    unsigned int    mipmap;
    unsigned int    array;
    const void*     external;
    const void*     image;
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
union STATEGL
{
    uint64_t        value;
    struct
    {
        uint64_t    alphaBlending:1;
        uint64_t    alphaTesting:1;
        uint64_t    depthTest:1;
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
    STATEGL             state;
};
//==============================================================================
//  Indirect Command
//==============================================================================
struct INDIRECTCOMMANDGL
{
    GLuint  count;
    GLuint  instanceCount;
    GLuint  firstIndex;
    GLuint  baseVertex;
    GLuint  baseInstance;
};
//==============================================================================
