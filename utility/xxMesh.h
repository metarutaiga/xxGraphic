//==============================================================================
// xxGraphic : Mesh Header
//
// Copyright (c) 2019-2026 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxMesh
{
public:
    enum
    {
        STORAGE0                = 0,
        STORAGE1                = 1,
        STORAGE2                = 2,
        STORAGE3                = 3,
        STORAGE4                = 4,
        STORAGE5                = 5,
        STORAGEMAX,
        INDEX                   = 6,
        VERTEX                  = 7,
        MAX,
    };

public:
    void                        Invalidate();
    void                        Setup(uint64_t device);
    void                        Draw(uint64_t commandEncoder, int instanceCount = 1, int firstIndex = 0, int vertexOffset = 0, int firstInstance = 0);

    uint64_t                    GetVertexAttribute() const;

    void                        SetIndexCount(int count);
    void                        SetVertexCount(int count);
    void                        SetStorageCount(int index, int count, int stride);

    xxStrideIterator<xxVector3> GetPosition() const;
    xxStrideIterator<xxVector3> GetBoneWeight() const;
    xxStrideIterator<uint32_t>  GetBoneIndices() const;

    xxStrideIterator<xxVector3> GetNormal(int index = 0) const;
    xxStrideIterator<uint32_t>  GetColor(int index = 0) const;
    xxStrideIterator<xxVector2> GetTexture(int index = 0) const;

    void                        CalculateBound() const;

    static void                 TransitionBufferCount(int count);

    static xxMeshPtr            Create(bool skinning = false, char normal = 0, char color = 0, char texture = 0);

    static xxMeshPtr          (*BinaryCreate)();
    virtual void                BinaryRead(xxBinary& binary);
    virtual void                BinaryWrite(xxBinary& binary) const;

protected:
    xxMesh(bool skinning, char normal, char color, char texture);
    virtual ~xxMesh();

    uint64_t                    m_device = 0;
    uint64_t                    m_vertexAttribute = 0;
    uint64_t                    m_buffers[MAX][4] = {};

    char                        m_bufferIndex[MAX] = {};
    bool                        m_dataModified[MAX] = {};
    bool                        m_sizeChanged[MAX][4] = {};

    static int                  ms_transitionBufferCount;

public:
    std::string                 Name = "";

    bool const                  Skinning = false;
    char const                  NormalCount = 0;
    char const                  ColorCount = 0;
    char const                  TextureCount = 0;

    union
    {
        struct
        {
            int const           Count[MAX];
            int const           Stride[MAX];
            char* const         Storage[MAX];
        };
        struct
        {
            int const           DummyCount[MAX - 2];
            int const           IndexCount;
            int const           VertexCount;
            int const           DummyStride[MAX - 2];
            int const           IndexStride;
            int const           VertexStride;
            char* const         DummyStorage[MAX - 2];
            char* const         Index;
            char* const         Vertex;
        };
    };

    xxVector4 const             Bound = xxVector4::ZERO;
};
