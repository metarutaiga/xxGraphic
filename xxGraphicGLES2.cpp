//==============================================================================
// xxGraphic : OpenGL ES 2.0 Source
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#undef  GL_ES_VERSION_2_0
#define GL_ES_VERSION_3_0 0
#define GL_ES_VERSION_3_1 0
#define GL_ES_VERSION_3_2 0
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalGL.h"
#include "xxGraphicGL.h"
#include "xxGraphicGLES2.h"

#if defined(xxANDROID)
#   include "xxGraphicEGL.h"
#endif

#if defined(xxIOS) && !defined(xxMACCATALYST)
#   include "xxGraphicEAGL.h"
#endif

#if defined(xxMACOS)
#   include "xxGraphicCGL.h"
#endif

#if defined(xxWINDOWS)
#   include "xxGraphicWGL.h"
#endif

#ifndef GL_UNIFORM_BUFFER
#define GL_UNIFORM_BUFFER                 0x8A11
#endif

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceGLES2()
{
    uint64_t instance = 0;

#if defined(xxANDROID)
    instance = xxGraphicCreateEGL(200);
#endif

#if defined(xxMACOS)
    instance = xxGraphicCreateCGL(200);
#endif

#if defined(xxIOS) && !defined(xxMACCATALYST)
    instance = xxGraphicCreateEAGL(200);
#endif

#if defined(xxWINDOWS)
    instance = xxGraphicCreateWGL(200);
#endif

    if (instance == 0)
        return 0;

    xxRegisterFunction(GLES2);

    return instance;
}
//------------------------------------------------------------------------------
void xxDestroyInstanceGLES2(uint64_t instance)
{
#if defined(xxANDROID)
    xxGraphicDestroyEGL(instance);
#endif

#if defined(xxIOS) && !defined(xxMACCATALYST)
    xxGraphicDestroyEAGL(instance);
#endif

#if defined(xxMACOS)
    xxGraphicDestroyCGL(instance);
#endif

#if defined(xxWINDOWS)
    xxGraphicDestroyWGL(instance);
#endif

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceGLES2(uint64_t instance)
{
    return instance;
}
//------------------------------------------------------------------------------
void xxDestroyDeviceGLES2(uint64_t device)
{

}
//------------------------------------------------------------------------------
bool xxResetDeviceGLES2(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
bool xxTestDeviceGLES2(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameGLES2()
{
    return "OpenGL ES 2.0";
}
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t xxCreateSwapchainGLES2(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain)
{
    SWAPCHAINGL* glOldSwapchain = reinterpret_cast<SWAPCHAINGL*>(oldSwapchain);
    if (glOldSwapchain && glOldSwapchain->view == view && glOldSwapchain->width == width && glOldSwapchain->height == height)
        return oldSwapchain;
    SWAPCHAINGL* glSwapchain = xxAlloc(SWAPCHAINGL);
    if (glSwapchain == nullptr)
        return 0;

    xxDestroySwapchainGLES2(oldSwapchain);

    void* display = nullptr;
    uint64_t context = glCreateContext(device, view, &display);
    if (context == 0)
    {
        xxFree(glSwapchain);
        return 0;
    }

    glSwapchain->context = context;
    glSwapchain->view = view;
    glSwapchain->display = display;
    glSwapchain->width = width;
    glSwapchain->height = height;
    glSwapchain->scale = glGetScaleContext(glSwapchain->context, glSwapchain->view);
    glSwapchain->pipeline = 0;
    memset(glSwapchain->vertexBuffers, 0, sizeof(glSwapchain->vertexBuffers));
    memset(glSwapchain->textureTypes, 0, sizeof(glSwapchain->textureTypes));
    memset(glSwapchain->textureMipmaps, 0, sizeof(glSwapchain->textureMipmaps));

    return reinterpret_cast<uint64_t>(glSwapchain);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainGLES2(uint64_t swapchain)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(swapchain);
    if (glSwapchain == nullptr)
        return;

    glDestroyContext(glSwapchain->context, glSwapchain->view, glSwapchain->display);
    xxFree(glSwapchain);
}
//------------------------------------------------------------------------------
void xxPresentSwapchainGLES2(uint64_t swapchain)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(swapchain);
    if (glSwapchain == nullptr)
        return;

    glPresentContext(glSwapchain->context, glSwapchain->display);
}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferGLES2(uint64_t device, uint64_t swapchain)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(swapchain);
    if (glSwapchain == nullptr)
        return 0;

    glMakeCurrentContext(glSwapchain->context, glSwapchain->display);
    glViewport(0, 0, glSwapchain->width, glSwapchain->height);
    glScissor(0, 0, glSwapchain->width, glSwapchain->height);

    glSwapchain->pipeline = 0;
    memset(glSwapchain->vertexBuffers, 0, sizeof(glSwapchain->vertexBuffers));
    memset(glSwapchain->textureTypes, 0, sizeof(glSwapchain->textureTypes));
    memset(glSwapchain->textureMipmaps, 0, sizeof(glSwapchain->textureMipmaps));

    return reinterpret_cast<uint64_t>(glSwapchain);
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferGLES2(uint64_t device, uint64_t swapchain, float* scale)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(swapchain);

    if (scale)
    {
        float scaleFactor = glGetScaleContext(glSwapchain->context, glSwapchain->view);
        if (glSwapchain->scale != scaleFactor)
        {
            glSwapchain->scale = scaleFactor;

            glDestroyContext(glSwapchain->context, glSwapchain->view, glSwapchain->display);
            glSwapchain->context = glCreateContext(device, glSwapchain->view, &glSwapchain->display);
        }

        (*scale) = scaleFactor;
    }

    return 0;
}
//==============================================================================
//  Command Buffer
//==============================================================================
bool xxBeginCommandBufferGLES2(uint64_t commandBuffer)
{
    return true;
}
//------------------------------------------------------------------------------
void xxEndCommandBufferGLES2(uint64_t commandBuffer)
{

}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferGLES2(uint64_t commandBuffer, uint64_t swapchain)
{
}
//==============================================================================
//  Render Pass
//==============================================================================
uint64_t xxCreateRenderPassGLES2(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
{
    GLbitfield mask = 0;

    if (clearColor)
        mask |= GL_COLOR_BUFFER_BIT;
    if (clearDepth)
        mask |= GL_DEPTH_BUFFER_BIT;
    if (clearStencil)
        mask |= GL_STENCIL_BUFFER_BIT;

    return mask;
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassGLES2(uint64_t renderPass)
{

}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassGLES2(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    GLbitfield mask = static_cast<GLbitfield>(renderPass);

    glViewport(0, 0, width, height);
    glScissor(0, 0, width, height);

    if (mask & GL_COLOR_BUFFER_BIT)
    {
        glClearColor(color[0], color[1], color[2], color[3]);
    }
    if (mask & GL_DEPTH_BUFFER_BIT)
    {
        glClearDepthf(depth);
    }
    if (mask & GL_STENCIL_BUFFER_BIT)
    {
        glClearStencil(stencil);
    }
    if (mask)
    {
        glClear(mask);
    }

    return commandBuffer;
}
//------------------------------------------------------------------------------
void xxEndRenderPassGLES2(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{

}
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t xxCreateVertexAttributeGLES2(uint64_t device, int count, int* attribute)
{
    VERTEXATTRIBUTEGL* glVertexAttribute = xxAlloc(VERTEXATTRIBUTEGL);
    if (glVertexAttribute == nullptr)
        return 0;

    VERTEXATTRIBUTEGL::Attribute* attributes = glVertexAttribute->attributes;
    int stride = 0;

    for (int i = 0; i < count; ++i)
    {
        int stream = (*attribute++);
        int offset = (*attribute++);
        int element = (*attribute++);
        int size = (*attribute++);

        stride += size;

        attributes[i].index = i;
        attributes[i].size = element;
        attributes[i].type = GL_FLOAT;
        attributes[i].normalized = GL_FALSE;
        attributes[i].stride = 0;
        attributes[i].pointer = (char*)nullptr + offset;
        attributes[i].stream = stream;

        if (offset == 0 && element == 3 && size == sizeof(float) * 3)
        {
            attributes[i].name = "position";
        }
        if (offset != 0 && element == 3 && size == sizeof(float) * 3)
        {
            attributes[i].name = "normal";
        }
        if (offset != 0 && element == 4 && size == sizeof(char) * 4)
        {
#if defined(xxMACOS) || defined(xxWINDOWS)
            attributes[i].size = GL_BGRA_EXT;
#else
            attributes[i].size = 4;
#endif
            attributes[i].type = GL_UNSIGNED_BYTE;
            attributes[i].normalized = GL_TRUE;
            attributes[i].name = "color";
        }
        if (offset != 0 && element == 2 && size == sizeof(float) * 2)
        {
            attributes[i].name = "uv";
        }
    }

    for (int i = 0; i < count; ++i)
    {
        attributes[i].stride = stride;
    }

    glVertexAttribute->count = count;

    return reinterpret_cast<uint64_t>(glVertexAttribute);
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeGLES2(uint64_t vertexAttribute)
{
    VERTEXATTRIBUTEGL* glVertexAttribute = reinterpret_cast<VERTEXATTRIBUTEGL*>(vertexAttribute);

    xxFree(glVertexAttribute);
}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateConstantBufferGLES2(uint64_t device, int size)
{
    BUFFERGL* glBuffer = xxAlloc(BUFFERGL);
    if (glBuffer == nullptr)
        return 0;

    glBuffer->type = GL_UNIFORM_BUFFER;
    glBuffer->buffer = 0;
    glBuffer->memory = xxAlloc(char, size);
    glBuffer->size = size;

    return reinterpret_cast<uint64_t>(glBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferGLES2(uint64_t device, int size)
{
    BUFFERGL* glBuffer = xxAlloc(BUFFERGL);
    if (glBuffer == nullptr)
        return 0;

    GLuint buffer = 0;
    glGenBuffers(1, &buffer);

    glBuffer->type = GL_ELEMENT_ARRAY_BUFFER;
    glBuffer->buffer = buffer;
    glBuffer->memory = xxAlloc(char, size);
    glBuffer->size = size;

    return reinterpret_cast<uint64_t>(glBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferGLES2(uint64_t device, int size, uint64_t vertexAttribute)
{
    BUFFERGL* glBuffer = xxAlloc(BUFFERGL);
    if (glBuffer == nullptr)
        return 0;

    GLuint buffer = 0;
    glGenBuffers(1, &buffer);

    glBuffer->type = GL_ARRAY_BUFFER;
    glBuffer->buffer = buffer;
    glBuffer->memory = xxAlloc(char, size);
    glBuffer->size = size;

    return reinterpret_cast<uint64_t>(glBuffer);
}
//------------------------------------------------------------------------------
void xxDestroyBufferGLES2(uint64_t device, uint64_t buffer)
{
    BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffer);
    if (glBuffer == nullptr)
        return;

    if (glBuffer->buffer)
        glDeleteBuffers(1, &glBuffer->buffer);
    xxFree(glBuffer->memory);
    xxFree(glBuffer);
}
//------------------------------------------------------------------------------
void* xxMapBufferGLES2(uint64_t device, uint64_t buffer)
{
    BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffer);
    if (glBuffer == nullptr)
        return nullptr;

    return glBuffer->memory;
}
//------------------------------------------------------------------------------
void xxUnmapBufferGLES2(uint64_t device, uint64_t buffer)
{
    BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffer);
    if (glBuffer == nullptr)
        return;
    if (glBuffer->type == GL_UNIFORM_BUFFER)
        return;

    glBindBuffer(glBuffer->type, glBuffer->buffer);
    glBufferData(glBuffer->type, glBuffer->size, glBuffer->memory, GL_STREAM_DRAW);
}
//==============================================================================
//  Texture
//==============================================================================
uint64_t xxCreateTextureGLES2(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, const void* external)
{
    if (width == 0 || height == 0 || depth == 0 || mipmap == 0 || array == 0)
        return 0;

    TEXTUREGL* glTexture = xxAlloc(TEXTUREGL);
    if (glTexture == nullptr)
        return 0;

    GLuint texture = 0;
    glGenTextures(1, &texture);
    if (texture == 0)
    {
        xxFree(glTexture);
        return 0;
    }

    glTexture->type = 0;
    glTexture->texture = texture;
    glTexture->memory = nullptr;
    glTexture->width = width;
    glTexture->height = height;
    glTexture->depth = depth;
    glTexture->mipmap = mipmap;
    glTexture->array = array;
    glTexture->external = external;
    glTexture->image = nullptr;
    glTexture->device = nullptr;

    GLuint externalTexture = static_cast<GLuint>(reinterpret_cast<size_t>(external));
    if (glIsTexture(externalTexture))
    {
        glDeleteTextures(1, &texture);
        glTexture->texture = externalTexture;
        glTexture->type = GL_TEXTURE_2D;
        glTexture->external = external;
        external = nullptr;
    }

    if (external)
    {
#if defined(xxANDROID)
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexture->type = GL_TEXTURE_2D;
        glTexture->image = xxCreateImageFromHardwareBuffer(external);
        xxBindTextureWithImage(glTexture->image);
#elif defined(xxMACOS)
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture);
        glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexture->type = GL_TEXTURE_RECTANGLE_ARB;
        xxBindTextureWithSurface(glTexture->external);
#elif defined(xxMACCATALYST)
#elif defined(xxIOS)
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexture->type = GL_TEXTURE_2D;
        xxBindTextureWithSurface(glTexture->external);
#elif defined(xxWINDOWS)
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexture->type = GL_TEXTURE_2D;
        glTexture->device = xxGetDeviceFromDirect3DTexture(external);
        glTexture->image = xxCreateImageFromDirect3DTexture(glTexture->device, external, texture);
#endif
    }

    return reinterpret_cast<uint64_t>(glTexture);
}
//------------------------------------------------------------------------------
void xxDestroyTextureGLES2(uint64_t texture)
{
    TEXTUREGL* glTexture = reinterpret_cast<TEXTUREGL*>(texture);
    if (glTexture == nullptr)
        return;

    GLuint externalTexture = static_cast<GLuint>(reinterpret_cast<size_t>(glTexture->external));
    if (glTexture->texture == externalTexture)
    {
        glTexture->texture = 0;
    }

    if (glTexture->image)
    {
#if defined(xxANDROID)
        xxDestroyImage(glTexture->image);
#elif defined(xxWINDOWS)
        xxDestroyImage(glTexture->device, glTexture->image);
#endif
    }   

    if (glTexture->texture)
    {
        glDeleteTextures(1, &glTexture->texture);
    }
    xxFree(glTexture->memory);
    xxFree(glTexture);
}
//------------------------------------------------------------------------------
void* xxMapTextureGLES2(uint64_t device, uint64_t texture, int* stride, int level, int array)
{
    TEXTUREGL* glTexture = reinterpret_cast<TEXTUREGL*>(texture);
    if (glTexture == nullptr)
        return nullptr;

#if defined(xxANDROID)
    if (glTexture->image)
        return nullptr;
#endif

    unsigned int width = glTexture->width >> level;
    unsigned int height = glTexture->height >> level;
    unsigned int depth = glTexture->depth >> level;
    if (width == 0)
        width = 1;
    if (height == 0)
        height = 1;
    if (depth == 0)
        depth = 1;
    glTexture->memory = xxRealloc(glTexture->memory, char, width * height * depth * 4);

    (*stride) = width * 4;
    return glTexture->memory;
}
//------------------------------------------------------------------------------
void xxUnmapTextureGLES2(uint64_t device, uint64_t texture, int level, int array)
{
    TEXTUREGL* glTexture = reinterpret_cast<TEXTUREGL*>(texture);
    if (glTexture == nullptr)
        return;

#if defined(xxANDROID)
    if (glTexture->image)
        return;
#endif

    if (glTexture->depth == 1 && glTexture->array == 1)
    {
        glBindTexture(GL_TEXTURE_2D, glTexture->texture);
        if (glTexture->type == 0)
        {
            glTexture->type = GL_TEXTURE_2D;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glTexture->mipmap > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        }
#if defined(xxMACOS) || defined(xxWINDOWS)
        int format = GL_BGRA_EXT;
#else
        int format = GL_RGBA;
#endif
        glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, glTexture->width, glTexture->height, 0, format, GL_UNSIGNED_BYTE, glTexture->memory);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (glTexture->width == glTexture->height && glTexture->depth == 1 && glTexture->array == 6)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, glTexture->texture);
        if (glTexture->type == 0)
        {
            glTexture->type = GL_TEXTURE_CUBE_MAP;
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, glTexture->mipmap > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        }
#if defined(xxMACOS) || defined(xxWINDOWS)
        int format = GL_BGRA_EXT;
#else
        int format = GL_RGBA;
#endif
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + array, level, GL_RGBA, glTexture->width, glTexture->height, 0, format, GL_UNSIGNED_BYTE, glTexture->memory);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    if (glTexture->mipmap == level + 1)
    {
        xxFree(glTexture->memory);
        glTexture->memory = nullptr;
    }
}
//==============================================================================
//  Sampler
//==============================================================================
uint64_t xxCreateSamplerGLES2(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    SAMPLERGL glSampler = {};

    glSampler.addressU = clampU;
    glSampler.addressV = clampV;
    glSampler.addressW = clampW;
    glSampler.magFilter = linearMag;
    glSampler.minFilter = linearMin;
    glSampler.mipFilter = linearMip;
    glSampler.anisotropy = anisotropy;

    return glSampler.value;
}
//------------------------------------------------------------------------------
void xxDestroySamplerGLES2(uint64_t sampler)
{

}
//==============================================================================
//  Shader
//==============================================================================
static void checkShader(GLuint glShader, const char* shader)
{
    GLint status = 0;
    glGetShaderiv(glShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        xxLog("xxGraphicGLES2", "failed to compile %s", shader);

        GLint length = 0;
        glGetShaderiv(glShader, GL_INFO_LOG_LENGTH, &length);
        char* log = xxAlloc(char, length + 1);
        if (log)
        {
            glGetShaderInfoLog(glShader, length, nullptr, log);
            log[length] = '\0';
            xxLog("xxGraphicGLES2", "%s", log);
            xxFree(log);
        }
    }
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexShaderGLES2(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    if (strcmp(shader, "default") == 0)
    {
        const char* sources[] =
        {
            "#version 100", "\n",
            "#define __VERTEX__ 1", "\n",
            "#define attribute attribute", "\n",
            "#define varying varying", "\n",
            "precision highp float;", "\n",
            glDefaultShaderCode
        };

        GLuint glShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(glShader, xxCountOf(sources), sources, nullptr);
        glCompileShader(glShader);
        checkShader(glShader, shader);

        return static_cast<uint64_t>(glShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderGLES2(uint64_t device, const char* shader)
{
    if (strcmp(shader, "default") == 0)
    {
        const char* sources[] =
        {
            "#version 100", "\n",
            "#define __FRAGMENT__ 1", "\n",
            "#define attribute", "\n",
            "#define varying varying", "\n",
            "precision mediump float;", "\n",
            glDefaultShaderCode
        };

        GLuint glShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(glShader, xxCountOf(sources), sources, nullptr);
        glCompileShader(glShader);
        checkShader(glShader, shader);

        return static_cast<uint64_t>(glShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderGLES2(uint64_t device, uint64_t shader)
{
    GLuint glShader = static_cast<GLuint>(shader);
    if (glShader == 0)
        return;

    glDeleteShader(glShader);
}
//==============================================================================
//  Pipeline
//==============================================================================
static void checkProgram(GLuint glProgram)
{
    GLint status = 0;
    glGetProgramiv(glProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        xxLog("xxGraphicGLES2", "failed to link %u", glProgram);

        GLint length = 0;
        glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &length);
        char* log = xxAlloc(char, length + 1);
        if (log)
        {
            glGetProgramInfoLog(glProgram, length, nullptr, log);
            log[length] = '\0';
            xxLog("xxGraphicGLES2", "%s", log);
            xxFree(log);
        }
    }
}
//------------------------------------------------------------------------------
uint64_t xxCreateBlendStateGLES2(uint64_t device, bool blending)
{
    STATEGL glState = {};
    glState.alphaBlending = blending;
    return glState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateGLES2(uint64_t device, bool depthTest, bool depthWrite)
{
    STATEGL glState = {};
    glState.depthTest = depthTest;
    glState.depthWrite = depthWrite;
    return glState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateGLES2(uint64_t device, bool cull, bool scissor)
{
    STATEGL glState = {};
    glState.cull = cull;
    glState.scissor = scissor;
    return glState.value;
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineGLES2(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    VERTEXATTRIBUTEGL* glVertexAttribute = reinterpret_cast<VERTEXATTRIBUTEGL*>(vertexAttribute);
    if (glVertexAttribute == nullptr)
        return 0;
    PIPELINEGL* glPipeline = xxAlloc(PIPELINEGL);
    if (glPipeline == nullptr)
        return 0;

    STATEGL glBlendState = { blendState };
    STATEGL glDepthStencilState = { depthStencilState };
    STATEGL glRasterizerState = { rasterizerState };

    GLuint glProgram = glCreateProgram();

    VERTEXATTRIBUTEGL::Attribute* attributes = glVertexAttribute->attributes;
    for (int i = 0; i < glVertexAttribute->count; ++i)
    {
        VERTEXATTRIBUTEGL::Attribute& attribute = attributes[i];
        glBindAttribLocation(glProgram, attribute.index, attribute.name);
    }

    GLuint glVertexShader = static_cast<GLuint>(vertexShader);
    GLuint glFragmentShader = static_cast<GLuint>(fragmentShader);
    glAttachShader(glProgram, glVertexShader);
    glAttachShader(glProgram, glFragmentShader);
    glLinkProgram(glProgram);
    checkProgram(glProgram);

    glPipeline->program = glProgram;
    glPipeline->vertexAttribute = glVertexAttribute;
    glPipeline->texture = glGetUniformLocation(glProgram, "tex");
    glPipeline->uniform = glGetUniformLocation(glProgram, "uniformBuffer");
    glPipeline->state.alphaBlending = glBlendState.alphaBlending;
    glPipeline->state.depthTest = glDepthStencilState.depthTest;
    glPipeline->state.depthWrite = glDepthStencilState.depthWrite;
    glPipeline->state.cull = glRasterizerState.cull;
    glPipeline->state.scissor = glRasterizerState.scissor;

    return reinterpret_cast<uint64_t>(glPipeline);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateGLES2(uint64_t blendState)
{

}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateGLES2(uint64_t depthStencilState)
{

}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateGLES2(uint64_t rasterizerState)
{

}
//------------------------------------------------------------------------------
void xxDestroyPipelineGLES2(uint64_t pipeline)
{
    PIPELINEGL* glPipeline = reinterpret_cast<PIPELINEGL*>(pipeline);
    if (glPipeline == nullptr)
        return;

    glDeleteProgram(glPipeline->program);
    xxFree(glPipeline);
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportGLES2(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{
    glViewport(x, y, width, height);
}
//------------------------------------------------------------------------------
void xxSetScissorGLES2(uint64_t commandEncoder, int x, int y, int width, int height)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(commandEncoder);

    y = (int)(glSwapchain->height * glSwapchain->scale) - y - height;
    glScissor(x, y, width, height);
}
//------------------------------------------------------------------------------
void xxSetPipelineGLES2(uint64_t commandEncoder, uint64_t pipeline)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(commandEncoder);
    PIPELINEGL* glPipeline = reinterpret_cast<PIPELINEGL*>(pipeline);
    VERTEXATTRIBUTEGL* vertexAttribute = glPipeline->vertexAttribute;
    VERTEXATTRIBUTEGL::Attribute* attributes = vertexAttribute->attributes;

    for (int i = 0; i < vertexAttribute->count; ++i)
    {
        VERTEXATTRIBUTEGL::Attribute& attribute = attributes[i];
        glEnableVertexAttribArray(attribute.index);
    }

    if (glPipeline->state.alphaBlending)
    {
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glDisable(GL_BLEND);
    }
    glPipeline->state.cull ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    glPipeline->state.depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glDepthMask(glPipeline->state.depthWrite ? GL_TRUE : GL_FALSE);
    glPipeline->state.scissor ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);

    glUseProgram(glPipeline->program);
    glUniform1i(glPipeline->texture, 0);
    glSwapchain->pipeline = pipeline;
}
//------------------------------------------------------------------------------
void xxSetIndexBufferGLES2(uint64_t commandEncoder, uint64_t buffer)
{
    BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glBuffer->buffer);
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersGLES2(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(commandEncoder);
 
    for (int i = 0; i < count; ++i)
    {
        glSwapchain->vertexBuffers[i] = buffers[i];
    }
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesGLES2(uint64_t commandEncoder, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesGLES2(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        TEXTUREGL* glTexture = reinterpret_cast<TEXTUREGL*>(textures[i]);
        if (glTexture == nullptr)
            continue;
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(glTexture->type, glTexture->texture);
        glSwapchain->textureTypes[i] = glTexture->type;
        glSwapchain->textureMipmaps[i] = (glTexture->mipmap > 1);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersGLES2(uint64_t commandEncoder, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersGLES2(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        SAMPLERGL glSampler = { samplers[i] };
        glActiveTexture(GL_TEXTURE0 + i);
        glTexParameteri(glSwapchain->textureTypes[i], GL_TEXTURE_WRAP_S, glSampler.addressU ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(glSwapchain->textureTypes[i], GL_TEXTURE_WRAP_T, glSampler.addressV ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(glSwapchain->textureTypes[i], GL_TEXTURE_MAG_FILTER, glSampler.magFilter ? GL_LINEAR : GL_NEAREST);

        GLenum minFilter;
        if (glSwapchain->textureMipmaps[i])
        {
            minFilter = glSampler.minFilter ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
        }
        else
        {
            minFilter = glSampler.minFilter ? GL_LINEAR : GL_NEAREST;
        }
        glTexParameteri(glSwapchain->textureTypes[i], GL_TEXTURE_MIN_FILTER, minFilter);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferGLES2(uint64_t commandEncoder, uint64_t buffer, int size)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(commandEncoder);
    PIPELINEGL* glPipeline = reinterpret_cast<PIPELINEGL*>(glSwapchain->pipeline);
    BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffer);

    glUniform4fv(glPipeline->uniform, size / sizeof(float) / 4, (GLfloat*)glBuffer->memory);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferGLES2(uint64_t commandEncoder, uint64_t buffer, int size)
{

}
//------------------------------------------------------------------------------
void xxDrawIndexedGLES2(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(commandEncoder);
    PIPELINEGL* glPipeline = reinterpret_cast<PIPELINEGL*>(glSwapchain->pipeline);
    VERTEXATTRIBUTEGL* vertexAttribute = glPipeline->vertexAttribute;
    VERTEXATTRIBUTEGL::Attribute* attributes = vertexAttribute->attributes;

    int currentStream = -1;
    for (int i = 0; i < vertexAttribute->count; ++i)
    {
        VERTEXATTRIBUTEGL::Attribute& attribute = attributes[i];
        if (currentStream != attribute.stream)
        {
            currentStream = attribute.stream;
            BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(glSwapchain->vertexBuffers[attribute.stream]);

            glBindBuffer(GL_ARRAY_BUFFER, glBuffer->buffer);
        }
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.pointer + vertexOffset * attribute.stride);
    }

    GLenum indexType = (INDEX_BUFFER_WIDTH == 2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    glDrawElements(GL_TRIANGLES, indexCount, indexType, (char*)nullptr + firstIndex * INDEX_BUFFER_WIDTH);
}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetTransformGLES2(uint64_t commandEncoder, const float* world, const float* view, const float* projection)
{

}
//==============================================================================
