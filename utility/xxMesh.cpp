//==============================================================================
// xxGraphic : Mesh Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphic.h"
#include "xxMesh.h"

//==============================================================================
//  Mesh
//==============================================================================
xxMesh::xxMesh(int color, int normal, int texture)
{
    m_vertexDataModified = false;
    m_vertexSizeChanged = false;
    m_indexDataModified = false;
    m_indexSizeChanged = false;

    m_vertexBufferIndex = 0;
    m_indexBufferIndex = 0;

    m_device = 0;
    m_vertexAttribute = 0;
    for (int i = 0; i < xxCountOf(m_vertexBuffers); ++i)
        m_vertexBuffers[i] = 0;
    for (int i = 0; i < xxCountOf(m_indexBuffers); ++i)
        m_indexBuffers[i] = 0;

    int stride = 0;
    stride += xxSizeOf(xxVector3) * 1;
    stride += xxSizeOf(uint32_t) * color;
    stride += xxSizeOf(xxVector3) * normal;
    stride += xxSizeOf(xxVector2) * texture;
    m_stride = stride;
    m_colorCount = color;
    m_normalCount = normal;
    m_textureCount = texture;
}
//------------------------------------------------------------------------------
xxMesh::~xxMesh()
{
    xxDestroyVertexAttribute(m_vertexAttribute);
    for (int i = 0; i < xxCountOf(m_vertexBuffers); ++i)
        xxDestroyBuffer(m_device, m_vertexBuffers[i]);
    for (int i = 0; i < xxCountOf(m_indexBuffers); ++i)
        xxDestroyBuffer(m_device, m_indexBuffers[i]);
}
//------------------------------------------------------------------------------
uint32_t xxMesh::GetVertexCount() const
{
    return static_cast<uint32_t>(m_vertex.size() / m_stride);
}
//------------------------------------------------------------------------------
void xxMesh::SetVertexCount(uint32_t count)
{
    m_vertex.resize(count * m_stride);
}
//------------------------------------------------------------------------------
uint32_t xxMesh::GetIndexCount() const
{
    return static_cast<uint32_t>(m_index.size());
}
//------------------------------------------------------------------------------
void xxMesh::SetIndexCount(uint32_t count)
{
    m_index.resize(count);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector2> xxMesh::GetVertex() const
{
    char* vertex = (char*)&m_vertex.front();
    return xxStrideIterator<xxVector2>(vertex, GetVertexCount(), m_stride);
}
//------------------------------------------------------------------------------
xxStrideIterator<uint32_t> xxMesh::GetColor(int index) const
{
    char* vertex = (char*)&m_vertex.front();
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(uint32_t) * index;
    return xxStrideIterator<uint32_t>(vertex, GetVertexCount(), m_stride);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector3> xxMesh::GetNormal(int index) const
{
    char* vertex = (char*)&m_vertex.front();
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(uint32_t) * m_colorCount;
    vertex += xxSizeOf(xxVector3) * index;
    return xxStrideIterator<xxVector3>(vertex, GetVertexCount(), m_stride);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector2> xxMesh::GetTexture(int index) const
{
    char* vertex = (char*)&m_vertex.front();
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(uint32_t) * m_colorCount;
    vertex += xxSizeOf(xxVector3) * m_normalCount;
    vertex += xxSizeOf(xxVector2)* index;
    return xxStrideIterator<xxVector2>(vertex, GetVertexCount(), m_stride);
}
//------------------------------------------------------------------------------
uint64_t xxMesh::GetVertexAttribute() const
{
    return m_vertexAttribute;
}
//------------------------------------------------------------------------------
void xxMesh::Update(xxNode& node, uint64_t device)
{
    m_device = device;

    if (m_vertexAttribute == 0)
    {
        int dataCount = 0;
        int data[64];
        int* dataPtr = data;
        int offset = 0;

        // position
        dataCount++;
        (*dataPtr++) = 0;
        (*dataPtr++) = offset;
        (*dataPtr++) = 3;
        (*dataPtr++) = xxSizeOf(xxVector3);
        offset += xxSizeOf(xxVector3);

        // color
        for (int i = 0; i < m_colorCount; ++i)
        {
            dataCount++;
            (*dataPtr++) = 0;
            (*dataPtr++) = offset;
            (*dataPtr++) = 4;
            (*dataPtr++) = xxSizeOf(uint32_t);
            offset += xxSizeOf(uint32_t);
        }

        // normal
        for (int i = 0; i < m_normalCount; ++i)
        {
            dataCount++;
            (*dataPtr++) = 0;
            (*dataPtr++) = offset;
            (*dataPtr++) = 3;
            (*dataPtr++) = xxSizeOf(xxVector3);
            offset += xxSizeOf(xxVector3);
        }

        // texture
        for (int i = 0; i < m_textureCount; ++i)
        {
            dataCount++;
            (*dataPtr++) = 0;
            (*dataPtr++) = offset;
            (*dataPtr++) = 2;
            (*dataPtr++) = xxSizeOf(xxVector2);
            offset += xxSizeOf(xxVector2);
        }

        m_vertexAttribute = xxCreateVertexAttribute(m_device, dataCount, data);
    }

    if (m_vertexDataModified || m_vertexSizeChanged)
    {
        if (m_vertexBuffers[m_vertexBufferIndex])
        {
            m_vertexBufferIndex++;
            if (m_vertexBufferIndex >= xxCountOf(m_vertexBuffers))
                m_vertexBufferIndex = 0;
        }
        int index = m_vertexBufferIndex;

        if (m_vertexSizeChanged)
        {
            m_vertexSizeChanged = false;
            m_vertexDataModified = true;

            xxDestroyBuffer(m_device, m_vertexBuffers[index]);
            m_vertexBuffers[index] = 0;
        }
        if (m_vertexBuffers[index] == 0)
        {
            m_vertexBuffers[index] = xxCreateVertexBuffer(m_device, m_stride * GetVertexCount(), m_vertexAttribute);
        }

        if (m_vertexDataModified)
        {
            void* ptr = xxMapBuffer(m_device, m_vertexBuffers[index]);
            if (ptr)
            {
                m_vertexDataModified = false;

                memcpy(ptr, &m_vertex.front(), m_stride * GetVertexCount());
                xxUnmapBuffer(m_device, m_vertexBuffers[index]);
            }
        }
    }

    if (m_indexDataModified || m_indexSizeChanged)
    {
        if (m_indexBuffers[m_indexBufferIndex])
        {
            m_indexBufferIndex++;
            if (m_indexBufferIndex >= xxCountOf(m_indexBuffers))
                m_indexBufferIndex = 0;
        }
        int index = m_indexBufferIndex;

        if (m_indexSizeChanged)
        {
            m_indexSizeChanged = false;
            m_indexDataModified = true;

            xxDestroyBuffer(m_device, m_indexBuffers[index]);
            m_indexBuffers[index] = 0;
        }
        if (m_indexBuffers[index] == 0)
        {
            m_indexBuffers[index] = xxCreateIndexBuffer(m_device, xxSizeOf(uint32_t) * GetIndexCount());
        }

        if (m_indexDataModified)
        {
            void* ptr = xxMapBuffer(m_device, m_indexBuffers[index]);
            if (ptr)
            {
                m_indexDataModified = false;

                memcpy(ptr, &m_index.front(), xxSizeOf(uint32_t) * GetIndexCount());
                xxUnmapBuffer(m_device, m_indexBuffers[index]);
            }
        }
    }
}
//------------------------------------------------------------------------------
void xxMesh::Draw(uint64_t commandEncoder, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    xxSetVertexBuffers(commandEncoder, 1, &m_vertexBuffers[m_vertexBufferIndex], m_vertexAttribute);
    xxSetIndexBuffer(commandEncoder, m_indexBuffers[m_indexBufferIndex]);
    xxDrawIndexed(commandEncoder, m_indexBuffers[m_indexBufferIndex], GetIndexCount(), instanceCount, firstIndex, vertexOffset, firstInstance);
}
//------------------------------------------------------------------------------
xxMeshPtr xxMesh::Create(int color, int normal, int texture)
{
    xxMeshPtr mesh = xxMeshPtr(new xxMesh(color, normal, texture));
    if (mesh == nullptr)
        return xxMeshPtr();

    return mesh;
}
//==============================================================================
