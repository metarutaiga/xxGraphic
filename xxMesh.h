//==============================================================================
// xxGraphic : Mesh Header
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxNode.h"
#include "xxMaterial.h"

class xxMesh;
typedef std::shared_ptr<xxMesh> xxMeshPtr;

class xxCPPAPI xxMesh : public xxNode
{
public:
    class xxCPPAPI Data
    {
    public:
        Data(int color, int normal, int texture);
        virtual ~Data();

        uint32_t        GetVertexCount() const;
        void            SetVertexCount(uint32_t count);

        uint32_t        GetIndexCount() const;
        void            SetIndexCount(uint32_t count);

        xxVector2*      GetVertex(int& stride) const;
        uint32_t*       GetColor(int index, int& stride) const;
        xxVector3*      GetNormal(int index, int& stride) const;
        xxVector2*      GetTexture(int index, int& stride) const;

        void            Update(uint64_t device);

    protected:
        void*           m_vertex;
        uint32_t*       m_index;

        uint32_t        m_vertexCount;
        uint32_t        m_indexCount;

        bool            m_vertexDataModified;
        bool            m_vertexSizeChanged;
        bool            m_indexDataModified;
        bool            m_indexSizeChanged;

        char            m_vertexBufferIndex;
        char            m_indexBufferIndex;
        uint64_t        m_device;
        uint64_t        m_vertexAttribute;
        uint64_t        m_vertexBuffers[4];
        uint64_t        m_indexBuffers[4];

        int             m_stride;
        int             m_colorCount;
        int             m_normalCount;
        int             m_textureCount;
    };
    typedef std::shared_ptr<Data> DataPtr;

public:
    virtual ~xxMesh();

    static xxMeshPtr Create(const DataPtr& data);

protected:
    xxMesh(const DataPtr& data);

    DataPtr         m_meshData;
    xxMaterialPtr   m_material;
};
