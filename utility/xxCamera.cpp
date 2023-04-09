//==============================================================================
// xxGraphic : Camera Source
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxCamera.h"

//==============================================================================
//  Camera
//==============================================================================
xxCamera::xxCamera()
{
    m_right = { 0, 0, 1 };
    m_up = { 0, 1, 0 };
    m_direction = { 1, 0, 0 };
    m_location = { 0, 0, 0 };

    m_frustumLeft = -0.5f;
    m_frustumRight = 0.5f;
    m_frustumBottom = -0.5f;
    m_frustumTop = 0.5f;
    m_frustumNear = 1.0f;
    m_frustumFar = 1000.0f;

    m_viewMatrix = xxMatrix4::IDENTITY;
    m_projectionMatrix = xxMatrix4::IDENTITY;
    m_viewportMatrix = xxMatrix4::IDENTITY;
}
//------------------------------------------------------------------------------
xxCamera::~xxCamera()
{

}
//------------------------------------------------------------------------------
xxCameraPtr xxCamera::Create()
{
    xxCameraPtr Camera = xxCameraPtr(new xxCamera());
    if (Camera == nullptr)
        return xxCameraPtr();

    return Camera;
}
//------------------------------------------------------------------------------
const xxMatrix4& xxCamera::GetViewMatrix() const
{
    return m_viewMatrix;
}
//------------------------------------------------------------------------------
const xxMatrix4& xxCamera::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}
//------------------------------------------------------------------------------
void xxCamera::SetViewportMatrix(float fromWidth, float fromHeight, float toX, float toY, float toWidth, float toHeight)
{
    float oldCenterX = fromWidth / 2.0f;
    float oldCenterY = fromHeight / 2.0f;
    float newCenterX = toX + toWidth / 2.0f;
    float newCenterY = toY + toHeight / 2.0f;
    float scaleX = toWidth / fromWidth;
    float scaleY = toHeight / fromHeight;
    float translateX = (newCenterX - oldCenterX) * 2.0f / fromWidth;
    float translateY = -(newCenterY - oldCenterY) * 2.0f / fromHeight;

    m_viewportMatrix.m11 = scaleX;
    m_viewportMatrix.m22 = scaleY;
    m_viewportMatrix.m41 = translateX;
    m_viewportMatrix.m42 = translateY;
}
//------------------------------------------------------------------------------
const xxVector3& xxCamera::GetRight() const
{
    return m_right;
}
//------------------------------------------------------------------------------
const xxVector3& xxCamera::GetUp() const
{
    return m_up;
}
//------------------------------------------------------------------------------
const xxVector3& xxCamera::GetDirection() const
{
    return m_direction;
}
//------------------------------------------------------------------------------
const xxVector3& xxCamera::GetLocation() const
{
    return m_location;
}
//------------------------------------------------------------------------------
void xxCamera::SetRight(const xxVector3& right)
{
    m_right = right;
}
//------------------------------------------------------------------------------
void xxCamera::SetUp(const xxVector3& up)
{
    m_up = up;
}
//------------------------------------------------------------------------------
void xxCamera::SetDirection(const xxVector3& direction)
{
    m_direction = direction;
}
//------------------------------------------------------------------------------
void xxCamera::SetLocation(const xxVector3& location)
{
    m_location = location;
}
//------------------------------------------------------------------------------
void xxCamera::Update()
{
    m_viewMatrix.m11 = m_right.x;
    m_viewMatrix.m21 = m_right.y;
    m_viewMatrix.m31 = m_right.z;
    m_viewMatrix.m12 = m_up.x;
    m_viewMatrix.m22 = m_up.y;
    m_viewMatrix.m32 = m_up.z;
    m_viewMatrix.m13 = m_direction.x;
    m_viewMatrix.m23 = m_direction.y;
    m_viewMatrix.m33 = m_direction.z;
    m_viewMatrix.m41 = -(m_right.Dot(m_location));
    m_viewMatrix.m42 = -(m_up.Dot(m_location));
    m_viewMatrix.m43 = -(m_direction.Dot(m_location));
    m_viewMatrix.m14 = 0.0f;
    m_viewMatrix.m24 = 0.0f;
    m_viewMatrix.m34 = 0.0f;
    m_viewMatrix.m44 = 1.0f;

    m_projectionMatrix.m11 = 1.0f / (m_frustumRight - m_frustumLeft);
    m_projectionMatrix.m21 = 0.0f;
    m_projectionMatrix.m31 = m_projectionMatrix.m11 * -(m_frustumRight + m_frustumLeft);
    m_projectionMatrix.m41 = 0.0f;
    m_projectionMatrix.m12 = 0.0f;
    m_projectionMatrix.m22 = 1.0f / (m_frustumTop - m_frustumBottom);
    m_projectionMatrix.m32 = m_projectionMatrix.m22 * -(m_frustumTop + m_frustumBottom);
    m_projectionMatrix.m42 = 0.0f;
    m_projectionMatrix.m13 = 0.0f;
    m_projectionMatrix.m23 = 0.0f;
    m_projectionMatrix.m33 = 1.0f / (m_frustumFar - m_frustumNear);
    m_projectionMatrix.m43 = m_projectionMatrix.m33 * -(m_frustumFar + m_frustumNear);
    m_projectionMatrix.m14 = 0.0f;
    m_projectionMatrix.m24 = 0.0f;
    m_projectionMatrix.m34 = 1.0f;
    m_projectionMatrix.m44 = 0.0f;

    m_projectionMatrix.m11 *= 2.0f;
    m_projectionMatrix.m22 *= 2.0f;
    m_projectionMatrix.m33 *= m_frustumFar;

    m_projectionMatrix = m_viewportMatrix * m_projectionMatrix;
}
//------------------------------------------------------------------------------
void xxCamera::LookAt(const xxVector3& worldPoint, const xxVector3& worldUp)
{
    xxVector3 direction = m_location - worldPoint;
    direction /= direction.Length();

    xxVector3 right = worldUp.Cross(direction);
    right /= right.Length();

    xxVector3 up = direction.Cross(right);
    up /= up.Length();

    m_right = right;
    m_up = up;
    m_direction = -direction;
}
//------------------------------------------------------------------------------
void xxCamera::SetFOV(float aspect, float fov, float far)
{
    float halfHeight = tanf(fov * 0.5f * (float)M_PI / 180.0f);
    float halfWidth = halfHeight * aspect;

    m_frustumLeft = -halfWidth;
    m_frustumRight = halfWidth;
    m_frustumBottom = -halfHeight;
    m_frustumTop = halfHeight;
    m_frustumNear = 1.0f;
    m_frustumFar = far;
}
//------------------------------------------------------------------------------
xxVector3 xxCamera::GetScreenToWorldPos(float x, float y) const
{
    float fVx = m_frustumLeft + x * (m_frustumRight - m_frustumLeft);
    float fVy = m_frustumBottom + y * (m_frustumTop - m_frustumBottom);

    xxVector3 kDir = m_direction + m_right * fVx + m_up * fVy;
    kDir /= kDir.Length();
    return kDir;
}
//==============================================================================
