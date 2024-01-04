//==============================================================================
// xxGraphic : Material Source
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphic.h"
#include "xxCamera.h"
#include "xxMesh.h"
#include "xxNode.h"
#include "xxMaterial.h"

int xxMaterial::ActiveMaterialIndex = 0;
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
void xxMaterial::Update(uint64_t device, xxNode& node, xxCameraPtr const& camera)
{
    m_device = device;

    xxMeshPtr const& mesh = node.Mesh;
    if (mesh == nullptr)
        return;

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
            std::string shader = GetShader(mesh);
            if (m_vertexShader == 0)
            {
                m_vertexShader = xxCreateVertexShader(m_device, shader.c_str(), vertexAttribute);
            }
            if (m_fragmentShader == 0)
            {
                m_fragmentShader = xxCreateFragmentShader(m_device, shader.c_str());
            }
        }
        if (m_renderPass == 0)
        {
            m_renderPass = xxCreateRenderPass(m_device, true, true, true, true, true, true);
        }
        m_pipeline = xxCreatePipeline(m_device, m_renderPass, m_blendState, m_depthStencilState, m_rasterizerState, vertexAttribute, m_vertexShader, m_fragmentShader);
    }

    if (node.ConstantDatas.size() <= ActiveMaterialIndex)
        node.ConstantDatas.resize(ActiveMaterialIndex + 1);
    xxConstantData& constantData = node.ConstantDatas[ActiveMaterialIndex];
    if (constantData.vertexConstant == 0 && constantData.fragmentConstant == 0)
    {
        constantData.device = device;
        if (constantData.vertexConstant == 0)
        {
            constantData.vertexConstantSize = GetVertexConstantSize();
            if (constantData.vertexConstantSize > 0)
            {
                constantData.vertexConstant = xxCreateConstantBuffer(m_device, constantData.vertexConstantSize);
            }
        }
        if (constantData.fragmentConstant == 0)
        {
            constantData.fragmentConstantSize = GetFragmentConstantSize();
            if (constantData.fragmentConstantSize > 0)
            {
                constantData.fragmentConstant = xxCreateConstantBuffer(m_device, constantData.fragmentConstantSize);
            }
        }
    }

    UpdateConstant(constantData, node, camera);
}
//------------------------------------------------------------------------------
void xxMaterial::Set(uint64_t commandEncoder, xxNode const& node)
{
    xxSetPipeline(commandEncoder, m_pipeline);

    xxConstantData const& constantData = node.ConstantDatas[ActiveMaterialIndex];
    if (constantData.vertexConstant)
        xxSetVertexConstantBuffer(commandEncoder, constantData.vertexConstant, constantData.vertexConstantSize);
    if (constantData.fragmentConstant)
        xxSetFragmentConstantBuffer(commandEncoder, constantData.fragmentConstant, constantData.fragmentConstantSize);
}
//------------------------------------------------------------------------------
std::string xxMaterial::GetShader(xxMeshPtr const& mesh) const
{
    auto define = [](char const* name, size_t value)
    {
        char integer[32];
        std::string line;
        snprintf(integer, 32, "%zd", value);
        line += "#define"; line += ' '; line += name; line += ' '; line += integer; line += '\n';
        return line;
    };

    std::string shader;
    shader += define("SHADER_NORMAL", mesh->NormalCount);
    shader += define("SHADER_COLOR", mesh->ColorCount);
    shader += define("SHADER_TEXTURE", mesh->TextureCount);
    shader += define("SHADER_UNIFORM", GetVertexConstantSize() / sizeof(xxVector4));
    shader += define("SHADER_LIGHTING", Lighting ? 1 : 0);
    shader += define("SHADER_SPECULAR", Specular ? 1 : 0);
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
        if (Specular)
        {
            size += 2 * sizeof(xxVector4);
        }
    }
    return size;
}
//------------------------------------------------------------------------------
int xxMaterial::GetFragmentConstantSize() const
{
    return 0;
}
//------------------------------------------------------------------------------
void xxMaterial::UpdateConstant(xxConstantData const& constantData, xxNode const& node, xxCameraPtr const& camera) const
{
    size_t size;

    size = constantData.vertexConstantSize;
    if (size)
    {
        xxVector4* vector = reinterpret_cast<xxVector4*>(xxMapBuffer(m_device, constantData.vertexConstant));
        if (vector)
        {
            if (size >= 3 * sizeof(xxMatrix4))
            {
                xxMatrix4* wvp = reinterpret_cast<xxMatrix4*>(vector);
                wvp[0] = node.WorldMatrix;
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

                    if (Specular && size >= 2 * sizeof(xxVector4))
                    {
                        if (camera)
                        {
                            vector[0].xyz = camera->Location;
                        }
                        else
                        {
                            vector[0].xyz = xxVector3::ZERO;
                        }
                        vector[1].xyz = SpecularColor;
                        vector[1].w = SpecularHighlight;
                        vector += 2;
                        size -= 2 * sizeof(xxVector4);
                    }
                }
            }
            xxUnmapBuffer(m_device, constantData.vertexConstant);
        }
    }
}
//==============================================================================
//  Default Shader
//==============================================================================
char const xxMaterial::ms_defaultShader[] =
R"(#if SHADER_GLSL
#define float2 vec2
#define float3 vec3
#define float4 vec4
#define float4x4 mat4
#endif

