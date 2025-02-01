//==============================================================================
// xxGraphic : Math Source
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxUtility.h"
#include "xxMath.h"

const xxVector2 xxVector2::ZERO         = { 0, 0 };
const xxVector2 xxVector2::ONE          = { 1, 1 };
const xxVector2 xxVector2::X            = { 1, 0 };
const xxVector2 xxVector2::Y            = { 0, 1 };

const xxVector3 xxVector3::ZERO         = { 0, 0, 0 };
const xxVector3 xxVector3::ONE          = { 1, 1, 1 };
const xxVector3 xxVector3::X            = { 1, 0, 0 };
const xxVector3 xxVector3::Y            = { 0, 1, 0 };
const xxVector3 xxVector3::Z            = { 0, 0, 1 };
const xxVector3 xxVector3::BLACK        = { 0, 0, 0 };
const xxVector3 xxVector3::BLUE         = { 0, 0, 1 };
const xxVector3 xxVector3::GREEN        = { 0, 1, 0 };
const xxVector3 xxVector3::RED          = { 1, 0, 0 };
const xxVector3 xxVector3::WHITE        = { 1, 1, 1 };

const xxVector4 xxVector4::ZERO         = { 0, 0, 0, 0 };
const xxVector4 xxVector4::ONE          = { 1, 1, 1, 1 };
const xxVector4 xxVector4::X            = { 1, 0, 0, 0 };
const xxVector4 xxVector4::Y            = { 0, 1, 0, 0 };
const xxVector4 xxVector4::Z            = { 0, 0, 1, 0 };
const xxVector4 xxVector4::W            = { 0, 0, 0, 1 };
const xxVector4 xxVector4::BLACK        = { 0, 0, 0, 1 };
const xxVector4 xxVector4::BLUE         = { 0, 0, 1, 1 };
const xxVector4 xxVector4::GREEN        = { 0, 1, 0, 1 };
const xxVector4 xxVector4::RED          = { 1, 0, 0, 1 };
const xxVector4 xxVector4::WHITE        = { 1, 1, 1, 1 };

const xxMatrix2x2 xxMatrix2x2::IDENTITY = { 1, 0,
                                            0, 1 };
const xxMatrix2x3 xxMatrix2x3::IDENTITY = { 1, 0,
                                            0, 1,
                                            0, 0 };
const xxMatrix2x4 xxMatrix2x4::IDENTITY = { 1, 0,
                                            0, 1,
                                            0, 0,
                                            0, 0 };
const xxMatrix3x2 xxMatrix3x2::IDENTITY = { 1, 0, 0,
                                            0, 1, 0 };
const xxMatrix3x3 xxMatrix3x3::IDENTITY = { 1, 0, 0,
                                            0, 1, 0,
                                            0, 0, 1 };
const xxMatrix3x4 xxMatrix3x4::IDENTITY = { 1, 0, 0,
                                            0, 1, 0,
                                            0, 0, 1,
                                            0, 0, 0 };
const xxMatrix4x2 xxMatrix4x2::IDENTITY = { 1, 0, 0, 0,
                                            0, 1, 0, 0 };
const xxMatrix4x3 xxMatrix4x3::IDENTITY = { 1, 0, 0, 0,
                                            0, 1, 0, 0,
                                            0, 0, 1, 0 };
const xxMatrix4x4 xxMatrix4x4::IDENTITY = { 1, 0, 0, 0,
                                            0, 1, 0, 0,
                                            0, 0, 1, 0,
                                            0, 0, 0, 1 };

