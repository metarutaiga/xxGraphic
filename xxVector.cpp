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

const xxVector2 xxVector2::ZERO     = { 0, 0 };
const xxVector2 xxVector2::ONE      = { 1, 1 };
const xxVector2 xxVector2::X        = { 1, 0 };
const xxVector2 xxVector2::Y        = { 0, 1 };

const xxVector3 xxVector3::ZERO     = { 0, 0, 0 };
const xxVector3 xxVector3::ONE      = { 1, 1, 1 };
const xxVector3 xxVector3::X        = { 1, 0, 0 };
const xxVector3 xxVector3::Y        = { 0, 1, 0 };
const xxVector3 xxVector3::Z        = { 0, 0, 1 };
const xxVector3 xxVector3::BLACK    = { 0, 0, 0 };
const xxVector3 xxVector3::BLUE     = { 0, 0, 1 };
const xxVector3 xxVector3::GREEN    = { 0, 1, 0 };
const xxVector3 xxVector3::RED      = { 1, 0, 0 };
const xxVector3 xxVector3::WHITE    = { 1, 1, 1 };

const xxVector4 xxVector4::ZERO     = { 0, 0, 0, 0 };
const xxVector4 xxVector4::ONE      = { 1, 1, 1, 1 };
const xxVector4 xxVector4::X        = { 1, 0, 0, 0 };
const xxVector4 xxVector4::Y        = { 0, 1, 0, 0 };
const xxVector4 xxVector4::Z        = { 0, 0, 1, 0 };
const xxVector4 xxVector4::W        = { 0, 0, 0, 1 };
const xxVector4 xxVector4::BLACK    = { 0, 0, 0, 1 };
const xxVector4 xxVector4::BLUE     = { 0, 0, 1, 1 };
const xxVector4 xxVector4::GREEN    = { 0, 1, 0, 1 };
const xxVector4 xxVector4::RED      = { 1, 0, 0, 1 };
const xxVector4 xxVector4::WHITE    = { 1, 1, 1, 1 };

const xxMatrix2 xxMatrix2::IDENTITY = { xxVector2::X, xxVector2::Y };
const xxMatrix3 xxMatrix3::IDENTITY = { xxVector3::X, xxVector3::Y, xxVector3::Z };
const xxMatrix4 xxMatrix4::IDENTITY = { xxVector4::X, xxVector4::Y, xxVector4::Z, xxVector4::W };

//==============================================================================
//  Vector 2
//==============================================================================
xxVector2 xxVector2::operator - () const
{
    return { -x, -y };
}
//==============================================================================
//  Vector 3
//==============================================================================
xxVector3 xxVector3::operator - () const
{
    return { -x, -y, -z };
}
//==============================================================================
//  Vector 4
//==============================================================================
xxVector4 xxVector4::operator - () const
{
    return { -x, -y, -z, -w };
}
//------------------------------------------------------------------------------
xxVector4& xxVector4::operator = (const xxVector4& other)
{
#if defined(_M_IX86) || defined(_M_AMD64) || defined(__i386__) || defined(__amd64__)
    (*this).v = other.v;
#else
    (*this).x = other.x;
    (*this).y = other.y;
    (*this).z = other.z;
    (*this).w = other.w;
#endif

    return (*this);
}
//==============================================================================
//  Matrix 4x4
//==============================================================================
xxMatrix4 xxMatrix4::Multiply(const xxMatrix4& __restrict m, float s)
{
    xxMatrix4 r;

    r.v[0].v = _mm_mul_ps(m.v[0].v, _mm_set1_ps(s));
    r.v[1].v = _mm_mul_ps(m.v[1].v, _mm_set1_ps(s));
    r.v[2].v = _mm_mul_ps(m.v[2].v, _mm_set1_ps(s));
    r.v[3].v = _mm_mul_ps(m.v[3].v, _mm_set1_ps(s));

    return r;
}
//------------------------------------------------------------------------------
xxVector4 xxMatrix4::Multiply(const xxMatrix4& __restrict m, const xxVector4& __restrict v)
{
    xxVector4 r;

    r.v =                   _mm_mul_ps(m.v[0].v, _mm_shuffle1_ps(v.v, _MM_SHUFFLE(0,0,0,0)));
    r.v = _mm_add_ps(r.v,   _mm_mul_ps(m.v[1].v, _mm_shuffle1_ps(v.v, _MM_SHUFFLE(1,1,1,1))));
    r.v = _mm_add_ps(r.v,   _mm_mul_ps(m.v[2].v, _mm_shuffle1_ps(v.v, _MM_SHUFFLE(2,2,2,2))));
    r.v = _mm_add_ps(r.v,   _mm_mul_ps(m.v[3].v, _mm_shuffle1_ps(v.v, _MM_SHUFFLE(3,3,3,3))));

    return r;
}
//------------------------------------------------------------------------------
xxMatrix4 xxMatrix4::Multiply(const xxMatrix4& __restrict m, const xxMatrix4& __restrict o)
{
    xxMatrix4 r;

    r.v[0] = Multiply(m, o.v[0]);
    r.v[1] = Multiply(m, o.v[1]);
    r.v[2] = Multiply(m, o.v[2]);
    r.v[3] = Multiply(m, o.v[3]);

    return r;
}
//------------------------------------------------------------------------------
void xxMatrix4::MultiplyArray(const xxMatrix4& __restrict matrix, int count, const xxVector4* __restrict input, int inputStride, xxVector4* __restrict output, int outputStride)
{
    for (int x = 0; x < count; ++x)
    {
        (*output) = Multiply(matrix, (*input));
        input = reinterpret_cast<xxVector4*>((char*)input + inputStride);
        output = reinterpret_cast<xxVector4*>((char*)output + outputStride);
    }
}
//------------------------------------------------------------------------------
void xxMatrix4::MultiplyArray(const xxMatrix4& __restrict matrix, int count, const xxMatrix4* __restrict input, int inputStride, xxMatrix4* __restrict output, int outputStride)
{
    for (int x = 0; x < count; ++x)
    {
        (*output) = Multiply(matrix, (*input));
        input = reinterpret_cast<xxMatrix4*>((char*)input + inputStride);
        output = reinterpret_cast<xxMatrix4*>((char*)output + outputStride);
    }
}
//==============================================================================
