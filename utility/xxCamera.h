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

    const xxMatrix4& GetViewMatrix() const;
    const xxMatrix4& GetProjectionMatrix() const;

    void SetViewportMatrix(float fromWidth, float fromHeight, float toX, float toY, float toWidth, float toHeight);

    const xxVector3& GetRight() const;
    const xxVector3& GetUp() const;
    const xxVector3& GetDirection() const;
    const xxVector3& GetLocation() const;
    void SetRight(const xxVector3& right);
    void SetUp(const xxVector3& up);
    void SetDirection(const xxVector3& direction);
    void SetLocation(const xxVector3& location);

    void Update();
    void LookAt(const xxVector3& worldPoint, const xxVector3& worldUp);
    void SetFOV(float aspect, float fov, float far);

    xxVector3 GetScreenToWorldPos(float x, float y) const;

protected:
    xxCamera();

    xxVector3   m_right;
    xxVector3   m_up;
    xxVector3   m_direction;
    xxVector3   m_location;

    float       m_frustumLeft;
    float       m_frustumRight;
    float       m_frustumBottom;
    float       m_frustumTop;
    float       m_frustumNear;
    float       m_frustumFar;

    xxMatrix4   m_viewMatrix;
    xxMatrix4   m_projectionMatrix;
    xxMatrix4   m_viewportMatrix;
};
