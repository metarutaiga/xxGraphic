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
}
//------------------------------------------------------------------------------
xxCamera::~xxCamera()
{

}
//------------------------------------------------------------------------------
xxCameraPtr xxCamera::Create()
{
    xxCameraPtr camera = xxCameraPtr(new xxCamera());
    if (camera == nullptr)
        return xxCameraPtr();

    return camera;
}
//------------------------------------------------------------------------------
void xxCamera::Update()
{
    m_viewMatrix._[0] = {  m_right.x,                m_up.x,                m_direction.x,               0 };
    m_viewMatrix._[1] = {  m_right.y,                m_up.y,                m_direction.y,               0 };
    m_viewMatrix._[2] = {  m_right.z,                m_up.z,                m_direction.z,               0 };
    m_viewMatrix._[3] = { -m_right.Dot(m_location), -m_up.Dot(m_location), -m_direction.Dot(m_location), 1 };

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
xxVector3 xxCamera::GetScreenToWorldPos(float x, float y) const
{
    float right = m_frustumLeft + x * (m_frustumRight - m_frustumLeft);
    float up = m_frustumBottom + y * (m_frustumTop - m_frustumBottom);

    xxVector3 kDir = m_direction + m_right * right + m_up * up;
    kDir /= kDir.Length();
    return kDir;
}
//==============================================================================
