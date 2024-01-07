//==============================================================================
// xxGraphic : Node Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxNode
{
    struct LinearMatrixHeader
    {
        xxMatrix4*  parentMatrix;
        size_t      childrenCount;
    };

public:
    virtual ~xxNode();

    static xxNodePtr Create();

    bool                        BinaryRead(xxBinary& binary);
    bool                        BinaryWrite(xxBinary& binary);

    xxNodePtr                   GetParent() const;
    xxNodePtr const&            GetChild(size_t index) const;
    size_t                      GetChildCount() const;
    bool                        AttachChild(xxNodePtr const& child);
    bool                        DetachChild(xxNodePtr const& child);

    void                        CreateLinearMatrix();
    bool                        UpdateMatrix();

    xxMatrix3                   GetRotate() const;
    xxVector3                   GetTranslate() const;
    float                       GetScale() const;
    void                        SetRotate(xxMatrix3 const& rotate);
    void                        SetTranslate(xxVector3 const& translate);
    void                        SetScale(float scale);
    void                        CreateRotateTranslateScale();
    void                        UpdateRotateTranslateScale();

    xxImagePtr const&           GetImage(size_t index) const;
    void                        SetImage(size_t index, xxImagePtr const& image);

    void                        Invalidate();
    void                        Update(float time, bool updateMatrix = true);
    void                        Draw(xxDrawData const& data);

protected:
    xxNode();

    typedef std::weak_ptr<class xxNode> xxNodeWeakPtr;

    // Parent / Children
    xxNodeWeakPtr               m_parent;
    xxNodeWeakPtr               m_this;
    std::vector<xxNodePtr>      m_children;

    // Matrix
    xxMatrix4                   m_classLocalMatrix;
    xxMatrix4                   m_classWorldMatrix;
    xxMatrix3                   m_legacyRotate;
    xxVector3                   m_legacyTranslate;
    float                       m_legacyScale;

    std::vector<xxMatrix4>      m_linearMatrix;
    bool                        m_linearMatrixCreate;

public:
    std::string                 Name = "";

    xxMatrix4&                  LocalMatrix;
    xxMatrix4&                  WorldMatrix;

    xxCameraPtr                 Camera;
    std::vector<xxImagePtr>     Images;
    xxMaterialPtr               Material;
    xxMeshPtr                   Mesh;

    std::vector<xxConstantData> ConstantDatas;
};

struct xxConstantData
{
    uint64_t    device = 0;
    uint64_t    pipeline = 0;
    uint64_t    vertexConstant = 0;
    uint64_t    fragmentConstant = 0;
    int         vertexConstantSize = 0;
    int         fragmentConstantSize = 0;
    int         ready = 0;
};

struct xxDrawData
{
    uint64_t                device = 0;
    uint64_t                commandEncoder = 0;
    xxCamera*               camera = nullptr;
    mutable xxConstantData* constantData = nullptr;
    mutable xxMesh*         mesh = nullptr;
    mutable xxNode*         node = nullptr;
    int                     materialIndex = 0;
};
