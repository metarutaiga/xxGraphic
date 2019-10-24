//==============================================================================
// xxGraphic : Material Header
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"
#include "xxVector.h"

#include <memory>

class xxMaterial;
typedef std::shared_ptr<xxMaterial> xxMaterialPtr;

class xxCPPAPI xxMaterial
{
public:
    xxMaterial(uint64_t device);
    virtual ~xxMaterial();

protected:
    uint64_t        m_device;
    uint64_t        m_blendState;
    uint64_t        m_depthStencilState;
    uint64_t        m_rasterizerState;
    uint64_t        m_vertexShader;
    uint64_t        m_fragmentShader;
    uint64_t        m_pipeline;
};
