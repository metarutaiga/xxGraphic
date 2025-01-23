//==============================================================================
// xxGraphic : Node Header
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxNode
{
public:
    struct xxPlusAPI BoneData
    {
        void            ResetPointer();

    public:
        xxNodeWeakPtr   bone;
        xxVector4       bound;
        xxMatrix4       classSkinMatrix;
        xxMatrix4       classBoneMatrix;
        xxMatrix4&      skinMatrix = classSkinMatrix;
        xxMatrix4&      boneMatrix = classBoneMatrix;
    };

    auto                        begin() { return m_children.begin(); }
    auto                        begin() const { return m_children.begin(); }
    auto                        end() { return m_children.end(); }
    auto                        end() const { return m_children.end(); }

    enum
    {
        RESERVED0               = 0b00000001,
        UPDATE_SKIP             = 0b00000010,
        UPDATE_NEED             = 0b00000100,
    };

public:
    xxNodePtr const&            GetParent() const;
    xxNodePtr const&            GetChild(size_t index) const;
    size_t                      GetChildCount() const { return m_children.size(); }
    bool                        AttachChild(xxNodePtr const& child);
    bool                        DetachChild(xxNodePtr const& child);
    void                        RemoveAllChildren();

    void                        CreateLinearMatrix();
    void                        UpdateMatrix();
    void                        UpdateBound();

    xxMatrix3                   GetRotate() const;
    xxVector3 const&            GetTranslate() const;
    float                       GetScale() const { return m_legacyScale; }
    xxMatrix3                   GetWorldRotate() const { return { WorldMatrix.v[0].xyz, WorldMatrix.v[1].xyz, WorldMatrix.v[2].xyz }; }
    xxVector3 const&            GetWorldTranslate() const { return WorldMatrix.v[3].xyz; }
    float                       GetWorldScale() const;
    void                        SetRotate(xxMatrix3 const& rotate);
    void                        SetTranslate(xxVector3 const& translate);
    void                        SetScale(float scale);
    void                        CreateRotateTranslateScale();
    void                        UpdateRotateTranslateScale();

    void                        Invalidate();
    void                        Update(float time, bool updateMatrix = true);
    void                        Draw(xxDrawData const& data);

    static bool                 Traversal(xxNodePtr const& node, std::function<bool(xxNodePtr const&)> callback);

    static xxNodePtr            Create();

    static xxNodePtr          (*BinaryCreate)();
    virtual void                BinaryRead(xxBinary& binary);
    virtual void                BinaryWrite(xxBinary& binary) const;

protected:
    xxNode();
    virtual ~xxNode();

    // Parent / Children
    xxNodeWeakPtr               m_parent;
    xxNodeWeakPtr               m_this;
    std::vector<xxNodePtr>      m_children;

    xxMatrix4                   m_classLocalMatrix = xxMatrix4::IDENTITY;
    xxMatrix4                   m_classWorldMatrix = xxMatrix4::IDENTITY;
    xxVector4                   m_classWorldBound = xxVector4::ZERO;

    xxMatrix3                   m_legacyRotate = xxMatrix3::IDENTITY;
    xxVector3                   m_legacyTranslate = xxVector3::ZERO;
    float                       m_legacyScale = 1.0f;

    std::vector<xxMatrix4>      m_linearMatrix;
    bool                        m_linearMatrixCreate = false;

public:
    std::string                 Name = "";
    size_t                      Flags = 0;

    xxMatrix4&                  LocalMatrix = m_classLocalMatrix;
    xxMatrix4&                  WorldMatrix = m_classWorldMatrix;
    xxVector4&                  WorldBound = m_classWorldBound;

    std::vector<BoneData>       Bones;

    xxCameraPtr                 Camera;
    xxMaterialPtr               Material;
    xxMeshPtr                   Mesh;

    std::vector<xxModifierData> Modifiers;

    std::vector<xxConstantData> ConstantDatas;
};
