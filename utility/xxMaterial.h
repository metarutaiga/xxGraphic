//==============================================================================
// xxGraphic : Material Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxMaterial
{
public:
    virtual ~xxMaterial();

    static xxMaterialPtr Create();

    void                Invalidate();
    void                Update(uint64_t device, xxNode& node, xxCameraPtr const& camera);
    void                Set(uint64_t commandEncoder, xxNode const& node);

protected:
    xxMaterial();

    virtual std::string GetShader(xxMeshPtr const& mesh) const;
    virtual int         GetVertexConstantSize() const;
    virtual int         GetFragmentConstantSize() const;
    virtual void        UpdateConstant(xxConstantData const& constant, xxNode const& node, xxCameraPtr const& camera) const;

    uint64_t            m_device = 0;
    uint64_t            m_blendState = 0;
    uint64_t            m_depthStencilState = 0;
    uint64_t            m_rasterizerState = 0;
    uint64_t            m_vertexShader = 0;
    uint64_t            m_fragmentShader = 0;
    uint64_t            m_renderPass = 0;
    uint64_t            m_pipeline = 0;

    static char const   ms_defaultShader[];

public:
    std::string         Name = "";

    char const*         Shader = ms_defaultShader;
    std::string         ShaderOption = "";

    xxVector3           AmbientColor = xxVector3::ZERO;
    xxVector3           DiffuseColor = xxVector3::ZERO;
    xxVector3           EmissiveColor = xxVector3::ZERO;
    xxVector3           SpecularColor = xxVector3::ZERO;
    float               SpecularHighlight = 0.0f;

    bool                Lighting = false;
    bool                Specular = false;

    bool                Blending = false;
    char const*         BlendSourceColor = "SrcAlpha";
    char const*         BlendOperationColor = "+";
    char const*         BlendDestinationColor = "1-SrcAlpha";
    char const*         BlendSourceAlpha = "1";
    char const*         BlendOperationAlpha = "+";
    char const*         BlendDestinationAlpha = "0";

    char const*         DepthTest = "Always";
    bool                DepthWrite = false;

    bool                Cull = false;
    bool                Scissor = false;

    static int          ActiveMaterialIndex;
};
