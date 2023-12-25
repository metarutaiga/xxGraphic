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

    void        Update(uint64_t device, xxNode const& node, xxCameraPtr const& camera);
    void        Set(uint64_t commandEncoder);

protected:
    xxMaterial();

    uint64_t    m_device = 0;
    uint64_t    m_blendState = 0;
    uint64_t    m_depthStencilState = 0;
    uint64_t    m_rasterizerState = 0;
    uint64_t    m_vertexConstant = 0;
    uint64_t    m_vertexShader = 0;
    uint64_t    m_fragmentConstant = 0;
    uint64_t    m_fragmentShader = 0;
    uint64_t    m_renderPass = 0;
    uint64_t    m_pipeline = 0;

public:
    std::string Name = "";

    char const* VertexShader = "default";
    char const* FragmentShader = "default";

    xxVector3   AmbientColor = xxVector3::WHITE;
    xxVector3   DiffuseColor = xxVector3::BLACK;
    xxVector3   SpecularColor = xxVector3::BLACK;
    xxVector3   EmissiveColor = xxVector3::BLACK;
    float       SpecularHighlight = 0.0f;

    bool        Blending = false;
    char const* BlendSourceColor = "SrcAlpha";
    char const* BlendOperationColor = "+";
    char const* BlendDestinationColor = "1-SrcAlpha";
    char const* BlendSourceAlpha = "1";
    char const* BlendOperationAlpha = "+";
    char const* BlendDestinationAlpha = "0";

    char const* DepthTest = "Always";
    bool        DepthWrite = false;

    bool        Cull = false;
    bool        Scissor = false;
};
