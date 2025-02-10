//==============================================================================
// xxGraphic : OpenGL ES 3.1 Source
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#undef  GL_ES_VERSION_2_0
#undef  GL_ES_VERSION_3_0
#undef  GL_ES_VERSION_3_1
#define GL_ES_VERSION_3_2 0
#include "internal/xxGraphicInternalGL.h"
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
static void*                        g_indirectCommand;
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
    glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * NUM_INDIRECT_BUFFER_SIZE, nullptr, GL_STREAM_DRAW);

    g_indirectBuffer = indirectBuffer;
    g_indirectCommand = nullptr;
    g_indirectIndex = 0;

    xxRegisterFunction(GLES2);

    xxRegisterFunctionSingle(xxCreateInstance, xxCreateInstanceGLES31);
    xxRegisterFunctionSingle(xxDestroyInstance, xxDestroyInstanceGLES31);
    xxRegisterFunctionSingle(xxGetInstanceName, xxGetInstanceNameGLES31);

    xxRegisterFunctionSingle(xxCreateIndexBuffer, xxCreateIndexBufferGLES3);
    xxRegisterFunctionSingle(xxCreateVertexBuffer, xxCreateVertexBufferGLES3);
    xxRegisterFunctionSingle(xxMapBuffer, xxMapBufferGLES3);
    xxRegisterFunctionSingle(xxUnmapBuffer, xxUnmapBufferGLES3);

    xxRegisterFunctionSingle(xxPresentSwapchain, xxPresentSwapchainGLES31);
    xxRegisterFunctionSingle(xxGetCommandBuffer, xxGetCommandBufferGLES31);
    xxRegisterFunctionSingle(xxSetVertexBuffers, xxSetVertexBuffersGLES31);
    xxRegisterFunctionSingle(xxDraw, xxDrawGLES31);
    xxRegisterFunctionSingle(xxDrawIndexed, xxDrawIndexedGLES31);

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
//  Swapchain
//==============================================================================
void xxPresentSwapchainGLES31(uint64_t swapchain)
{
    glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);

    g_indirectCommand = nullptr;

    xxPresentSwapchainGLES2(swapchain);
}
//==============================================================================
//  Command Buffer
//==============================================================================
uint64_t xxGetCommandBufferGLES31(uint64_t device, uint64_t swapchain)
{
    uint64_t commandBuffer = xxGetCommandBufferGLES2(0, swapchain);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectBuffer);
    g_indirectCommand = glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawElementsIndirectCommand) * NUM_INDIRECT_BUFFER_SIZE, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

    return commandBuffer;
}
//==============================================================================
//  Command
//==============================================================================
void xxSetVertexBuffersGLES31(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    GLVERTEXATTRIBUTE* glVertexAttribute = reinterpret_cast<GLVERTEXATTRIBUTE*>(vertexAttribute);
    GLVERTEXATTRIBUTE::Attribute* attributes = glVertexAttribute->attributes;

    int currentStream = -1;
    for (int i = 0; i < glVertexAttribute->count; ++i)
    {
        GLVERTEXATTRIBUTE::Attribute& attribute = attributes[i];
        if (currentStream != attribute.stream)
        {
            currentStream = attribute.stream;
            GLBUFFER* glBuffer = reinterpret_cast<GLBUFFER*>(buffers[attribute.stream]);

            glBindBuffer(GL_ARRAY_BUFFER, glBuffer->buffer);
        }
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.pointer);
    }
}
//------------------------------------------------------------------------------
void xxDrawGLES31(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance)
{
    int index = g_indirectIndex++;
    if (g_indirectIndex >= NUM_INDIRECT_BUFFER_SIZE)
        g_indirectIndex = 0;

    DrawArraysIndirectCommand& indirectCommand = *(DrawArraysIndirectCommand*)((char*)g_indirectCommand + sizeof(DrawElementsIndirectCommand) * index);
    indirectCommand.count = vertexCount;
    indirectCommand.instanceCount = instanceCount;
    indirectCommand.firstVertex = firstVertex;
    indirectCommand.baseInstance = firstInstance;

    glDrawArraysIndirect(GL_TRIANGLES, (DrawElementsIndirectCommand*)nullptr + index);
}
//------------------------------------------------------------------------------
void xxDrawIndexedGLES31(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int vertexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    int index = g_indirectIndex++;
    if (g_indirectIndex >= NUM_INDIRECT_BUFFER_SIZE)
        g_indirectIndex = 0;

    DrawElementsIndirectCommand& indirectCommand = *(DrawElementsIndirectCommand*)((char*)g_indirectCommand + sizeof(DrawElementsIndirectCommand) * index);
    indirectCommand.count = indexCount;
    indirectCommand.instanceCount = instanceCount;
    indirectCommand.firstIndex = firstIndex;
    indirectCommand.baseVertex = vertexOffset;
    indirectCommand.baseInstance = firstInstance;

    GLenum indexType = vertexCount < 65536 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    glDrawElementsIndirect(GL_TRIANGLES, indexType, (DrawElementsIndirectCommand*)nullptr + index);
}
//==============================================================================
