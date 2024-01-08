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
    virtual ~xxMesh();

    typedef uint16_t IndexType;

    static xxMeshPtr            Create(int normal = 0, int color = 0, int texture = 0);

    static xxMeshPtr          (*BinaryCreate)();
    virtual bool                BinaryRead(xxBinary& binary);
    virtual bool                BinaryWrite(xxBinary& binary);

    void                        Invalidate();
    void                        Update(uint64_t device);
    void                        Draw(uint64_t commandEncoder, int instanceCount = 1, int firstIndex = 0, int vertexOffset = 0, int firstInstance = 0);

    uint64_t                    GetVertexAttribute() const;

    int                         GetVertexCount() const;
    void                        SetVertexCount(int count);

    int                         GetIndexCount() const;
    void                        SetIndexCount(int count);

    xxStrideIterator<xxVector3> GetVertex() const;
    xxStrideIterator<xxVector3> GetNormal(int index) const;
    xxStrideIterator<uint32_t>  GetColor(int index) const;
    xxStrideIterator<xxVector2> GetTexture(int index) const;

    IndexType*                  GetIndex() const;

protected:
    xxMesh(int normal, int color, int texture);

    uint64_t                    m_device = 0;
    uint64_t                    m_vertexAttribute = 0;
    uint64_t                    m_vertexBuffers[4] = {};
    uint64_t                    m_indexBuffers[4] = {};

    int                         m_vertexCount = 0;
    int                         m_indexCount = 0;

    char*                       m_vertex = nullptr;
    IndexType*                  m_index = nullptr;

    bool                        m_vertexDataModified = false;
    bool                        m_vertexSizeChanged = false;
    bool                        m_indexDataModified = false;
    bool                        m_indexSizeChanged = false;

    char                        m_vertexBufferIndex = 0;
    char                        m_indexBufferIndex = 0;

public:
    std::string                 Name = "";

    int const                   Stride = 0;
    int const                   NormalCount = 0;
    int const                   ColorCount = 0;
    int const                   TextureCount = 0;
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
