//==============================================================================
// xxGraphic : Material Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphic.h"
#include "xxCamera.h"
#include "xxMesh.h"
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
xxMaterialPtr xxMaterial::Create()
{
    xxMaterialPtr material = xxMaterialPtr(new xxMaterial());
    if (material == nullptr)
        return xxMaterialPtr();

    return material;
}
//------------------------------------------------------------------------------
void xxMaterial::Invalidate()
{
    xxDestroyBlendState(m_blendState);
    xxDestroyDepthStencilState(m_depthStencilState);
    xxDestroyRasterizerState(m_rasterizerState);
    xxDestroyShader(m_device, m_vertexShader);
    xxDestroyShader(m_device, m_fragmentShader);
    xxDestroyRenderPass(m_renderPass);
    xxDestroyPipeline(m_pipeline);

    m_device = 0;
    m_blendState = 0;
    m_depthStencilState = 0;
    m_rasterizerState = 0;
    m_vertexShader = 0;
    m_fragmentShader = 0;
    m_renderPass = 0;
    m_pipeline = 0;
}
//------------------------------------------------------------------------------
void xxMaterial::Update(xxDrawData const& data)
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
void xxMaterial::Set(xxDrawData const& data) const
{
    auto* constantData = data.constantData;

    xxSetPipeline(data.commandEncoder, constantData->pipeline);
    if (constantData->vertexConstant)
        xxSetVertexConstantBuffer(data.commandEncoder, constantData->vertexConstant, constantData->vertexConstantSize);
    if (constantData->fragmentConstant)
        xxSetFragmentConstantBuffer(data.commandEncoder, constantData->fragmentConstant, constantData->fragmentConstantSize);
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
                m_blendState = xxCreateBlendState(m_device, BlendSourceColor, BlendOperationColor, BlendDestinationColor, BlendSourceAlpha, BlendOperationAlpha, BlendDestinationAlpha);
            }
            else
            {
                m_blendState = xxCreateBlendState(m_device, "1", "+", "0", "1", "+", "0");
            }
        }
        if (m_depthStencilState == 0)
        {
            m_depthStencilState = xxCreateDepthStencilState(m_device, DepthTest, DepthWrite);
        }
        if (m_rasterizerState == 0)
        {
            m_rasterizerState = xxCreateRasterizerState(m_device, Cull, Scissor);
        }
        if (m_vertexShader == 0 || m_fragmentShader == 0)
        {
            if (m_vertexShader == 0)
            {
                m_vertexShader = xxCreateVertexShader(m_device, GetShader(data.mesh, 0).c_str(), vertexAttribute);
            }
            if (m_fragmentShader == 0)
            {
                m_fragmentShader = xxCreateFragmentShader(m_device, GetShader(data.mesh, 1).c_str());
            }
        }
        if (m_renderPass == 0)
        {
            m_renderPass = xxCreateRenderPass(m_device, true, true, true, true, true, true);
        }
        m_pipeline = xxCreatePipeline(m_device, m_renderPass, m_blendState, m_depthStencilState, m_rasterizerState, vertexAttribute, m_vertexShader, m_fragmentShader);
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
            constantData->vertexConstantSize = GetVertexConstantSize();
            if (constantData->vertexConstantSize > 0)
            {
                constantData->vertexConstant = xxCreateConstantBuffer(m_device, constantData->vertexConstantSize);
            }
        }
        if (constantData->fragmentConstant == 0)
        {
            constantData->fragmentConstantSize = GetFragmentConstantSize();
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

                xxCamera* camera = data.camera;
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

                if (Lighting && size >= 5 * sizeof(xxVector4))
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
            if (Specular && size >= 3 * sizeof(xxVector4))
            {
                xxCamera* camera = data.camera;
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
std::string xxMaterial::GetShader(xxMesh const* mesh, int type) const
{
    auto define = [](char const* name, size_t value)
    {
        char integer[32];
        std::string line;
        snprintf(integer, 32, "%zd", value);
        line += "#define"; line += ' '; line += name; line += ' '; line += integer; line += '\n';
        return line;
    };

    size_t constantSize = 0;
    switch (type)
    {
    case 0:
        constantSize = GetVertexConstantSize() / sizeof(xxVector4);
        break;
    case 1:
        constantSize = GetFragmentConstantSize() / sizeof(xxVector4);
        break;
    }
    if (constantSize == 0)
        constantSize = 1;

    std::string shader;
    shader += define("SHADER_NORMAL", mesh->NormalCount);
    shader += define("SHADER_COLOR", mesh->ColorCount);
    shader += define("SHADER_TEXTURE", mesh->TextureCount);
    shader += define("SHADER_UNIFORM", constantSize);
    shader += define("SHADER_LIGHTING", mesh->NormalCount && Lighting ? 1 : 0);
    shader += define("SHADER_SPECULAR", mesh->NormalCount && Lighting && Specular ? 1 : 0);
    shader += ShaderOption;
    shader += Shader;

    return shader;
}
//------------------------------------------------------------------------------
int xxMaterial::GetVertexConstantSize() const
{
    int size = 3 * sizeof(xxMatrix4);
    if (Lighting)
    {
        size += 5 * sizeof(xxVector4);
    }
    return size;
}
//------------------------------------------------------------------------------
int xxMaterial::GetFragmentConstantSize() const
{
    int size = 0;
    if (Specular)
    {
        size += 3 * sizeof(xxVector4);
    }
    return size;
}
//==============================================================================
//  Default Shader
//==============================================================================
char const xxMaterial::ms_defaultShader[] =
//------------------------------------------------------------------------------
//  Compatible type
//------------------------------------------------------------------------------
R"(
#if SHADER_GLSL
#define float2 vec2
#define float3 vec3
#define float4 vec4
#define float4x4 mat4
#endif)"
//------------------------------------------------------------------------------
//  mul
//------------------------------------------------------------------------------
R"(
#if SHADER_GLSL || SHADER_MSL
#define mul(a, b) (b * a)
#endif)"
//------------------------------------------------------------------------------
//  Uniform
//------------------------------------------------------------------------------
R"(
#if SHADER_GLSL || SHADER_HLSL
uniform float4 uniBuffer[SHADER_UNIFORM];
#elif SHADER_MSL
struct Uniform
{
#if SHADER_MSL_ARGUMENT
    device float4* Buffer [[id(0)]];
#else
    float4 Buffer[SHADER_UNIFORM];
#endif
};
#endif)"
//------------------------------------------------------------------------------
//  Attribute
//------------------------------------------------------------------------------
R"(
#if SHADER_GLSL
attribute vec3 attrPosition;
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
    float3 Position : POSITION;
#if SHADER_NORMAL
    float3 Normal   : NORMAL;
#endif
#if SHADER_COLOR
    float4 Color    : COLOR;
#endif
#if SHADER_TEXTURE
    float2 UV0      : TEXCOORD;
#endif
};
#elif SHADER_MSL
struct Attribute
{
    float3 Position [[attribute(0)]];
#if SHADER_NORMAL
    float3 Normal   [[attribute(1)]];
#endif
#if SHADER_COLOR
    float4 Color    [[attribute(1 + SHADER_NORMAL)]];
#endif
#if SHADER_TEXTURE
    float2 UV0      [[attribute(1 + SHADER_NORMAL + SHADER_COLOR)]];
#endif
};
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
//  Sampler
//------------------------------------------------------------------------------
R"(
#if SHADER_GLSL || SHADER_HLSL
uniform sampler2D samDiffuse;
#elif SHADER_MSL
struct Sampler
{
#if SHADER_MSL_ARGUMENT
    device float4* Buffer       [[id(1)]];
    texture2d<float> Diffuse    [[id(4)]];
    sampler DiffuseSampler      [[id(18)]];
#else
    texture2d<float> Diffuse    [[texture(0)]];
    sampler DiffuseSampler      [[sampler(0)]];
#endif
};
#endif)"
//------------------------------------------------------------------------------
//  Vertex Shader
//------------------------------------------------------------------------------
R"(
#if SHADER_VERTEX
#if SHADER_GLSL
void main()
#elif SHADER_HLSL
Varying Main(Attribute attr)
#elif SHADER_MSL
vertex Varying Main(Attribute attr [[stage_in]],
                    constant Uniform& uni [[buffer(0)]])
#endif
{
#if SHADER_HLSL || SHADER_MSL
    float3 attrPosition = attr.Position;
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
#if SHADER_MSL
    auto uniBuffer = uni.Buffer;
#endif

    float4x4 world = float4x4(uniBuffer[0], uniBuffer[1], uniBuffer[2], uniBuffer[3]);
    float4x4 view = float4x4(uniBuffer[4], uniBuffer[5], uniBuffer[6], uniBuffer[7]);
    float4x4 projection = float4x4(uniBuffer[8], uniBuffer[9], uniBuffer[10], uniBuffer[11]);
    float4 worldPosition = mul(float4(attrPosition, 1.0), world);
    float4 screenPosition = mul(mul(worldPosition, view), projection);

    float3 normal = float3(0.0, 0.0, 1.0);
    float4 color = float4(1.0, 1.0, 1.0, 1.0);
#if SHADER_NORMAL
    normal = attrNormal;
#endif
#if SHADER_COLOR
    color = attrColor;
#endif

#if SHADER_LIGHTING
    float3 worldNormal = normalize(mul(float4(attrNormal, 1.0), world).xyz);
    float3 lightDirection = uniBuffer[12].xyz;
    float3 lightColor = uniBuffer[13].xyz;
    float3 ambientColor = uniBuffer[14].xyz;
    float3 diffuseColor = uniBuffer[15].xyz;
    float3 emissiveColor = uniBuffer[16].xyz;
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
    return vary;
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
fragment float4 Main(Varying vary [[stage_in]],
#if SHADER_MSL_ARGUMENT
                     constant Sampler& sam [[buffer(0)]])
#else
                     constant Uniform& uni [[buffer(0)]], Sampler sam)
#endif
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
#if SHADER_MSL && SHADER_MSL_ARGUMENT
    auto uniBuffer = sam.Buffer;
#elif SHADER_MSL
    auto uniBuffer = uni.Buffer;
#endif

    float4 color = float4(1.0, 1.0, 1.0, 1.0);
#if SHADER_COLOR || SHADER_LIGHTING
    color = varyColor;
#endif

#if SHADER_GLSL && SHADER_TEXTURE
    color = color * texture2D(samDiffuse, varyUV0);
#elif SHADER_HLSL && SHADER_TEXTURE
    color = color * tex2D(samDiffuse, vary.UV0);
#elif SHADER_MSL && SHADER_TEXTURE
    color = color * sam.Diffuse.sample(sam.DiffuseSampler, vary.UV0);
#endif

#if SHADER_SPECULAR
    float3 cameraPosition = uniBuffer[0].xyz;
    float3 lightDirection = uniBuffer[1].xyz;
    float3 specularColor = uniBuffer[2].xyz;
    float specularHighlight = uniBuffer[2].w;
    float3 L = lightDirection;
    float3 V = normalize(cameraPosition - varyWorldPosition);
    float3 N = normalize(varyWorldNormal);
    float3 H = normalize(V + L);
    float phong = pow(max(dot(N, H), 0.0001), specularHighlight);
    color.rgb = color.rgb + specularColor * phong;
#endif

#if SHADER_GLSL
    gl_FragColor = color;
#elif SHADER_HLSL || SHADER_MSL
    return color;
#endif
}
#endif)";
