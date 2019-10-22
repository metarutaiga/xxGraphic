//==============================================================================
// xxGraphic : Vector Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxVector.h"

#if defined(_MSC_VER)
#   pragma optimize("y", on)
#endif

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

//==============================================================================
//  Matrix 4x4
//==============================================================================
float xxMatrix4::Determinant(const xxMatrix4& __restrict matrix)
{
    return  matrix.v[0].x * (matrix.v[1].y * matrix.v[2].z - matrix.v[1].z * matrix.v[2].y) +
            matrix.v[0].y * (matrix.v[1].z * matrix.v[2].x - matrix.v[1].x * matrix.v[2].z) +
            matrix.v[0].z * (matrix.v[1].x * matrix.v[2].y - matrix.v[1].y * matrix.v[2].x);
}
//------------------------------------------------------------------------------
void xxMatrix4::FastDecompose(const xxMatrix4& __restrict matrix, xxMatrix3& __restrict rotate, xxVector3& __restrict translate, float& __restrict scale)
{
    scale = xxVector3{ matrix.v[0].x, matrix.v[0].y, matrix.v[0].z }.Length();
    translate = { matrix.v[3].x, matrix.v[3].y, matrix.v[3].z };

    float invScale = 1.0f / scale;
    for (int i = 0; i < 3; ++i)
    {
        rotate.v[i].x = matrix.v[i].x * invScale;
        rotate.v[i].y = matrix.v[i].y * invScale;
        rotate.v[i].z = matrix.v[i].z * invScale;
    }
}
//------------------------------------------------------------------------------
void xxMatrix4::Multiply(xxMatrix4& __restrict r, const xxMatrix4& __restrict m, float s)
{
    __m128 vs = _mm_set1_ps(s);

    r.v[0].v = _mm_mul_ps(m.v[0].v, vs);
    r.v[1].v = _mm_mul_ps(m.v[1].v, vs);
    r.v[2].v = _mm_mul_ps(m.v[2].v, vs);
    r.v[3].v = _mm_mul_ps(m.v[3].v, vs);
}
//------------------------------------------------------------------------------
void xxMatrix4::Multiply(xxVector4& __restrict r, const xxMatrix4& __restrict m, const xxVector4& __restrict v)
{
    __m128 vv = v.v;
    __m128 v0 = _mm_shuffle1_ps(vv, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 v1 = _mm_shuffle1_ps(vv, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 v2 = _mm_shuffle1_ps(vv, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 v3 = _mm_shuffle1_ps(vv, _MM_SHUFFLE(3, 3, 3, 3));

    r.v =                   _mm_mul_ps(m.v[0].v, v0);
    r.v = _mm_add_ps(r.v,   _mm_mul_ps(m.v[1].v, v1));
    r.v = _mm_add_ps(r.v,   _mm_mul_ps(m.v[2].v, v2));
    r.v = _mm_add_ps(r.v,   _mm_mul_ps(m.v[3].v, v3));
}
//------------------------------------------------------------------------------
void xxMatrix4::Multiply(xxMatrix4& __restrict r, const xxMatrix4& __restrict m, const xxMatrix4& __restrict o)
{
    Multiply(r.v[0], m, o.v[0]);
    Multiply(r.v[1], m, o.v[1]);
    Multiply(r.v[2], m, o.v[2]);
    Multiply(r.v[3], m, o.v[3]);
}
//------------------------------------------------------------------------------
void xxMatrix4::MultiplyArray(const xxMatrix4& __restrict matrix, size_t count, const xxVector4* __restrict input, int inputStride, xxVector4* __restrict output, int outputStride)
{
    for (size_t i = 0; i < count; ++i)
    {
        Multiply((*output), matrix, (*input));
        input = reinterpret_cast<xxVector4*>((char*)input + inputStride);
        output = reinterpret_cast<xxVector4*>((char*)output + outputStride);
    }
}
//------------------------------------------------------------------------------
void xxMatrix4::MultiplyArray(const xxMatrix4& __restrict matrix, size_t count, const xxMatrix4* __restrict input, int inputStride, xxMatrix4* __restrict output, int outputStride)
{
    for (size_t i = 0; i < count; ++i)
    {
        Multiply((*output), matrix, (*input));
        input = reinterpret_cast<xxMatrix4*>((char*)input + inputStride);
        output = reinterpret_cast<xxMatrix4*>((char*)output + outputStride);
    }
}
//==============================================================================
