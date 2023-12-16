//==============================================================================
// xxGraphic : Node Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxNode
{
public:
    virtual ~xxNode();

    static xxNodePtr Create();

    xxNodePtr               GetParent() const;
    xxNodePtr const&        GetChild(size_t index) const;
    size_t                  GetChildCount() const;
    bool                    AttachChild(xxNodePtr const& child);
    bool                    DetachChild(xxNodePtr const& child);

    xxMatrix4 const&        GetLocalMatrix() const;
    xxMatrix4 const&        GetWorldMatrix() const;
    void                    SetLocalMatrix(xxMatrix4 const& matrix);
    void                    SetWorldMatrix(xxMatrix4 const& matrix);
    void                    CreateLinearMatrix();
    bool                    UpdateMatrix();

    xxMatrix3               GetRotate() const;
    xxVector3               GetTranslate() const;
    float                   GetScale() const;
    void                    SetRotate(xxMatrix3 const& rotate);
    void                    SetTranslate(xxVector3 const& translate);
    void                    SetScale(float scale);
    void                    CreateRotateTranslateScale();
    void                    UpdateRotateTranslateScale();

    xxImagePtr const&       GetImage(size_t index) const;
    xxMaterialPtr const&    GetMaterial() const;
    xxMeshPtr const&        GetMesh() const;
    void                    SetImage(size_t index, xxImagePtr const& image);
    void                    SetMaterial(xxMaterialPtr const& material);
    void                    SetMesh(xxMeshPtr const& mesh);

    void                    Update(float time, bool updateMatrix = true);
    void                    Draw(uint64_t device, uint64_t commandEncoder, xxCameraPtr const& camera);

protected:
    xxNode();

    typedef std::weak_ptr<class xxNode> xxNodeWeakPtr;

    // Parent / Children
    xxNodeWeakPtr           m_parent;
    xxNodeWeakPtr           m_this;
    std::vector<xxNodePtr>  m_children;

    // Matrix
    xxMatrix4*              m_localMatrix;
    xxMatrix4*              m_worldMatrix;
    xxMatrix4               m_classLocalMatrix;
    xxMatrix4               m_classWorldMatrix;
    xxMatrix3               m_legacyRotate;
    xxVector3               m_legacyTranslate;
    float                   m_legacyScale;

    struct LinearMatrixHeader
    {
        xxMatrix4*  parentMatrix;
        size_t      childrenCount;
    };

    std::vector<xxMatrix4>  m_linearMatrix;
    bool                    m_linearMatrixCreate;

    // Component
    std::vector<xxImagePtr> m_images;
    xxMaterialPtr           m_material;
    xxMeshPtr               m_mesh;
};
