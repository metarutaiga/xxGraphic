//==============================================================================
// xxGraphic : Mesh Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include <xxGraphic/xxGraphic.h>
#include "xxBinary.h"
#include "xxMesh.h"
#include "xxNode.h"

//==============================================================================
int xxMesh::ms_bufferCount = 4;
//==============================================================================
//  Mesh
//==============================================================================
xxMesh::xxMesh(bool skinning, char normal, char color, char texture)
    :Skinning(skinning)
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
void xxMesh::Setup(uint64_t device)
{
    m_device = device;

    if (m_vertexAttribute == 0)
    {
        std::vector<std::array<int, 4>> attributes;
        int offset = 0;

        // position
        attributes.push_back({0, offset, 'POS3', xxSizeOf(xxVector3)});
        offset += xxSizeOf(xxVector3);

        // skinning
        if (Skinning)
        {
            // bone weight
            attributes.push_back({0, offset, 'BON3', xxSizeOf(xxVector3)});
            offset += xxSizeOf(xxVector3);

            // bone index
            attributes.push_back({0, offset, 'BON4', xxSizeOf(uint32_t)});
            offset += xxSizeOf(uint32_t);
        }

        // normal
        for (int i = 0; i < NormalCount; ++i)
        {
            attributes.push_back({0, offset, 'NOR3', xxSizeOf(xxVector3)});
            offset += xxSizeOf(xxVector3);
        }

        // color
        for (int i = 0; i < ColorCount; ++i)
        {
            attributes.push_back({0, offset, 'COL4', xxSizeOf(uint32_t)});
            offset += xxSizeOf(uint32_t);
        }

        // texture
        for (int i = 0; i < TextureCount; ++i)
        {
            attributes.push_back({0, offset, 'TEX2', xxSizeOf(xxVector2)});
            offset += xxSizeOf(xxVector2);
        }

        int count = (int)attributes.size();
        int* attribute = attributes.front().data();
        m_vertexAttribute = xxCreateVertexAttribute(m_device, count, attribute);
    }

    int vertexBufferIndex = m_vertexBufferIndex;
    if (m_vertexDataModified || m_vertexSizeChanged[vertexBufferIndex])
    {
        m_vertexDataModified = false;

        if (m_vertexBuffers[vertexBufferIndex])
        {
            m_vertexBufferIndex++;
            if (m_vertexBufferIndex >= ms_bufferCount)
                m_vertexBufferIndex = 0;
            vertexBufferIndex = m_vertexBufferIndex;
        }

        uint64_t vertexBuffer = m_vertexBuffers[vertexBufferIndex];
        if (m_vertexSizeChanged[vertexBufferIndex])
        {
            m_vertexSizeChanged[vertexBufferIndex] = false;

            xxDestroyBuffer(m_device, vertexBuffer);
            vertexBuffer = 0;
        }
        if (vertexBuffer == 0)
        {
            vertexBuffer = m_vertexBuffers[vertexBufferIndex] = VertexCount ? xxCreateVertexBuffer(m_device, Stride * VertexCount, m_vertexAttribute) : 0;
        }

        if (vertexBuffer)
        {
            void* ptr = xxMapBuffer(m_device, vertexBuffer);
            if (ptr)
            {
                memcpy(ptr, Vertex, Stride * VertexCount);
                xxUnmapBuffer(m_device, vertexBuffer);
            }
        }
    }

    int indexBufferIndex = m_indexBufferIndex;
    if (m_indexDataModified || m_indexSizeChanged[indexBufferIndex])
    {
        m_indexDataModified = false;

        if (m_indexBuffers[indexBufferIndex])
        {
            m_indexBufferIndex++;
            if (m_indexBufferIndex >= ms_bufferCount)
                m_indexBufferIndex = 0;
            indexBufferIndex = m_indexBufferIndex;
        }

        uint64_t indexBuffer = m_indexBuffers[indexBufferIndex];
        if (m_indexSizeChanged[indexBufferIndex])
        {
            m_indexSizeChanged[indexBufferIndex] = false;

            xxDestroyBuffer(m_device, indexBuffer);
            indexBuffer = 0;
        }
        if (indexBuffer == 0)
        {
            indexBuffer = m_indexBuffers[indexBufferIndex] = IndexCount ? xxCreateIndexBuffer(m_device, xxSizeOf(uint16_t) * IndexCount) : 0;
        }

        if (indexBuffer)
        {
            void* ptr = xxMapBuffer(m_device, indexBuffer);
            if (ptr)
            {
                memcpy(ptr, Index, xxSizeOf(uint16_t) * IndexCount);
                xxUnmapBuffer(m_device, indexBuffer);
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
    if (Stride == 0)
    {
        const_cast<int&>(Stride) += xxSizeOf(xxVector3) * 1;
        const_cast<int&>(Stride) += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
        const_cast<int&>(Stride) += xxSizeOf(uint32_t) * (Skinning ? 1 : 0);
        const_cast<int&>(Stride) += xxSizeOf(xxVector3) * NormalCount;
        const_cast<int&>(Stride) += xxSizeOf(uint32_t) * ColorCount;
        const_cast<int&>(Stride) += xxSizeOf(xxVector2) * TextureCount;
    }
    char* vertex = Vertex;
    if (count != VertexCount)
    {
        vertex = xxRealloc(Vertex, char, count * Stride);
        if (vertex == nullptr)
        {
            xxFree(Vertex);
            count = 0;
        }
        for (int i = 0; i < ms_bufferCount; ++i)
        {
            m_vertexSizeChanged[i] = true;
        }
    }
    const_cast<int&>(VertexCount) = count;
    const_cast<char*&>(Vertex) = vertex;
    m_vertexDataModified = true;
}
//------------------------------------------------------------------------------
void xxMesh::SetIndexCount(int count)
{
    uint16_t* index = Index;
    if (count != IndexCount)
    {
        index = xxRealloc(Index, uint16_t, count);
        if (index == nullptr)
        {
            xxFree(Index);
            count = 0;
        }
        for (int i = 0; i < ms_bufferCount; ++i)
        {
            m_indexSizeChanged[i] = true;
        }
    }
    const_cast<int&>(IndexCount) = count;
    const_cast<uint16_t*&>(Index) = index;
    m_indexDataModified = true;
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector3> xxMesh::GetPosition() const
{
    char* vertex = Vertex;
    return xxStrideIterator<xxVector3>(vertex, Stride, VertexCount);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector3> xxMesh::GetBoneWeight() const
{
    char* vertex = Vertex;
    vertex += xxSizeOf(xxVector3);
    return xxStrideIterator<xxVector3>(vertex, Stride, Skinning ? VertexCount : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<uint32_t> xxMesh::GetBoneIndices() const
{
    char* vertex = Vertex;
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
    return xxStrideIterator<uint32_t>(vertex, Stride, Skinning ? VertexCount : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector3> xxMesh::GetNormal(int index) const
{
    char* vertex = Vertex;
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(uint32_t) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(xxVector3) * index;
    return xxStrideIterator<xxVector3>(vertex, Stride, NormalCount ? VertexCount : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<uint32_t> xxMesh::GetColor(int index) const
{
    char* vertex = Vertex;
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(uint32_t) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(xxVector3) * NormalCount;
    vertex += xxSizeOf(uint32_t) * index;
    return xxStrideIterator<uint32_t>(vertex, Stride, ColorCount ? VertexCount : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector2> xxMesh::GetTexture(int index) const
{
    char* vertex = Vertex;
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(uint32_t) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(xxVector3) * NormalCount;
    vertex += xxSizeOf(uint32_t) * ColorCount;
    vertex += xxSizeOf(xxVector2) * index;
    return xxStrideIterator<xxVector2>(vertex, Stride, TextureCount ? VertexCount : 0);
}
//------------------------------------------------------------------------------
void xxMesh::CalculateBound() const
{
    if (VertexCount == 0)
    {
        const_cast<xxVector4&>(Bound) = xxVector4::ZERO;
        return;
    }
    xxVector4 bound = xxVector4::ZERO;
    for (auto& position : GetPosition())
    {
        bound.BoundMerge(position);
    }
    const_cast<xxVector4&>(Bound) = bound;
}
//------------------------------------------------------------------------------
void xxMesh::BufferCount(int count)
{
    ms_bufferCount = std::clamp(count, 1, 4);
}
//------------------------------------------------------------------------------
xxMeshPtr xxMesh::Create(bool skinning, char normal, char color, char texture)
{
    xxMeshPtr mesh = xxMeshPtr(new xxMesh(skinning, normal, color, texture), [](xxMesh* mesh) { delete mesh; });
    if (mesh == nullptr)
        return nullptr;

    return mesh;
}
//==============================================================================
//  Binary
//==============================================================================
xxMeshPtr (*xxMesh::BinaryCreate)() = []() { return Create(); };
//------------------------------------------------------------------------------
void xxMesh::BinaryRead(xxBinary& binary)
{
    binary.ReadString(Name);

    binary.Read(const_cast<bool&>(Skinning));
    binary.Read(const_cast<char&>(NormalCount));
    binary.Read(const_cast<char&>(ColorCount));
    binary.Read(const_cast<char&>(TextureCount));

    size_t vertexCount = 0;
    size_t indexCount = 0;
    binary.ReadSize(vertexCount);
    binary.ReadSize(indexCount);
    SetVertexCount(static_cast<int>(vertexCount));
    SetIndexCount(static_cast<int>(indexCount));

    binary.ReadArray(Vertex, Stride * VertexCount);
    binary.ReadArray(Index, IndexCount);

    binary.Read(const_cast<xxVector4&>(Bound));
}
//------------------------------------------------------------------------------
void xxMesh::BinaryWrite(xxBinary& binary) const
{
    binary.WriteString(Name);

    binary.Write(Skinning);
    binary.Write(NormalCount);
    binary.Write(ColorCount);
    binary.Write(TextureCount);

    binary.WriteSize(VertexCount);
    binary.WriteSize(IndexCount);

    binary.WriteArray(Vertex, Stride * VertexCount);
    binary.WriteArray(Index, IndexCount);

    binary.Write(Bound);
}
//==============================================================================
