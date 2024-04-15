//==============================================================================
// xxGraphic : Mesh Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

template<class T>
struct xxStrideIterator;

class xxPlusAPI xxMesh
{
public:
    void                        Invalidate();
    void                        Setup(uint64_t device);
    void                        Draw(uint64_t commandEncoder, int instanceCount = 1, int firstIndex = 0, int vertexOffset = 0, int firstInstance = 0);

    uint64_t                    GetVertexAttribute() const;

    void                        SetVertexCount(int count);
    void                        SetIndexCount(int count);

    xxStrideIterator<xxVector3> GetVertex() const;
    xxStrideIterator<xxVector3> GetBoneWeight() const;
    xxStrideIterator<uint32_t>  GetBoneIndices() const;

    xxStrideIterator<xxVector3> GetNormal(int index) const;
    xxStrideIterator<uint32_t>  GetColor(int index) const;
    xxStrideIterator<xxVector2> GetTexture(int index) const;

    static xxMeshPtr            Create(bool skinning = false, char normal = 0, char color = 0, char texture = 0);

    static xxMeshPtr          (*BinaryCreate)();
    virtual void                BinaryRead(xxBinary& binary);
    virtual void                BinaryWrite(xxBinary& binary) const;

protected:
    xxMesh(bool skinning, char normal, char color, char texture);
    virtual ~xxMesh();

    uint64_t                    m_device = 0;
    uint64_t                    m_vertexAttribute = 0;
    uint64_t                    m_vertexBuffers[4] = {};
    uint64_t                    m_indexBuffers[4] = {};

    bool                        m_vertexDataModified = false;
    bool                        m_vertexSizeChanged = false;
    bool                        m_indexDataModified = false;
    bool                        m_indexSizeChanged = false;

    char                        m_vertexBufferIndex = 0;
    char                        m_indexBufferIndex = 0;

public:
    std::string                 Name = "";

    bool const                  Skinning = false;
    char const                  NormalCount = 0;
    char const                  ColorCount = 0;
    char const                  TextureCount = 0;

    int const                   VertexCount = 0;
    int const                   IndexCount = 0;

    int const                   Stride = 0;
    char* const                 Vertex = nullptr;
    uint16_t* const             Index = nullptr;
};

template<class T>
struct xxStrideIterator
{
    xxStrideIterator(void* base, size_t stride, size_t size)
        :m_stride(stride)
        ,m_now(reinterpret_cast<T*>(base))
        ,m_begin(reinterpret_cast<T*>(base))
        ,m_end(reinterpret_cast<T*>(reinterpret_cast<char*>(base) + stride * size))
    {
    }

    T& operator * () const
    {
        return (*m_now);
    }

    xxStrideIterator& operator ++ ()
    {
        m_now = reinterpret_cast<T*>(reinterpret_cast<char*>(m_now) + m_stride);
        return (*this);
    }

    xxStrideIterator operator ++ (int)
    {
        xxStrideIterator iterator = (*this);
        m_now = reinterpret_cast<T*>(reinterpret_cast<char*>(m_now) + m_stride);
        return iterator;
    }

    void toBegin()
    {
        m_now = m_begin;
    }

    bool isEnd() const
    {
        return m_now == m_end;
    }

    T*              m_now;
    T* const        m_begin;
    T* const        m_end;
    size_t const    m_stride;
};
