//==============================================================================
// xxGraphic : Vector Header
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

#if defined(_MSC_VER)
#   pragma optimize("y", on)
#endif

union xxCPPAPI xxVector2
{
    struct { float x, y; };
    struct { float u, v; };

    xxVector2   operator +  (const xxVector2& __restrict v) const   { return xxVector2(*this) += v; }
    xxVector2   operator -  (const xxVector2& __restrict v) const   { return xxVector2(*this) -= v; }
    xxVector2   operator *  (const xxVector2& __restrict v) const   { return xxVector2(*this) *= v; }
    xxVector2   operator /  (const xxVector2& __restrict v) const   { return xxVector2(*this) /= v; }
    xxVector2   operator *  (float s) const                         { return xxVector2(*this) *= s; }
    xxVector2   operator /  (float s) const                         { return xxVector2(*this) /= s; }
    xxVector2   operator -  () const                                { return { -x, -y };            }

    xxVector2&  operator += (const xxVector2& __restrict v) { x += v.x; y += v.y;       return (*this); }
    xxVector2&  operator -= (const xxVector2& __restrict v) { x -= v.x; y -= v.y;       return (*this); }
    xxVector2&  operator *= (const xxVector2& __restrict v) { x *= v.x; y *= v.y;       return (*this); }
    xxVector2&  operator /= (const xxVector2& __restrict v) { x /= v.x; y /= v.y;       return (*this); }
    xxVector2&  operator *= (float s)                       { x *= s;   y *= s;         return (*this); }
    xxVector2&  operator /= (float s)                       { (*this) *= (1.0f / s);    return (*this); }

    float   Dot             (const xxVector2& __restrict v) const   { return x * v.x + y * v.y;         }
    float   SquaredLength   () const                                { return Dot(*this);                }
    float   Length          () const                                { return sqrtf(SquaredLength());    }

    float   Cross           (const xxVector2& __restrict v) const   { return x * v.y - y * v.x;         }

    static const xxVector2 ZERO;
    static const xxVector2 ONE;

    static const xxVector2 X;
    static const xxVector2 Y;
};

union xxCPPAPI xxVector3
{
    struct { float x, y, z; };
    struct { float r, g, b; };

    xxVector3   operator +  (const xxVector3& __restrict v) const   { return xxVector3(*this) += v; }
    xxVector3   operator -  (const xxVector3& __restrict v) const   { return xxVector3(*this) -= v; }
    xxVector3   operator *  (const xxVector3& __restrict v) const   { return xxVector3(*this) *= v; }
    xxVector3   operator /  (const xxVector3& __restrict v) const   { return xxVector3(*this) /= v; }
    xxVector3   operator *  (float s) const                         { return xxVector3(*this) *= s; }
    xxVector3   operator /  (float s) const                         { return xxVector3(*this) /= s; }
    xxVector3   operator -  () const                                { return { -x, -y, -z };        }

    xxVector3&  operator += (const xxVector3& __restrict v) { x += v.x; y += v.y; z += v.z; return (*this); }
    xxVector3&  operator -= (const xxVector3& __restrict v) { x -= v.x; y -= v.y; z -= v.z; return (*this); }
    xxVector3&  operator *= (const xxVector3& __restrict v) { x *= v.x; y *= v.y; z *= v.z; return (*this); }
    xxVector3&  operator /= (const xxVector3& __restrict v) { x /= v.x; y /= v.y; z /= v.z; return (*this); }
    xxVector3&  operator *= (float s)                       { x *= s;   y *= s;   z *= s;   return (*this); }
    xxVector3&  operator /= (float s)                       { (*this) *= (1.0f / s);        return (*this); }

    float   Dot             (const xxVector3& __restrict v) const   { return x * v.x + y * v.y + z * v.z;   }
    float   SquaredLength   () const                                { return Dot(*this);                    }
    float   Length          () const                                { return sqrtf(SquaredLength());        }

    xxVector3   Cross       (const xxVector3& __restrict v) const   { return xxVector3{ y, z, x } * xxVector3{ v.z, v.x, v.y } -
                                                                             xxVector3{ z, x, y } * xxVector3{ v.y, v.z, v.x }; }

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

    xxVector4   operator +  (const xxVector4& __restrict v) const   { return xxVector4(*this) += v; }
    xxVector4   operator -  (const xxVector4& __restrict v) const   { return xxVector4(*this) -= v; }
    xxVector4   operator *  (const xxVector4& __restrict v) const   { return xxVector4(*this) *= v; }
    xxVector4   operator /  (const xxVector4& __restrict v) const   { return xxVector4(*this) /= v; }
    xxVector4   operator *  (float s) const                         { return xxVector4(*this) *= s; }
    xxVector4   operator /  (float s) const                         { return xxVector4(*this) /= s; }
    xxVector4   operator -  () const                                { return { -x, -y, -z, -w };    }

    xxVector4&  operator += (const xxVector4& __restrict v) { x += v.x; y += v.y; z += v.z; w += v.w;   return (*this); }
    xxVector4&  operator -= (const xxVector4& __restrict v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w;   return (*this); }
    xxVector4&  operator *= (const xxVector4& __restrict v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w;   return (*this); }
    xxVector4&  operator /= (const xxVector4& __restrict v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w;   return (*this); }
    xxVector4&  operator *= (float s)                       { x *= s;   y *= s;   z *= s;   w *= s;     return (*this); }
    xxVector4&  operator /= (float s)                       { (*this) *= (1.0f / s);                    return (*this); }

    float   Dot             (const xxVector4& __restrict v) const   { return x * v.x + y * v.y + z * v.z + w * v.w; }
    float   SquaredLength   () const                                { return Dot(*this);                            }
    float   Length          () const                                { return sqrtf(SquaredLength());                }

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

#if defined(_MSC_VER)
#   pragma optimize("", on)
#endif
