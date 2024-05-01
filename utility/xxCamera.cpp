//==============================================================================
// xxGraphic : Camera Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxBinary.h"
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
void xxCamera::Update()
{
    float l = m_frustumLeft;
    float r = m_frustumRight;
    float b = m_frustumBottom;
    float t = m_frustumTop;
    float n = m_frustumNear;
    float f = m_frustumFar;

    ViewMatrix.v[0] = {  Right.x,              Up.x,              Direction.x,             0 };
    ViewMatrix.v[1] = {  Right.y,              Up.y,              Direction.y,             0 };
    ViewMatrix.v[2] = {  Right.z,              Up.z,              Direction.z,             0 };
    ViewMatrix.v[3] = { -Right.Dot(Location), -Up.Dot(Location), -Direction.Dot(Location), 1 };

    ProjectionMatrix.v[0] = {    (-2) / (l - r),                 0,                  0, 0 };
    ProjectionMatrix.v[1] = {                 0,    (-2) / (b - t),                  0, 0 };
//  ProjectionMatrix.v[2] = { (l + r) / (l - r), (b + t) / (b - t),      (f) / (f - n), 1 };
    ProjectionMatrix.v[2] = {                 0,                 0,      (f) / (f - n), 1 };
    ProjectionMatrix.v[3] = {                 0,                 0, (f * -n) / (f - n), 0 };

    ProjectionMatrix = ViewportMatrix * ProjectionMatrix;
    ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}
//------------------------------------------------------------------------------
void xxCamera::LookAt(xxVector3 const& worldPoint, xxVector3 const& worldUp)
{
    xxVector3 direction = worldPoint - Location;
    direction /= direction.Length();

    xxVector3 right = worldUp.Cross(direction);
    right /= right.Length();

    xxVector3 up = direction.Cross(right);
    up /= up.Length();

    Right = right;
    Up = up;
    Direction = direction;
}
//------------------------------------------------------------------------------
void xxCamera::SetFOV(float aspect, float fov, float depth)
{
    float halfHeight = tanf(fov * (float)M_PI / 180.0f * 0.5f);
    float halfWidth = halfHeight * aspect;

    m_frustumLeft = -halfWidth;
    m_frustumRight = halfWidth;
    m_frustumBottom = -halfHeight;
    m_frustumTop = halfHeight;
    m_frustumNear = 1.0f;
    m_frustumFar = depth;
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

    ViewportMatrix.v[0] = {     scaleX,          0, 0, 0 };
    ViewportMatrix.v[1] = {          0,     scaleY, 0, 0 };
    ViewportMatrix.v[2] = {          0,          0, 1, 0 };
    ViewportMatrix.v[3] = { translateX, translateY, 0, 1 };
}
//------------------------------------------------------------------------------
xxVector3 xxCamera::GetDirectionFromScreenPos(float x, float y) const
{
    float right = m_frustumLeft + x * (m_frustumRight - m_frustumLeft);
    float up = m_frustumBottom + y * (m_frustumTop - m_frustumBottom);

    xxVector3 direction = Direction + Right * right + Up * up;
    direction /= direction.Length();
    return direction;
}
//------------------------------------------------------------------------------
xxVector4 xxCamera::GetWorldPosToScreenPos(xxVector3 const& point) const
{
    xxVector4 screen = ViewProjectionMatrix * xxVector4{point.x, point.y, point.z, 1.0f};
    screen.xyz /= screen.w;
    screen.x = screen.x *  0.5f + 0.5f;
    screen.y = screen.y * -0.5f + 0.5f;
    return screen;
}
//------------------------------------------------------------------------------
xxCameraPtr xxCamera::Create()
{
    xxCameraPtr camera = xxCameraPtr(new xxCamera(), [](xxCamera* camera) { delete camera; });
    if (camera == nullptr)
        return nullptr;

    return camera;
}
//==============================================================================
//  Binary
//==============================================================================
xxCameraPtr (*xxCamera::BinaryCreate)() = xxCamera::Create;
//------------------------------------------------------------------------------
void xxCamera::BinaryRead(xxBinary& binary)
{
    binary.ReadString(Name);

    binary.Read(m_frustumLeft);
    binary.Read(m_frustumRight);
    binary.Read(m_frustumBottom);
    binary.Read(m_frustumTop);
    binary.Read(m_frustumNear);
    binary.Read(m_frustumFar);

    binary.Read(ViewMatrix);
    binary.Read(ProjectionMatrix);
//  binary.Read(ViewportMatrix);

    binary.Read(Right);
    binary.Read(Up);
    binary.Read(Direction);
    binary.Read(Location);

    binary.Read(LightDirection);
    binary.Read(LightColor);
}
//------------------------------------------------------------------------------
void xxCamera::BinaryWrite(xxBinary& binary) const
{
    binary.WriteString(Name);

    binary.Write(m_frustumLeft);
    binary.Write(m_frustumRight);
    binary.Write(m_frustumBottom);
    binary.Write(m_frustumTop);
    binary.Write(m_frustumNear);
    binary.Write(m_frustumFar);

    binary.Write(ViewMatrix);
    binary.Write(ProjectionMatrix);
//  binary.Write(ViewportMatrix);

    binary.Write(Right);
    binary.Write(Up);
    binary.Write(Direction);
    binary.Write(Location);

    binary.Write(LightDirection);
    binary.Write(LightColor);
}
//==============================================================================
