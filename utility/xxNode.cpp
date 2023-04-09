//==============================================================================
// xxGraphic : Node Source
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphic.h"
#include "xxImage.h"
#include "xxMaterial.h"
#include "xxMesh.h"
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
}
//------------------------------------------------------------------------------
xxNode::~xxNode()
{
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
                m_children[j - 1] = std::move(m_children[j]);
            m_children.pop_back();

            struct TraversalResetMatrix
            {
                static void Execute(xxNode* node)
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
                        Execute(child);
                    }
                }
            };
            TraversalResetMatrix::Execute(child.get());

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
        static uint32_t Execute(xxNode* node)
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
                    count += Execute(child);
                }
            }
            return count;
        }
    };

    uint32_t newLinearMatrixSize = 2 + TraversalMatrixCount::Execute(this) + 1;
    std::vector<xxMatrix4> newLinearMatrix(newLinearMatrixSize);
    if (newLinearMatrix.empty())
        return;

    struct TraversalLinearMatrix
    {
        static void Execute(xxNode* node, xxMatrix4*& linearMatrix)
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
                    Execute(child, linearMatrix);
                }
            }
        }
    };

    // Root
    xxMatrix4* linearMatrix = newLinearMatrix.data();
    linearMatrix[0] = (*m_localMatrix);
    linearMatrix[1] = (*m_worldMatrix);
    m_localMatrix = &linearMatrix[0];
    m_worldMatrix = &linearMatrix[1];
    linearMatrix += 2;

    // Traversal
    TraversalLinearMatrix::Execute(this, linearMatrix);

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

    xxMatrix4* linearMatrix = m_linearMatrix.empty() ? nullptr : m_linearMatrix.data();
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
    {
        (*m_localMatrix)._[0] = { rotate._[0].x, rotate._[0].y, rotate._[0].z };
        (*m_localMatrix)._[1] = { rotate._[1].x, rotate._[1].y, rotate._[1].z };
        (*m_localMatrix)._[2] = { rotate._[2].x, rotate._[2].y, rotate._[2].z };
        return;
    }

    m_legacyRotate = rotate;
}
//------------------------------------------------------------------------------
void xxNode::SetTranslate(const xxVector3& translate)
{
    if (m_legacyScale < 0.0f)
    {
        (*m_localMatrix)._[3] = { translate.x, translate.y, translate.z, 1.0f };
        return;
    }

    m_legacyTranslate = translate;
}
//------------------------------------------------------------------------------
void xxNode::SetScale(float scale)
{
    CreateRotateTranslateScale();

    m_legacyScale = scale;
}
//------------------------------------------------------------------------------
void xxNode::CreateRotateTranslateScale()
{
    if (m_legacyScale >= 0.0f)
        return;

    (*m_localMatrix).FastDecompose(m_legacyRotate, m_legacyTranslate, m_legacyScale);
}
//------------------------------------------------------------------------------
void xxNode::UpdateRotateTranslateScale()
{
    if (m_legacyScale < 0.0f)
        return;

    (*m_localMatrix)._[0] = xxVector4{ m_legacyRotate._[0].x, m_legacyRotate._[0].y, m_legacyRotate._[0].z } * m_legacyScale;
    (*m_localMatrix)._[1] = xxVector4{ m_legacyRotate._[1].x, m_legacyRotate._[1].y, m_legacyRotate._[1].z } * m_legacyScale;
    (*m_localMatrix)._[2] = xxVector4{ m_legacyRotate._[2].x, m_legacyRotate._[2].y, m_legacyRotate._[2].z } * m_legacyScale;
    (*m_localMatrix)._[3] = xxVector4{ m_legacyTranslate.x, m_legacyTranslate.y, m_legacyTranslate.z, 1.0f };
}
//------------------------------------------------------------------------------
xxImagePtr xxNode::GetImage(size_t index) const
{
    if (m_images.size() <= index)
        return nullptr;

    return m_images[index];
}
//------------------------------------------------------------------------------
xxMaterialPtr xxNode::GetMaterial() const
{
    return m_material;
}
//------------------------------------------------------------------------------
xxMeshPtr xxNode::GetMesh() const
{
    return m_mesh;
}
//------------------------------------------------------------------------------
void xxNode::SetImage(size_t index, const xxImagePtr& image)
{
    if (m_images.size() <= index) {
        m_images.resize(index + 1);
    }
    m_images[index] = image;
}
//------------------------------------------------------------------------------
void xxNode::SetMaterial(const xxMaterialPtr& material)
{
    m_material = material;
}
//------------------------------------------------------------------------------
void xxNode::SetMesh(const xxMeshPtr& mesh)
{
    m_mesh = mesh;
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
void xxNode::Draw(uint64_t device, uint64_t commandEncoder, const xxCameraPtr& camera)
{
    if (m_material == nullptr || m_mesh == nullptr)
        return;

    int textureCount = (int)m_images.size();
    uint64_t textures[16];
    uint64_t samplers[16];

    for (int i = 0; i < textureCount; ++i)
    {
        xxImage* image = m_images[i].get();
        image->Update(device);
        textures[i] = image->GetTexture();
        samplers[i] = image->GetSampler();
    }

    m_mesh->Update(*this, device);
    m_material->Update(*this, device, camera);

    m_material->Draw(commandEncoder);

    xxSetFragmentTextures(commandEncoder, textureCount, textures);
    xxSetFragmentSamplers(commandEncoder, textureCount, samplers);

    m_mesh->Draw(commandEncoder);
}
//------------------------------------------------------------------------------
