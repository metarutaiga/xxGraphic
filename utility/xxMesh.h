//==============================================================================
// xxGraphic : Mesh Header
//
// Copyright (c) 2019-2023 TAiGA
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

    static xxMeshPtr Create(int color, int normal, int texture);

    typedef uint16_t IndexType;

    void                        Update(xxNode& node, uint64_t device);
    void                        Draw(uint64_t commandEncoder, int instanceCount = 1, int firstIndex = 0, int vertexOffset = 0, int firstInstance = 0);

    uint64_t                    GetVertexAttribute() const;

    int                         GetVertexCount() const;
    void                        SetVertexCount(int count);

    int                         GetIndexCount() const;
    void                        SetIndexCount(int count);

    xxStrideIterator<xxVector3> GetVertex() const;
    xxStrideIterator<uint32_t>  GetColor(int index) const;
    xxStrideIterator<xxVector3> GetNormal(int index) const;
    xxStrideIterator<xxVector2> GetTexture(int index) const;

    IndexType*                  GetIndex() const;

protected:
    xxMesh(int color, int normal, int texture);

    uint64_t                    m_device = 0;
    uint64_t                    m_vertexAttribute = 0;
    uint64_t                    m_vertexBuffers[4] = {};
    uint64_t                    m_indexBuffers[4] = {};

    char*                       m_vertex = nullptr;
    IndexType*                  m_index = nullptr;

    int                         m_vertexCount = 0;
    int                         m_indexCount = 0;

    bool                        m_vertexDataModified = false;
    bool                        m_vertexSizeChanged = false;
    bool                        m_indexDataModified = false;
    bool                        m_indexSizeChanged = false;

    char                        m_vertexBufferIndex = 0;
    char                        m_indexBufferIndex = 0;

    int                         m_stride = 0;
    int                         m_colorCount = 0;
    int                         m_normalCount = 0;
    int                         m_textureCount = 0;
};

template<class T>
struct xxStrideIterator
{
    xxStrideIterator(void* base, size_t size, size_t stride)
    {
        m_now = reinterpret_cast<T*>(base);
        m_begin = reinterpret_cast<T*>(base);
        m_end = reinterpret_cast<T*>(reinterpret_cast<char*>(base) + size * stride);
        m_stride = stride;
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
        return m_now == m_end();
    }

    T*      m_now;
    T*      m_begin;
    T*      m_end;
    size_t  m_stride;
};
