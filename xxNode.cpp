//==============================================================================
// xxGraphic : Node Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxNode.h"

#include <functional>

//==============================================================================
//  Node
//==============================================================================
xxNode::xxNode()
{
    m_children = nullptr;
    m_childrenCount = 0;
    m_childrenSize = 0;

    m_localMatrix = &m_classLocalMatrix;
    m_worldMatrix = &m_classWorldMatrix;

    m_classLocalMatrix = xxMatrix4::IDENTITY;
    m_classWorldMatrix = xxMatrix4::IDENTITY;

    m_linearMatrix = nullptr;
    m_linearMatrixSize = 0;

    m_resetLinearMatrix = false;
}
//------------------------------------------------------------------------------
xxNode::~xxNode()
{
    delete[] m_children;
    delete[] m_linearMatrix;
}
//------------------------------------------------------------------------------
xxNodePtr xxNode::Create()
{
    return std::make_shared<xxNode>();
}
//------------------------------------------------------------------------------
xxNodePtr xxNode::GetParent() const
{
    return m_parent;
}
//------------------------------------------------------------------------------
xxNodePtr xxNode::GetChild(uint32_t index)
{
    if (index >= m_childrenSize)
        return xxNodePtr();

    return m_children[index];
}
//------------------------------------------------------------------------------
uint32_t xxNode::GetChildSize() const
{
    return m_childrenSize;
}
//------------------------------------------------------------------------------
bool xxNode::AttachChild(xxNodePtr& child)
{
    if (child == nullptr)
        return false;
    if (child->m_parent != nullptr)
        return false;

    if (m_childrenCount >= m_childrenSize)
    {
        uint32_t newSize = m_childrenSize * 2;
        if (newSize == 0)
            newSize = 1;

        xxNodePtr* newChildren = new xxNodePtr[newSize];
        if (newChildren == nullptr)
            return false;

        for (uint32_t i = 0; i < m_childrenSize; ++i)
            newChildren[i] = m_children[i];
        delete[] m_children;

        m_children = newChildren;
        m_childrenSize = newSize;
    }

    for (uint32_t i = 0; i < m_childrenSize; ++i)
    {
        if (m_children[i] == nullptr)
        {
            m_children[i] = child;
            m_childrenCount++;

            xxNode* node = this;
            while (node->m_parent)
            {
                node = node->m_parent.get();
            }
            node->m_resetLinearMatrix = true;
            return true;
        }
    }

    return false;
}
//------------------------------------------------------------------------------
bool xxNode::DetachChild(xxNodePtr& child)
{
    if (child == nullptr)
        return false;
    if (child->m_parent == nullptr)
        return false;

    for (uint32_t i = 0; i < m_childrenSize; ++i)
    {
        if (m_children[i] == child)
        {
            m_children[i].reset();
            m_childrenCount--;

            child->m_classLocalMatrix = (*child->m_localMatrix);
            child->m_classWorldMatrix = (*child->m_worldMatrix);
            child->m_localMatrix = &child->m_classLocalMatrix;
            child->m_worldMatrix = &child->m_classWorldMatrix;
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
}
//------------------------------------------------------------------------------
void xxNode::SetWorldMatrix(const xxMatrix4& matrix)
{
    (*m_worldMatrix) = matrix;
}
//------------------------------------------------------------------------------
void xxNode::Update(float time)
{
    if (m_resetLinearMatrix)
    {
        m_resetLinearMatrix = false;
        LinearMatrix();
    }

    xxMatrix4* linearMatrix = m_linearMatrix;
    if (linearMatrix)
    {
        // Root
        linearMatrix[1] = linearMatrix[0];
        linearMatrix += 2;

        // Linear
        LinearMatrixHeader* header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
        while (header->parentMatrix && header->childrenCount)
        {
            xxMatrix4 parentMatrix = (*header->parentMatrix);
            uint32_t childrenCount = header->childrenCount;
            for (uint32_t i = 0; i < childrenCount; ++i)
            {
                linearMatrix[1] = xxMatrix4::Multiply(parentMatrix, linearMatrix[0]);
                linearMatrix += 2;
            }
            header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
        }
    }
    else if (m_parent != nullptr)
    {
        (*m_worldMatrix) = xxMatrix4::Multiply(*m_parent->m_worldMatrix, *m_localMatrix);
    }
    else
    {
        (*m_worldMatrix) = (*m_localMatrix);
    }
}
//------------------------------------------------------------------------------
void xxNode::LinearMatrix()
{
    if (m_parent != nullptr)
        return;

    struct TraversalMatrixCount
    {
        uint32_t operator() (xxNode* node)
        {
            // Header
            uint32_t count = 1;

            // Children Count
            count += node->m_childrenCount * 2;

            // Traversal
            for (uint32_t i = 0; i < node->m_childrenSize; ++i)
            {
                xxNode* child = node->m_children[i].get();
                if (child != nullptr && child->m_childrenCount != 0)
                {
                    count += (*this)(child);
                }
            }
            return count;
        }
    };

    uint32_t newLinearMatrixSize = 2 + TraversalMatrixCount()(this) + 1;
    xxMatrix4* newLinearMatrix = new xxMatrix4[newLinearMatrixSize];
    if (newLinearMatrix == nullptr)
        return;

    struct TraversalLinearMatrix
    {
        void operator() (xxNode* node, xxMatrix4*& linearMatrix, xxMatrix4* parentMatrix)
        {
            // Header
            LinearMatrixHeader* header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
            header->parentMatrix = parentMatrix;
            header->childrenCount = node->m_childrenCount;

            // Children Count
            for (uint32_t i = 0; i < node->m_childrenSize; ++i)
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
            for (uint32_t i = 0; i < node->m_childrenSize; ++i)
            {
                xxNode* child = node->m_children[i].get();
                if (child != nullptr && child->m_childrenCount != 0)
                {
                    (*this)(child, linearMatrix, node->m_worldMatrix);
                }
            }
        }
    };

    // Root
    xxMatrix4* linearMatrix = newLinearMatrix;
    linearMatrix[0] = (*m_localMatrix);
    linearMatrix[1] = (*m_worldMatrix);
    m_localMatrix = &linearMatrix[0];
    m_worldMatrix = &linearMatrix[1];
    linearMatrix += 2;

    // Traversal
    TraversalLinearMatrix()(this, linearMatrix, nullptr);

    // Final
    LinearMatrixHeader* header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
    header->parentMatrix = nullptr;
    header->childrenCount = 0;

    delete[] m_linearMatrix;
    m_linearMatrix = newLinearMatrix;
    m_linearMatrixSize = newLinearMatrixSize;
}
//------------------------------------------------------------------------------
