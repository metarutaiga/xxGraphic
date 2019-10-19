//==============================================================================
// xxGraphic : Vector Header
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

union xxCPPAPI xxVector2
{
    struct { float x, y; };
    struct { float u, v; };

    xxVector2 operator - () const;

    static const xxVector2 ZERO;
    static const xxVector2 ONE;

    static const xxVector2 X;
    static const xxVector2 Y;
};

union xxCPPAPI xxVector3
{
    struct { float x, y, z; };
    struct { float r, g, b; };
    struct { float u, v, w; };

    xxVector3 operator - () const;

    static float SquaredLength(const xxVector3& vector);
    static float Length(const xxVector3& vector);

    static const xxVector3 ZERO;
    static const xxVector3 ONE;

    static const xxVector3 X;
    static const xxVector3 Y;
    static const xxVector3 Z;

    static const xxVector3 BLACK;
    static const xxVector3 BLUE;
    static const xxVector3 GREEN;
    static const xxVector3 RED;
    static const xxVector3 WHITE;
};

union xxCPPAPI xxVector4
{
    struct { float x, y, z, w; };
    struct { float r, g, b, a; };
#if defined(_M_IX86) || defined(_M_AMD64) || defined(__i386__) || defined(__amd64__)
    __m128 v;
#endif

    xxVector4 operator - () const;

    xxVector4& operator = (const xxVector4& other);

    static void Multiply(xxVector4& output, const xxVector4& vector, float scale);

    static const xxVector4 ZERO;
    static const xxVector4 ONE;

    static const xxVector4 X;
    static const xxVector4 Y;
    static const xxVector4 Z;
    static const xxVector4 W;

    static const xxVector4 BLACK;
    static const xxVector4 BLUE;
    static const xxVector4 GREEN;
    static const xxVector4 RED;
    static const xxVector4 WHITE;
};

struct xxCPPAPI xxMatrix2
{
    xxVector2 v[2];

    static const xxMatrix2 IDENTITY;
};

struct xxCPPAPI xxMatrix3
{
    xxVector3 v[3];

    static const xxMatrix3 IDENTITY;
};

struct xxCPPAPI xxMatrix4
{
    xxVector4 v[4];

    static float Determinant(const xxMatrix4& matrix);

    static void FastDecompose(const xxMatrix4& matrix, xxMatrix3& rotate, xxVector3& translate, float& scale);

    static void Multiply(xxMatrix4& output, const xxMatrix4& matrix, float scale);
    static void Multiply(xxVector4& output, const xxMatrix4& matrix, const xxVector4& vector);
    static void Multiply(xxMatrix4& output, const xxMatrix4& matrix, const xxMatrix4& other);
    static void MultiplyArray(const xxMatrix4& matrix, int count, const xxVector4* input, int inputStride, xxVector4* output, int outputStride);
    static void MultiplyArray(const xxMatrix4& matrix, int count, const xxMatrix4* input, int inputStride, xxMatrix4* output, int outputStride);

    static const xxMatrix4 IDENTITY;
};
