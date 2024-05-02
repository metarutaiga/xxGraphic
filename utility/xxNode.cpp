//==============================================================================
// xxGraphic : Node Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphic.h"
#include "xxBinary.h"
#include "xxCamera.h"
#include "xxMaterial.h"
#include "xxMesh.h"
#include "xxModifier.h"
#include "xxNode.h"

#define HAVE_LINEAR_MATRIX 1

//==============================================================================
struct LinearMatrixHeader
{
    xxMatrix4*  parentMatrix;
    size_t      childrenCount;
};
static_assert(sizeof(LinearMatrixHeader) <= sizeof(xxMatrix4));
//==============================================================================
//  Node
//==============================================================================
xxNode::xxNode()
{
}
//------------------------------------------------------------------------------
xxNode::~xxNode()
{
    Invalidate();
}
//------------------------------------------------------------------------------
void xxNode::BoneData::ResetPointer()
{
    void** pointer = reinterpret_cast<void**>((char*)&classBoneMatrix + sizeof(classBoneMatrix));
    pointer[0] = &classSkinMatrix;
    pointer[1] = &classBoneMatrix;
}
//------------------------------------------------------------------------------
xxNodePtr const& xxNode::GetParent() const
{
    if (m_parent.expired())
    {
        static xxNodePtr empty;
        return empty;
    }

    return (xxNodePtr&)m_parent;
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
bool xxNode::AttachChild(xxNodePtr const& node)
{
    if (node == nullptr)
        return false;
    if (node->m_parent.use_count())
        return false;
    m_children.push_back(node);
    node->m_parent = m_this;

#if HAVE_LINEAR_MATRIX
    node->m_linearMatrix = std::vector<xxMatrix4>();
    node->m_linearMatrixCreate = false;

    xxNode* root = this;
    while (xxNodePtr const& parent = root->GetParent())
    {
        root = parent.get();
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
    if (node->m_parent.use_count() == 0)
        return false;

    for (xxNodePtr const& child : m_children)
    {
        if (node != child)
            continue;
        node->m_parent.reset();

#if HAVE_LINEAR_MATRIX
        Traversal([](xxNodePtr const& node)
        {
            void** pointer = reinterpret_cast<void**>((char*)&node->Name + sizeof(node->Name));
            node->m_classLocalMatrix = node->LocalMatrix;
            node->m_classWorldMatrix = node->WorldMatrix;
            pointer[0] = &node->m_classLocalMatrix;
            pointer[1] = &node->m_classWorldMatrix;
            return true;
        }, child);

        for (auto& boneData : Bones)
        {
            void** pointer = reinterpret_cast<void**>((char*)&boneData.classBoneMatrix + sizeof(boneData.classBoneMatrix));
            boneData.classSkinMatrix = boneData.skinMatrix;
            boneData.classBoneMatrix = boneData.boneMatrix;
            pointer[0] = &boneData.classSkinMatrix;
            pointer[1] = &boneData.classBoneMatrix;
        }
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
    if (m_parent.use_count())
        return;

    std::function<size_t(xxNode*)> countMatrix = [&countMatrix](xxNode* node)
    {
        size_t count = 0;

        // Children
        if (node->m_children.empty() == false)
        {
            // Header
            count += 1;

            // Children
            for (xxNodePtr const& child : node->m_children)
            {
                if (child == nullptr)
                    continue;
                count += 2;
            }
        }

        // Bone
        for (auto& boneData : node->Bones)
        {
            if (boneData.bone.use_count())
            {
                // Header
                count += 1;

                // Matrix
                count += 2;
            }
        }

        // Traversal
        for (xxNodePtr const& child : node->m_children)
        {
            if (child == nullptr)
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
        // Children
        if (node->m_children.empty() == false)
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
                void** pointer = reinterpret_cast<void**>((char*)&child->Name + sizeof(child->Name));
                linearMatrix[0] = child->LocalMatrix;
                linearMatrix[1] = child->WorldMatrix;
                pointer[0] = &linearMatrix[0];
                pointer[1] = &linearMatrix[1];
                linearMatrix += 2;
            }
        }

        // Bone
        for (auto& boneData : node->Bones)
        {
            if (boneData.bone.use_count())
            {
                xxNodePtr const& bone = (xxNodePtr&)boneData.bone;

                // Header
                LinearMatrixHeader* header = reinterpret_cast<LinearMatrixHeader*>(linearMatrix++);
                header->parentMatrix = &bone->WorldMatrix;
                header->childrenCount = 1;

                // Matrix
                void** pointer = reinterpret_cast<void**>((char*)&boneData.classBoneMatrix + sizeof(boneData.classBoneMatrix));
                linearMatrix[0] = boneData.skinMatrix;
                linearMatrix[1] = boneData.boneMatrix;
                pointer[0] = &linearMatrix[0];
                pointer[1] = &linearMatrix[1];
                linearMatrix += 2;
            }
        }

        // Traversal
        for (xxNodePtr const& child : node->m_children)
        {
            if (child == nullptr)
                continue;
            createLinearMatrix(child.get(), linearMatrix);
        }
    };

    // Root
    xxMatrix4* linearMatrix = newLinearMatrix.data();
    void** pointer = reinterpret_cast<void**>((char*)&Name + sizeof(Name));
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
void xxNode::UpdateMatrix()
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

        return;
    }
#endif
    UpdateRotateTranslateScale();

    xxNodePtr const& parent = GetParent();
    if (parent == nullptr)
    {
        WorldMatrix = LocalMatrix;
    }
    else
    {
        WorldMatrix = parent->WorldMatrix * LocalMatrix;
    }

    for (auto& boneData : Bones)
    {
        if (boneData.bone.use_count())
        {
            xxNodePtr const& bone = (xxNodePtr&)boneData.bone;
            boneData.boneMatrix = bone->WorldMatrix * boneData.skinMatrix;
            continue;
        }
        boneData.boneMatrix = xxMatrix4::IDENTITY;
    }
}
//------------------------------------------------------------------------------
void xxNode::UpdateBound()
{
    WorldBound.xyz = GetWorldTranslate();
    WorldBound.w = 0.0f;
    if (Bones.empty() == false)
    {
        for (auto const& boneData : Bones)
        {
            if (boneData.bone.use_count())
            {
                xxNodePtr const& bone = (xxNodePtr&)boneData.bone;
                WorldBound.BoundMerge(boneData.bound.BoundTransform(bone->WorldMatrix, bone->GetScale()));
                continue;
            }
            boneData.boneMatrix = xxMatrix4::IDENTITY;
        }
    }
    else if (Mesh)
    {
        WorldBound = Mesh->Bound.BoundTransform(WorldMatrix, GetScale());
    }

    for (xxNodePtr const& child : m_children)
    {
        if (child == nullptr)
            continue;
        WorldBound.BoundMerge(child->WorldBound);
    }
}
//------------------------------------------------------------------------------
xxMatrix3 xxNode::GetRotate() const
{
    if (m_legacyScale == 1.0f)
    {
        return { LocalMatrix[0].xyz, LocalMatrix[1].xyz, LocalMatrix[2].xyz };
    }

    return m_legacyRotate;
}
//------------------------------------------------------------------------------
xxVector3 xxNode::GetTranslate() const
{
    if (m_legacyScale == 1.0f)
    {
        return LocalMatrix.v[3].xyz;
    }

    return m_legacyTranslate;
}
//------------------------------------------------------------------------------
void xxNode::SetRotate(xxMatrix3 const& rotate)
{
    if (m_legacyScale == 1.0f)
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
    if (m_legacyScale == 1.0f)
    {
        LocalMatrix.v[3].xyz = translate;
        return;
    }

    m_legacyTranslate = translate;
}
//------------------------------------------------------------------------------
void xxNode::SetScale(float scale)
{
    if (m_legacyScale == 1.0f && scale != 1.0f)
    {
        CreateRotateTranslateScale();
    }

    m_legacyScale = scale;
}
//------------------------------------------------------------------------------
void xxNode::CreateRotateTranslateScale()
{
    if (m_legacyScale == 1.0f)
        return;

    LocalMatrix.FastDecompose(m_legacyRotate, m_legacyTranslate, m_legacyScale);
}
//------------------------------------------------------------------------------
void xxNode::UpdateRotateTranslateScale()
{
    if (m_legacyScale == 1.0f)
        return;

    LocalMatrix.v[0].xyz = m_legacyRotate.v[0] * m_legacyScale;
    LocalMatrix.v[1].xyz = m_legacyRotate.v[1] * m_legacyScale;
    LocalMatrix.v[2].xyz = m_legacyRotate.v[2] * m_legacyScale;
    LocalMatrix.v[3].xyz = m_legacyTranslate;
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
    for (auto& data : Modifiers)
    {
        if (data.modifier == nullptr)
            continue;
        data.modifier->Update(data.modifier.get(), this, &data, time);
    }

#if HAVE_LINEAR_MATRIX
    if (m_linearMatrix.empty() == false)
    {
        for (xxNodePtr const& child : m_children)
        {
            if (child == nullptr)
                continue;
            child->Update(time, false);
        }
        UpdateMatrix();
        UpdateBound();
        return;
    }
#endif

    if (updateMatrix)
    {
        UpdateMatrix();
    }

    for (xxNodePtr const& child : m_children)
    {
        if (child == nullptr)
            continue;
        child->Update(time, updateMatrix);
    }

    UpdateBound();
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
void xxNode::BinaryRead(xxBinary& binary)
{
    binary.ReadString(Name);

    binary.Read(LocalMatrix);

    binary.ReadContainer(Bones, [](xxBinary& binary, BoneData& data)
    {
        xxNodePtr bone;
        data.ResetPointer();
        binary.ReadReference(bone);
        data.bone = bone;
        binary.Read(data.bound);
        binary.Read(data.classSkinMatrix);
    });

    binary.ReadReference(Camera);
    binary.ReadReference(Material);
    binary.ReadReference(Mesh);

    binary.ReadContainer(Modifiers, [](xxBinary& binary, xxModifierData& data)
    {
        binary.ReadReference(data.modifier);
    });

    binary.ReadReferences(m_children);
    for (xxNodePtr const& child : m_children)
    {
        if (child == nullptr)
            return;
        child->m_parent = m_this;
    }
}
//------------------------------------------------------------------------------
void xxNode::BinaryWrite(xxBinary& binary) const
{
    binary.WriteString(Name);

    binary.Write(LocalMatrix);

    binary.WriteContainer(Bones, [](xxBinary& binary, BoneData const& data)
    {
        binary.WriteReference((xxNodePtr const&)data.bone);
        binary.Write(data.bound);
        binary.Write(data.classSkinMatrix);
    });

    binary.WriteReference(Camera);
    binary.WriteReference(Material);
    binary.WriteReference(Mesh);

    binary.WriteContainer(Modifiers, [](xxBinary& binary, xxModifierData const& data)
    {
        binary.WriteReference(data.modifier);
    });

    binary.WriteReferences(m_children);
}
//==============================================================================
