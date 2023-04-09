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

    bool            GetAlphaBlending() const;
    void            SetAlphaBlending(bool alphaBlending);

    const char*     GetDepthTest() const;
    void            SetDepthTest(const char* depthTest);
    bool            GetDepthWrite() const;
    void            SetDepthWrite(bool depthWrite);

    bool            GetCull() const;
    void            SetCull(bool cull);
    bool            GetScissor() const;
    void            SetScissor(bool scissor);

    void            Update(xxNode& node, uint64_t device, const xxCameraPtr& camera);
    void            Draw(uint64_t commandEncoder);

protected:
    xxMaterial();

    bool            m_alphaBlending;
    const char*     m_depthTest;
    bool            m_depthWrite;
    bool            m_cull;
    bool            m_scissor;

    uint64_t        m_device;
    uint64_t        m_blendState;
    uint64_t        m_depthStencilState;
    uint64_t        m_rasterizerState;
    uint64_t        m_vertexAttribute;
    uint64_t        m_vertexConstant;
    uint64_t        m_vertexShader;
    uint64_t        m_fragmentConstant;
    uint64_t        m_fragmentShader;
    uint64_t        m_renderPass;
    uint64_t        m_pipeline;
};
