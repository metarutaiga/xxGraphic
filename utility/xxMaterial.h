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

    void        Update(xxNode& node, uint64_t device, const xxCameraPtr& camera);
    void        Draw(uint64_t commandEncoder);

protected:
    xxMaterial();

    uint64_t    m_device = 0;
    uint64_t    m_blendState = 0;
    uint64_t    m_depthStencilState = 0;
    uint64_t    m_rasterizerState = 0;
    uint64_t    m_vertexAttribute = 0;
    uint64_t    m_vertexConstant = 0;
    uint64_t    m_vertexShader = 0;
    uint64_t    m_fragmentConstant = 0;
    uint64_t    m_fragmentShader = 0;
    uint64_t    m_renderPass = 0;
    uint64_t    m_pipeline = 0;

public:
    bool        m_blending = false;
    const char* m_blendSourceColor = "SrcAlpha";
    const char* m_blendOperationColor = "+";
    const char* m_blendDestinationColor = "1-SrcAlpha";
    const char* m_blendSourceAlpha = "1";
    const char* m_blendOperationAlpha = "+";
    const char* m_blendDestinationAlpha = "0";
    const char* m_depthTest = "Always";
    bool        m_depthWrite = false;
    bool        m_cull = false;
    bool        m_scissor = false;
};
