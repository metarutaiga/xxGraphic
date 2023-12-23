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

#define HAVE_LINEAR_MATRIX 1

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
xxNodePtr const& xxNode::GetChild(size_t index) const
{
    if (index >= m_children.size())
    {
        static xxNodePtr empty;
        return empty;
    }

    return m_children[index];
}
//------------------------------------------------------------------------------
size_t xxNode::GetChildCount() const
{
    return m_children.size();
}
//------------------------------------------------------------------------------
bool xxNode::AttachChild(xxNodePtr const& node)
{
    if (node == nullptr)
        return false;
    if (node->m_parent.lock() != nullptr)
        return false;
    m_children.push_back(node);
    node->m_parent = m_this;

#if HAVE_LINEAR_MATRIX
    node->m_linearMatrix = std::vector<xxMatrix4>();
    node->m_linearMatrixCreate = false;

    xxNode* root = this;
    while (xxNode* parent = root->m_parent.lock().get())
    {
        root = parent;
    }
    root->m_linearMatrixCreate = true;
#endif

    return true;
}
//------------------------------------------------------------------------------
bool xxNode::DetachChild(xxNodePtr const& node)
{
    if (node == nullptr)
        return false;
    if (node->m_parent.lock() == nullptr)
        return false;

    for (xxNodePtr const& child : m_children)
    {
        if (node != child)
            continue;
        node->m_parent.reset();

        size_t i = std::distance<xxNodePtr const*>(m_children.data(), &child);
        for (size_t j = i + 1; j < m_children.size(); ++j)
            m_children[j - 1] = std::move(m_children[j]);
        m_children.pop_back();

#if HAVE_LINEAR_MATRIX
        std::function<void(xxNode*)> resetMatrix = [&resetMatrix](xxNode* node)
        {
            node->m_classLocalMatrix = (*node->m_localMatrix);
            node->m_classWorldMatrix = (*node->m_worldMatrix);
            node->m_localMatrix = &node->m_classLocalMatrix;
            node->m_worldMatrix = &node->m_classWorldMatrix;

            // Traversal
            for (xxNodePtr const& child : node->m_children)
            {
                if (child == nullptr)
                    continue;
                resetMatrix(child.get());
            }
        };
        resetMatrix(child.get());
#endif

        return true;
    }

    return false;
}
//------------------------------------------------------------------------------
xxMatrix4 const& xxNode::GetLocalMatrix() const
{
    return (*m_localMatrix);
}
//------------------------------------------------------------------------------
xxMatrix4 const& xxNode::GetWorldMatrix() const
{
    return (*m_worldMatrix);
}
//------------------------------------------------------------------------------
void xxNode::SetLocalMatrix(xxMatrix4 const& matrix)
{
    (*m_localMatrix) = matrix;

    m_legacyScale = -1.0f;
}
//------------------------------------------------------------------------------
void xxNode::SetWorldMatrix(xxMatrix4 const& matrix)
{
    (*m_worldMatrix) = matrix;
}
//------------------------------------------------------------------------------
void xxNode::CreateLinearMatrix()
{
#if HAVE_LINEAR_MATRIX
    if (m_parent.lock() != nullptr)
        return;

    std::function<size_t(xxNode*)> countMatrix = [&countMatrix](xxNode* node)
    {
        // Header
        size_t count = 1;

        // Traversal
        for (xxNodePtr const& child : node->m_children)
        {
            if (child == nullptr)
                continue;
            count += 2;
            if (child->m_children.empty())
                continue;
            count += countMatrix(child.get());
        }
        return count;
    };

    size_t newLinearMatrixSize = 2 + countMatrix(this) + 1;
    std::vector<xxMatrix4> newLinearMatrix(newLinearMatrixSize);
    if (newLinearMatrix.empty())
        return;

    std::function<void(xxNode*, xxMatrix4*&)> createLinearMatrix = [&createLinearMatrix](xxNode* node, xxMatrix4*& linearMatrix)
    {
        // Header
        LinearMatrixHeader* header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
        header->parentMatrix = node->m_worldMatrix;
        header->childrenCount = node->GetChildCount();

        // Children
        for (xxNodePtr const& child : node->m_children)
        {
            if (child == nullptr)
                continue;
            linearMatrix[0] = (*child->m_localMatrix);
            linearMatrix[1] = (*child->m_worldMatrix);
            child->m_localMatrix = &linearMatrix[0];
            child->m_worldMatrix = &linearMatrix[1];
            linearMatrix += 2;
        }

        // Traversal
        for (xxNodePtr const& child : node->m_children)
        {
            if (child == nullptr)
                continue;
            if (child->m_children.empty())
                continue;
            createLinearMatrix(child.get(), linearMatrix);
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
    createLinearMatrix(this, linearMatrix);

    // Final
    LinearMatrixHeader* header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
    header->parentMatrix = nullptr;
    header->childrenCount = 0;

    m_linearMatrix.swap(newLinearMatrix);
#endif
}
//------------------------------------------------------------------------------
bool xxNode::UpdateMatrix()
{
#if HAVE_LINEAR_MATRIX
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
            xxMatrix4 const& parentMatrix = (*header->parentMatrix);
            size_t childrenCount = header->childrenCount;

            parentMatrix.MultiplyArray(childrenCount, &linearMatrix[0], sizeof(xxMatrix4) * 2, &linearMatrix[1], sizeof(xxMatrix4) * 2);
            linearMatrix += childrenCount * 2;

            header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
        }

        return false;
    }
#endif
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

        rotate.v[0] = { (*m_localMatrix).v[0].x, (*m_localMatrix).v[0].y, (*m_localMatrix).v[0].z };
        rotate.v[1] = { (*m_localMatrix).v[1].x, (*m_localMatrix).v[1].y, (*m_localMatrix).v[1].z };
        rotate.v[2] = { (*m_localMatrix).v[2].x, (*m_localMatrix).v[2].y, (*m_localMatrix).v[2].z };

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

        translate = { (*m_localMatrix).v[3].x, (*m_localMatrix).v[3].y, (*m_localMatrix).v[3].z };

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
void xxNode::SetRotate(xxMatrix3 const& rotate)
{
    if (m_legacyScale < 0.0f)
    {
        (*m_localMatrix).v[0] = { rotate.v[0].x, rotate.v[0].y, rotate.v[0].z };
        (*m_localMatrix).v[1] = { rotate.v[1].x, rotate.v[1].y, rotate.v[1].z };
        (*m_localMatrix).v[2] = { rotate.v[2].x, rotate.v[2].y, rotate.v[2].z };
        return;
    }

    m_legacyRotate = rotate;
}
//------------------------------------------------------------------------------
void xxNode::SetTranslate(xxVector3 const& translate)
{
    if (m_legacyScale < 0.0f)
    {
        (*m_localMatrix).v[3] = { translate.x, translate.y, translate.z, 1.0f };
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

    (*m_localMatrix).v[0] = xxVector4{ m_legacyRotate.v[0].x, m_legacyRotate.v[0].y, m_legacyRotate.v[0].z } * m_legacyScale;
    (*m_localMatrix).v[1] = xxVector4{ m_legacyRotate.v[1].x, m_legacyRotate.v[1].y, m_legacyRotate.v[1].z } * m_legacyScale;
    (*m_localMatrix).v[2] = xxVector4{ m_legacyRotate.v[2].x, m_legacyRotate.v[2].y, m_legacyRotate.v[2].z } * m_legacyScale;
    (*m_localMatrix).v[3] = xxVector4{ m_legacyTranslate.x, m_legacyTranslate.y, m_legacyTranslate.z, 1.0f };
}
//------------------------------------------------------------------------------
xxImagePtr const& xxNode::GetImage(size_t index) const
{
    if (m_images.size() <= index)
    {
        static xxImagePtr empty;
        return empty;
    }

    return m_images[index];
}
//------------------------------------------------------------------------------
xxMaterialPtr const& xxNode::GetMaterial() const
{
    return m_material;
}
//------------------------------------------------------------------------------
xxMeshPtr const& xxNode::GetMesh() const
{
    return m_mesh;
}
//------------------------------------------------------------------------------
void xxNode::SetImage(size_t index, xxImagePtr const& image)
{
    if (m_images.size() <= index)
    {
        m_images.resize(index + 1);
    }

    m_images[index] = image;
}
//------------------------------------------------------------------------------
void xxNode::SetMaterial(xxMaterialPtr const& material)
{
    m_material = material;
}
//------------------------------------------------------------------------------
void xxNode::SetMesh(xxMeshPtr const& mesh)
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

    for (xxNodePtr const& child : m_children)
    {
        if (child == nullptr)
            continue;
        child->Update(time, updateMatrix);
    }
}
//------------------------------------------------------------------------------
void xxNode::Draw(uint64_t device, uint64_t commandEncoder, xxCameraPtr const& camera)
{
    if (m_material == nullptr || m_mesh == nullptr)
        return;

    m_mesh->Update(device);
    m_material->Update(device, *this, camera);
    m_material->Set(commandEncoder);

    uint64_t textures[16];
    uint64_t samplers[16];

    int textureCount = (int)m_images.size();
    for (int i = 0; i < textureCount; ++i)
    {
        xxImage* image = m_images[i].get();
        image->Update(device);
        textures[i] = image->GetTexture();
        samplers[i] = image->GetSampler();
    }

    xxSetFragmentTextures(commandEncoder, textureCount, textures);
    xxSetFragmentSamplers(commandEncoder, textureCount, samplers);

    m_mesh->Draw(commandEncoder);
}
//------------------------------------------------------------------------------
