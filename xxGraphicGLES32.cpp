//==============================================================================
// xxGraphic : OpenGL ES 3.2 Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#undef  GL_ES_VERSION_2_0
#undef  GL_ES_VERSION_3_0
#undef  GL_ES_VERSION_3_1
#undef  GL_ES_VERSION_3_2
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalGL.h"
#include "xxGraphicGL.h"
#include "xxGraphicGLES2.h"
#include "xxGraphicGLES3.h"
#include "xxGraphicGLES31.h"
#include "xxGraphicGLES32.h"

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
uint64_t xxCreateInstanceGLES32()
{
    uint64_t instance = 0;

#if defined(xxANDROID)
    instance = xxGraphicCreateEGL(320);
#endif

#if defined(xxMACOS)
    instance = xxGraphicCreateCGL(320);
#endif

#if defined(xxIOS) && !defined(xxMACCATALYST)
    instance = xxGraphicCreateEAGL(320);
#endif

#if defined(xxWINDOWS)
    instance = xxGraphicCreateWGL(320);
#endif

    if (instance == 0)
        return 0;

    xxRegisterFunction(GLES2);

    xxCreateInstance = xxCreateInstanceGLES32;
    xxGetDeviceName = xxGetDeviceNameGLES32;
    xxCreateIndexBuffer = xxCreateIndexBufferGLES3;
    xxCreateVertexBuffer = xxCreateVertexBufferGLES3;
    xxMapBuffer = xxMapBufferGLES3;
    xxUnmapBuffer = xxUnmapBufferGLES3;
    xxSetVertexBuffers = xxSetVertexBuffersGLES32;
    xxDrawIndexed = xxDrawIndexedGLES32;

    return instance;
}
//==============================================================================
//  Device
//==============================================================================
const char* xxGetDeviceNameGLES32()
{
    return "OpenGL ES 3.2";
}
//==============================================================================
//  Command
//==============================================================================
void xxSetVertexBuffersGLES32(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    VERTEXATTRIBUTEGL* glVertexAttribute = reinterpret_cast<VERTEXATTRIBUTEGL*>(vertexAttribute);
    VERTEXATTRIBUTEGL::Attribute* attributes = glVertexAttribute->attributes;

    int currentStream = -1;
    for (int i = 0; i < glVertexAttribute->count; ++i)
    {
        VERTEXATTRIBUTEGL::Attribute& attribute = attributes[i];
        if (currentStream != attribute.stream)
        {
            currentStream = attribute.stream;
            BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffers[attribute.stream]);

            glBindBuffer(GL_ARRAY_BUFFER, glBuffer->buffer);
        }
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.pointer);
    }
}
//------------------------------------------------------------------------------
void xxDrawIndexedGLES32(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    GLenum indexType = (INDEX_BUFFER_WIDTH == 2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    glDrawElementsInstancedBaseVertex(GL_TRIANGLES, indexCount, indexType, (char*)nullptr + firstIndex * INDEX_BUFFER_WIDTH, instanceCount, vertexOffset);
}
//==============================================================================
