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
    float l = m_frustumLeft;
    float r = m_frustumRight;
    float b = m_frustumBottom;
    float t = m_frustumTop;
    float n = m_frustumNear;
    float f = m_frustumFar;

    m_viewMatrix.v[0] = {  m_right.x,                m_up.x,                m_direction.x,               0 };
    m_viewMatrix.v[1] = {  m_right.y,                m_up.y,                m_direction.y,               0 };
    m_viewMatrix.v[2] = {  m_right.z,                m_up.z,                m_direction.z,               0 };
    m_viewMatrix.v[3] = { -m_right.Dot(m_location), -m_up.Dot(m_location), -m_direction.Dot(m_location), 1 };

    m_projectionMatrix.v[0] = {    (-2) / (l - r),                 0,                  0, 0 };
    m_projectionMatrix.v[1] = {                 0,    (-2) / (b - t),                  0, 0 };
//  m_projectionMatrix.v[2] = { (l + r) / (l - r), (b + t) / (b - t),      (f) / (f - n), 1 };
    m_projectionMatrix.v[2] = {                 0,                 0,      (f) / (f - n), 1 };
    m_projectionMatrix.v[3] = {                 0,                 0, (f * -n) / (f - n), 0 };

    m_projectionMatrix = m_projectionMatrix * m_viewportMatrix;
}
//------------------------------------------------------------------------------
void xxCamera::LookAt(xxVector3 const& worldPoint, xxVector3 const& worldUp)
{
    xxVector3 direction = worldPoint - m_location;
    direction /= direction.Length();

    xxVector3 right = worldUp.Cross(direction);
    right /= right.Length();

    xxVector3 up = direction.Cross(right);
    up /= up.Length();

    m_right = right;
    m_up = up;
    m_direction = direction;
}
//------------------------------------------------------------------------------
void xxCamera::SetFOV(float aspect, float fov, float far)
{
    float halfHeight = tanf(fov * (float)M_PI / 180.0f * 0.5f);
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

    m_viewportMatrix.v[0] = {     scaleX,          0, 0, 0 };
    m_viewportMatrix.v[1] = {          0,     scaleY, 0, 0 };
    m_viewportMatrix.v[2] = {          0,          0, 1, 0 };
    m_viewportMatrix.v[3] = { translateX, translateY, 0, 0 };
}
//------------------------------------------------------------------------------
xxVector3 xxCamera::GetDirectionFromScreenPos(float x, float y) const
{
    float right = m_frustumLeft + x * (m_frustumRight - m_frustumLeft);
    float up = m_frustumBottom + y * (m_frustumTop - m_frustumBottom);

    xxVector3 direction = m_direction + m_right * right + m_up * up;
    direction /= direction.Length();
    return direction;
}
//==============================================================================
