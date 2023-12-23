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

//==============================================================================
//  Material
//==============================================================================
xxMaterial::xxMaterial()
{
}
//------------------------------------------------------------------------------
xxMaterial::~xxMaterial()
{
    xxDestroyBlendState(m_blendState);
    xxDestroyDepthStencilState(m_depthStencilState);
    xxDestroyRasterizerState(m_rasterizerState);
    xxDestroyBuffer(m_device, m_vertexConstant);
    xxDestroyShader(m_device, m_vertexShader);
    xxDestroyBuffer(m_device, m_fragmentConstant);
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
void xxMaterial::Update(uint64_t device, xxNode const& node, xxCameraPtr const& camera)
{
    xxMeshPtr mesh = node.GetMesh();
    if (mesh == nullptr)
        return;

    uint64_t vertexAttribute = mesh->GetVertexAttribute();
    if (vertexAttribute == 0)
        return;

    m_device = device;

    if (m_blendState == 0)
    {
        if (m_blending)
        {
            m_blendState = xxCreateBlendState(m_device, m_blendSourceColor, m_blendOperationColor, m_blendDestinationColor, m_blendSourceAlpha, m_blendOperationAlpha, m_blendDestinationAlpha);
        }
        else
        {
            m_blendState = xxCreateBlendState(m_device, "1", "+", "0", "1", "+", "0");
        }
    }
    if (m_depthStencilState == 0)
    {
        m_depthStencilState = xxCreateDepthStencilState(m_device, m_depthTest, m_depthWrite);
    }
    if (m_rasterizerState == 0)
    {
        m_rasterizerState = xxCreateRasterizerState(m_device, m_cull, m_scissor);
    }
    if (m_vertexConstant == 0)
    {
        m_vertexConstant = xxCreateConstantBuffer(m_device, 3 * sizeof(xxMatrix4));
    }
    if (m_vertexShader == 0)
    {
        m_vertexShader = xxCreateVertexShader(m_device, "default", vertexAttribute);
    }
    if (m_fragmentConstant == 0)
    {
        m_fragmentConstant = 0;
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
        m_pipeline = xxCreatePipeline(m_device, m_renderPass, m_blendState, m_depthStencilState, m_rasterizerState, vertexAttribute, m_vertexShader, m_fragmentShader);
    }

    xxMatrix4* matrix = (xxMatrix4*)xxMapBuffer(m_device, m_vertexConstant);
    if (matrix)
    {
        matrix[0] = node.GetWorldMatrix();
        if (camera)
        {
            matrix[1] = camera->m_viewMatrix;
            matrix[2] = camera->m_projectionMatrix;
        }
        else
        {
            matrix[1] = xxMatrix4::IDENTITY;
            matrix[2] = xxMatrix4::IDENTITY;
        }
        xxUnmapBuffer(m_device, m_vertexConstant);
    }
}
//------------------------------------------------------------------------------
void xxMaterial::Set(uint64_t commandEncoder)
{
    xxSetPipeline(commandEncoder, m_pipeline);
    xxSetVertexConstantBuffer(commandEncoder, m_vertexConstant, 3 * sizeof(xxMatrix4));
}
//==============================================================================
