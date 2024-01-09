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
    xxFree(Vertex);
    xxFree(Index);
}
//------------------------------------------------------------------------------
xxMeshPtr xxMesh::Create(int normal, int color, int texture)
{
    xxMeshPtr mesh = xxMeshPtr(new xxMesh(normal, color, texture), [](xxMesh* mesh) { delete mesh; });
    if (mesh == nullptr)
        return nullptr;

    return mesh;
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

    m_vertexDataModified = (VertexCount != 0);
    m_indexDataModified = (IndexCount != 0);
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
        if (m_vertexBuffers[index] == 0 && VertexCount != 0)
        {
            m_vertexBuffers[index] = xxCreateVertexBuffer(m_device, Stride * VertexCount, m_vertexAttribute);
            m_vertexDataModified = true;
        }

        if (m_vertexDataModified)
        {
            void* ptr = xxMapBuffer(m_device, m_vertexBuffers[index]);
            if (ptr)
            {
                m_vertexDataModified = false;

                memcpy(ptr, Vertex, Stride * VertexCount);
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
        if (m_indexBuffers[index] == 0 && IndexCount != 0)
        {
            m_indexBuffers[index] = xxCreateIndexBuffer(m_device, xxSizeOf(uint16_t) * IndexCount);
            m_indexDataModified = true;
        }

        if (m_indexDataModified)
        {
            void* ptr = xxMapBuffer(m_device, m_indexBuffers[index]);
            if (ptr)
            {
                m_indexDataModified = false;

                memcpy(ptr, Index, xxSizeOf(uint16_t) * IndexCount);
                xxUnmapBuffer(m_device, m_indexBuffers[index]);
            }
        }
    }
}
//------------------------------------------------------------------------------
void xxMesh::Draw(uint64_t commandEncoder, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    xxSetVertexBuffers(commandEncoder, 1, &m_vertexBuffers[m_vertexBufferIndex], m_vertexAttribute);
    if (IndexCount == 0)
    {
        xxDraw(commandEncoder, VertexCount - firstIndex * 3, instanceCount, vertexOffset + firstIndex * 3, firstInstance);
    }
    else
    {
        xxDrawIndexed(commandEncoder, m_indexBuffers[m_indexBufferIndex], IndexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }
}
//------------------------------------------------------------------------------
uint64_t xxMesh::GetVertexAttribute() const
{
    return m_vertexAttribute;
}
//------------------------------------------------------------------------------
void xxMesh::SetVertexCount(int count)
{
    char* vertex = nullptr;
    if (count)
    {
        vertex = xxAlloc(char, count * Stride);
        if (vertex == nullptr)
        {
            count = 0;
        }
    }
    xxFree(Vertex);
    const_cast<int&>(VertexCount) = count;
    const_cast<char*&>(Vertex) = vertex;
    m_vertexSizeChanged = true;
}
//------------------------------------------------------------------------------
void xxMesh::SetIndexCount(int count)
{
    uint16_t* index = nullptr;
    if (count)
    {
        index = xxAlloc(uint16_t, count);
        if (index == nullptr)
        {
            count = 0;
        }
    }
    xxFree(Index);
    const_cast<int&>(IndexCount) = count;
    const_cast<uint16_t*&>(Index) = index;
    m_indexSizeChanged = true;
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector3> xxMesh::GetVertex() const
{
    char* vertex = Vertex;
    return xxStrideIterator<xxVector3>(vertex, Stride, VertexCount);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector3> xxMesh::GetNormal(int index) const
{
    char* vertex = Vertex;
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * index;
    return xxStrideIterator<xxVector3>(vertex, Stride, NormalCount ? VertexCount : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<uint32_t> xxMesh::GetColor(int index) const
{
    char* vertex = Vertex;
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * NormalCount;
    vertex += xxSizeOf(uint32_t) * index;
    return xxStrideIterator<uint32_t>(vertex, Stride, ColorCount ? VertexCount : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector2> xxMesh::GetTexture(int index) const
{
    char* vertex = Vertex;
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * NormalCount;
    vertex += xxSizeOf(uint32_t) * ColorCount;
    vertex += xxSizeOf(xxVector2) * index;
    return xxStrideIterator<xxVector2>(vertex, Stride, TextureCount ? VertexCount : 0);
}
//==============================================================================
//  Binary
//==============================================================================
xxMeshPtr (*xxMesh::BinaryCreate)() = []() { return Create(); };
//------------------------------------------------------------------------------
bool xxMesh::BinaryRead(xxBinary& binary)
{
    binary.ReadString(Name);

    binary.Read(const_cast<int&>(Stride));
    binary.Read(const_cast<int&>(NormalCount));
    binary.Read(const_cast<int&>(ColorCount));
    binary.Read(const_cast<int&>(TextureCount));

    size_t vertexCount = 0;
    size_t indexCount = 0;
    binary.ReadSize(vertexCount);
    binary.ReadSize(indexCount);
    SetVertexCount(static_cast<int>(vertexCount));
    SetIndexCount(static_cast<int>(indexCount));

    binary.ReadArray(Vertex, Stride * VertexCount);
    binary.ReadArray(Index, IndexCount);

    return binary.Safe;
}
//------------------------------------------------------------------------------
bool xxMesh::BinaryWrite(xxBinary& binary) const
{
    binary.WriteString(Name);

    binary.Write(Stride);
    binary.Write(NormalCount);
    binary.Write(ColorCount);
    binary.Write(TextureCount);

    binary.WriteSize(VertexCount);
    binary.WriteSize(IndexCount);

    binary.WriteArray(Vertex, Stride * VertexCount);
    binary.WriteArray(Index, IndexCount);

    return binary.Safe;
}
//==============================================================================
