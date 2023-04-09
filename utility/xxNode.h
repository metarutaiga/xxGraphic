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

    xxNodePtr GetParent() const;
    xxNodePtr GetChild(uint32_t index);
    uint32_t GetChildCount() const;
    bool AttachChild(const xxNodePtr& child);
    bool DetachChild(const xxNodePtr& child);

    const xxMatrix4& GetLocalMatrix() const;
    const xxMatrix4& GetWorldMatrix() const;
    void SetLocalMatrix(const xxMatrix4& matrix);
    void SetWorldMatrix(const xxMatrix4& matrix);
    void CreateLinearMatrix();
    bool UpdateMatrix();

    xxMatrix3 GetRotate() const;
    xxVector3 GetTranslate() const;
    float GetScale() const;
    void SetRotate(const xxMatrix3& rotate);
    void SetTranslate(const xxVector3& translate);
    void SetScale(float scale);
    void CreateRotateTranslateScale();
    void UpdateRotateTranslateScale();

    xxImagePtr GetImage(size_t index) const;
    xxMaterialPtr GetMaterial() const;
    xxMeshPtr GetMesh() const;
    void SetImage(size_t index, const xxImagePtr& image);
    void SetMaterial(const xxMaterialPtr& material);
    void SetMesh(const xxMeshPtr& mesh);

    void Update(float time, bool updateMatrix = true);
    void Draw(uint64_t device, uint64_t commandEncoder, const xxCameraPtr& camera);

protected:
    xxNode();

    typedef std::weak_ptr<class xxNode> xxNodeWeakPtr;

    // Parent / Children
    xxNodeWeakPtr               m_parent;
    xxNodeWeakPtr               m_this;
    std::vector<xxNodePtr>      m_children;

    // Matrix
    xxMatrix4*                  m_localMatrix;
    xxMatrix4*                  m_worldMatrix;
    xxMatrix4                   m_classLocalMatrix;
    xxMatrix4                   m_classWorldMatrix;
    xxMatrix3                   m_legacyRotate;
    xxVector3                   m_legacyTranslate;
    float                       m_legacyScale;

    struct LinearMatrixHeader
    {
        xxMatrix4*  parentMatrix;
        uint32_t    childrenCount;
    };

    std::vector<xxMatrix4>      m_linearMatrix;
    bool                        m_linearMatrixCreate;

    // Component
    std::vector<xxImagePtr>     m_images;
    xxMaterialPtr               m_material;
    xxMeshPtr                   m_mesh;
};
