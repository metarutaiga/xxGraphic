//==============================================================================
// xxGraphic : Node Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "../xxSystem.h"
#include "xxMath.h"

#include "xxImage.h"
#include "xxMaterial.h"
#include "xxMesh.h"

#include <memory>
#include <vector>

class xxNode;
typedef std::shared_ptr<xxNode> xxNodePtr;
typedef std::weak_ptr<xxNode> xxNodeWeakPtr;

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

    xxImage* GetImage(size_t index) const;
    xxMaterial* GetMaterial() const;
    xxMesh* GetMesh() const;

    void Update(float time, bool updateMatrix = true);
    void Draw(uint64_t device, uint64_t commandEncoder);

protected:
    xxNode();

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
