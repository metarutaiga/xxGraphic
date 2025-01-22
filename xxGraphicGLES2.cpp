//==============================================================================
// xxGraphic : OpenGL ES 2.0 Source
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#undef  GL_ES_VERSION_2_0
#define GL_ES_VERSION_3_0 0
#define GL_ES_VERSION_3_1 0
#define GL_ES_VERSION_3_2 0
#include "internal/xxGraphicInternalGL.h"
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
#define GL_UNIFORM_BUFFER           0x8A11
#endif

#ifndef GL_SHADER_STORAGE_BUFFER
#define GL_SHADER_STORAGE_BUFFER    0x90D2
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
    xxLog("xxGraphic", "%s %s %s %s", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
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
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t xxCreateSwapchainGLES2(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain)
{
    GLSWAPCHAIN* glOldSwapchain = reinterpret_cast<GLSWAPCHAIN*>(oldSwapchain);
    if (glOldSwapchain && glOldSwapchain->view == view && glOldSwapchain->width == width && glOldSwapchain->height == height)
        return oldSwapchain;
    GLSWAPCHAIN* glSwapchain = xxAlloc(GLSWAPCHAIN);
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
    memset(glSwapchain->textureTargets, 0, sizeof(glSwapchain->textureTargets));
    memset(glSwapchain->textureMipmaps, 0, sizeof(glSwapchain->textureMipmaps));

    return reinterpret_cast<uint64_t>(glSwapchain);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainGLES2(uint64_t swapchain)
{
    GLSWAPCHAIN* glSwapchain = reinterpret_cast<GLSWAPCHAIN*>(swapchain);
    if (glSwapchain == nullptr)
        return;

    glDestroyContext(glSwapchain->context, glSwapchain->view, glSwapchain->display);
    xxFree(glSwapchain);
}
//------------------------------------------------------------------------------
void xxPresentSwapchainGLES2(uint64_t swapchain)
{
    GLSWAPCHAIN* glSwapchain = reinterpret_cast<GLSWAPCHAIN*>(swapchain);
    if (glSwapchain == nullptr)
        return;

    glPresentContext(glSwapchain->context, glSwapchain->display);
}
//==============================================================================
//  Framebuffer
//==============================================================================
uint64_t xxCreateFramebufferGLES2(uint64_t device, uint64_t texture)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyFramebufferGLES2(uint64_t framebuffer)
{

}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferGLES2(uint64_t device, uint64_t swapchain, float* scale)
{
    GLSWAPCHAIN* glSwapchain = reinterpret_cast<GLSWAPCHAIN*>(swapchain);

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
uint64_t xxGetCommandBufferGLES2(uint64_t device, uint64_t swapchain)
{
    GLSWAPCHAIN* glSwapchain = reinterpret_cast<GLSWAPCHAIN*>(swapchain);
    if (glSwapchain == nullptr)
        return 0;

    glMakeCurrentContext(glSwapchain->context, glSwapchain->display);
    glViewport(0, 0, glSwapchain->width, glSwapchain->height);
    glScissor(0, 0, glSwapchain->width, glSwapchain->height);
    glFrontFace(GL_CW);

    glSwapchain->pipeline = 0;
    memset(glSwapchain->vertexBuffers, 0, sizeof(glSwapchain->vertexBuffers));
    memset(glSwapchain->textureTargets, 0, sizeof(glSwapchain->textureTargets));
    memset(glSwapchain->textureMipmaps, 0, sizeof(glSwapchain->textureMipmaps));

    return reinterpret_cast<uint64_t>(glSwapchain);
}
//------------------------------------------------------------------------------
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

    if (mask)
    {
        if (mask & GL_COLOR_BUFFER_BIT)
        {
            glClearColor(color[0], color[1], color[2], color[3]);
        }
        if (mask & (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT))
        {
            glDepthMask(true);
        }
        if (mask & GL_DEPTH_BUFFER_BIT)
        {
            glClearDepthf(depth);
        }
        if (mask & GL_STENCIL_BUFFER_BIT)
        {
            glClearStencil(stencil);
        }
        glViewport(0, 0, width, height);
        glScissor(0, 0, width, height);
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
    GLVERTEXATTRIBUTE* glVertexAttribute = xxAlloc(GLVERTEXATTRIBUTE);
    if (glVertexAttribute == nullptr)
        return 0;

    GLVERTEXATTRIBUTE::Attribute* attributes = glVertexAttribute->attributes;
    int normalIndex = 0;
    int textureIndex = 0;
    int stride = 0;

    for (int i = 0; i < count; ++i)
    {
        int stream = (*attribute++);
        int offset = (*attribute++);
        int element = (*attribute++);
        int size = (*attribute++);

        stride += size;

        attributes[i].index = i;
        attributes[i].size = (element & 0xFF) - '0';
        attributes[i].type = GL_FLOAT;
        attributes[i].normalized = GL_FALSE;
        attributes[i].stride = 0;
        attributes[i].pointer = (char*)nullptr + offset;
        attributes[i].stream = stream;

        if (element == 'POS3' && size == sizeof(float) * 3)
        {
            attributes[i].name = "attrPosition";
            continue;
        }

        if (element == 'BON3' && size == sizeof(float) * 3)
        {
            attributes[i].name = "attrBoneWeight";
            continue;
        }

        if (element == 'BON4' && size == sizeof(char) * 4)
        {
            attributes[i].type = GL_UNSIGNED_BYTE;
            attributes[i].name = "attrBoneIndices";
            continue;
        }

        if (element == 'NOR3' && size == sizeof(float) * 3)
        {
            switch (normalIndex)
            {
            case 0: attributes[i].name = "attrNormal"; break;
            case 1: attributes[i].name = "attrTangent"; break;
            case 2: attributes[i].name = "attrBinormal"; break;
            }
            normalIndex++;
            continue;
        }

        if (element == 'COL4' && size == sizeof(char) * 4)
        {
#if defined(xxWINDOWS)
            attributes[i].size = GL_BGRA_EXT;
#else
            attributes[i].size = 4;
#endif
            attributes[i].type = GL_UNSIGNED_BYTE;
            attributes[i].normalized = GL_TRUE;
            attributes[i].name = "attrColor";
            continue;
        }

        if (element == 'TEX2' && size == sizeof(float) * 2)
        {
            switch (textureIndex)
            {
            case 0: attributes[i].name = "attrUV0"; break;
            case 1: attributes[i].name = "attrUV1"; break;
            case 2: attributes[i].name = "attrUV2"; break;
            case 3: attributes[i].name = "attrUV3"; break;
            case 4: attributes[i].name = "attrUV4"; break;
            case 5: attributes[i].name = "attrUV5"; break;
            case 6: attributes[i].name = "attrUV6"; break;
            case 7: attributes[i].name = "attrUV7"; break;
            }
            textureIndex++;
            continue;
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
    GLVERTEXATTRIBUTE* glVertexAttribute = reinterpret_cast<GLVERTEXATTRIBUTE*>(vertexAttribute);

    xxFree(glVertexAttribute);
}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateConstantBufferGLES2(uint64_t device, int size)
{
    GLBUFFER* glBuffer = xxAlloc(GLBUFFER);
    if (glBuffer == nullptr)
        return 0;

    glBuffer->type = GL_UNIFORM_BUFFER;
    glBuffer->buffer = 0;
    glBuffer->memory = xxAlloc(char, size);
    glBuffer->size = size;

    return reinterpret_cast<uint64_t>(glBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferGLES2(uint64_t device, int size, int bits)
{
    GLBUFFER* glBuffer = xxAlloc(GLBUFFER);
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
    GLBUFFER* glBuffer = xxAlloc(GLBUFFER);
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
uint64_t xxCreateStorageBufferGLES2(uint64_t device, int size)
{
    GLBUFFER* glBuffer = xxAlloc(GLBUFFER);
    if (glBuffer == nullptr)
        return 0;

    glBuffer->type = GL_SHADER_STORAGE_BUFFER;
    glBuffer->buffer = 0;
    glBuffer->memory = xxAlloc(char, size);
    glBuffer->size = size;

    return reinterpret_cast<uint64_t>(glBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateInstanceBufferGLES2(uint64_t device, int size)
{
    GLBUFFER* glBuffer = xxAlloc(GLBUFFER);
    if (glBuffer == nullptr)
        return 0;

    glBuffer->type = GL_ARRAY_BUFFER;
    glBuffer->buffer = 0;
    glBuffer->memory = xxAlloc(char, size);
    glBuffer->size = size;

    return reinterpret_cast<uint64_t>(glBuffer);
}
//------------------------------------------------------------------------------
void xxDestroyBufferGLES2(uint64_t device, uint64_t buffer)
{
    GLBUFFER* glBuffer = reinterpret_cast<GLBUFFER*>(buffer);
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
    GLBUFFER* glBuffer = reinterpret_cast<GLBUFFER*>(buffer);
    if (glBuffer == nullptr)
        return nullptr;

    return glBuffer->memory;
}
//------------------------------------------------------------------------------
void xxUnmapBufferGLES2(uint64_t device, uint64_t buffer)
{
    GLBUFFER* glBuffer = reinterpret_cast<GLBUFFER*>(buffer);
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
uint64_t xxCreateTextureGLES2(uint64_t device, uint64_t format, int width, int height, int depth, int mipmap, int array, void const* external)
{
    if (width == 0 || height == 0 || depth == 0 || mipmap == 0 || array == 0)
        return 0;

    GLTEXTURE* glTexture = xxAlloc(GLTEXTURE);
    if (glTexture == nullptr)
        return 0;

    GLuint texture = 0;
    glGenTextures(1, &texture);
    if (texture == 0)
    {
        xxFree(glTexture);
        return 0;
    }

    struct { GLenum pixelFormat; GLenum pixelType; int onePixel; int stride; } T;
    switch (format)
    {
    case "RGB565"_CC:   T = { GL_RGB,       GL_UNSIGNED_SHORT_5_6_5_REV,                2,  width * 2 }; break;
    case "BGR565"_CC:   T = { GL_RGB,       GL_UNSIGNED_SHORT_5_6_5,                    2,  width * 2 }; break;
    case "ARGB1555"_CC: T = { GL_BGRA_EXT,  GL_UNSIGNED_SHORT_5_5_5_1,                  2,  width * 2 }; break;
    case "ABGR1555"_CC: T = { GL_RGBA,      GL_UNSIGNED_SHORT_5_5_5_1,                  2,  width * 2 }; break;
    case "ARGB4444"_CC: T = { GL_BGRA_EXT,  GL_UNSIGNED_SHORT_4_4_4_4,                  2,  width * 2 }; break;
    case "ABGR4444"_CC: T = { GL_RGBA,      GL_UNSIGNED_SHORT_4_4_4_4,                  2,  width * 2 }; break;
    case "RGBA4444"_CC: T = { GL_RGBA,      GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT,          2,  width * 2 }; break;
    case "BGRA4444"_CC: T = { GL_BGRA_EXT,  GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT,          2,  width * 2 }; break;
    case "RGBA5551"_CC: T = { GL_RGBA,      GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT,          2,  width * 2 }; break;
    case "BGRA5551"_CC: T = { GL_BGRA_EXT,  GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT,          2,  width * 2 }; break;
    case "ARGB8888"_CC: T = { GL_BGRA_EXT,  GL_UNSIGNED_INT_8_8_8_8,                    4,  width * 4 }; break;
    case "RGBA8888"_CC: T = { GL_RGBA,      GL_UNSIGNED_BYTE,                           4,  width * 4 }; break;
    case "ABGR8888"_CC: T = { GL_RGBA,      GL_UNSIGNED_INT_8_8_8_8,                    4,  width * 4 }; break;
    case "BGRA8888"_CC: T = { GL_BGRA_EXT,  GL_UNSIGNED_BYTE,                           4,  width * 4 }; break;
    case "DS24"_cc:     T = { GL_DEPTH_STENCIL_OES, GL_DEPTH24_STENCIL8_OES,            4,  width * 4 }; break;
    case "BC1"_cc:
    case "DXT1"_cc:     T = { 0,            GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,           8,  (width + 3) / 4 * 8 };  break;
    case "BC2"_cc:
    case "DXT3"_cc:     T = { 0,            GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,           16, (width + 3) / 4 * 16 }; break;
    case "BC3"_cc:
    case "DXT5"_cc:     T = { 0,            GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,           16, (width + 3) / 4 * 16 }; break;
    case "BC4S"_cc:     T = { 0,            GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,         8,  (width + 3) / 4 * 8 };  break;
    case "BC4U"_cc:
    case "ATI1"_cc:     T = { 0,            GL_COMPRESSED_RED_RGTC1_EXT,                8,  (width + 3) / 4 * 8 };  break;
    case "BC5S"_cc:     T = { 0,            GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT,   16, (width + 3) / 4 * 16 }; break;
    case "BC5U"_cc:
    case "ATI2"_cc:     T = { 0,            GL_COMPRESSED_RED_GREEN_RGTC2_EXT,          16, (width + 3) / 4 * 16 }; break;
    case "BC6H"_cc:     T = { 0,            GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_EXT,    16, (width + 3) / 4 * 16 }; break;
    case "BC7"_cc:      T = { 0,            GL_COMPRESSED_RGBA_BPTC_UNORM_EXT,          16, (width + 3) / 4 * 16 }; break;
    default:
        xxLog("xxGraphic", "Unknown format (%.8s)", &format);
        return 0;
    }

    glTexture->texture = texture;
    glTexture->target = 0;
    glTexture->format = T.pixelFormat;
    glTexture->type = T.pixelType;
    glTexture->memory = nullptr;
    glTexture->width = width;
    glTexture->height = height;
    glTexture->depth = depth;
    glTexture->mipmap = mipmap;
    glTexture->array = array;
    glTexture->external = external;
    glTexture->image = nullptr;
    glTexture->device = nullptr;
    for (int i = 0; i < mipmap; ++i)
    {
        glTexture->strides[i] = (T.stride >> i);
        if (glTexture->strides[i] < T.onePixel)
            glTexture->strides[i] = T.onePixel;
    }

    GLuint externalTexture = static_cast<GLuint>(reinterpret_cast<size_t>(external));
    if (glIsTexture(externalTexture))
    {
        glDeleteTextures(1, &texture);
        glTexture->texture = externalTexture;
        glTexture->target = GL_TEXTURE_2D;
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
        glTexture->target = GL_TEXTURE_2D;
        glTexture->image = xxCreateImageFromHardwareBuffer(external);
        xxBindTextureWithImage(glTexture->image);
#elif defined(xxMACOS)
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture);
        glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexture->target = GL_TEXTURE_RECTANGLE_ARB;
        xxBindTextureWithSurface(glTexture->external);
#elif defined(xxMACCATALYST)
#elif defined(xxIOS)
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexture->target = GL_TEXTURE_2D;
        xxBindTextureWithSurface(glTexture->external);
#elif defined(xxWINDOWS)
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexture->target = GL_TEXTURE_2D;
        glTexture->device = xxGetDeviceFromDirect3DTexture(external);
        glTexture->image = xxCreateImageFromDirect3DTexture(glTexture->device, external, texture);
#endif
    }

    return reinterpret_cast<uint64_t>(glTexture);
}
//------------------------------------------------------------------------------
void xxDestroyTextureGLES2(uint64_t texture)
{
    GLTEXTURE* glTexture = reinterpret_cast<GLTEXTURE*>(texture);
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
    GLTEXTURE* glTexture = reinterpret_cast<GLTEXTURE*>(texture);
    if (glTexture == nullptr)
        return nullptr;

#if defined(xxANDROID)
    if (glTexture->image)
        return nullptr;
#endif

    unsigned int height = glTexture->height >> level;
    unsigned int depth = glTexture->depth >> level;
    if (height == 0)
        height = 1;
    if (depth == 0)
        depth = 1;

    glTexture->memory = xxRealloc(glTexture->memory, char, glTexture->strides[level] * height * depth);

    (*stride) = glTexture->strides[level];
    return glTexture->memory;
}
//------------------------------------------------------------------------------
void xxUnmapTextureGLES2(uint64_t device, uint64_t texture, int level, int array)
{
    GLTEXTURE* glTexture = reinterpret_cast<GLTEXTURE*>(texture);
    if (glTexture == nullptr)
        return;

#if defined(xxANDROID)
    if (glTexture->image)
        return;
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

    if (glTexture->depth == 1 && glTexture->array == 1)
    {
        glBindTexture(GL_TEXTURE_2D, glTexture->texture);
        if (glTexture->target == 0)
        {
            glTexture->target = GL_TEXTURE_2D;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glTexture->mipmap > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        }

        switch (glTexture->format)
        {
        case GL_RGB:
            glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, width, height, 0, glTexture->format, glTexture->type, glTexture->memory);
            break;
        case GL_RGBA:
        case GL_BGRA_EXT:
            glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, width, height, 0, glTexture->format, glTexture->type, glTexture->memory);
            break;
        case GL_DEPTH24_STENCIL8_OES:
            glTexImage2D(GL_TEXTURE_2D, level, GL_DEPTH_STENCIL_OES, width, height, 0, glTexture->format, glTexture->type, glTexture->memory);
            break;
        case 0:
            GLsizei imageSize = glTexture->strides[level] * ((height + 3) / 4);
            glCompressedTexImage2D(GL_TEXTURE_2D, level, glTexture->type, width, height, 0, imageSize, glTexture->memory);
            break;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (glTexture->width == glTexture->height && glTexture->depth == 1 && glTexture->array == 6)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, glTexture->texture);
        if (glTexture->target == 0)
        {
            glTexture->target = GL_TEXTURE_CUBE_MAP;
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, glTexture->mipmap > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        }

        switch (glTexture->format)
        {
        case GL_RGB:
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + array, level, GL_RGB, width, height, 0, glTexture->format, glTexture->type, glTexture->memory);
            break;
        case GL_RGBA:
        case GL_BGRA_EXT:
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + array, level, GL_RGBA, width, height, 0, glTexture->format, glTexture->type, glTexture->memory);
            break;
        case 0:
            GLsizei imageSize = glTexture->strides[level] * ((height + 3) / 4);
            glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + array, level, glTexture->type, width, height, 0, imageSize, glTexture->memory);
            break;
        }
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
    GLSAMPLER glSampler = {};

    glSampler.addressU = clampU;
    glSampler.addressV = clampV;
    glSampler.addressW = clampW;
    glSampler.magFilter = linearMag;
    glSampler.minFilter = linearMin;
    glSampler.mipFilter = linearMip;
    glSampler.anisotropy = anisotropy;

    return static_cast<uint64_t>(glSampler.value);
}
//------------------------------------------------------------------------------
void xxDestroySamplerGLES2(uint64_t sampler)
{

}
//==============================================================================
//  Shader
//==============================================================================
static void checkShader(GLuint glShader, char const* shader)
{
    GLint status = 0;
    glGetShaderiv(glShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        xxLog("xxGraphic", "failed to compile %d", glShader);
        char* dup = strdup(shader);
        if (dup)
        {
            int index = 0;
            char* lasts = dup;
            char* line = strsep(&lasts, "\r\n");
            while (line)
            {
                xxLog("xxGraphic", "%d : %s", index++, line);
                line = strsep(&lasts, "\r\n");
            }
            free(dup);
        }

        GLint length = 0;
        glGetShaderiv(glShader, GL_INFO_LOG_LENGTH, &length);
        char* log = xxAlloc(char, length + 1);
        if (log)
        {
            glGetShaderInfoLog(glShader, length, nullptr, log);
            log[length] = '\0';

            char* lasts = log;
            char* line = strsep(&lasts, "\r\n");
            while (line)
            {
                xxLog("xxGraphic", "%s", line);
                line = strsep(&lasts, "\r\n");
            }
            xxFree(log);
        }
    }
}
//------------------------------------------------------------------------------
uint64_t xxCreateMeshShaderGLES2(uint64_t device, char const* shader)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexShaderGLES2(uint64_t device, char const* shader, uint64_t vertexAttribute)
{
    static char const* const macro[][2] =
    {
        { "SHADER_GLSL", "1" },
        { "SHADER_VERTEX", "1" },
        { "uniBuffer", "uniBufferVS" },
        { "uniform", "uniform highp" },
        { "attribute", "attribute" },
        { "varying", "varying" },
    };
    constexpr size_t size = xxCountOf(macro);

    if (strcmp(shader, "default") == 0)
    {
        shader = glDefaultShaderCode;
    }

    GLsizei count = 0;
    char const* sources[2 + size * 6 + 2 + 1];
    sources[count++] = "#version 100";
    sources[count++] = "\n";
    for (size_t i = 0; i < size; ++i)
    {
        sources[count++] = "#define";
        sources[count++] = " ";
        sources[count++] = macro[i][0];
        sources[count++] = " ";
        sources[count++] = macro[i][1];
        sources[count++] = "\n";
    }
    sources[count++] = "precision highp float;";
    sources[count++] = "\n";
    sources[count++] = shader;

    GLuint glShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(glShader, count, sources, nullptr);
    glCompileShader(glShader);
    checkShader(glShader, shader);

    return static_cast<uint64_t>(glShader);
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderGLES2(uint64_t device, char const* shader)
{
    static char const* const macro[][2] =
    {
        { "SHADER_GLSL", "1" },
        { "SHADER_FRAGMENT", "1" },
        { "uniBuffer", "uniBufferFS" },
        { "uniform", "uniform highp" },
        { "attribute", "" },
        { "varying", "varying" },
    };
    constexpr size_t size = xxCountOf(macro);

    if (strcmp(shader, "default") == 0)
    {
        shader = glDefaultShaderCode;
    }

    GLsizei count = 0;
    char const* sources[2 + size * 6 + 2 + 1];
    sources[count++] = "#version 100";
    sources[count++] = "\n";
    for (size_t i = 0; i < size; ++i)
    {
        sources[count++] = "#define";
        sources[count++] = " ";
        sources[count++] = macro[i][0];
        sources[count++] = " ";
        sources[count++] = macro[i][1];
        sources[count++] = "\n";
    }
    sources[count++] = "precision mediump float;";
    sources[count++] = "\n";
    sources[count++] = shader;

    GLuint glShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(glShader, count, sources, nullptr);
    glCompileShader(glShader);
    checkShader(glShader, shader);

    return static_cast<uint64_t>(glShader);
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
        xxLog("xxGraphic", "failed to link %u", glProgram);

        GLint length = 0;
        glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &length);
        char* log = xxAlloc(char, length + 1);
        if (log)
        {
            glGetProgramInfoLog(glProgram, length, nullptr, log);
            log[length] = '\0';

            char* lasts = log;
            char* line = strsep(&lasts, "\r\n");
            while (line)
            {
                xxLog("xxGraphic", "%s", line);
                line = strsep(&lasts, "\r\n");
            }
            xxFree(log);
        }
    }
}
//------------------------------------------------------------------------------
uint64_t xxCreateBlendStateGLES2(uint64_t device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha)
{
    GLBLEND* glBlend = xxAlloc(GLBLEND);
    if (glBlend == nullptr)
        return 0;
    glBlend->blendSourceColor = glBlendFactor(sourceColor);
    glBlend->blendFunctionColor = glBlendOp(operationColor);
    glBlend->blendDestinationColor = glBlendFactor(destinationColor);
    glBlend->blendSourceAlpha = glBlendFactor(sourceAlpha);
    glBlend->blendFunctionAlpha = glBlendOp(operationAlpha);
    glBlend->blendDestinationAlpha = glBlendFactor(destinationAlpha);
    glBlend->blendEnable = (glBlend->blendSourceColor != GL_ONE ||
                            glBlend->blendFunctionColor != GL_ZERO ||
                            glBlend->blendDestinationColor != GL_FUNC_ADD ||
                            glBlend->blendSourceAlpha != GL_ONE ||
                            glBlend->blendFunctionAlpha != GL_FUNC_ADD ||
                            glBlend->blendDestinationAlpha != GL_FUNC_ADD) ? GL_TRUE : GL_FALSE;
    return reinterpret_cast<uint64_t>(glBlend);
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateGLES2(uint64_t device, char const* depthTest, bool depthWrite)
{
    GLSTATE glState = {};
    glState.depthTest = glCompareOp(depthTest);
    glState.depthWrite = depthWrite;
    return static_cast<uint64_t>(glState.value);
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateGLES2(uint64_t device, bool cull, bool scissor)
{
    GLSTATE glState = {};
    glState.cull = cull;
    glState.scissor = scissor;
    return static_cast<uint64_t>(glState.value);
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineGLES2(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t meshShader, uint64_t vertexShader, uint64_t fragmentShader)
{
    GLVERTEXATTRIBUTE* glVertexAttribute = reinterpret_cast<GLVERTEXATTRIBUTE*>(vertexAttribute);
    if (glVertexAttribute == nullptr)
        return 0;
    GLPIPELINE* glPipeline = xxAlloc(GLPIPELINE);
    if (glPipeline == nullptr)
        return 0;

    GLBLEND* glBlendState = reinterpret_cast<GLBLEND*>(blendState);
    GLSTATE glDepthStencilState = { depthStencilState };
    GLSTATE glRasterizerState = { rasterizerState };

    GLuint glProgram = glCreateProgram();

    GLVERTEXATTRIBUTE::Attribute* attributes = glVertexAttribute->attributes;
    for (int i = 0; i < glVertexAttribute->count; ++i)
    {
        GLVERTEXATTRIBUTE::Attribute& attribute = attributes[i];
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
    glPipeline->texture = glGetUniformLocation(glProgram, "samDiffuse");
    glPipeline->uniformVS = glGetUniformLocation(glProgram, "uniBufferVS");
    glPipeline->uniformFS = glGetUniformLocation(glProgram, "uniBufferFS");
    glPipeline->blend = glBlendState ? (*glBlendState) : GLBLEND{ .blendEnable = GL_FALSE };
    glPipeline->state.depthTest = glDepthStencilState.depthTest;
    glPipeline->state.depthWrite = glDepthStencilState.depthWrite;
    glPipeline->state.cull = glRasterizerState.cull;
    glPipeline->state.scissor = glRasterizerState.scissor;

    return reinterpret_cast<uint64_t>(glPipeline);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateGLES2(uint64_t blendState)
{
    GLBLEND* glBlend = reinterpret_cast<GLBLEND*>(blendState);

    xxFree(glBlend);
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
    GLPIPELINE* glPipeline = reinterpret_cast<GLPIPELINE*>(pipeline);
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
    GLSWAPCHAIN* glSwapchain = reinterpret_cast<GLSWAPCHAIN*>(commandEncoder);

    y = (int)(glSwapchain->height * glSwapchain->scale) - y - height;
    glScissor(x, y, width, height);
}
//------------------------------------------------------------------------------
void xxSetPipelineGLES2(uint64_t commandEncoder, uint64_t pipeline)
{
    GLSWAPCHAIN* glSwapchain = reinterpret_cast<GLSWAPCHAIN*>(commandEncoder);
    GLPIPELINE* glPipeline = reinterpret_cast<GLPIPELINE*>(pipeline);
    GLVERTEXATTRIBUTE* vertexAttribute = glPipeline->vertexAttribute;
    GLVERTEXATTRIBUTE::Attribute* attributes = vertexAttribute->attributes;

    for (int i = 0; i < vertexAttribute->count; ++i)
    {
        GLVERTEXATTRIBUTE::Attribute& attribute = attributes[i];
        glEnableVertexAttribArray(attribute.index);
    }

    if (glPipeline->blend.blendEnable)
    {
        glEnable(GL_BLEND);
        glBlendEquation(glPipeline->blend.blendFunctionColor);
        glBlendFunc(glPipeline->blend.blendSourceColor, glPipeline->blend.blendDestinationColor);
    }
    else
    {
        glDisable(GL_BLEND);
    }
    glPipeline->state.cull ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    if (glPipeline->state.depthTest != GL_ALWAYS)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(glPipeline->state.depthTest);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    glDepthMask(glPipeline->state.depthWrite ? GL_TRUE : GL_FALSE);
    glPipeline->state.scissor ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);

    glUseProgram(glPipeline->program);
    glUniform1i(glPipeline->texture, 0);
    glSwapchain->pipeline = pipeline;
}
//------------------------------------------------------------------------------
void xxSetMeshBuffersGLES2(uint64_t commandEncoder, int count, const uint64_t* buffers)
{

}
//------------------------------------------------------------------------------
void xxSetVertexBuffersGLES2(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    GLSWAPCHAIN* glSwapchain = reinterpret_cast<GLSWAPCHAIN*>(commandEncoder);
 
    for (int i = 0; i < count; ++i)
    {
        glSwapchain->vertexBuffers[i] = buffers[i];
    }
}
//------------------------------------------------------------------------------
void xxSetMeshTexturesGLES2(uint64_t commandEncoder, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetVertexTexturesGLES2(uint64_t commandEncoder, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesGLES2(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    GLSWAPCHAIN* glSwapchain = reinterpret_cast<GLSWAPCHAIN*>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        GLTEXTURE* glTexture = reinterpret_cast<GLTEXTURE*>(textures[i]);
        if (glTexture == nullptr)
            continue;
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(glTexture->target, glTexture->texture);
        glSwapchain->textureTargets[i] = glTexture->target;
        glSwapchain->textureMipmaps[i] = (glTexture->mipmap > 1);
    }
}
//------------------------------------------------------------------------------
void xxSetMeshSamplersGLES2(uint64_t commandEncoder, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetVertexSamplersGLES2(uint64_t commandEncoder, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersGLES2(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    GLSWAPCHAIN* glSwapchain = reinterpret_cast<GLSWAPCHAIN*>(commandEncoder);

    for (int i = 0; i < count; ++i)
    {
        GLSAMPLER glSampler = { samplers[i] };
        glActiveTexture(GL_TEXTURE0 + i);
        glTexParameteri(glSwapchain->textureTargets[i], GL_TEXTURE_WRAP_S, glSampler.addressU ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(glSwapchain->textureTargets[i], GL_TEXTURE_WRAP_T, glSampler.addressV ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(glSwapchain->textureTargets[i], GL_TEXTURE_MAG_FILTER, glSampler.magFilter ? GL_LINEAR : GL_NEAREST);

        GLenum minFilter;
        if (glSwapchain->textureMipmaps[i])
        {
            minFilter = glSampler.minFilter ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
        }
        else
        {
            minFilter = glSampler.minFilter ? GL_LINEAR : GL_NEAREST;
        }
        glTexParameteri(glSwapchain->textureTargets[i], GL_TEXTURE_MIN_FILTER, minFilter);
    }
}
//------------------------------------------------------------------------------
void xxSetMeshConstantBufferGLES2(uint64_t commandEncoder, uint64_t buffer, int size)
{
    
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferGLES2(uint64_t commandEncoder, uint64_t buffer, int size)
{
    GLSWAPCHAIN* glSwapchain = reinterpret_cast<GLSWAPCHAIN*>(commandEncoder);
    GLPIPELINE* glPipeline = reinterpret_cast<GLPIPELINE*>(glSwapchain->pipeline);
    GLBUFFER* glBuffer = reinterpret_cast<GLBUFFER*>(buffer);

    glUniform4fv(glPipeline->uniformVS, size / sizeof(float) / 4, (GLfloat*)glBuffer->memory);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferGLES2(uint64_t commandEncoder, uint64_t buffer, int size)
{
    GLSWAPCHAIN* glSwapchain = reinterpret_cast<GLSWAPCHAIN*>(commandEncoder);
    GLPIPELINE* glPipeline = reinterpret_cast<GLPIPELINE*>(glSwapchain->pipeline);
    GLBUFFER* glBuffer = reinterpret_cast<GLBUFFER*>(buffer);

    glUniform4fv(glPipeline->uniformFS, size / sizeof(float) / 4, (GLfloat*)glBuffer->memory);
}
//------------------------------------------------------------------------------
void xxDrawGLES2(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance)
{
    GLSWAPCHAIN* glSwapchain = reinterpret_cast<GLSWAPCHAIN*>(commandEncoder);
    GLPIPELINE* glPipeline = reinterpret_cast<GLPIPELINE*>(glSwapchain->pipeline);
    GLVERTEXATTRIBUTE* vertexAttribute = glPipeline->vertexAttribute;
    GLVERTEXATTRIBUTE::Attribute* attributes = vertexAttribute->attributes;

    int currentStream = -1;
    for (int i = 0; i < vertexAttribute->count; ++i)
    {
        GLVERTEXATTRIBUTE::Attribute& attribute = attributes[i];
        if (currentStream != attribute.stream)
        {
            currentStream = attribute.stream;
            GLBUFFER* glBuffer = reinterpret_cast<GLBUFFER*>(glSwapchain->vertexBuffers[attribute.stream]);

            glBindBuffer(GL_ARRAY_BUFFER, glBuffer->buffer);
        }
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.pointer);
    }

    glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
}
//------------------------------------------------------------------------------
void xxDrawMeshedGLES2(uint64_t commandEncoder, int x, int y, int z)
{
    
}
//------------------------------------------------------------------------------
void xxDrawIndexedGLES2(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int vertexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    GLSWAPCHAIN* glSwapchain = reinterpret_cast<GLSWAPCHAIN*>(commandEncoder);
    GLBUFFER* glIndexBuffer = reinterpret_cast<GLBUFFER*>(indexBuffer);
    GLPIPELINE* glPipeline = reinterpret_cast<GLPIPELINE*>(glSwapchain->pipeline);
    GLVERTEXATTRIBUTE* vertexAttribute = glPipeline->vertexAttribute;
    GLVERTEXATTRIBUTE::Attribute* attributes = vertexAttribute->attributes;

    int currentStream = -1;
    for (int i = 0; i < vertexAttribute->count; ++i)
    {
        GLVERTEXATTRIBUTE::Attribute& attribute = attributes[i];
        if (currentStream != attribute.stream)
        {
            currentStream = attribute.stream;
            GLBUFFER* glBuffer = reinterpret_cast<GLBUFFER*>(glSwapchain->vertexBuffers[attribute.stream]);

            glBindBuffer(GL_ARRAY_BUFFER, glBuffer->buffer);
        }
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.pointer + vertexOffset * attribute.stride);
    }

    GLenum indexType = vertexCount < 65536 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer->buffer);
    glDrawElements(GL_TRIANGLES, indexCount, indexType, (char*)nullptr + firstIndex * (vertexCount < 65536 ? 2 : 4));
}
//==============================================================================
