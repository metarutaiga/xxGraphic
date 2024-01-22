//==============================================================================
// xxGraphic : Node Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphic.h"
#include "xxBinary.h"
#include "xxCamera.h"
#include "xxImage.h"
#include "xxMaterial.h"
#include "xxMesh.h"
#include "xxModifier.h"
#include "xxNode.h"

#define HAVE_LINEAR_MATRIX 1

//==============================================================================
//  Node
//==============================================================================
xxNode::xxNode()
    :LocalMatrix(m_classLocalMatrix)
    ,WorldMatrix(m_classWorldMatrix)
{
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
    Invalidate();
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

#if HAVE_LINEAR_MATRIX
        Traversal([](xxNodePtr const& node)
        {
            xxMatrix4** pointer = reinterpret_cast<xxMatrix4**>((char*)&node->Name + sizeof(node->Name));
            node->m_classLocalMatrix = node->LocalMatrix;
            node->m_classWorldMatrix = node->WorldMatrix;
            pointer[0] = &node->m_classLocalMatrix;
            pointer[1] = &node->m_classWorldMatrix;
            return true;
        }, child);
#endif

        size_t i = std::distance<xxNodePtr const*>(m_children.data(), &child);
        for (size_t j = i + 1; j < m_children.size(); ++j)
            m_children[j - 1] = std::move(m_children[j]);
        m_children.pop_back();

        return true;
    }

    return false;
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
        header->parentMatrix = &node->WorldMatrix;
        header->childrenCount = node->GetChildCount();

        // Children
        for (xxNodePtr const& child : node->m_children)
        {
            if (child == nullptr)
                continue;
            xxMatrix4** pointer = reinterpret_cast<xxMatrix4**>((char*)&child->Name + sizeof(child->Name));
            linearMatrix[0] = child->LocalMatrix;
            linearMatrix[1] = child->WorldMatrix;
            pointer[0] = &linearMatrix[0];
            pointer[1] = &linearMatrix[1];
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
    xxMatrix4** pointer = reinterpret_cast<xxMatrix4**>((char*)&Name + sizeof(Name));
    linearMatrix[0] = LocalMatrix;
    linearMatrix[1] = WorldMatrix;
    pointer[0] = &linearMatrix[0];
    pointer[1] = &linearMatrix[1];
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
        WorldMatrix = LocalMatrix;
    }
    else
    {
        WorldMatrix = m_parent.lock()->WorldMatrix * LocalMatrix;
    }

    return true;
}
//------------------------------------------------------------------------------
xxMatrix3 xxNode::GetRotate() const
{
    if (m_legacyScale < 0.0f)
    {
        return { LocalMatrix[0].xyz, LocalMatrix[1].xyz, LocalMatrix[2].xyz };
    }

    return m_legacyRotate;
}
//------------------------------------------------------------------------------
xxVector3 xxNode::GetTranslate() const
{
    if (m_legacyScale < 0.0f)
    {
        return LocalMatrix.v[3].xyz;
    }

    return m_legacyTranslate;
}
//------------------------------------------------------------------------------
float xxNode::GetScale() const
{
    if (m_legacyScale < 0.0f)
    {
        return 1.0f;
    }

    return m_legacyScale;
}
//------------------------------------------------------------------------------
void xxNode::SetRotate(xxMatrix3 const& rotate)
{
    if (m_legacyScale < 0.0f)
    {
        LocalMatrix.v[0].xyz = rotate.v[0];
        LocalMatrix.v[1].xyz = rotate.v[1];
        LocalMatrix.v[2].xyz = rotate.v[2];
        return;
    }

    m_legacyRotate = rotate;
}
//------------------------------------------------------------------------------
void xxNode::SetTranslate(xxVector3 const& translate)
{
    if (m_legacyScale < 0.0f)
    {
        LocalMatrix.v[3].xyz = translate;
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

    LocalMatrix.FastDecompose(m_legacyRotate, m_legacyTranslate, m_legacyScale);
}
//------------------------------------------------------------------------------
void xxNode::UpdateRotateTranslateScale()
{
    if (m_legacyScale < 0.0f)
        return;

    LocalMatrix.v[0].xyz = m_legacyRotate.v[0] * m_legacyScale;
    LocalMatrix.v[1].xyz = m_legacyRotate.v[1] * m_legacyScale;
    LocalMatrix.v[2].xyz = m_legacyRotate.v[2] * m_legacyScale;
    LocalMatrix.v[3].xyz = m_legacyTranslate;
}
//------------------------------------------------------------------------------
xxImagePtr const& xxNode::GetImage(size_t index) const
{
    if (Images.size() <= index)
    {
        static xxImagePtr empty;
        return empty;
    }

    return Images[index];
}
//------------------------------------------------------------------------------
void xxNode::SetImage(size_t index, xxImagePtr const& image)
{
    if (Images.size() <= index)
    {
        Images.resize(index + 1);
    }

    Images[index] = image;
}
//------------------------------------------------------------------------------
void xxNode::Invalidate()
{
    for (auto& constantData : ConstantDatas)
    {
        xxDestroyBuffer(constantData.device, constantData.vertexConstant);
        xxDestroyBuffer(constantData.device, constantData.fragmentConstant);
        constantData.device = 0;
        constantData.pipeline = 0;
        constantData.vertexConstant = 0;
        constantData.fragmentConstant = 0;
        constantData.vertexConstantSize = 0;
        constantData.fragmentConstantSize = 0;
        constantData.ready = 0;
    }
}
//------------------------------------------------------------------------------
void xxNode::Update(float time, bool updateMatrix)
{
    for (xxModifierPtr const& modifier : Modifiers)
    {
        modifier->Update(time, this);
    }

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
void xxNode::Draw(xxDrawData const& data)
{
    if (Material == nullptr || Mesh == nullptr)
        return;

    data.mesh = Mesh.get();
    data.node = this;

    Mesh->Setup(data.device);
    Material->Setup(data);

    if (data.constantData->ready <= 0)
        return;

    Material->Draw(data);

    uint64_t textures[16];
    uint64_t samplers[16];

    int textureCount = (int)Images.size();
    for (int i = 0; i < textureCount; ++i)
    {
        xxImage* image = Images[i].get();
        image->Update(data.device);
        textures[i] = image->GetTexture();
        samplers[i] = image->GetSampler();
    }

    xxSetFragmentTextures(data.commandEncoder, textureCount, textures);
    xxSetFragmentSamplers(data.commandEncoder, textureCount, samplers);

    Mesh->Draw(data.commandEncoder);
}
//------------------------------------------------------------------------------
bool xxNode::Traversal(std::function<bool(xxNodePtr const&)> callback, xxNodePtr const& node)
{
    if (node == nullptr)
        return false;
    if (callback(node) == false)
        return false;
    for (xxNodePtr const& child : node->m_children)
        if (Traversal(callback, child) == false)
            return false;
    return true;
}
//------------------------------------------------------------------------------
xxNodePtr xxNode::Create()
{
    xxNodePtr node = xxNodePtr(new xxNode, [](xxNode* node) { delete node; });
    if (node == nullptr)
        return nullptr;

    node->m_this = node;
    return node;
}
//==============================================================================
//  Binary
//==============================================================================
xxNodePtr (*xxNode::BinaryCreate)() = []() { return xxNode::Create(); };
//------------------------------------------------------------------------------
bool xxNode::BinaryRead(xxBinary& binary)
{
    binary.ReadString(Name);

    binary.Read(LocalMatrix);

    Camera = binary.ReadReference<xxCamera>();

    size_t imageCount = 0;
    binary.ReadSize(imageCount);
    for (size_t i = 0; i < imageCount; ++i)
    {
        xxImagePtr image = binary.ReadReference<xxImage>();
        if (image == nullptr)
            return false;

        xxImage::ImageLoader(image, binary.Path);
        Images.push_back(image);
    }

    Material = binary.ReadReference<xxMaterial>();
    Mesh = binary.ReadReference<xxMesh>();

    size_t modifierCount = 0;
    binary.ReadSize(modifierCount);
    for (size_t i = 0; i < modifierCount; ++i)
    {
        xxModifierPtr modifier = binary.ReadReference<xxModifier>();
        if (modifier == nullptr)
            return false;

        Modifiers.push_back(modifier);
    }

    size_t childCount = 0;
    binary.ReadSize(childCount);
    for (size_t i = 0; i < childCount; ++i)
    {
        xxNodePtr child = xxNode::Create();
        if (child == nullptr || child->BinaryRead(binary) == false)
            return false;

        AttachChild(child);
    }

    return binary.Safe;
}
//------------------------------------------------------------------------------
bool xxNode::BinaryWrite(xxBinary& binary) const
{
    binary.WriteString(Name);

    binary.Write(LocalMatrix);

    binary.WriteReference(Camera);

    size_t imageCount = Images.size();
    binary.WriteSize(imageCount);
    for (size_t i = 0; i < imageCount; ++i)
    {
        binary.WriteReference(Images[i]);
    }

    binary.WriteReference(Material);
    binary.WriteReference(Mesh);

    size_t modifierCount = Modifiers.size();
    binary.WriteSize(modifierCount);
    for (size_t i = 0; i < modifierCount; ++i)
    {
        binary.WriteReference(Modifiers[i]);
    }

    size_t childCount = m_children.size();
    binary.WriteSize(childCount);
    for (size_t i = 0; i < childCount; ++i)
    {
        xxNodePtr const& child = m_children[i];
        if (child == nullptr || child->BinaryWrite(binary) == false)
            return false;
    }

    return binary.Safe;
}
//==============================================================================