static_assert(sizeof(xxVector2) == 8);
static_assert(sizeof(xxVector3) == 12);
static_assert(sizeof(xxVector4) == 16);
static_assert(sizeof(xxMatrix2x2) == 16);
static_assert(sizeof(xxMatrix2x3) == 24);
static_assert(sizeof(xxMatrix2x4) == 32);
static_assert(sizeof(xxMatrix3x2) == 24);
static_assert(sizeof(xxMatrix3x3) == 36);
static_assert(sizeof(xxMatrix3x4) == 48);
static_assert(sizeof(xxMatrix4x2) == 32);
static_assert(sizeof(xxMatrix4x3) == 48);
static_assert(sizeof(xxMatrix4x4) == 64);
//==============================================================================
//  Vector3
//==============================================================================
int xxVector3::Intersect(xxVector3 const& sphere)
{
    float distance = (xy - sphere.xy).Length();
    if (distance <= sphere.radius - radius)
        return 2;
    if (distance <= radius - sphere.radius)
        return 1;
    if (distance <= radius + sphere.radius)
        return 0;
    return -1;
}
//==============================================================================
//  Vector4
//==============================================================================
int xxVector4::Intersect(xxVector4 const& sphere)
{
    float distance = (xyz - sphere.xyz).Length();
    if (distance <= sphere.radius - radius)
        return 2;
    if (distance <= radius - sphere.radius)
        return 1;
    if (distance <= radius + sphere.radius)
        return 0;
    return -1;
}
//------------------------------------------------------------------------------
bool xxVector4::Intersect(xxVector3 const& point, xxVector3 const& direction)
{
    xxVector3 diff = xyz - point;
    float t0 = diff.Dot(direction);
    float t2 = diff.Dot(diff) - t0 * t0;
    if (radius * radius < t2)
        return false;
    float t1 = sqrtf(radius * radius - t2);
    float distance = t0 > t1 + FLT_EPSILON ? t0 - t1 : t0 + t1;
    return distance > FLT_EPSILON;
}
//------------------------------------------------------------------------------
xxVector4& xxVector4::BoundMerge(xxVector3 const& point)
{
    if (radius == 0.0f)
    {
        xyz = point;
        radius = FLT_EPSILON;
        return (*this);
    }

    xxVector3 diff = point - xyz;

    float length = diff.Length();
    float radius1 = radius;

    if (radius1 >= length)
    {
        return (*this);
    }

    float radius2 = (radius1 + length) / 2.0f;
    float ratio = (radius2 - radius1) / length;
    xyz += diff * ratio;
    radius = radius2;

    return (*this);
}
//------------------------------------------------------------------------------
xxVector4& xxVector4::BoundMerge(xxVector4 const& sphere)
{
    if (sphere.radius == 0.0f)
    {
        return (*this);
    }
    if (radius == 0.0f)
    {
        return (*this) = sphere;
    }

    xxVector3 diff = sphere.xyz - xyz;

    float length = diff.Length();
    float radius1 = radius;
    float radius2 = sphere.radius;

    if (radius1 + radius2 >= length)
    {
        if (radius1 - radius2 >= length)
        {
            return (*this);
        }

        if (radius2 - radius1 >= length)
        {
            return (*this) = sphere;
        }
    }

    float radius3 = (radius1 + length + radius2) / 2.0f;
    float ratio = (radius3 - radius1) / length;
    xyz += diff * ratio;
    radius = radius3;

    return (*this);
}
//------------------------------------------------------------------------------
xxVector4 xxVector4::BoundTransform(xxMatrix4 const& transform, float scale) const
{
    xxVector4 t;
    t = xxVector4{ x, y, z, 1 } * transform;
    t.radius = scale * radius;
    return t;
}
//==============================================================================
//  Matrix 2x2
//==============================================================================
float xxMatrix2::Determinant() const
{
    float det = m11 * m22 - m12 * m21;

    return det;
}
//------------------------------------------------------------------------------
xxMatrix2 xxMatrix2::Inverse() const
{
    float det = m11 * m22 - m12 * m21;
    if (det == 0.0f)
        return {};
    float invDet = 1.0f / det;

    xxMatrix2 m;
    m.m11 =  m22 * invDet;
    m.m12 = -m12 * invDet;
    m.m21 = -m21 * invDet;
    m.m22 =  m11 * invDet;

    return m;
}
//------------------------------------------------------------------------------
xxMatrix2 xxMatrix2::Transpose() const
{
    xxMatrix2 m;

    for (int y = 0; y < M; y++)
        for (int x = 0; x < N; x++)
            m[x].f[y] = v[y].f[x];

    return m;
}
//==============================================================================
//  Matrix 3x3
//==============================================================================
float xxMatrix3::Determinant() const
{
    xxVector3 t;

    t.x = (m22 * m33 - m23 * m32);
    t.y = (m23 * m31 - m21 * m33);
    t.z = (m21 * m32 - m22 * m31);

    float det = v[0].Dot(t);

    return det;
}
//------------------------------------------------------------------------------
xxMatrix3 xxMatrix3::Inverse() const
{
    xxVector3 t;

    t.x = (m22 * m33 - m23 * m32);
    t.y = (m23 * m31 - m21 * m33);
    t.z = (m21 * m32 - m22 * m31);

    float det = v[0].Dot(t);
    if (det == 0.0f)
        return {};
    float invDet = 1.0f / det;

    xxMatrix3 m;

    m.m11 =                     t.x * invDet;
    m.m21 =                     t.y * invDet;
    m.m31 =                     t.z * invDet;
    m.m12 = (m13 * m32 - m12 * m33) * invDet;
    m.m22 = (m11 * m33 - m13 * m31) * invDet;
    m.m32 = (m12 * m31 - m11 * m32) * invDet;
    m.m13 = (m12 * m23 - m13 * m22) * invDet;
    m.m23 = (m13 * m21 - m11 * m23) * invDet;
    m.m33 = (m11 * m22 - m12 * m21) * invDet;

    return m;
}
//------------------------------------------------------------------------------
xxMatrix3 xxMatrix3::Transpose() const
{
    xxMatrix3 m;

    for (int y = 0; y < M; y++)
        for (int x = 0; x < N; x++)
            m[x].f[y] = v[y].f[x];

    return m;
}
//------------------------------------------------------------------------------
xxMatrix3 xxMatrix3::Quaternion(xxVector4 const& q)
{
    xxVector4 xyzw = q;
    xxVector4 xyzw_2 = xyzw + xyzw;
    xxVector4 wwww = { __builtin_shufflevector(xyzw.v, xyzw.v, 3, 3, 3, 3) };
    xxVector4 yzxw = { __builtin_shufflevector(xyzw.v, xyzw.v, 1, 2, 0, 1) };
    xxVector4 zxyw = { __builtin_shufflevector(xyzw.v, xyzw.v, 2, 0, 1, 2) };
    xxVector4 yzxw_2 = { __builtin_shufflevector(xyzw_2.v, xyzw_2.v, 1, 2, 0, 1) };
    xxVector4 zxyw_2 = { __builtin_shufflevector(xyzw_2.v, xyzw_2.v, 2, 0, 1, 2) };
    xxVector4 a = xxVector4::ONE - yzxw * yzxw_2 - zxyw * zxyw_2;
    xxVector4 b = zxyw_2 * wwww + xyzw * yzxw_2;
    xxVector4 c = xyzw * zxyw_2 - yzxw_2 * wwww;
    xxMatrix3 m;
    m[0] = { a.x, b.x, c.x };
    m[1] = { c.y, a.y, b.y };
    m[2] = { b.z, c.z, a.z };
    return m;
}
//==============================================================================
//  Matrix 4x3
//==============================================================================
xxMatrix4x3 xxMatrix4x3::FromMatrix4(xxMatrix4 const& m)
{
    xxMatrix4x3 t;

    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 3; x++)
            t[x].f[y] = m[y].f[x];

    return t;
}
//------------------------------------------------------------------------------
xxMatrix4 xxMatrix4x3::ToMatrix4() const
{
    xxMatrix4 t;

    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 4; x++)
            t[x].f[y] = v[y].f[x];

    t[3].f[3] = 1.0f;
    return t;
}
//==============================================================================
//  Matrix 4x4
//==============================================================================
float xxMatrix4::Determinant() const
{
    float x12 = (m31 * m42 - m32 * m41);
    float x13 = (m31 * m43 - m33 * m41);
    float x14 = (m31 * m44 - m34 * m41);
    float x23 = (m32 * m43 - m33 * m42);
    float x24 = (m32 * m44 - m34 * m42);
    float x34 = (m33 * m44 - m34 * m43);

    xxVector4 t;

    t.x =  (m22 * x34 - m23 * x24 + m24 * x23);
    t.y = -(m21 * x34 - m23 * x14 + m24 * x13);
    t.z =  (m21 * x24 - m22 * x14 + m24 * x12);
    t.w = -(m21 * x23 - m22 * x13 + m23 * x12);

    float det = v[0].Dot(t);

    return det;
}
//------------------------------------------------------------------------------
xxMatrix4 xxMatrix4::Inverse() const
{
    float x12 = (m31 * m42 - m32 * m41);
    float x13 = (m31 * m43 - m33 * m41);
    float x14 = (m31 * m44 - m34 * m41);
    float x23 = (m32 * m43 - m33 * m42);
    float x24 = (m32 * m44 - m34 * m42);
    float x34 = (m33 * m44 - m34 * m43);

    xxVector4 t;

    t.x =  (m22 * x34 - m23 * x24 + m24 * x23);
    t.y = -(m21 * x34 - m23 * x14 + m24 * x13);
    t.z =  (m21 * x24 - m22 * x14 + m24 * x12);
    t.w = -(m21 * x23 - m22 * x13 + m23 * x12);

    float det = v[0].Dot(t);
    if (det == 0.0f)
        return {};
    float invDet = 1.0f / det;

    xxMatrix4 m;

    m.m11 =                                  t.x * invDet;
    m.m21 =                                  t.y * invDet;
    m.m31 =                                  t.z * invDet;
    m.m41 =                                  t.w * invDet;
    m.m12 = -(m12 * x34 - m13 * x24 + m14 * x23) * invDet;
    m.m22 =  (m11 * x34 - m13 * x14 + m14 * x13) * invDet;
    m.m32 = -(m11 * x24 - m12 * x14 + m14 * x12) * invDet;
    m.m42 =  (m11 * x23 - m12 * x13 + m13 * x12) * invDet;
 
    x12 = (m11 * m22 - m21 * m12);
    x13 = (m11 * m23 - m21 * m13);
    x14 = (m11 * m24 - m21 * m14);
    x23 = (m12 * m23 - m22 * m13);
    x24 = (m12 * m24 - m22 * m14);
    x34 = (m13 * m24 - m23 * m14);

    m.m13 =  (m42 * x34 - m43 * x24 + m44 * x23) * invDet;
    m.m23 = -(m41 * x34 - m43 * x14 + m44 * x13) * invDet;
    m.m33 =  (m41 * x24 - m42 * x14 + m44 * x12) * invDet;
    m.m43 = -(m41 * x23 - m42 * x13 + m43 * x12) * invDet;
    m.m14 = -(m32 * x34 - m33 * x24 + m34 * x23) * invDet;
    m.m24 =  (m31 * x34 - m33 * x14 + m34 * x13) * invDet;
    m.m34 = -(m31 * x24 - m32 * x14 + m34 * x12) * invDet;
    m.m44 =  (m31 * x23 - m32 * x13 + m33 * x12) * invDet;

    return m;
}
//------------------------------------------------------------------------------
xxMatrix4 xxMatrix4::Transpose() const
{
    xxMatrix4 m;

    for (int y = 0; y < M; y++)
        for (int x = 0; x < N; x++)
            m[x].f[y] = v[y].f[x];

    return m;
}
//------------------------------------------------------------------------------
float xxMatrix4::FastDeterminant() const
{
    float t1 = (m22 * m33 - m23 * m32);
    float t2 = (m23 * m31 - m21 * m33);
    float t3 = (m21 * m32 - m22 * m31);
    float det = m11 * t1 + m12 * t2 + m13 * t3;

    return det;
}
//------------------------------------------------------------------------------
void xxMatrix4::FastDecompose(xxMatrix3& __restrict rotate, xxVector3& __restrict translate, float& __restrict scale) const
{
    scale = v[0].xyz.Length();
    translate = v[3].xyz;

    float invScale = 1.0f / scale;
    for (int i = 0; i < 3; ++i)
    {
        rotate[i].x = v[i].x * invScale;
        rotate[i].y = v[i].y * invScale;
        rotate[i].z = v[i].z * invScale;
    }
}
//------------------------------------------------------------------------------
void xxMatrix4::MultiplyArray(size_t count, xxVector4 const* __restrict input, int inputStride, xxVector4* __restrict output, int outputStride) const
{
    xxMatrix4 matrix(*this);

    for (size_t i = 0; i < count; ++i)
    {
        (*output).v = __builtin_vectormultiply((*input).v, &matrix.v->v);

        input = reinterpret_cast<xxVector4*>((char*)input + inputStride);
        output = reinterpret_cast<xxVector4*>((char*)output + outputStride);
    }
}
//------------------------------------------------------------------------------
void xxMatrix4::MultiplyArray(size_t count, xxMatrix4 const* __restrict input, int inputStride, xxMatrix4* __restrict output, int outputStride) const
{
    xxMatrix4 matrix(*this);

    for (size_t i = 0; i < count; ++i)
    {
        (*output)[0].v = __builtin_vectormultiply((*input)[0].v, &matrix.v->v);
        (*output)[1].v = __builtin_vectormultiply((*input)[1].v, &matrix.v->v);
        (*output)[2].v = __builtin_vectormultiply((*input)[2].v, &matrix.v->v);
        (*output)[3].v = __builtin_vectormultiply((*input)[3].v, &matrix.v->v);

        input = reinterpret_cast<xxMatrix4*>((char*)input + inputStride);
        output = reinterpret_cast<xxMatrix4*>((char*)output + outputStride);
    }
}
//------------------------------------------------------------------------------
void xxMatrix4::MultiplyLink(size_t count, xxMatrix4 const* __restrict input, int inputStride, xxMatrix4* __restrict output, int outputStride) const
{
    xxMatrix4 matrix(*this);

    for (size_t i = 0; i < count; ++i)
    {
        (*output)[0].v = __builtin_vectormultiply((*input)[0].v, &matrix.v->v);
        (*output)[1].v = __builtin_vectormultiply((*input)[1].v, &matrix.v->v);
        (*output)[2].v = __builtin_vectormultiply((*input)[2].v, &matrix.v->v);
        (*output)[3].v = __builtin_vectormultiply((*input)[3].v, &matrix.v->v);
        matrix = (*output);

        input = reinterpret_cast<xxMatrix4*>((char*)input + inputStride);
        output = reinterpret_cast<xxMatrix4*>((char*)output + outputStride);
    }
}
//==============================================================================
