//==============================================================================
// xxGraphic : Vector Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxVector.h"

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
//  Matrix 4x4
//==============================================================================
xxMatrix4 xxMatrix4::Multiply(const xxMatrix4& left, const xxMatrix4& right)
{
    xxMatrix4 newMatrix;

    newMatrix.v[0].v =                               _mm_mul_ps(right.v[0].v, _mm_set1_ps(left.v[0].x));
    newMatrix.v[0].v = _mm_add_ps(newMatrix.v[0].v,  _mm_mul_ps(right.v[1].v, _mm_set1_ps(left.v[0].y)));
    newMatrix.v[0].v = _mm_add_ps(newMatrix.v[0].v,  _mm_mul_ps(right.v[2].v, _mm_set1_ps(left.v[0].z)));
    newMatrix.v[0].v = _mm_add_ps(newMatrix.v[0].v,  _mm_mul_ps(right.v[3].v, _mm_set1_ps(left.v[0].w)));

    newMatrix.v[1].v =                               _mm_mul_ps(right.v[0].v, _mm_set1_ps(left.v[1].x));
    newMatrix.v[1].v = _mm_add_ps(newMatrix.v[1].v,  _mm_mul_ps(right.v[1].v, _mm_set1_ps(left.v[1].y)));
    newMatrix.v[1].v = _mm_add_ps(newMatrix.v[1].v,  _mm_mul_ps(right.v[2].v, _mm_set1_ps(left.v[1].z)));
    newMatrix.v[1].v = _mm_add_ps(newMatrix.v[1].v,  _mm_mul_ps(right.v[3].v, _mm_set1_ps(left.v[1].w)));

    newMatrix.v[2].v =                               _mm_mul_ps(right.v[0].v, _mm_set1_ps(left.v[2].x));
    newMatrix.v[2].v = _mm_add_ps(newMatrix.v[2].v,  _mm_mul_ps(right.v[1].v, _mm_set1_ps(left.v[2].y)));
    newMatrix.v[2].v = _mm_add_ps(newMatrix.v[2].v,  _mm_mul_ps(right.v[2].v, _mm_set1_ps(left.v[2].z)));
    newMatrix.v[2].v = _mm_add_ps(newMatrix.v[2].v,  _mm_mul_ps(right.v[3].v, _mm_set1_ps(left.v[2].w)));

    newMatrix.v[3].v =                               _mm_mul_ps(right.v[0].v, _mm_set1_ps(left.v[3].x));
    newMatrix.v[3].v = _mm_add_ps(newMatrix.v[3].v,  _mm_mul_ps(right.v[1].v, _mm_set1_ps(left.v[3].y)));
    newMatrix.v[3].v = _mm_add_ps(newMatrix.v[3].v,  _mm_mul_ps(right.v[2].v, _mm_set1_ps(left.v[3].z)));
    newMatrix.v[3].v = _mm_add_ps(newMatrix.v[3].v,  _mm_mul_ps(right.v[3].v, _mm_set1_ps(left.v[3].w)));

    return newMatrix;
}
//==============================================================================
