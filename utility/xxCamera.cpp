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
    float l = FrustumLeft;
    float r = FrustumRight;
    float b = FrustumBottom;
    float t = FrustumTop;
    float n = FrustumNear;
    float f = FrustumFar;

    ViewMatrix.v[0] = {  Right.x,              Up.x,              Direction.x,             0 };
    ViewMatrix.v[1] = {  Right.y,              Up.y,              Direction.y,             0 };
    ViewMatrix.v[2] = {  Right.z,              Up.z,              Direction.z,             0 };
    ViewMatrix.v[3] = { -Right.Dot(Location), -Up.Dot(Location), -Direction.Dot(Location), 1 };

    ProjectionMatrix.v[0] = {    (-2) / (l - r),                 0,                  0, 0 };
    ProjectionMatrix.v[1] = {                 0,    (-2) / (b - t),                  0, 0 };
//  ProjectionMatrix.v[2] = { (l + r) / (l - r), (b + t) / (b - t),      (f) / (f - n), 1 };
    ProjectionMatrix.v[2] = {                 0,                 0,      (f) / (f - n), 1 };
    ProjectionMatrix.v[3] = {                 0,                 0, (f * -n) / (f - n), 0 };

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

    FrustumLeft = -halfWidth;
    FrustumRight = halfWidth;
    FrustumBottom = -halfHeight;
    FrustumTop = halfHeight;
    FrustumNear = 1.0f;
    FrustumFar = depth;
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

    binary.Read(FrustumLeft);
    binary.Read(FrustumRight);
    binary.Read(FrustumBottom);
    binary.Read(FrustumTop);
    binary.Read(FrustumNear);
    binary.Read(FrustumFar);

    binary.Read(ViewMatrix);
    binary.Read(ProjectionMatrix);

    binary.Read(Up);
    binary.Read(Right);
    binary.Read(Direction);
    binary.Read(Location);

    binary.Read(LightDirection);
    binary.Read(LightColor);
}
//------------------------------------------------------------------------------
void xxCamera::BinaryWrite(xxBinary& binary) const
{
    binary.WriteString(Name);

    binary.Write(FrustumLeft);
    binary.Write(FrustumRight);
    binary.Write(FrustumBottom);
    binary.Write(FrustumTop);
    binary.Write(FrustumNear);
    binary.Write(FrustumFar);

    binary.Write(ViewMatrix);
    binary.Write(ProjectionMatrix);

    binary.Write(Up);
    binary.Write(Right);
    binary.Write(Direction);
    binary.Write(Location);

    binary.Write(LightDirection);
    binary.Write(LightColor);
}
//==============================================================================
