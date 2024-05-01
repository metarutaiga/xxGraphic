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

    static xxCameraPtr      Create();

    static xxCameraPtr    (*BinaryCreate)();
    virtual void            BinaryRead(xxBinary& binary);
    virtual void            BinaryWrite(xxBinary& binary) const;

protected:
    xxCamera();
    virtual ~xxCamera();

public:
    std::string             Name = "";

    float                   FrustumLeft = -0.5f;
    float                   FrustumRight = 0.5f;
    float                   FrustumBottom = -0.5f;
    float                   FrustumTop = 0.5f;
    float                   FrustumNear = 1.0f;
    float                   FrustumFar = 1000.0f;

    xxMatrix4               ViewMatrix = xxMatrix4::IDENTITY;
    xxMatrix4               ProjectionMatrix = xxMatrix4::IDENTITY;
    xxMatrix4               ViewProjectionMatrix = xxMatrix4::IDENTITY;

    xxVector3               Up = xxVector3::Z;
    xxVector3               Right = xxVector3::Y;
    xxVector3               Direction = xxVector3::X;
    xxVector3               Location = xxVector3::ZERO;

    xxVector3               LightDirection = xxVector3::Y;
    xxVector3               LightColor = xxVector3::WHITE;
};
