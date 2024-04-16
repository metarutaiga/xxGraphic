//==============================================================================
// xxGraphic : Camera Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxCamera
{
public:
    void                    Update();

    void                    LookAt(xxVector3 const& worldPoint, xxVector3 const& worldUp);

    void                    SetFOV(float aspect, float fov, float depth);
    void                    SetViewportMatrix(float fromWidth, float fromHeight, float toX, float toY, float toWidth, float toHeight);

    xxVector3               GetDirectionFromScreenPos(float x, float y) const;
    xxVector3               GetWorldPosToScreenPos(xxVector3 const& point) const;

    static xxCameraPtr      Create();

    static xxCameraPtr    (*BinaryCreate)();
    virtual void            BinaryRead(xxBinary& binary);
    virtual void            BinaryWrite(xxBinary& binary) const;

protected:
    xxCamera();
    virtual ~xxCamera();

    float                   m_frustumLeft = -0.5f;
    float                   m_frustumRight = 0.5f;
    float                   m_frustumBottom = -0.5f;
    float                   m_frustumTop = 0.5f;
    float                   m_frustumNear = 1.0f;
    float                   m_frustumFar = 1000.0f;

public:
    std::string             Name = "";

    xxMatrix4               ViewMatrix = xxMatrix4::IDENTITY;
    xxMatrix4               ProjectionMatrix = xxMatrix4::IDENTITY;
    xxMatrix4               ViewportMatrix = xxMatrix4::IDENTITY;
    xxMatrix4               ViewProjectionMatrix = xxMatrix4::IDENTITY;

    xxVector3               Right = xxVector3::Z;
    xxVector3               Up = xxVector3::Y;
    xxVector3               Direction = xxVector3::X;
    xxVector3               Location = xxVector3::ZERO;

    xxVector3               LightDirection = xxVector3::Y;
    xxVector3               LightColor = xxVector3::WHITE;
};
