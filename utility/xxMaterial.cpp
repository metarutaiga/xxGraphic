//==============================================================================
// xxGraphic : Material Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include <xxGraphic/xxGraphic.h>
#include "xxBinary.h"
#include "xxCamera.h"
#include "xxTexture.h"
#include "xxMesh.h"
#include "xxModifier.h"
#include "xxNode.h"
#include "xxMaterial.h"

//==============================================================================
//  Material
//==============================================================================
xxMaterial::xxMaterial()
{
}
//------------------------------------------------------------------------------
xxMaterial::~xxMaterial()
{
    Invalidate();
}
//------------------------------------------------------------------------------
void xxMaterial::Invalidate()
{
    xxDestroyBlendState(m_blendState);
    xxDestroyDepthStencilState(m_depthStencilState);
    xxDestroyRasterizerState(m_rasterizerState);
    xxDestroyShader(m_device, m_meshShader);
    xxDestroyShader(m_device, m_vertexShader);
    xxDestroyShader(m_device, m_fragmentShader);
    xxDestroyRenderPass(m_renderPass);
    xxDestroyPipeline(m_pipeline);

    m_device = 0;
    m_blendState = 0;
    m_depthStencilState = 0;
    m_rasterizerState = 0;
    m_meshShader = 0;
    m_vertexShader = 0;
    m_fragmentShader = 0;
    m_renderPass = 0;
    m_pipeline = 0;
}
//------------------------------------------------------------------------------
void xxMaterial::Update(float time)
{
    for (auto& data : Modifiers)
    {
        data.modifier->Update(data.modifier.get(), this, &data, time);
    }
}
//------------------------------------------------------------------------------
void xxMaterial::Setup(xxDrawData const& data)
{
    xxNode* node = data.node;
    if (node->ConstantDatas.size() <= data.materialIndex)
        node->ConstantDatas.resize(data.materialIndex + 1);
    auto* constantData = data.constantData = &node->ConstantDatas[data.materialIndex];

    if (constantData->ready == 0)
    {
        m_device = data.device;
        if (m_pipeline == 0)
            CreatePipeline(data);
        CreateConstant(data);
        constantData->ready = (m_pipeline != 0) ? 1 : -1;
    }

    UpdateConstant(data);
}
//------------------------------------------------------------------------------
void xxMaterial::Draw(xxDrawData const& data) const
{
    auto* constantData = data.constantData;

    xxSetPipeline(data.commandEncoder, constantData->pipeline);
    if (constantData->vertexConstant)
    {
        if (m_meshShader)
        {
            xxSetMeshConstantBuffer(data.commandEncoder, constantData->vertexConstant, constantData->vertexConstantSize);
        }
        else
        {
            xxSetVertexConstantBuffer(data.commandEncoder, constantData->vertexConstant, constantData->vertexConstantSize);
        }
    }
    if (constantData->fragmentConstant)
    {
        xxSetFragmentConstantBuffer(data.commandEncoder, constantData->fragmentConstant, constantData->fragmentConstantSize);
    }

    uint64_t textures[16];
    uint64_t samplers[16];

    int textureCount = (int)Textures.size();
    for (int i = 0; i < textureCount; ++i)
    {
        xxTexturePtr const& texture = Textures[i];
        texture->Update(data.device);
        textures[i] = texture->Texture;
        samplers[i] = texture->Sampler;
    }

    xxSetFragmentTextures(data.commandEncoder, textureCount, textures);
    xxSetFragmentSamplers(data.commandEncoder, textureCount, samplers);
}
//------------------------------------------------------------------------------
xxTexturePtr const& xxMaterial::GetTexture(size_t index) const
{
    if (Textures.size() <= index)
    {
        static xxTexturePtr empty;
        return empty;
    }

    return Textures[index];
}
//------------------------------------------------------------------------------
void xxMaterial::SetTexture(size_t index, xxTexturePtr const& texture)
{
    if (Textures.size() <= index)
    {
        Textures.resize(index + 1);
    }

    Textures[index] = texture;
}
//------------------------------------------------------------------------------
void xxMaterial::CreatePipeline(xxDrawData const& data)
{
    xxMesh* mesh = data.mesh;
    uint64_t vertexAttribute = mesh->GetVertexAttribute();
    if (vertexAttribute == 0)
        return;

    if (m_pipeline == 0)
    {
        if (m_blendState == 0)
        {
            if (Blending)
            {
                m_blendState = xxCreateBlendState(m_device, BlendSourceColor.c_str(),
                                                            BlendOperationColor.c_str(),
                                                            BlendDestinationColor.c_str(),
                                                            BlendSourceAlpha.c_str(),
                                                            BlendOperationAlpha.c_str(),
                                                            BlendDestinationAlpha.c_str());
            }
            else
            {
                m_blendState = xxCreateBlendState(m_device, "1", "+", "0", "1", "+", "0");
            }
        }
        if (m_depthStencilState == 0)
        {
            m_depthStencilState = xxCreateDepthStencilState(m_device, DepthTest.c_str(), DepthWrite);
        }
        if (m_rasterizerState == 0)
        {
            m_rasterizerState = xxCreateRasterizerState(m_device, Cull, Scissor);
        }
        if (m_meshShader == 0 && m_vertexShader == 0 && m_fragmentShader == 0)
        {
            if (m_meshShader == 0 && mesh->Count[xxMesh::STORAGE0] && mesh->Count[xxMesh::STORAGE1] && mesh->Count[xxMesh::STORAGE2])
            {
                m_meshShader = xxCreateMeshShader(m_device, GetShader(data, 0).c_str());
            }
            if (m_meshShader == 0 && m_vertexShader == 0)
            {
                m_vertexShader = xxCreateVertexShader(m_device, GetShader(data, 0).c_str(), vertexAttribute);
            }
            if (m_fragmentShader == 0)
            {
                m_fragmentShader = xxCreateFragmentShader(m_device, GetShader(data, 1).c_str());
            }
        }
        if (m_renderPass == 0)
        {
            m_renderPass = xxCreateRenderPass(m_device, true, true, true, true, true, true);
        }
        m_pipeline = xxCreatePipeline(m_device, m_renderPass, m_blendState, m_depthStencilState, m_rasterizerState, vertexAttribute, m_meshShader, m_vertexShader, m_fragmentShader);
    }
}
//------------------------------------------------------------------------------
void xxMaterial::CreateConstant(xxDrawData const& data) const
{
    auto* constantData = data.constantData;

    if (constantData->vertexConstant == 0 && constantData->fragmentConstant == 0)
    {
        constantData->device = data.device;
        constantData->pipeline = m_pipeline;
        if (constantData->vertexConstant == 0)
        {
            constantData->vertexConstantSize = GetVertexConstantSize(data);
            if (constantData->vertexConstantSize > 0)
            {
                constantData->vertexConstant = xxCreateConstantBuffer(m_device, constantData->vertexConstantSize);
            }
        }
        if (constantData->fragmentConstant == 0)
        {
            constantData->fragmentConstantSize = GetFragmentConstantSize(data);
            if (constantData->fragmentConstantSize > 0)
            {
                constantData->fragmentConstant = xxCreateConstantBuffer(m_device, constantData->fragmentConstantSize);
            }
        }
    }
}
//------------------------------------------------------------------------------
void xxMaterial::UpdateConstant(xxDrawData const& data) const
{
    size_t size;
    xxCamera* camera = data.camera;
    auto* constantData = data.constantData;

    size = constantData->vertexConstantSize;
    if (size)
    {
        xxVector4* vector = reinterpret_cast<xxVector4*>(xxMapBuffer(m_device, constantData->vertexConstant));
        if (vector)
        {
            if (size >= 3 * sizeof(xxMatrix4))
            {
                xxMatrix4* wvp = reinterpret_cast<xxMatrix4*>(vector);
                wvp[0] = data.node->WorldMatrix;

                if (camera)
                {
                    wvp[1] = camera->ViewMatrix;
                    wvp[2] = camera->ProjectionMatrix;
                }
                else
                {
                    wvp[1] = xxMatrix4::IDENTITY;
                    wvp[2] = xxMatrix4::IDENTITY;
                }
                vector += 3 * 4;
                size -= 3 * sizeof(xxMatrix4);
            }

            if (size >= 12 * sizeof(xxVector4) && m_meshShader && (BackfaceCulling || FrustumCulling))
            {
                xxVector4* frustum = reinterpret_cast<xxVector4*>(vector);

                if (camera)
                {
                    for (int i = 0; i < 12; ++i)
                    {
                        frustum[i] = data.frustum[i];
                    }
                }
                else
                {
                    for (int i = 0; i < 12; ++i)
                    {
                        frustum[i] = xxVector4::ZERO;
                    }
                }
                vector += 12;
                size -= 12 * sizeof(xxVector4);
            }

            if (size >= 75 * sizeof(xxMatrix4x3) && data.mesh->Skinning)
            {
                xxMatrix4x3* boneMatrix = reinterpret_cast<xxMatrix4x3*>(vector);

                for (auto const& data : data.node->Bones)
                {
                    (*boneMatrix++) = xxMatrix4x3::FromMatrix4(data.boneMatrix);
                }
                vector += 75 * 3;
                size -= 75 * sizeof(xxMatrix4x3);
            }

            if (size >= 1 * sizeof(xxVector4) && Blending)
            {
                vector[0].x = Opacity;
                vector += 1;
                size -= 1 * sizeof(xxVector4);
            }

            if (size >= 5 * sizeof(xxVector4) && Lighting)
            {
                if (camera)
                {
                    vector[0].xyz = camera->LightDirection;
                    vector[1].xyz = camera->LightColor;
                }
                else
                {
                    vector[0].xyz = xxVector3::Y;
                    vector[1].xyz = xxVector3::WHITE;
                }
                vector[2].xyz = AmbientColor;
                vector[3].xyz = DiffuseColor;
                vector[4].xyz = EmissiveColor;
                vector += 5;
                size -= 5 * sizeof(xxVector4);
            }
            xxUnmapBuffer(m_device, constantData->vertexConstant);
        }
    }

    size = constantData->fragmentConstantSize;
    if (size)
    {
        xxVector4* vector = reinterpret_cast<xxVector4*>(xxMapBuffer(m_device, constantData->fragmentConstant));
        if (vector)
        {
            if (size >= 1 * sizeof(xxVector4) && AlphaTest)
            {
                vector[0].x = AlphaTestReference;
                vector += 1;
                size -= 1 * sizeof(xxVector4);
            }
            if (size >= 3 * sizeof(xxVector4) && Specular)
            {
                if (camera)
                {
                    vector[0].xyz = camera->Location;
                    vector[1].xyz = camera->LightDirection;
                }
                else
                {
                    vector[0].xyz = xxVector3::ZERO;
                    vector[1].xyz = xxVector3::Y;
                }
                vector[2].xyz = SpecularColor;
                vector[2].w = SpecularHighlight;
                vector += 3;
                size -= 3 * sizeof(xxVector4);
            }
            xxUnmapBuffer(m_device, constantData->fragmentConstant);
        }
    }
}
//------------------------------------------------------------------------------
std::string xxMaterial::GetShader(xxDrawData const& data, int type) const
{
    xxMesh* mesh = data.mesh;

    auto define = [](char const* name, size_t value)
    {
        return std::string("#define") + ' ' + name + ' ' + std::to_string(value) + '\n';
    };

    size_t constantSize = 0;
    switch (type)
    {
    case 0:
        constantSize = GetVertexConstantSize(data) / sizeof(xxVector4);
        break;
    case 1:
        constantSize = GetFragmentConstantSize(data) / sizeof(xxVector4);
        break;
    }

    std::string shader;
    shader += define("SHADER_BACKFACE_CULLING", type == 0 && BackfaceCulling ? 1 : 0);
    shader += define("SHADER_FRUSTUM_CULLING", type == 0 && FrustumCulling ? 1 : 0);
    shader += define("SHADER_SKINNING", type == 0 && mesh->Skinning ? 1 : 0);
    shader += define("SHADER_NORMAL", mesh->NormalCount);
    shader += define("SHADER_COLOR", mesh->ColorCount);
    shader += define("SHADER_TEXTURE", mesh->TextureCount);
    shader += define("SHADER_UNIFORM", constantSize);
    shader += define("SHADER_ALPHATEST", type == 1 && AlphaTest ? 1 : 0);
    shader += define("SHADER_OPACITY", type == 0 && Blending ? 1 : 0);
    shader += define("SHADER_LIGHTING", mesh->NormalCount && Lighting ? 1 : 0);
    shader += define("SHADER_SPECULAR", mesh->NormalCount && Lighting && Specular ? 1 : 0);
    shader += ShaderOption;
    shader += Shader.empty() ? DefaultShader : Shader;

    return shader;
}
//------------------------------------------------------------------------------
int xxMaterial::GetVertexConstantSize(xxDrawData const& data) const
{
    int size = 3 * sizeof(xxMatrix4);
    if (m_meshShader && (BackfaceCulling || FrustumCulling))
    {
        size += 12 * sizeof(xxVector4);
    }
    if (data.mesh->Skinning)
    {
        size += 75 * sizeof(xxMatrix4x3);
    }
    if (Blending)
    {
        size += 1 * sizeof(xxVector4);
    }
    if (Lighting)
    {
        size += 5 * sizeof(xxVector4);
    }
    return size;
}
//------------------------------------------------------------------------------
int xxMaterial::GetFragmentConstantSize(xxDrawData const& data) const
{
    int size = 0;
    if (AlphaTest)
    {
        size += 1 * sizeof(xxVector4);
    }
    if (Specular)
    {
        size += 3 * sizeof(xxVector4);
    }
    return size;
}
//------------------------------------------------------------------------------
xxMaterialPtr xxMaterial::Create()
{
    xxMaterialPtr material = xxMaterialPtr(new xxMaterial(), [](xxMaterial* material) { delete material; });
    if (material == nullptr)
        return nullptr;

    return material;
}
//==============================================================================
//  Binary
//==============================================================================
xxMaterialPtr (*xxMaterial::BinaryCreate)() = xxMaterial::Create;
//------------------------------------------------------------------------------
void xxMaterial::BinaryRead(xxBinary& binary)
{
    binary.ReadString(Name);

    binary.ReadString(Shader);
    binary.ReadString(ShaderOption);

    binary.Read(AmbientColor);
    binary.Read(DiffuseColor);
    binary.Read(EmissiveColor);
    binary.Read(SpecularColor);
    binary.Read(SpecularHighlight);
    binary.Read(Opacity);

    binary.Read(Lighting);
    binary.Read(Specular);

    binary.Read(AlphaTest);
    if (AlphaTest)
    {
        binary.Read(AlphaTestReference);
    }

    binary.Read(Blending);
    if (Blending)
    {
        binary.ReadString(BlendSourceColor);
        binary.ReadString(BlendOperationColor);
        binary.ReadString(BlendDestinationColor);
        binary.ReadString(BlendSourceAlpha);
        binary.ReadString(BlendOperationAlpha);
        binary.ReadString(BlendDestinationAlpha);
    }

    binary.ReadString(DepthTest);
    binary.Read(DepthWrite);

    binary.Read(Cull);
    binary.Read(Scissor);

    if (binary.Version >= 0x20241221)
    {
        binary.Read(BackfaceCulling);
        binary.Read(FrustumCulling);
    }

    binary.ReadContainer(Colors);
    binary.ReadReferences(Textures);
    for (auto& texture : Textures)
    {
        xxTexture::Loader(texture, binary.Path);
    }

    binary.ReadContainer(Modifiers, [](xxBinary& binary, xxModifierData& data)
    {
        binary.ReadReference(data.modifier);
    });
}
//------------------------------------------------------------------------------
void xxMaterial::BinaryWrite(xxBinary& binary) const
{
    binary.WriteString(Name);

    binary.WriteString(Shader);
    binary.WriteString(ShaderOption);

    binary.Write(AmbientColor);
    binary.Write(DiffuseColor);
    binary.Write(EmissiveColor);
    binary.Write(SpecularColor);
    binary.Write(SpecularHighlight);
    binary.Write(Opacity);

    binary.Write(Lighting);
    binary.Write(Specular);

    binary.Write(AlphaTest);
    if (AlphaTest)
    {
        binary.Write(AlphaTestReference);
    }

    binary.Write(Blending);
    if (Blending)
    {
        binary.WriteString(BlendSourceColor);
        binary.WriteString(BlendOperationColor);
        binary.WriteString(BlendDestinationColor);
        binary.WriteString(BlendSourceAlpha);
        binary.WriteString(BlendOperationAlpha);
        binary.WriteString(BlendDestinationAlpha);
    }

    binary.WriteString(DepthTest);
    binary.Write(DepthWrite);

    binary.Write(Cull);
    binary.Write(Scissor);

    binary.Write(BackfaceCulling);
    binary.Write(FrustumCulling);

    binary.WriteContainer(Colors);
    binary.WriteReferences(Textures);

    binary.WriteContainer(Modifiers, [](xxBinary& binary, xxModifierData const& data)
    {
        binary.WriteReference(data.modifier);
    });
}
//==============================================================================
//  Default Shader
//==============================================================================
#if defined(__clang__)
extern char const* const glDefaultShaderCode = xxMaterial::DefaultShader;
extern char const* const mtlDefaultShaderCode = xxMaterial::DefaultShader;
#else
#pragma comment(linker, "/alternatename:glDefaultShaderCode=?DefaultShader@xxMaterial@@2QBDB")
#endif
//------------------------------------------------------------------------------
char const xxMaterial::DefaultShader[] =
//------------------------------------------------------------------------------
//  Default
//------------------------------------------------------------------------------
R"(
#ifndef SHADER_GLSL
#define SHADER_GLSL 0
#endif
#ifndef SHADER_HLSL
#define SHADER_HLSL 0
#endif
#ifndef SHADER_MSL
#define SHADER_MSL 0
#endif
#ifndef SHADER_MESH
#define SHADER_MESH 0
#endif
#ifndef SHADER_VERTEX
#define SHADER_VERTEX 0
#endif
#ifndef SHADER_FRAGMENT
#define SHADER_FRAGMENT 0
#endif
#ifndef SHADER_BACKFACE_CULLING
#define SHADER_BACKFACE_CULLING 0
#endif
#ifndef SHADER_FRUSTUM_CULLING
#define SHADER_FRUSTUM_CULLING 0
#endif
#ifndef SHADER_SKINNING
#define SHADER_SKINNING 0
#endif
#ifndef SHADER_NORMAL
#define SHADER_NORMAL 0
#endif
#ifndef SHADER_COLOR
#define SHADER_COLOR 1
#endif
#ifndef SHADER_TEXTURE
#define SHADER_TEXTURE 1
#endif
#ifndef SHADER_UNIFORM
#define SHADER_UNIFORM 12
#endif
#ifndef SHADER_ALPHATEST
#define SHADER_ALPHATEST 0
#endif
#ifndef SHADER_OPACITY
#define SHADER_OPACITY 0
#endif
#ifndef SHADER_LIGHTING
#define SHADER_LIGHTING 0
#endif
#ifndef SHADER_SPECULAR
#define SHADER_SPECULAR 0
#endif)"
//------------------------------------------------------------------------------
//  Compatible type
//------------------------------------------------------------------------------
R"(
#if SHADER_GLSL
#define float2 vec2
#define float3 vec3
#define float4 vec4
#define float2x2 mat2
#define float3x3 mat3
#define float4x4 mat4
#define int2 ivec2
#define int3 ivec3
#define int4 ivec4
#endif)"
//------------------------------------------------------------------------------
//  mul
//------------------------------------------------------------------------------
R"(
#if SHADER_GLSL || SHADER_MSL
#define mul(a, b) (b * a)
#endif)"
//------------------------------------------------------------------------------
//  Structure
//------------------------------------------------------------------------------
R"(
#if SHADER_MESH
#if SHADER_MSL
struct Vertex
{
    packed_float3 Position;
#if SHADER_SKINNING
    packed_float3 BoneWeight;
    packed_uint4 BoneIndices;
#endif
#if SHADER_NORMAL
    packed_float3 Normal;
#endif
#if SHADER_COLOR
    packed_float4 Color;
#endif
#if SHADER_TEXTURE
    packed_float2 UV0;
#endif
};
struct Meshlet
{
    uint VertexOffset;
    uint TriangleOffset;
    uint VertexCount;
    uint TriangleCount;
    float4 CenterRadius;
    float4 ConeApex;
    float4 ConeAxisCutoff;
};
#endif
#endif)"
//------------------------------------------------------------------------------
//  Uniform
//------------------------------------------------------------------------------
R"(
#if SHADER_GLSL || SHADER_HLSL
#if SHADER_UNIFORM
uniform float4 uniBuffer[SHADER_UNIFORM];
#endif
#if SHADER_HLSL >= 10
sampler samDiffuse;
Texture2D<float4> texDiffuse;
#else
uniform sampler2D samDiffuse;
#endif
#elif SHADER_MSL
struct Uniform
{
#if SHADER_MSL >= 2
#if SHADER_MESH
    device Vertex* Vertices     [[id(0)]];
    device Meshlet* Meshlets    [[id(1)]];
    device uint* VertexIndices  [[id(2)]];
    device uint* TriangeIndices [[id(3)]];
    device float4* Buffer       [[id(8)]];
#elif SHADER_VERTEX
    device float4* Buffer       [[id(0)]];
#else
    device float4* Buffer       [[id(1)]];
    texture2d<float> Diffuse    [[id(4)]];
    sampler DiffuseSampler      [[id(18)]];
#endif
#elif SHADER_UNIFORM
    float4 Buffer[SHADER_UNIFORM];
#endif
};
struct Sampler
{
    texture2d<float> Diffuse    [[texture(0)]];
    sampler DiffuseSampler      [[sampler(0)]];
};
#endif)"
//------------------------------------------------------------------------------
//  Attribute
//------------------------------------------------------------------------------
R"(
#if SHADER_VERTEX
#if SHADER_GLSL
attribute vec3 attrPosition;
#if SHADER_SKINNING
attribute vec3 attrBoneWeight;
attribute vec4 attrBoneIndices;
#endif
#if SHADER_NORMAL
attribute vec3 attrNormal;
#endif
#if SHADER_COLOR
attribute vec4 attrColor;
#endif
#if SHADER_TEXTURE
attribute vec2 attrUV0;
#endif
#elif SHADER_HLSL
struct Attribute
{
    float3 Position     : POSITION;
#if SHADER_SKINNING
    float3 BoneWeight   : BLENDWEIGHT;
    uint4 BoneIndices   : BLENDINDICES;
#endif
#if SHADER_NORMAL
    float3 Normal       : NORMAL;
#endif
#if SHADER_COLOR
    float4 Color        : COLOR;
#endif
#if SHADER_TEXTURE
    float2 UV0          : TEXCOORD;
#endif
};
#elif SHADER_MSL
struct Attribute
{
    float3 Position     [[attribute(__COUNTER__)]];
#if SHADER_SKINNING
    float3 BoneWeight   [[attribute(__COUNTER__)]];
    uint4 BoneIndices   [[attribute(__COUNTER__)]];
#endif
#if SHADER_NORMAL
    float3 Normal       [[attribute(__COUNTER__)]];
#endif
#if SHADER_COLOR
    float4 Color        [[attribute(__COUNTER__)]];
#endif
#if SHADER_TEXTURE
    float2 UV0          [[attribute(__COUNTER__)]];
#endif
};
#endif
#endif)"
//------------------------------------------------------------------------------
//  Varying
//------------------------------------------------------------------------------
R"(
#if SHADER_GLSL
#if SHADER_COLOR || SHADER_LIGHTING
varying vec4 varyColor;
#if SHADER_SPECULAR
varying vec3 varyWorldPosition;
varying vec3 varyWorldNormal;
#endif
#endif
#if SHADER_TEXTURE
varying vec2 varyUV0;
#endif
#elif SHADER_HLSL
struct Varying
{
    float4 Position         : SV_POSITION;
#if SHADER_COLOR || SHADER_LIGHTING
    float4 Color            : COLOR;
#if SHADER_SPECULAR
    float3 WorldPosition    : COLOR1;
    float3 WorldNormal      : COLOR2;
#endif
#endif
#if SHADER_TEXTURE
    float2 UV0              : TEXCOORD0;
#endif
};
#elif SHADER_MSL
struct Varying
{
    float4 Position [[position]];
#if SHADER_COLOR || SHADER_LIGHTING
    float4 Color;
#if SHADER_SPECULAR
    float3 WorldPosition;
    float3 WorldNormal;
#endif
#endif
#if SHADER_TEXTURE
    float2 UV0;
#endif
};
#endif)"
//------------------------------------------------------------------------------
//  Mesh / Vertex Shader
//------------------------------------------------------------------------------
R"(
#if SHADER_MESH
#if SHADER_MSL
[[mesh]]
#if SHADER_MSL >= 2
void Main(constant Uniform& uni [[buffer(0)]],
#else
void Main(constant Uniform& uni [[buffer(8)]],
          device Vertex* Vertices [[buffer(0)]],
          device Meshlet* Meshlets [[buffer(1)]],
          device uint* VertexIndices [[buffer(2)]],
          device uint* TriangeIndices [[buffer(3)]],
#endif
          uint gtid [[thread_position_in_threadgroup]],
          uint gid [[threadgroup_position_in_grid]],
          mesh<Varying, void, 64, 128, topology::triangle> output)
#endif
#endif
#if SHADER_VERTEX
#if SHADER_GLSL
void main()
#elif SHADER_HLSL
Varying Main(Attribute attr)
#elif SHADER_MSL
vertex Varying Main(Attribute attr [[stage_in]], constant Uniform& uni [[buffer(0)]])
#endif
#endif
#if SHADER_MESH || SHADER_VERTEX
{
    int uniIndex = 0;
#if SHADER_MSL
    auto uniBuffer = uni.Buffer;
#endif

#if SHADER_MESH
#if SHADER_MSL
#if SHADER_MSL >= 2
    device Vertex* Vertices = uni.Vertices;
    device Meshlet* Meshlets = uni.Meshlets;
    device uint* VertexIndices = uni.VertexIndices;
    device uint* TriangeIndices = uni.TriangeIndices;
#endif
    device Meshlet& m = Meshlets[gid];
#if SHADER_BACKFACE_CULLING || SHADER_FRUSTUM_CULLING
    uint visible = 1;
    if (simd_is_first())
    {
#if SHADER_BACKFACE_CULLING
        if (visible)
        {
            float c = m.ConeAxisCutoff.w; 
            float d = dot(m.ConeAxisCutoff.xyz, normalize(m.ConeApex.xyz - uniBuffer[13].xyz));
            if (d >= c)
                visible = 0;
        }
#endif
#if SHADER_FRUSTUM_CULLING
        if (visible)
        {
            float r = -m.CenterRadius.w;
            float d0 = dot(uniBuffer[12].xyz, m.CenterRadius.xyz - uniBuffer[13].xyz);
            float d1 = dot(uniBuffer[14].xyz, m.CenterRadius.xyz - uniBuffer[15].xyz);
            float d2 = dot(uniBuffer[16].xyz, m.CenterRadius.xyz - uniBuffer[17].xyz);
            float d3 = dot(uniBuffer[18].xyz, m.CenterRadius.xyz - uniBuffer[19].xyz);
            float d4 = dot(uniBuffer[20].xyz, m.CenterRadius.xyz - uniBuffer[21].xyz);
            float d5 = dot(uniBuffer[22].xyz, m.CenterRadius.xyz - uniBuffer[23].xyz);
            if (d0 < r || d1 < r || d2 < r || d3 < r || d4 < r || d5 < r)
                visible = 0;
        }
#endif
    }
    visible = simd_broadcast_first(visible);
    if (visible == 0)
        return;
    uniIndex += 12;
#endif
    if (gtid == 0)
    {
        output.set_primitive_count(m.TriangleCount);
    }

    if (gtid < m.TriangleCount)
    {
        uint index = 3 * gtid;
        uint packed = TriangeIndices[m.TriangleOffset + gtid];
        output.set_index(index + 0, (packed >>  0) & 0xFF);
        output.set_index(index + 1, (packed >>  8) & 0xFF);
        output.set_index(index + 2, (packed >> 16) & 0xFF);
    }

    if (gtid >= m.VertexCount)
        return;
    uint vertexIndex = VertexIndices[m.VertexOffset + gtid];
    device Vertex& attr = Vertices[vertexIndex];
#endif
#endif
#if SHADER_HLSL || SHADER_MSL
    float3 attrPosition = attr.Position;
#if SHADER_SKINNING
    float3 attrBoneWeight = attr.BoneWeight;
    uint4 attrBoneIndices = attr.BoneIndices;
#endif
#if SHADER_NORMAL
    float3 attrNormal = attr.Normal;
#endif
#if SHADER_COLOR
    float4 attrColor = attr.Color;
#endif
#if SHADER_TEXTURE
    float2 attrUV0 = attr.UV0;
#endif
#endif

    float4x4 world = float4x4(uniBuffer[0], uniBuffer[1], uniBuffer[2], uniBuffer[3]);
    float4x4 view = float4x4(uniBuffer[4], uniBuffer[5], uniBuffer[6], uniBuffer[7]);
    float4x4 projection = float4x4(uniBuffer[8], uniBuffer[9], uniBuffer[10], uniBuffer[11]);
    uniIndex += 12;

#if SHADER_SKINNING
    float4 zero4 = float4(0.0, 0.0, 0.0, 0.0);
    float4 boneWeight = float4(attrBoneWeight, 1.0 - attrBoneWeight.x - attrBoneWeight.y - attrBoneWeight.z);
    int4 boneIndices = int4(attrBoneIndices);
    world  = float4x4(uniBuffer[boneIndices.x * 3 + 12], uniBuffer[boneIndices.x * 3 + 13], uniBuffer[boneIndices.x * 3 + 14], zero4) * boneWeight.x;
    world += float4x4(uniBuffer[boneIndices.y * 3 + 12], uniBuffer[boneIndices.y * 3 + 13], uniBuffer[boneIndices.y * 3 + 14], zero4) * boneWeight.y;
    world += float4x4(uniBuffer[boneIndices.z * 3 + 12], uniBuffer[boneIndices.z * 3 + 13], uniBuffer[boneIndices.z * 3 + 14], zero4) * boneWeight.z;
    world += float4x4(uniBuffer[boneIndices.w * 3 + 12], uniBuffer[boneIndices.w * 3 + 13], uniBuffer[boneIndices.w * 3 + 14], zero4) * boneWeight.w;
    world = transpose(world);
    world[3][3] = 1.0;
    uniIndex += 75 * 3;
#endif
    float4 worldPosition = mul(float4(attrPosition, 1.0), world);
    float4 screenPosition = mul(mul(worldPosition, view), projection);

    float3 normal = float3(0.0, 0.0, 1.0);
    float4 color = float4(1.0, 1.0, 1.0, 1.0);
#if SHADER_NORMAL
    normal = mul(float4(attrNormal, 1.0), world).xyz;
#endif
#if SHADER_COLOR
    color = attrColor;
#endif
#if SHADER_OPACITY
    color.a = uniBuffer[uniIndex++].x;
#endif
#if SHADER_LIGHTING
    float3 worldNormal = normalize(mul(float4(normal, 1.0), world).xyz);
    float3 lightDirection = uniBuffer[uniIndex++].xyz;
    float3 lightColor = uniBuffer[uniIndex++].xyz;
    float3 ambientColor = uniBuffer[uniIndex++].xyz;
    float3 diffuseColor = uniBuffer[uniIndex++].xyz;
    float3 emissiveColor = uniBuffer[uniIndex++].xyz;
    float3 N = worldNormal;
    float3 L = lightDirection;
    float lambert = dot(N, L);
    color.rgb = color.rgb * (ambientColor + diffuseColor * lightColor * lambert) + emissiveColor;
#endif

#if SHADER_GLSL
    gl_Position = screenPosition;
#if SHADER_COLOR || SHADER_LIGHTING
    varyColor = color;
#if SHADER_SPECULAR
    varyWorldPosition = worldPosition.xyz;
    varyWorldNormal = worldNormal;
#endif
#endif
#if SHADER_TEXTURE
    varyUV0 = attrUV0;
#endif
#elif SHADER_HLSL || SHADER_MSL
    Varying vary;
    vary.Position = screenPosition;
#if SHADER_COLOR || SHADER_LIGHTING
    vary.Color = color;
#if SHADER_SPECULAR
    vary.WorldPosition = worldPosition.xyz;
    vary.WorldNormal = worldNormal;
#endif
#endif
#if SHADER_TEXTURE
    vary.UV0 = attrUV0;
#endif
#if SHADER_MESH
    output.set_vertex(gtid, vary);
#endif
#if SHADER_VERTEX
    return vary;
#endif
#endif
}
#endif)"
//------------------------------------------------------------------------------
//  Fragment Shader
//------------------------------------------------------------------------------
R"(
#if SHADER_FRAGMENT
#if SHADER_GLSL
void main()
#elif SHADER_HLSL
float4 Main(Varying vary) : COLOR0
#elif SHADER_MSL
fragment float4 Main(Varying vary [[stage_in]], constant Uniform& uni [[buffer(0)]], Sampler sam)
#endif
{
#if SHADER_HLSL || SHADER_MSL
#if SHADER_COLOR || SHADER_LIGHTING
    float4 varyColor = vary.Color;
#if SHADER_SPECULAR
    float3 varyWorldPosition = vary.WorldPosition;
    float3 varyWorldNormal = vary.WorldNormal;
#endif
#endif
#endif

    float4 color = float4(1.0, 1.0, 1.0, 1.0);
#if SHADER_COLOR || SHADER_LIGHTING
    color = varyColor;
#endif

#if SHADER_GLSL && SHADER_TEXTURE
    color = color * texture2D(samDiffuse, varyUV0);
#elif SHADER_HLSL && SHADER_TEXTURE
#if SHADER_HLSL >= 10
    color = color * texDiffuse.Sample(samDiffuse, vary.UV0);
#else
    color = color * tex2D(samDiffuse, vary.UV0);
#endif
#elif SHADER_MSL && SHADER_TEXTURE
#if SHADER_MSL >= 2
    color = color * uni.Diffuse.sample(uni.DiffuseSampler, vary.UV0);
#else
    color = color * sam.Diffuse.sample(sam.DiffuseSampler, vary.UV0);
#endif
#endif

#if SHADER_UNIFORM
#if SHADER_MSL
    auto uniBuffer = uni.Buffer;
#endif
    int uniIndex = 0;
#if SHADER_ALPHATEST
    float alphaRef = uniBuffer[uniIndex++].x;
    if (color.a < alphaRef)
    {
#if SHADER_GLSL
        discard;
#elif SHADER_HLSL
        clip(-1);
#elif SHADER_MSL
        discard_fragment();
#endif
    }
#endif
#if SHADER_SPECULAR
    float3 cameraPosition = uniBuffer[uniIndex++].xyz;
    float3 lightDirection = uniBuffer[uniIndex++].xyz;
    float3 specularColor = uniBuffer[uniIndex].xyz;
    float specularHighlight = uniBuffer[uniIndex++].w;
    float3 L = lightDirection;
    float3 V = normalize(cameraPosition - varyWorldPosition);
    float3 N = normalize(varyWorldNormal);
    float3 H = normalize(V + L);
    float phong = pow(max(dot(N, H), 0.0001), specularHighlight);
    color.rgb = color.rgb + specularColor * phong;
#endif
#endif

#if SHADER_GLSL
    gl_FragColor = color;
#elif SHADER_HLSL || SHADER_MSL
    return color;
#endif
}
#endif)";
