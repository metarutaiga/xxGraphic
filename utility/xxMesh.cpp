//==============================================================================
// xxGraphic : Mesh Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphic.h"
#include "xxBinary.h"
#include "xxMesh.h"

//==============================================================================
//  Mesh
//==============================================================================
xxMesh::xxMesh(int normal, int color, int texture)
    :Stride(xxSizeOf(xxVector3) * 1 + 
            xxSizeOf(xxVector3) * normal +
            xxSizeOf(uint32_t) * color +
            xxSizeOf(xxVector2) * texture)
    ,NormalCount(normal)
    ,ColorCount(color)
    ,TextureCount(texture)
{
}
//------------------------------------------------------------------------------
xxMesh::~xxMesh()
{
    Invalidate();
    xxFree(m_vertex);
    xxFree(m_index);
}
//------------------------------------------------------------------------------
xxMeshPtr xxMesh::Create(int normal, int color, int texture)
{
    xxMeshPtr mesh = xxMeshPtr(new xxMesh(normal, color, texture));
    if (mesh == nullptr)
        return xxMeshPtr();

    return mesh;
}
//------------------------------------------------------------------------------
bool xxMesh::BinaryRead(xxBinary& binary)
{
    binary.ReadString(Name);

    binary.Read(const_cast<int&>(Stride));
    binary.Read(const_cast<int&>(NormalCount));
    binary.Read(const_cast<int&>(ColorCount));
    binary.Read(const_cast<int&>(TextureCount));

    binary.Read(m_vertexCount);
    binary.Read(m_indexCount);

    SetVertexCount(m_vertexCount);
    SetIndexCount(m_indexCount);

    binary.ReadArray(m_vertex, Stride * m_vertexCount);
    binary.ReadArray(m_index, m_indexCount);

    return binary.Safe;
}
//------------------------------------------------------------------------------
bool xxMesh::BinaryWrite(xxBinary& binary)
{
    binary.WriteString(Name);

    binary.Write(Stride);
    binary.Write(NormalCount);
    binary.Write(ColorCount);
    binary.Write(TextureCount);

    binary.Write(m_vertexCount);
    binary.Write(m_indexCount);

    binary.WriteArray(m_vertex, Stride * m_vertexCount);
    binary.WriteArray(m_index, m_indexCount);

    return binary.Safe;
}
//------------------------------------------------------------------------------
void xxMesh::Invalidate()
{
    xxDestroyVertexAttribute(m_vertexAttribute);
    for (uint64_t buffer : m_vertexBuffers)
        xxDestroyBuffer(m_device, buffer);
    for (uint64_t buffer : m_indexBuffers)
        xxDestroyBuffer(m_device, buffer);

    m_device = 0;
    m_vertexAttribute = 0;
    for (uint64_t& buffer : m_vertexBuffers)
        buffer = 0;
    for (uint64_t& buffer : m_indexBuffers)
        buffer = 0;

    m_vertexDataModified = (m_vertexCount != 0);
    m_indexDataModified = (m_indexCount != 0);
}
//------------------------------------------------------------------------------
void xxMesh::Update(uint64_t device)
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

        // normal
        for (int i = 0; i < NormalCount; ++i)
        {
            dataCount++;
            (*dataPtr++) = 0;
            (*dataPtr++) = offset;
            (*dataPtr++) = 3;
            (*dataPtr++) = xxSizeOf(xxVector3);
            offset += xxSizeOf(xxVector3);
        }

        // color
        for (int i = 0; i < ColorCount; ++i)
        {
            dataCount++;
            (*dataPtr++) = 0;
            (*dataPtr++) = offset;
            (*dataPtr++) = 4;
            (*dataPtr++) = xxSizeOf(uint32_t);
            offset += xxSizeOf(uint32_t);
        }

        // texture
        for (int i = 0; i < TextureCount; ++i)
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

            xxDestroyBuffer(m_device, m_vertexBuffers[index]);
            m_vertexBuffers[index] = 0;
        }
        if (m_vertexBuffers[index] == 0 && m_vertexCount != 0)
        {
            m_vertexBuffers[index] = xxCreateVertexBuffer(m_device, Stride * m_vertexCount, m_vertexAttribute);
            m_vertexDataModified = true;
        }

        if (m_vertexDataModified)
        {
            void* ptr = xxMapBuffer(m_device, m_vertexBuffers[index]);
            if (ptr)
            {
                m_vertexDataModified = false;

                memcpy(ptr, m_vertex, Stride * m_vertexCount);
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

            xxDestroyBuffer(m_device, m_indexBuffers[index]);
            m_indexBuffers[index] = 0;
        }
        if (m_indexBuffers[index] == 0 && m_indexCount != 0)
        {
            m_indexBuffers[index] = xxCreateIndexBuffer(m_device, xxSizeOf(IndexType) * m_indexCount);
            m_indexDataModified = true;
        }

        if (m_indexDataModified)
        {
            void* ptr = xxMapBuffer(m_device, m_indexBuffers[index]);
            if (ptr)
            {
                m_indexDataModified = false;

                memcpy(ptr, m_index, xxSizeOf(IndexType) * m_indexCount);
                xxUnmapBuffer(m_device, m_indexBuffers[index]);
            }
        }
    }
}
//------------------------------------------------------------------------------
void xxMesh::Draw(uint64_t commandEncoder, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    xxSetVertexBuffers(commandEncoder, 1, &m_vertexBuffers[m_vertexBufferIndex], m_vertexAttribute);
    if (m_indexCount == 0)
    {
        xxDraw(commandEncoder, m_vertexCount - firstIndex * 3, instanceCount, vertexOffset + firstIndex * 3, firstInstance);
    }
    else
    {
        xxDrawIndexed(commandEncoder, m_indexBuffers[m_indexBufferIndex], m_indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }
}
//------------------------------------------------------------------------------
uint64_t xxMesh::GetVertexAttribute() const
{
    return m_vertexAttribute;
}
//------------------------------------------------------------------------------
int xxMesh::GetVertexCount() const
{
    return m_vertexCount;
}
//------------------------------------------------------------------------------
void xxMesh::SetVertexCount(int count)
{
    m_vertexCount = 0;
    xxFree(m_vertex);
    m_vertex = nullptr;
    if (count)
    {
        m_vertex = xxAlloc(char, count * Stride);
        if (m_vertex)
        {
            m_vertexCount = count;
        }
    }
    m_vertexSizeChanged = true;
}
//------------------------------------------------------------------------------
int xxMesh::GetIndexCount() const
{
    return m_indexCount;
}
//------------------------------------------------------------------------------
void xxMesh::SetIndexCount(int count)
{
    m_indexCount = 0;
    xxFree(m_index);
    m_index = nullptr;
    if (count)
    {
        m_index = xxAlloc(IndexType, count);
        if (m_index)
        {
            m_indexCount = count;
        }
    }
    m_indexSizeChanged = true;
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector3> xxMesh::GetVertex() const
{
    char* vertex = m_vertex;
    return xxStrideIterator<xxVector3>(vertex, Stride, m_vertexCount);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector3> xxMesh::GetNormal(int index) const
{
    char* vertex = m_vertex;
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * index;
    return xxStrideIterator<xxVector3>(vertex, Stride, NormalCount ? m_vertexCount : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<uint32_t> xxMesh::GetColor(int index) const
{
    char* vertex = m_vertex;
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * NormalCount;
    vertex += xxSizeOf(uint32_t) * index;
    return xxStrideIterator<uint32_t>(vertex, Stride, ColorCount ? m_vertexCount : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector2> xxMesh::GetTexture(int index) const
{
    char* vertex = m_vertex;
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * NormalCount;
    vertex += xxSizeOf(uint32_t) * ColorCount;
    vertex += xxSizeOf(xxVector2) * index;
    return xxStrideIterator<xxVector2>(vertex, Stride, TextureCount ? m_vertexCount : 0);
}
//------------------------------------------------------------------------------
xxMesh::IndexType* xxMesh::GetIndex() const
{
    return m_index;
}
//==============================================================================
