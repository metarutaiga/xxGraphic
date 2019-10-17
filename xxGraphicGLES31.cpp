//==============================================================================
// xxGraphic : OpenGL ES 3.1 Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#undef  GL_ES_VERSION_2_0
#undef  GL_ES_VERSION_3_0
#undef  GL_ES_VERSION_3_1
#define GL_ES_VERSION_3_2 0
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalGL.h"
#include "xxGraphicGL.h"
#include "xxGraphicGLES2.h"
#include "xxGraphicGLES3.h"
#include "xxGraphicGLES31.h"

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

#define NUM_INDIRECT_BUFFER_SIZE    32768
static GLuint                       g_indirectBuffer;
static INDIRECTCOMMANDGL*           g_indirectCommand;
static int                          g_indirectIndex;

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceGLES31()
{
    uint64_t instance = 0;

#if defined(xxANDROID)
    instance = xxGraphicCreateEGL(310);
#endif

#if defined(xxMACOS)
    instance = xxGraphicCreateCGL(310);
#endif

#if defined(xxIOS) && !defined(xxMACCATALYST)
    instance = xxGraphicCreateEAGL(310);
#endif

#if defined(xxWINDOWS)
    instance = xxGraphicCreateWGL(310);
#endif

    if (instance == 0)
        return 0;

    GLuint indirectBuffer = 0;
    glGenBuffers(1, &indirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(INDIRECTCOMMANDGL) * NUM_INDIRECT_BUFFER_SIZE, nullptr, GL_STREAM_DRAW);

    g_indirectBuffer = indirectBuffer;
    g_indirectCommand = nullptr;
    g_indirectIndex = 0;

    xxRegisterFunction(GLES2);

    xxCreateInstance = xxCreateInstanceGLES31;
    xxDestroyInstance = xxDestroyInstanceGLES31;
    xxGetDeviceName = xxGetDeviceNameGLES31;
    xxCreateIndexBuffer = xxCreateIndexBufferGLES3;
    xxCreateVertexBuffer = xxCreateVertexBufferGLES3;
    xxMapBuffer = xxMapBufferGLES3;
    xxUnmapBuffer = xxUnmapBufferGLES3;
    xxPresentSwapchain = xxPresentSwapchainGLES31;
    xxGetCommandBuffer = xxGetCommandBufferGLES31;
    xxSetVertexBuffers = xxSetVertexBuffersGLES31;
    xxDrawIndexed = xxDrawIndexedGLES31;

    return instance;
}
//------------------------------------------------------------------------------
void xxDestroyInstanceGLES31(uint64_t instance)
{
    if (g_indirectBuffer != 0)
    {
        glDeleteBuffers(1, &g_indirectBuffer);
        g_indirectBuffer = 0;
    }

    xxDestroyInstanceGLES2(instance);
}
//==============================================================================
//  Device
//==============================================================================
const char* xxGetDeviceNameGLES31()
{
    return "OpenGL ES 3.1";
}
//==============================================================================
//  Swapchain
//==============================================================================
void xxPresentSwapchainGLES31(uint64_t swapchain)
{
    glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);

    g_indirectCommand = nullptr;

    xxPresentSwapchainGLES2(swapchain);
}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferGLES31(uint64_t device, uint64_t swapchain)
{
    uint64_t commandBuffer = xxGetCommandBufferGLES2(0, swapchain);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectBuffer);
    (void*&)g_indirectCommand = glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(INDIRECTCOMMANDGL) * NUM_INDIRECT_BUFFER_SIZE, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

    return commandBuffer;
}
//==============================================================================
//  Command
//==============================================================================
void xxSetVertexBuffersGLES31(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
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
void xxDrawIndexedGLES31(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    int index = g_indirectIndex++;
    if (g_indirectIndex >= NUM_INDIRECT_BUFFER_SIZE)
        g_indirectIndex = 0;

    INDIRECTCOMMANDGL& indirectCommand = g_indirectCommand[index];
    indirectCommand.count = indexCount;
    indirectCommand.instanceCount = instanceCount;
    indirectCommand.firstIndex = firstIndex;
    indirectCommand.baseVertex = vertexOffset;
    indirectCommand.baseInstance = firstInstance;

    glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (INDIRECTCOMMANDGL*)nullptr + index);
}
//==============================================================================
