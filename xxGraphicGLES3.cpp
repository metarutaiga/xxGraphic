//==============================================================================
// xxGraphic : OpenGL ES 3.0 Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#undef  GL_ES_VERSION_2_0
#undef  GL_ES_VERSION_3_0
#define GL_ES_VERSION_3_1 0
#define GL_ES_VERSION_3_2 0
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalGL.h"
#include "xxGraphicGL.h"
#include "xxGraphicGLES2.h"
#include "xxGraphicGLES3.h"

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

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceGLES3()
{
    uint64_t instance = 0;

#if defined(xxANDROID)
    instance = xxGraphicCreateEGL(300);
#endif

#if defined(xxMACOS)
    instance = xxGraphicCreateCGL(300);
#endif

#if defined(xxIOS) && !defined(xxMACCATALYST)
    instance = xxGraphicCreateEAGL(300);
#endif

#if defined(xxWINDOWS)
    instance = xxGraphicCreateWGL(300);
#endif

    if (instance == 0)
        return 0;

    xxRegisterFunction(GLES2);

    xxCreateInstance = xxCreateInstanceGLES3;
    xxGetDeviceName = xxGetDeviceNameGLES3;
    xxCreateIndexBuffer = xxCreateIndexBufferGLES3;
    xxCreateVertexBuffer = xxCreateVertexBufferGLES3;
    xxMapBuffer = xxMapBufferGLES3;
    xxUnmapBuffer = xxUnmapBufferGLES3;

    return instance;
}
//==============================================================================
//  Device
//==============================================================================
const char* xxGetDeviceNameGLES3()
{
    return "OpenGL ES 3.0";
}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateIndexBufferGLES3(uint64_t device, int size)
{
    BUFFERGL* glBuffer = xxAlloc(BUFFERGL);
    if (glBuffer == nullptr)
        return 0;

    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_STREAM_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBuffer->type = GL_ELEMENT_ARRAY_BUFFER;
    glBuffer->buffer = buffer;
    glBuffer->memory = nullptr;
    glBuffer->size = size;

    return reinterpret_cast<uint64_t>(glBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferGLES3(uint64_t device, int size, uint64_t vertexAttribute)
{
    BUFFERGL* glBuffer = xxAlloc(BUFFERGL);
    if (glBuffer == nullptr)
        return 0;

    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBuffer->type = GL_ARRAY_BUFFER;
    glBuffer->buffer = buffer;
    glBuffer->memory = nullptr;
    glBuffer->size = size;

    return reinterpret_cast<uint64_t>(glBuffer);
}
//------------------------------------------------------------------------------
void* xxMapBufferGLES3(uint64_t device, uint64_t buffer)
{
    BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffer);
    if (glBuffer == nullptr)
        return nullptr;
    if (glBuffer->memory)
        return glBuffer->memory;

    glBindBuffer(glBuffer->type, glBuffer->buffer);
    return glMapBufferRange(glBuffer->type, 0, glBuffer->size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
}
//------------------------------------------------------------------------------
void xxUnmapBufferGLES3(uint64_t device, uint64_t buffer)
{
    BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffer);
    if (glBuffer == nullptr)
        return;
    if (glBuffer->memory)
        return;

    glBindBuffer(glBuffer->type, glBuffer->buffer);
    glUnmapBuffer(glBuffer->type);
}
//==============================================================================
