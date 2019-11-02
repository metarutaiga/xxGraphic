//==============================================================================
// xxGraphic : Material Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphic.h"
#include "xxMaterial.h"

//==============================================================================
//  Material
//==============================================================================
xxMaterial::xxMaterial(uint64_t device)
{
    m_device = device;
    m_blendState = 0;
    m_depthStencilState = 0;
    m_rasterizerState = 0;
    m_vertexShader = 0;
    m_fragmentShader = 0;
    m_pipeline = 0;
}
//------------------------------------------------------------------------------
xxMaterial::~xxMaterial()
{
    xxDestroyBlendState(m_blendState);
    xxDestroyDepthStencilState(m_depthStencilState);
    xxDestroyRasterizerState(m_rasterizerState);
    xxDestroyShader(m_device, m_vertexShader);
    xxDestroyShader(m_device, m_fragmentShader);
    xxDestroyPipeline(m_pipeline);
}
//==============================================================================
