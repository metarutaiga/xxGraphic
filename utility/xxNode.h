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
    struct xxPlusAPI BoneData
    {
        void            ResetPointer();

    public:
        xxNodeWeakPtr   bone;
        xxMatrix4       classSkinMatrix;
        xxMatrix4       classBoneMatrix;
        xxMatrix4&      skinMatrix = classSkinMatrix;
        xxMatrix4&      boneMatrix = classBoneMatrix;
    };

public:
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

    void                        Invalidate();
    void                        Update(float time, bool updateMatrix = true);
    void                        Draw(xxDrawData const& data);

    static bool                 Traversal(std::function<bool(xxNodePtr const&)> callback, xxNodePtr const& node);

    static xxNodePtr            Create();

    static xxNodePtr          (*BinaryCreate)();
    void                        BinaryRead(xxBinary& binary);
    void                        BinaryWrite(xxBinary& binary) const;

protected:
    xxNode();
    virtual ~xxNode();

    // Parent / Children
    xxNodeWeakPtr               m_parent;
    xxNodeWeakPtr               m_this;
    std::vector<xxNodePtr>      m_children;

    // Matrix
    xxMatrix4                   m_classLocalMatrix = xxMatrix4::IDENTITY;
    xxMatrix4                   m_classWorldMatrix = xxMatrix4::IDENTITY;
    xxMatrix3                   m_legacyRotate = xxMatrix3::IDENTITY;
    xxVector3                   m_legacyTranslate = xxVector3::ZERO;
    float                       m_legacyScale = -1.0f;

    std::vector<xxMatrix4>      m_linearMatrix;
    bool                        m_linearMatrixCreate = false;

public:
    std::string                 Name = "";

    xxMatrix4&                  LocalMatrix;
    xxMatrix4&                  WorldMatrix;

    std::vector<BoneData>       Bones;

    xxCameraPtr                 Camera;
    xxMaterialPtr               Material;
    xxMeshPtr                   Mesh;

    std::vector<xxModifierData> Modifiers;

    std::vector<xxConstantData> ConstantDatas;
};
