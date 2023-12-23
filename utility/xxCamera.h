//==============================================================================
// xxGraphic : Camera Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxCamera
{
public:
    virtual ~xxCamera();

    static xxCameraPtr Create();

    void        Update();

    void        LookAt(xxVector3 const& worldPoint, xxVector3 const& worldUp);

    void        SetFOV(float aspect, float fov, float depth);
    void        SetViewportMatrix(float fromWidth, float fromHeight, float toX, float toY, float toWidth, float toHeight);

    xxVector3   GetDirectionFromScreenPos(float x, float y) const;

protected:
    xxCamera();

public:
    xxMatrix4   m_viewMatrix = xxMatrix4::IDENTITY;
    xxMatrix4   m_projectionMatrix = xxMatrix4::IDENTITY;
    xxMatrix4   m_viewportMatrix = xxMatrix4::IDENTITY;

    xxVector3   m_right = { 0, 0, 1 };
    xxVector3   m_up = { 0, 1, 0 };
    xxVector3   m_direction = { 1, 0, 0 };
    xxVector3   m_location = { 0, 0, 0 };

    float       m_frustumLeft = -0.5f;
    float       m_frustumRight = 0.5f;
    float       m_frustumBottom = -0.5f;
    float       m_frustumTop = 0.5f;
    float       m_frustumNear = 1.0f;
    float       m_frustumFar = 1000.0f;
};
