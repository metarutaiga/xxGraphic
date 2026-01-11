//==============================================================================
// xxGraphic : Mesh Source
//
// Copyright (c) 2019-2026 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include <xxGraphic/xxGraphic.h>
#include "xxBinary.h"
#include "xxMesh.h"
#include "xxNode.h"

//==============================================================================
int xxMesh::ms_transitionBufferCount = 4;
//==============================================================================
//  Mesh
//==============================================================================
xxMesh::xxMesh(bool skinning, char normal, char color, char texture)
    :Skinning(skinning)
    ,NormalCount(normal)
    ,ColorCount(color)
    ,TextureCount(texture)
{
    for (int i = 0; i < MAX; ++i)
    {
        (int&)Count[i] = 0;
        (int&)Stride[i] = 0;
        (void*&)Storage[i] = nullptr;
    }
}
//------------------------------------------------------------------------------
xxMesh::~xxMesh()
{
    Invalidate();
    for (int i = STORAGE0; i < MAX; ++i)
        xxFree(Storage[i]);
}
//------------------------------------------------------------------------------
void xxMesh::Invalidate()
{
    xxDestroyVertexAttribute(m_vertexAttribute);
    for (int i = STORAGE0; i < MAX; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            xxDestroyBuffer(m_device, m_buffers[i][j]);
            m_buffers[i][j] = 0;
        }
        m_dataModified[i] = (Count[i] != 0);
    }

    m_device = 0;
    m_vertexAttribute = 0;
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

    for (int i = STORAGE0; i < MAX; ++i)
    {
        int bufferIndex = m_bufferIndex[i];
        if (m_dataModified[i] || m_sizeChanged[i][bufferIndex])
        {
            m_dataModified[i] = false;

            if (m_buffers[i][bufferIndex])
            {
                bufferIndex++;
                if (bufferIndex >= ms_transitionBufferCount)
                    bufferIndex = 0;
                m_bufferIndex[i] = bufferIndex;
            }

            uint64_t buffer = m_buffers[i][bufferIndex];
            if (m_sizeChanged[i][bufferIndex])
            {
                m_sizeChanged[i][bufferIndex] = false;

                xxDestroyBuffer(m_device, buffer);
                buffer = 0;
            }
            if (buffer == 0)
            {
                if (Count[i])
                {
                    switch (i)
                    {
                    case INDEX:
                        buffer = xxCreateIndexBuffer(m_device, Count[INDEX] * Stride[INDEX], Stride[INDEX] != xxSizeOf(uint32_t) ? 16 : 32);
                        break;
                    case VERTEX:
                        buffer = xxCreateVertexBuffer(m_device, Count[VERTEX] * Stride[VERTEX], m_vertexAttribute);
                        break;
                    default:
                        buffer = xxCreateStorageBuffer(m_device, Count[i] * Stride[i]);
                        break;
                    }
                }
                m_buffers[i][bufferIndex] = buffer;
            }

            if (buffer)
            {
                void* ptr = xxMapBuffer(m_device, buffer);
                if (ptr)
                {
                    memcpy(ptr, Storage[i], Count[i] * Stride[i]);
                    xxUnmapBuffer(m_device, buffer);
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
void xxMesh::Draw(uint64_t commandEncoder, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    if (Count[INDEX] == 0)
    {
        xxSetVertexBuffers(commandEncoder, 1, &m_buffers[VERTEX][m_bufferIndex[VERTEX]], m_vertexAttribute);
        xxDraw(commandEncoder, Count[VERTEX] - firstIndex * 3, instanceCount, vertexOffset + firstIndex * 3, firstInstance);
    }
    else
    {
        xxSetVertexBuffers(commandEncoder, 1, &m_buffers[VERTEX][m_bufferIndex[VERTEX]], m_vertexAttribute);
        xxDrawIndexed(commandEncoder, m_buffers[INDEX][m_bufferIndex[INDEX]], Count[INDEX], Count[VERTEX], instanceCount, firstIndex, vertexOffset, firstInstance);
    }
}
//------------------------------------------------------------------------------
uint64_t xxMesh::GetVertexAttribute() const
{
    return m_vertexAttribute;
}
//------------------------------------------------------------------------------
void xxMesh::SetIndexCount(int count)
{
    if (Stride[INDEX] == 0)
    {
        const_cast<int&>(Stride[INDEX]) = Count[VERTEX] < 65536 ? xxSizeOf(uint16_t) : xxSizeOf(uint32_t);
    }
    if (count != Count[INDEX])
    {
        char* index = nullptr;
        if (count)
        {
            index = xxRealloc(Storage[INDEX], char, count * xxSizeOf(uint32_t));
        }
        if (index == nullptr)
        {
            xxFree(Storage[INDEX]);
            count = 0;
        }
        for (int i = 0; i < ms_transitionBufferCount; ++i)
        {
            m_sizeChanged[INDEX][i] = true;
        }
        const_cast<int&>(Count[INDEX]) = count;
        const_cast<char*&>(Storage[INDEX]) = index;
    }
    m_dataModified[INDEX] = true;
}
//------------------------------------------------------------------------------
void xxMesh::SetVertexCount(int count)
{
    if (Stride[VERTEX] == 0)
    {
        const_cast<int&>(Stride[VERTEX]) += xxSizeOf(xxVector3) * 1;
        const_cast<int&>(Stride[VERTEX]) += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
        const_cast<int&>(Stride[VERTEX]) += xxSizeOf(uint32_t) * (Skinning ? 1 : 0);
        const_cast<int&>(Stride[VERTEX]) += xxSizeOf(xxVector3) * NormalCount;
        const_cast<int&>(Stride[VERTEX]) += xxSizeOf(uint32_t) * ColorCount;
        const_cast<int&>(Stride[VERTEX]) += xxSizeOf(xxVector2) * TextureCount;
    }
    if (count != Count[VERTEX])
    {
        char* vertex = nullptr;
        if (count)
        {
            vertex = xxRealloc(Storage[VERTEX], char, count * Stride[VERTEX]);
        }
        if (vertex == nullptr)
        {
            xxFree(Storage[VERTEX]);
            count = 0;
        }
        for (int i = 0; i < ms_transitionBufferCount; ++i)
        {
            m_sizeChanged[VERTEX][i] = true;
        }
        const_cast<int&>(Count[VERTEX]) = count;
        const_cast<char*&>(Storage[VERTEX]) = vertex;
        if (Count[INDEX] && (Stride[INDEX] != (count < 65536 ? xxSizeOf(uint16_t) : xxSizeOf(uint32_t))))
        {
            count = Count[INDEX];
            const_cast<int&>(Count[INDEX]) = 0;
            const_cast<int&>(Stride[INDEX]) = 0;
            SetIndexCount(count);
        }
    }
    m_dataModified[VERTEX] = true;
}
//------------------------------------------------------------------------------
void xxMesh::SetStorageCount(int index, int count, int stride)
{
    if (index < STORAGE0 || index >= STORAGEMAX)
        return;

    if (count != Count[index] || stride != Stride[index])
    {
        char* storage = nullptr;
        if (count)
        {
            storage = xxRealloc(Storage[index], char, count * stride);
        }
        if (storage == nullptr)
        {
            xxFree(Storage[index]);
            count = 0;
        }
        for (int i = 0; i < ms_transitionBufferCount; ++i)
        {
            m_sizeChanged[index][i] = true;
        }
        const_cast<int&>(Count[index]) = count;
        const_cast<int&>(Stride[index]) = stride;
        const_cast<char*&>(Storage[index]) = storage;
    }
    m_dataModified[index] = true;
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector3> xxMesh::GetPosition() const
{
    char* vertex = Storage[VERTEX];
    return xxStrideIterator<xxVector3>(vertex, Stride[VERTEX], Count[VERTEX]);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector3> xxMesh::GetBoneWeight() const
{
    char* vertex = Storage[VERTEX];
    vertex += xxSizeOf(xxVector3);
    return xxStrideIterator<xxVector3>(vertex, Stride[VERTEX], Skinning ? Count[VERTEX] : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<uint32_t> xxMesh::GetBoneIndices() const
{
    char* vertex = Storage[VERTEX];
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
    return xxStrideIterator<uint32_t>(vertex, Stride[VERTEX], Skinning ? Count[VERTEX] : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector3> xxMesh::GetNormal(int index) const
{
    char* vertex = Storage[VERTEX];
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(uint32_t) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(xxVector3) * index;
    return xxStrideIterator<xxVector3>(vertex, Stride[VERTEX], NormalCount ? Count[VERTEX] : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<uint32_t> xxMesh::GetColor(int index) const
{
    char* vertex = Storage[VERTEX];
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(uint32_t) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(xxVector3) * NormalCount;
    vertex += xxSizeOf(uint32_t) * index;
    return xxStrideIterator<uint32_t>(vertex, Stride[VERTEX], ColorCount ? Count[VERTEX] : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector2> xxMesh::GetTexture(int index) const
{
    char* vertex = Storage[VERTEX];
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(uint32_t) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(xxVector3) * NormalCount;
    vertex += xxSizeOf(uint32_t) * ColorCount;
    vertex += xxSizeOf(xxVector2) * index;
    return xxStrideIterator<xxVector2>(vertex, Stride[VERTEX], TextureCount ? Count[VERTEX] : 0);
}
//------------------------------------------------------------------------------
void xxMesh::CalculateBound() const
{
    if (Count[VERTEX] == 0)
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
void xxMesh::TransitionBufferCount(int count)
{
    ms_transitionBufferCount = std::clamp(count, 1, 4);
}
//------------------------------------------------------------------------------
xxMeshPtr xxMesh::Create(bool skinning, char normal, char color, char texture)
{
    xxMeshPtr mesh = xxMesh::BinaryCreate();
    if (mesh == nullptr)
        return nullptr;

    const_cast<bool&>(mesh->Skinning) = skinning;
    const_cast<char&>(mesh->NormalCount) = normal;
    const_cast<char&>(mesh->ColorCount) = color;
    const_cast<char&>(mesh->TextureCount) = texture;

    return mesh;
}
//==============================================================================
//  Binary
//==============================================================================
xxMeshPtr (*xxMesh::BinaryCreate)() = []() -> xxMeshPtr
{
    xxMeshPtr mesh = xxMeshPtr(new xxMesh(false, 0, 0, 0), [](xxMesh* mesh) { delete mesh; });
    if (mesh == nullptr)
        return nullptr;

    return mesh;
};
//------------------------------------------------------------------------------
void xxMesh::BinaryRead(xxBinary& binary)
{
    binary.ReadString(Name);

    binary.Read(const_cast<bool&>(Skinning));
    binary.Read(const_cast<char&>(NormalCount));
    binary.Read(const_cast<char&>(ColorCount));
    binary.Read(const_cast<char&>(TextureCount));

    size_t count[MAX] = {};
    size_t stride[MAX] = {};
    if (binary.Version < 0x20241221)
    {
        binary.ReadSize(count[VERTEX]);
        binary.ReadSize(count[INDEX]);
    }
    else
    {
        for (int i = STORAGE0; i < MAX; ++i)
        {
            binary.ReadSize(count[i]);
            binary.ReadSize(stride[i]);
        }
        const_cast<int&>(Stride[INDEX]) = static_cast<int>(stride[INDEX]);
        const_cast<int&>(Stride[VERTEX]) = static_cast<int>(stride[VERTEX]);
    }

    SetIndexCount(static_cast<int>(count[INDEX]));
    SetVertexCount(static_cast<int>(count[VERTEX]));
    for (int i = STORAGE0; i < STORAGEMAX; ++i)
    {
        SetStorageCount(i, static_cast<int>(count[i]), static_cast<int>(stride[i]));
    }

    if (binary.Version < 0x20241221)
    {
        binary.ReadArray(Storage[VERTEX], Count[VERTEX] * Stride[VERTEX]);
        binary.ReadArray((uint16_t*)Storage[INDEX], Count[INDEX]);
    }
    else
    {
        for (int i = STORAGE0; i < MAX; ++i)
        {
            binary.ReadArray(Storage[i], Count[i] * Stride[i]);
        }
    }

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

    for (int i = STORAGE0; i < MAX; ++i)
    {
        binary.WriteSize(Count[i]);
        binary.WriteSize(Stride[i]);
    }

    for (int i = STORAGE0; i < MAX; ++i)
    {
        binary.WriteArray(Storage[i], Count[i] * Stride[i]);
    }

    binary.Write(Bound);
}
//==============================================================================
