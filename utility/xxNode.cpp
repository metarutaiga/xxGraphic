//==============================================================================
// xxGraphic : Node Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxNode.h"

//==============================================================================
//  Node
//==============================================================================
xxNode::xxNode()
{
    m_localMatrix = &m_classLocalMatrix;
    m_worldMatrix = &m_classWorldMatrix;
    m_classLocalMatrix = xxMatrix4::IDENTITY;
    m_classWorldMatrix = xxMatrix4::IDENTITY;
    m_legacyRotate = xxMatrix3::IDENTITY;
    m_legacyTranslate = xxVector3::ZERO;
    m_legacyScale = -1.0f;

    m_linearMatrixCreate = false;

#if 0
    xxLog("xxNode", "Constructor : %p", this);
#endif
}
//------------------------------------------------------------------------------
xxNode::~xxNode()
{
#if 0
    xxLog("xxNode", "Destructor : %p", this);
#endif
}
//------------------------------------------------------------------------------
xxNodePtr xxNode::Create()
{
    xxNodePtr node = xxNodePtr(new xxNode);
    if (node == nullptr)
        return xxNodePtr();

    node->m_this = node;
    return node;
}
//------------------------------------------------------------------------------
xxNodePtr xxNode::GetParent() const
{
    return m_parent.lock();
}
//------------------------------------------------------------------------------
xxNodePtr xxNode::GetChild(uint32_t index)
{
    if (index >= m_children.size())
        return xxNodePtr();

    return m_children[index];
}
//------------------------------------------------------------------------------
uint32_t xxNode::GetChildCount() const
{
    return static_cast<uint32_t>(m_children.size());
}
//------------------------------------------------------------------------------
bool xxNode::AttachChild(const xxNodePtr& child)
{
    if (child == nullptr)
        return false;
    if (child->m_parent.lock() != nullptr)
        return false;

    m_children.push_back(child);
    child->m_parent = m_this;

    xxNode* node = this;
    while (xxNode* parent = node->m_parent.lock().get())
    {
        node = parent;
    }
    node->m_linearMatrixCreate = true;

    return true;
}
//------------------------------------------------------------------------------
bool xxNode::DetachChild(const xxNodePtr& child)
{
    if (child == nullptr)
        return false;
    if (child->m_parent.lock() == nullptr)
        return false;

    for (size_t i = 0; i < m_children.size(); ++i)
    {
        if (m_children[i] == child)
        {
            child->m_parent.reset();
            for (size_t j = i + 1; j < m_children.size(); ++j)
                std::move(m_children[j - 1] = m_children[j]);
            m_children.pop_back();

            struct TraversalResetMatrix
            {
                void operator() (xxNode* node)
                {
                    node->m_classLocalMatrix = (*node->m_localMatrix);
                    node->m_classWorldMatrix = (*node->m_worldMatrix);
                    node->m_localMatrix = &node->m_classLocalMatrix;
                    node->m_worldMatrix = &node->m_classWorldMatrix;

                    for (size_t i = 0; i < node->m_children.size(); ++i)
                    {
                        xxNode* child = node->m_children[i].get();
                        if (child == nullptr)
                            continue;
                        (*this)(child);
                    }
                }
            };
            TraversalResetMatrix()(child.get());

            return true;
        }
    }

    return false;
}
//------------------------------------------------------------------------------
const xxMatrix4& xxNode::GetLocalMatrix() const
{
    return (*m_localMatrix);
}
//------------------------------------------------------------------------------
const xxMatrix4& xxNode::GetWorldMatrix() const
{
    return (*m_worldMatrix);
}
//------------------------------------------------------------------------------
void xxNode::SetLocalMatrix(const xxMatrix4& matrix)
{
    (*m_localMatrix) = matrix;

    m_legacyScale = -1.0f;
}
//------------------------------------------------------------------------------
void xxNode::SetWorldMatrix(const xxMatrix4& matrix)
{
    (*m_worldMatrix) = matrix;
}
//------------------------------------------------------------------------------
void xxNode::CreateLinearMatrix()
{
    if (m_parent.lock() != nullptr)
        return;

    struct TraversalMatrixCount
    {
        uint32_t operator() (xxNode* node)
        {
            // Header
            uint32_t count = 1;

            // Children Count
            count += node->GetChildCount() * 2;

            // Traversal
            for (size_t i = 0; i < node->m_children.size(); ++i)
            {
                xxNode* child = node->m_children[i].get();
                if (child != nullptr && child->m_children.empty() == false)
                {
                    count += (*this)(child);
                }
            }
            return count;
        }
    };

    uint32_t newLinearMatrixSize = 2 + TraversalMatrixCount()(this) + 1;
    std::vector<xxMatrix4> newLinearMatrix(newLinearMatrixSize);
    if (newLinearMatrix.empty())
        return;

    struct TraversalLinearMatrix
    {
        void operator() (xxNode* node, xxMatrix4*& linearMatrix)
        {
            // Header
            LinearMatrixHeader* header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
            header->parentMatrix = node->m_worldMatrix;
            header->childrenCount = node->GetChildCount();

            // Children Count
            for (size_t i = 0; i < node->m_children.size(); ++i)
            {
                xxNode* child = node->m_children[i].get();
                if (child != nullptr)
                {
                    linearMatrix[0] = (*child->m_localMatrix);
                    linearMatrix[1] = (*child->m_worldMatrix);
                    child->m_localMatrix = &linearMatrix[0];
                    child->m_worldMatrix = &linearMatrix[1];
                    linearMatrix += 2;
                }
            }

            // Traversal
            for (size_t i = 0; i < node->m_children.size(); ++i)
            {
                xxNode* child = node->m_children[i].get();
                if (child != nullptr && child->m_children.empty() == false)
                {
                    (*this)(child, linearMatrix);
                }
            }
        }
    };

    // Root
    xxMatrix4* linearMatrix = &newLinearMatrix.front();
    linearMatrix[0] = (*m_localMatrix);
    linearMatrix[1] = (*m_worldMatrix);
    m_localMatrix = &linearMatrix[0];
    m_worldMatrix = &linearMatrix[1];
    linearMatrix += 2;

    // Traversal
    TraversalLinearMatrix()(this, linearMatrix);

    // Final
    LinearMatrixHeader* header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
    header->parentMatrix = nullptr;
    header->childrenCount = 0;

    m_linearMatrix.swap(newLinearMatrix);
}
//------------------------------------------------------------------------------
bool xxNode::UpdateMatrix()
{
    if (m_linearMatrixCreate)
    {
        m_linearMatrixCreate = false;
        CreateLinearMatrix();
    }

    xxMatrix4* linearMatrix = m_linearMatrix.empty() ? nullptr : &m_linearMatrix.front();
    if (linearMatrix)
    {
        // Root
        linearMatrix[1] = linearMatrix[0];
        linearMatrix += 2;

        // Linear
        LinearMatrixHeader* header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
        while (header->parentMatrix)
        {
            const xxMatrix4& parentMatrix = (*header->parentMatrix);
            size_t childrenCount = header->childrenCount;

            parentMatrix.MultiplyArray(childrenCount, &linearMatrix[0], sizeof(xxMatrix4) * 2, &linearMatrix[1], sizeof(xxMatrix4) * 2);
            linearMatrix += childrenCount * 2;

            header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
        }

        return false;
    }

    if (m_parent.lock() == nullptr)
    {
        (*m_worldMatrix) = (*m_localMatrix);
    }
    else
    {
       (*m_worldMatrix) = (*m_parent.lock()->m_worldMatrix) * (*m_localMatrix);
    }

    return true;
}
//------------------------------------------------------------------------------
xxMatrix3 xxNode::GetRotate() const
{
    if (m_legacyScale < 0.0f)
    {
        xxMatrix3 rotate;

        rotate._[0] = { (*m_localMatrix)._[0].x, (*m_localMatrix)._[0].y, (*m_localMatrix)._[0].z };
        rotate._[1] = { (*m_localMatrix)._[1].x, (*m_localMatrix)._[1].y, (*m_localMatrix)._[1].z };
        rotate._[2] = { (*m_localMatrix)._[2].x, (*m_localMatrix)._[2].y, (*m_localMatrix)._[2].z };

        return rotate;
    }

    return m_legacyRotate;
}
//------------------------------------------------------------------------------
xxVector3 xxNode::GetTranslate() const
{
    if (m_legacyScale < 0.0f)
    {
        xxVector3 translate;

        translate = { (*m_localMatrix)._[3].x, (*m_localMatrix)._[3].y, (*m_localMatrix)._[3].z };

        return translate;
    }

    return m_legacyTranslate;
}
//------------------------------------------------------------------------------
float xxNode::GetScale() const
{
    if (m_legacyScale < 0.0f)
        return 1.0f;

    return m_legacyScale;
}
//------------------------------------------------------------------------------
void xxNode::SetRotate(const xxMatrix3& rotate)
{
    if (m_legacyScale < 0.0f)
        CreateRotateTranslateScale();

    m_legacyRotate = rotate;
}
//------------------------------------------------------------------------------
void xxNode::SetTranslate(const xxVector3& translate)
{
    if (m_legacyScale < 0.0f)
        CreateRotateTranslateScale();

    m_legacyTranslate = translate;
}
//------------------------------------------------------------------------------
void xxNode::SetScale(float scale)
{
    if (m_legacyScale < 0.0f)
        CreateRotateTranslateScale();

    m_legacyScale = scale;
}
//------------------------------------------------------------------------------
void xxNode::CreateRotateTranslateScale()
{
    (*m_localMatrix).FastDecompose(m_legacyRotate, m_legacyTranslate, m_legacyScale);
}
//------------------------------------------------------------------------------
void xxNode::UpdateRotateTranslateScale()
{
    for (int i = 0; i < 3; ++i)
    {
        (*m_localMatrix)._[i] = xxVector4{ m_legacyRotate._[i].x, m_legacyRotate._[i].y, m_legacyRotate._[i].z } * m_legacyScale;
    }
    (*m_localMatrix)._[3] = { m_legacyTranslate.x, m_legacyTranslate.y, m_legacyTranslate.z, 1.0f };
}
//------------------------------------------------------------------------------
void xxNode::Update(float time, bool updateMatrix)
{
    if (updateMatrix)
    {
        updateMatrix = UpdateMatrix();
    }

    for (size_t i = 0; i < m_children.size(); ++i)
    {
        xxNode* child = m_children[i].get();
        if (child != nullptr)
        {
            child->Update(time, updateMatrix);
        }
    }
}
//------------------------------------------------------------------------------
