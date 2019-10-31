//==============================================================================
// xxGraphic : Node Header
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"
#include "xxVector.h"

#include "xxImage.h"
#include "xxMaterial.h"
#include "xxMesh.h"

#include <memory>
#include <vector>

class xxNode;
typedef std::shared_ptr<xxNode> xxNodePtr;
typedef std::weak_ptr<xxNode> xxNodeWeakPtr;

class xxCPPAPI xxNode
{
public:
    virtual ~xxNode();

    static xxNodePtr Create();

    xxNodePtr GetParent() const;
    xxNodePtr GetChild(uint32_t index);
    uint32_t GetChildCount() const;
    uint32_t GetChildSize() const;
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

    void Update(float time, bool updateMatrix = true);

protected:
    xxNode();

    // Parent / Children
    xxNodeWeakPtr           m_parent;
    xxNodeWeakPtr           m_this;
    std::vector<xxNodePtr>  m_children;
    uint32_t                m_childrenCount;

    // Matrix
    xxMatrix4*              m_localMatrix;
    xxMatrix4*              m_worldMatrix;

    xxMatrix3               m_legacyRotate;
    xxVector3               m_legacyTranslate;
    float                   m_legacyScale;

    xxMatrix4               m_classLocalMatrix;
    xxMatrix4               m_classWorldMatrix;

    struct LinearMatrixHeader
    {
        xxMatrix4*  parentMatrix;
        uint32_t    childrenCount;
    };

    std::vector<xxMatrix4>  m_linearMatrix;
    bool                    m_linearMatrixCreate;

    // Component
    std::vector<xxImagePtr> m_images;
    xxMaterialPtr           m_material;
    xxMeshPtr               m_mesh;
};