#if SHADER_GLSL || SHADER_MSL
#define mul(a, b) (b * a)
#endif

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
#endif

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
#endif

#if SHADER_GLSL
#if SHADER_COLOR || SHADER_LIGHTING
varying vec4 varyColor;
#endif
#if SHADER_TEXTURE
varying vec2 varyUV0;
#endif
#elif SHADER_HLSL
struct Varying
{
    float4 Position : SV_POSITION;
#if SHADER_COLOR || SHADER_LIGHTING
    float4 Color    : COLOR;
#endif
#if SHADER_TEXTURE
    float2 UV0      : TEXCOORD0;
#endif
};
#elif SHADER_MSL
struct Varying
{
    float4 Position [[position]];
#if SHADER_COLOR || SHADER_LIGHTING
    float4 Color;
#endif
#if SHADER_TEXTURE
    float2 UV0;
#endif
};
#endif

#if SHADER_GLSL || SHADER_HLSL
uniform sampler2D samDiffuse;
#elif SHADER_MSL
struct Sampler
{
#if SHADER_MSL_ARGUMENT
    texture2d<float> Diffuse    [[id(4)]];
    sampler DiffuseSampler      [[id(18)]];
#else
    texture2d<float> Diffuse    [[texture(0)]];
    sampler DiffuseSampler      [[sampler(0)]];
#endif
};
#endif

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
    float3 N = normalize(mul(float4(attrNormal, 1.0), world).xyz);
    float3 L = uniBuffer[12].xyz;
    float3 lightColor = uniBuffer[13].xyz;
    float3 ambientColor = uniBuffer[14].xyz;
    float3 diffuseColor = uniBuffer[15].xyz;
    float3 emissiveColor = uniBuffer[16].xyz;
    float lambert = dot(N, L);
    color.rgb = color.rgb * (ambientColor + diffuseColor * lightColor * lambert) + emissiveColor;
#if SHADER_SPECULAR
    float3 cameraPosition = uniBuffer[17].xyz;
    float3 specularColor = uniBuffer[18].xyz;
    float specularHighlight = uniBuffer[18].w;
    float3 V = normalize(cameraPosition - worldPosition.xyz);
    float3 H = normalize(V + L);
    float phong = pow(max(dot(N, H), 0.0001), specularHighlight);
    color.rgb = color.rgb + specularColor * phong;
#endif
#endif

#if SHADER_GLSL
    gl_Position = screenPosition;
#if SHADER_COLOR || SHADER_LIGHTING
    varyColor = color;
#endif
#if SHADER_TEXTURE
    varyUV0 = attrUV0;
#endif
#elif SHADER_HLSL || SHADER_MSL
    Varying vary;
    vary.Position = screenPosition;
#if SHADER_COLOR || SHADER_LIGHTING
    vary.Color = color;
#endif
#if SHADER_TEXTURE
    vary.UV0 = attrUV0;
#endif
    return vary;
#endif
}
#endif

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
                     Sampler sam)
#endif
#endif
{
#if SHADER_HLSL || SHADER_MSL
#if SHADER_COLOR || SHADER_LIGHTING
    float4 varyColor = vary.Color;
#endif
#endif

    float4 color = float4(1.0, 1.0, 1.0, 1.0);
#if SHADER_COLOR || SHADER_LIGHTING
    color = varyColor;
#endif

#if SHADER_GLSL
#if SHADER_TEXTURE
    color = color * texture2D(samDiffuse, varyUV0);
#endif
#elif SHADER_HLSL
#if SHADER_TEXTURE
    color = color * tex2D(samDiffuse, vary.UV0);
#endif
#elif SHADER_MSL
#if SHADER_TEXTURE
    color = color * sam.Diffuse.sample(sam.DiffuseSampler, vary.UV0);
#endif
#endif

#if SHADER_GLSL
    gl_FragColor = color;
#elif SHADER_HLSL || SHADER_MSL
    return color;
#endif
}
#endif)";
