//==============================================================================
// xxGraphic : Math Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxMath.h"

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
float xxMatrix4::Determinant() const
{
    return  _[0].x * (_[1].y * _[2].z - _[1].z * _[2].y) +
            _[0].y * (_[1].z * _[2].x - _[1].x * _[2].z) +
            _[0].z * (_[1].x * _[2].y - _[1].y * _[2].x);
}
//------------------------------------------------------------------------------
void xxMatrix4::FastDecompose(xxMatrix3& __restrict rotate, xxVector3& __restrict translate, float& __restrict scale) const
{
    scale = xxVector3{ _[0].x, _[0].y, _[0].z }.Length();
    translate = { _[3].x, _[3].y, _[3].z };

    float invScale = 1.0f / scale;
    for (int i = 0; i < 3; ++i)
    {
        rotate._[i].x = _[i].x * invScale;
        rotate._[i].y = _[i].y * invScale;
        rotate._[i].z = _[i].z * invScale;
    }
}
//------------------------------------------------------------------------------
void xxMatrix4::MultiplyArray(size_t count, const xxVector4* __restrict input, int inputStride, xxVector4* __restrict output, int outputStride) const
{
    xxMatrix4x4 matrix(*this);

    for (size_t i = 0; i < count; ++i)
    {
#if defined(_M_IX86) || defined(_M_AMD64)
        __m128 vv = (*input).v;
        __m128 v0 = _mm_shuffle1_ps(vv, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 v1 = _mm_shuffle1_ps(vv, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 v2 = _mm_shuffle1_ps(vv, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 v3 = _mm_shuffle1_ps(vv, _MM_SHUFFLE(3, 3, 3, 3));

        (*output).v =                         _mm_mul_ps(matrix._[0].v, v0);
        (*output).v = _mm_add_ps((*output).v, _mm_mul_ps(matrix._[1].v, v1));
        (*output).v = _mm_add_ps((*output).v, _mm_mul_ps(matrix._[2].v, v2));
        (*output).v = _mm_add_ps((*output).v, _mm_mul_ps(matrix._[3].v, v3));
#else
        (*output) = matrix * (*input);
#endif
        input = reinterpret_cast<xxVector4*>((char*)input + inputStride);
        output = reinterpret_cast<xxVector4*>((char*)output + outputStride);
    }
}
//------------------------------------------------------------------------------
void xxMatrix4::MultiplyArray(size_t count, const xxMatrix4* __restrict input, int inputStride, xxMatrix4* __restrict output, int outputStride) const
{
    xxMatrix4x4 matrix(*this);

    for (size_t i = 0; i < count; ++i)
    {
#if defined(_M_IX86) || defined(_M_AMD64)

        __m128 v0 = (*input)._[0].v;
        __m128 v00 = _mm_shuffle1_ps(v0, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 v01 = _mm_shuffle1_ps(v0, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 v02 = _mm_shuffle1_ps(v0, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 v03 = _mm_shuffle1_ps(v0, _MM_SHUFFLE(3, 3, 3, 3));
        (*output)._[0].v =                              _mm_mul_ps(matrix._[0].v, v00);
        (*output)._[0].v = _mm_add_ps((*output)._[0].v, _mm_mul_ps(matrix._[1].v, v01));
        (*output)._[0].v = _mm_add_ps((*output)._[0].v, _mm_mul_ps(matrix._[2].v, v02));
        (*output)._[0].v = _mm_add_ps((*output)._[0].v, _mm_mul_ps(matrix._[3].v, v03));

        __m128 v1 = (*input)._[1].v;
        __m128 v10 = _mm_shuffle1_ps(v1, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 v11 = _mm_shuffle1_ps(v1, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 v12 = _mm_shuffle1_ps(v1, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 v13 = _mm_shuffle1_ps(v1, _MM_SHUFFLE(3, 3, 3, 3));
        (*output)._[1].v =                              _mm_mul_ps(matrix._[0].v, v10);
        (*output)._[1].v = _mm_add_ps((*output)._[1].v, _mm_mul_ps(matrix._[1].v, v11));
        (*output)._[1].v = _mm_add_ps((*output)._[1].v, _mm_mul_ps(matrix._[2].v, v12));
        (*output)._[1].v = _mm_add_ps((*output)._[1].v, _mm_mul_ps(matrix._[3].v, v13));

        __m128 v2 = (*input)._[2].v;
        __m128 v20 = _mm_shuffle1_ps(v2, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 v21 = _mm_shuffle1_ps(v2, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 v22 = _mm_shuffle1_ps(v2, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 v23 = _mm_shuffle1_ps(v2, _MM_SHUFFLE(3, 3, 3, 3));
        (*output)._[2].v =                              _mm_mul_ps(matrix._[0].v, v20);
        (*output)._[2].v = _mm_add_ps((*output)._[2].v, _mm_mul_ps(matrix._[1].v, v21));
        (*output)._[2].v = _mm_add_ps((*output)._[2].v, _mm_mul_ps(matrix._[2].v, v22));
        (*output)._[2].v = _mm_add_ps((*output)._[2].v, _mm_mul_ps(matrix._[3].v, v23));

        __m128 v3 = (*input)._[3].v;
        __m128 v30 = _mm_shuffle1_ps(v3, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 v31 = _mm_shuffle1_ps(v3, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 v32 = _mm_shuffle1_ps(v3, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 v33 = _mm_shuffle1_ps(v3, _MM_SHUFFLE(3, 3, 3, 3));
        (*output)._[3].v =                              _mm_mul_ps(matrix._[0].v, v30);
        (*output)._[3].v = _mm_add_ps((*output)._[3].v, _mm_mul_ps(matrix._[1].v, v31));
        (*output)._[3].v = _mm_add_ps((*output)._[3].v, _mm_mul_ps(matrix._[2].v, v32));
        (*output)._[3].v = _mm_add_ps((*output)._[3].v, _mm_mul_ps(matrix._[3].v, v33));
#else
        (*output) = matrix * (*input);
#endif
        input = reinterpret_cast<xxMatrix4*>((char*)input + inputStride);
        output = reinterpret_cast<xxMatrix4*>((char*)output + outputStride);
    }
}
//==============================================================================
