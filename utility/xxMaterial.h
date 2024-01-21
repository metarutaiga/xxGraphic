//==============================================================================
// xxGraphic : Material Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxMaterial
{
public:
    static xxMaterialPtr        Create();

    void                        Invalidate();
    void                        Update(float time);
    void                        Setup(xxDrawData const& data);
    void                        Draw(xxDrawData const& data) const;

    virtual void                CreatePipeline(xxDrawData const& data);
    virtual void                CreateConstant(xxDrawData const& data) const;
    virtual void                UpdateConstant(xxDrawData const& data) const;

    static xxMaterialPtr      (*BinaryCreate)();
    virtual bool                BinaryRead(xxBinary& binary);
    virtual bool                BinaryWrite(xxBinary& binary) const;

protected:
    xxMaterial();
    virtual ~xxMaterial();

    virtual std::string         GetShader(xxMesh const* mesh, int type) const;
    virtual int                 GetVertexConstantSize() const;
    virtual int                 GetFragmentConstantSize() const;

    uint64_t                    m_device = 0;
    uint64_t                    m_blendState = 0;
    uint64_t                    m_depthStencilState = 0;
    uint64_t                    m_rasterizerState = 0;
    uint64_t                    m_vertexShader = 0;
    uint64_t                    m_fragmentShader = 0;
    uint64_t                    m_renderPass = 0;
    uint64_t                    m_pipeline = 0;

    static char const           ms_defaultShader[];

public:
    std::string                 Name = "";

    std::string                 Shader = "";
    std::string                 ShaderOption = "";

    xxVector3                   AmbientColor = xxVector3::ZERO;
    xxVector3                   DiffuseColor = xxVector3::ZERO;
    xxVector3                   EmissiveColor = xxVector3::ZERO;
    xxVector3                   SpecularColor = xxVector3::ZERO;
    float                       SpecularHighlight = 0.0f;
    float                       Opacity = 1.0f;

    bool                        Lighting = false;
    bool                        Specular = false;

    bool                        Blending = false;
    std::string                 BlendSourceColor = "SrcAlpha";
    std::string                 BlendOperationColor = "+";
    std::string                 BlendDestinationColor = "1-SrcAlpha";
    std::string                 BlendSourceAlpha = "1";
    std::string                 BlendOperationAlpha = "+";
    std::string                 BlendDestinationAlpha = "0";

    std::string                 DepthTest = "Always";
    bool                        DepthWrite = false;

    bool                        Cull = false;
    bool                        Scissor = false;

    std::vector<xxModifierPtr>  Modifiers;
};
