//==============================================================================
// xxGraphic : Material Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "../xxSystem.h"
#include "xxMath.h"

#include <memory>

class xxNode;

class xxMaterial;
typedef std::shared_ptr<xxMaterial> xxMaterialPtr;

class xxPlusAPI xxMaterial
{
public:
    virtual ~xxMaterial();

    static xxMaterialPtr Create();

    void    Update(xxNode& node, uint64_t device);
    void    Draw(uint64_t commandEncoder);

protected:
    xxMaterial();

    uint64_t        m_device;
    uint64_t        m_blendState;
    uint64_t        m_depthStencilState;
    uint64_t        m_rasterizerState;
    uint64_t        m_vertexAttribute;
    uint64_t        m_vertexShader;
    uint64_t        m_fragmentShader;
    uint64_t        m_renderPass;
    uint64_t        m_pipeline;
};
