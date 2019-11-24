//==============================================================================
// xxGraphic : Material Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphic.h"
#include "xxNode.h"
#include "xxMesh.h"
#include "xxMaterial.h"

//==============================================================================
//  Material
//==============================================================================
xxMaterial::xxMaterial()
{
    m_device = 0;
    m_blendState = 0;
    m_depthStencilState = 0;
    m_rasterizerState = 0;
    m_vertexAttribute = 0;
    m_vertexShader = 0;
    m_fragmentShader = 0;
    m_renderPass = 0;
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
    xxDestroyRenderPass(m_renderPass);
    xxDestroyPipeline(m_pipeline);
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
void xxMaterial::Update(xxNode& node, uint64_t device)
{
    xxMesh* mesh = node.GetMesh();
    if (mesh == nullptr)
        return;

    uint64_t vertexAttribute = mesh->GetVertexAttribute();
    if (vertexAttribute == 0)
        return;

    m_device = device;
    m_vertexAttribute = vertexAttribute;

    if (m_blendState == 0)
    {
        m_blendState = xxCreateBlendState(m_device, false);
    }
    if (m_depthStencilState == 0)
    {
        m_depthStencilState = xxCreateDepthStencilState(m_device, false, false);
    }
    if (m_rasterizerState == 0)
    {
        m_rasterizerState = xxCreateRasterizerState(m_device, false, false);
    }
    if (m_vertexShader == 0)
    {
        m_vertexShader = xxCreateVertexShader(m_device, "default", m_vertexAttribute);
    }
    if (m_fragmentShader == 0)
    {
        m_fragmentShader = xxCreateFragmentShader(m_device, "default");
    }
    if (m_renderPass == 0)
    {
        m_renderPass = xxCreateRenderPass(m_device, true, true, true, true, true, true);
    }
    if (m_pipeline == 0)
    {
        m_pipeline = xxCreatePipeline(m_device, m_renderPass, m_blendState, m_depthStencilState, m_rasterizerState, m_vertexAttribute, m_vertexShader, m_fragmentShader);
    }
}
//------------------------------------------------------------------------------
void xxMaterial::Draw(uint64_t commandEncoder)
{
    xxSetPipeline(commandEncoder, m_pipeline);
}
//==============================================================================
