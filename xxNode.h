//==============================================================================
// xxGraphic : Node Header
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"
#include "xxVector.h"

#define _ENABLE_EXTENDED_ALIGNED_STORAGE
#include <memory>

class xxNode;
typedef std::shared_ptr<xxNode> xxNodePtr;

class xxCPPAPI xxNode
{
public:
    xxNode();
    virtual ~xxNode();

    static xxNodePtr Create();

    xxNodePtr GetParent() const;
    xxNodePtr GetChild(uint32_t index);
    uint32_t GetChildSize() const;
    bool AttachChild(xxNodePtr& child);
    bool DetachChild(xxNodePtr& child);

    const xxMatrix4& GetLocalMatrix() const;
    const xxMatrix4& GetWorldMatrix() const;
    void SetLocalMatrix(const xxMatrix4& matrix);
    void SetWorldMatrix(const xxMatrix4& matrix);

    void Update(float time);
    void LinearMatrix();

protected:
    xxNodePtr   m_parent;
    xxNodePtr*  m_children;
    uint32_t    m_childrenCount;
    uint32_t    m_childrenSize;

    xxMatrix4*  m_localMatrix;
    xxMatrix4*  m_worldMatrix;

    xxMatrix4   m_classLocalMatrix;
    xxMatrix4   m_classWorldMatrix;
    xxMatrix4*  m_linearMatrix;
    uint32_t    m_linearMatrixSize;

    bool        m_resetLinearMatrix;

    struct LinearMatrixHeader
    {
        xxMatrix4*  parentMatrix;
        uint32_t    childrenCount;
    };
};
