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
    m_children = nullptr;
    m_childrenCount = 0;
    m_childrenSize = 0;

    m_localMatrix = &m_classLocalMatrix;
    m_worldMatrix = &m_classWorldMatrix;

    m_classLocalMatrix = xxMatrix4::IDENTITY;
    m_classWorldMatrix = xxMatrix4::IDENTITY;

    m_linearMatrix = nullptr;
    m_linearMatrixSize = 0;

    m_createLinearMatrix = false;

#if 0
    xxLog("xxNode", "Construct : %p", this);
#endif
}
//------------------------------------------------------------------------------
xxNode::~xxNode()
{
#if 0
    xxLog("xxNode", "Destruct : %p", this);
#endif

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
    return m_parent.lock();
}
//------------------------------------------------------------------------------
xxNodePtr xxNode::GetChild(uint32_t index)
{
    if (index >= m_childrenSize)
        return xxNodePtr();

    return m_children[index];
}
//------------------------------------------------------------------------------
uint32_t xxNode::GetChildCount() const
{
    return m_childrenCount;
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
    if (child->m_parent.lock() != nullptr)
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
            while (node->m_parent.lock())
            {
                node = node->m_parent.lock().get();
            }
            node->m_createLinearMatrix = true;
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
    if (child->m_parent.lock() == nullptr)
        return false;

    for (uint32_t i = 0; i < m_childrenSize; ++i)
    {
        if (m_children[i] == child)
        {
            m_children[i].reset();
            m_childrenCount--;

            struct TraversalResetMatrix
            {
                void operator() (xxNode* node)
                {
                    node->m_classLocalMatrix = (*node->m_localMatrix);
                    node->m_classWorldMatrix = (*node->m_worldMatrix);
                    node->m_localMatrix = &node->m_classLocalMatrix;
                    node->m_worldMatrix = &node->m_classWorldMatrix;

                    for (uint32_t i = 0; i < node->m_childrenSize; ++i)
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
        void operator() (xxNode* node, xxMatrix4*& linearMatrix)
        {
            // Header
            LinearMatrixHeader* header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
            header->parentMatrix = node->m_worldMatrix;
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
                    (*this)(child, linearMatrix);
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
    TraversalLinearMatrix()(this, linearMatrix);

    // Final
    LinearMatrixHeader* header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
    header->parentMatrix = nullptr;
    header->childrenCount = 0;

    delete[] m_linearMatrix;
    m_linearMatrix = newLinearMatrix;
    m_linearMatrixSize = newLinearMatrixSize;
}
//------------------------------------------------------------------------------
bool xxNode::UpdateMatrix()
{
    if (m_createLinearMatrix)
    {
        m_createLinearMatrix = false;
        CreateLinearMatrix();
    }

    xxMatrix4* linearMatrix = m_linearMatrix;
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
            uint32_t childrenCount = header->childrenCount;

            xxMatrix4::MultiplyArray(parentMatrix, childrenCount, &linearMatrix[0], sizeof(xxMatrix4) * 2, &linearMatrix[1], sizeof(xxMatrix4) * 2);
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
        (*m_worldMatrix) = xxMatrix4::Multiply(*m_parent.lock()->m_worldMatrix, *m_localMatrix);
    }

    return true;
}
//------------------------------------------------------------------------------
void xxNode::Update(float time, bool updateMatrix)
{
    if (updateMatrix)
    {
        updateMatrix = UpdateMatrix();
    }

    for (uint32_t i = 0; i < m_childrenSize; ++i)
    {
        xxNode* child = m_children[i].get();
        if (child != nullptr)
        {
            child->Update(time, updateMatrix);
        }
    }
}
//------------------------------------------------------------------------------
