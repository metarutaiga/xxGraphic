//==============================================================================
// xxGraphic : Mesh Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "../xxSystem.h"
#include "xxMath.h"

#include <memory>
#include <vector>

class xxNode;

class xxMesh;
typedef std::shared_ptr<xxMesh> xxMeshPtr;

template<class T>
struct xxStrideIterator;

class xxPlusAPI xxMesh
{
public:
    virtual ~xxMesh();

    static xxMeshPtr Create(int color, int normal, int texture);

    uint32_t                    GetVertexCount() const;
    void                        SetVertexCount(uint32_t count);

    uint32_t                    GetIndexCount() const;
    void                        SetIndexCount(uint32_t count);

    xxStrideIterator<xxVector2> GetVertex() const;
    xxStrideIterator<uint32_t>  GetColor(int index) const;
    xxStrideIterator<xxVector3> GetNormal(int index) const;
    xxStrideIterator<xxVector2> GetTexture(int index) const;

    uint64_t                    GetVertexAttribute() const;

    void                        Update(xxNode& node, uint64_t device);
    void                        Draw(uint64_t commandEncoder, int instanceCount = 1, int firstIndex = 0, int vertexOffset = 0, int firstInstance = 0);

public:
    typedef uint16_t IndexType;

protected:
    xxMesh(int color, int normal, int texture);

    std::vector<char>           m_vertex;
    std::vector<IndexType>      m_index;

    bool                        m_vertexDataModified;
    bool                        m_vertexSizeChanged;
    bool                        m_indexDataModified;
    bool                        m_indexSizeChanged;

    char                        m_vertexBufferIndex;
    char                        m_indexBufferIndex;

    uint64_t                    m_device;
    uint64_t                    m_vertexAttribute;
    uint64_t                    m_vertexBuffers[4];
    uint64_t                    m_indexBuffers[4];

    int                         m_stride;
    int                         m_colorCount;
    int                         m_normalCount;
    int                         m_textureCount;
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
