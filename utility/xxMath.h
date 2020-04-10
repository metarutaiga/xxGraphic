//==============================================================================
// xxGraphic : Math Header
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

#if defined(_MSC_VER)
#   pragma optimize("y", on)
#endif

union xxCPPAPI xxVector2
{
    enum { N = 2 };
    struct { float x, y; };
    struct { float u, v; };
    float f[N];

    typedef xxVector2 type;

    xxVector2       operator -      () const                     { type t;        for (int i = 0; i < N; ++i) t.f[i] =   -f[i];              return t;       }
    xxVector2&      operator +=     (const xxVector2& v)         { type t(*this); for (int i = 0; i < N; ++i) t.f[i] += v.f[i]; (*this) = t; return (*this); }
    xxVector2&      operator -=     (const xxVector2& v)         { type t(*this); for (int i = 0; i < N; ++i) t.f[i] -= v.f[i]; (*this) = t; return (*this); }
    xxVector2&      operator *=     (const xxVector2& v)         { type t(*this); for (int i = 0; i < N; ++i) t.f[i] *= v.f[i]; (*this) = t; return (*this); }
    xxVector2&      operator /=     (const xxVector2& v)         { type t(*this); for (int i = 0; i < N; ++i) t.f[i] /= v.f[i]; (*this) = t; return (*this); }
    xxVector2&      operator *=     (float s)                    { type t(*this); for (int i = 0; i < N; ++i) t.f[i] *= s;      (*this) = t; return (*this); }
    xxVector2&      operator /=     (float s)                    { (*this) *= (1.0f / s);                                                    return (*this); }

    xxVector2       operator +      (const xxVector2& v) const   { return xxVector2(*this) += v; }
    xxVector2       operator -      (const xxVector2& v) const   { return xxVector2(*this) -= v; }
    xxVector2       operator *      (const xxVector2& v) const   { return xxVector2(*this) *= v; }
    xxVector2       operator /      (const xxVector2& v) const   { return xxVector2(*this) /= v; }
    xxVector2       operator *      (float s) const              { return xxVector2(*this) *= s; }
    xxVector2       operator /      (float s) const              { return xxVector2(*this) /= s; }

    float           Dot             (const xxVector2& v) const   { return x * v.x + y * v.y;      }
    float           SquaredLength   () const                     { return Dot(*this);             }
    float           Length          () const                     { return sqrtf(SquaredLength()); }

    float           Cross           (const xxVector2& v) const   { return x * v.y - y * v.x;      }

    static const xxVector2 ZERO;
    static const xxVector2 ONE;

    static const xxVector2 X;
    static const xxVector2 Y;
};

union xxCPPAPI xxVector3
{
    enum { N = 3 };
    struct { float x, y, z; };
    struct { float r, g, b; };
    float f[N];

    typedef xxVector3 type;

    xxVector3       operator -      () const                     { type t;        for (int i = 0; i < N; ++i) t.f[i] =   -f[i];              return t;       }
    xxVector3&      operator +=     (const xxVector3& v)         { type t(*this); for (int i = 0; i < N; ++i) t.f[i] += v.f[i]; (*this) = t; return (*this); }
    xxVector3&      operator -=     (const xxVector3& v)         { type t(*this); for (int i = 0; i < N; ++i) t.f[i] -= v.f[i]; (*this) = t; return (*this); }
    xxVector3&      operator *=     (const xxVector3& v)         { type t(*this); for (int i = 0; i < N; ++i) t.f[i] *= v.f[i]; (*this) = t; return (*this); }
    xxVector3&      operator /=     (const xxVector3& v)         { type t(*this); for (int i = 0; i < N; ++i) t.f[i] /= v.f[i]; (*this) = t; return (*this); }
    xxVector3&      operator *=     (float s)                    { type t(*this); for (int i = 0; i < N; ++i) t.f[i] *= s;      (*this) = t; return (*this); }
    xxVector3&      operator /=     (float s)                    { (*this) *= (1.0f / s);                                                    return (*this); }

    xxVector3       operator +      (const xxVector3& v) const   { return xxVector3(*this) += v; }
    xxVector3       operator -      (const xxVector3& v) const   { return xxVector3(*this) -= v; }
    xxVector3       operator *      (const xxVector3& v) const   { return xxVector3(*this) *= v; }
    xxVector3       operator /      (const xxVector3& v) const   { return xxVector3(*this) /= v; }
    xxVector3       operator *      (float s) const              { return xxVector3(*this) *= s; }
    xxVector3       operator /      (float s) const              { return xxVector3(*this) /= s; }

    float           Dot             (const xxVector3& v) const   { return x * v.x + y * v.y + z * v.z; }
    float           SquaredLength   () const                     { return Dot(*this);                  }
    float           Length          () const                     { return sqrtf(SquaredLength());      }

    xxVector3       Cross           (const xxVector3& v) const   { return xxVector3{ y, z, x } * xxVector3{ v.z, v.x, v.y } -
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
    enum { N = 4 };
    struct { float x, y, z, w; };
    struct { float r, g, b, a; };
    float f[N];
#if defined(_M_IX86) || defined(_M_AMD64) || defined(__i386__) || defined(__amd64__)
    __m128 v;
#endif

    typedef xxVector4 type;

    xxVector4       operator -      () const                     { type t;        for (int i = 0; i < N; ++i) t.f[i] =   -f[i];              return t;       }
    xxVector4&      operator +=     (const xxVector4& v)         { type t(*this); for (int i = 0; i < N; ++i) t.f[i] += v.f[i]; (*this) = t; return (*this); }
    xxVector4&      operator -=     (const xxVector4& v)         { type t(*this); for (int i = 0; i < N; ++i) t.f[i] -= v.f[i]; (*this) = t; return (*this); }
    xxVector4&      operator *=     (const xxVector4& v)         { type t(*this); for (int i = 0; i < N; ++i) t.f[i] *= v.f[i]; (*this) = t; return (*this); }
    xxVector4&      operator /=     (const xxVector4& v)         { type t(*this); for (int i = 0; i < N; ++i) t.f[i] /= v.f[i]; (*this) = t; return (*this); }
    xxVector4&      operator *=     (float s)                    { type t(*this); for (int i = 0; i < N; ++i) t.f[i] *= s;      (*this) = t; return (*this); }
    xxVector4&      operator /=     (float s)                    { (*this) *= (1.0f / s);                                                    return (*this); }

    xxVector4       operator +      (const xxVector4& v) const   { return xxVector4(*this) += v; }
    xxVector4       operator -      (const xxVector4& v) const   { return xxVector4(*this) -= v; }
    xxVector4       operator *      (const xxVector4& v) const   { return xxVector4(*this) *= v; }
    xxVector4       operator /      (const xxVector4& v) const   { return xxVector4(*this) /= v; }
    xxVector4       operator *      (float s) const              { return xxVector4(*this) *= s; }
    xxVector4       operator /      (float s) const              { return xxVector4(*this) /= s; }

    float           Dot             (const xxVector4& v) const   { return x * v.x + y * v.y + z * v.z + w * v.w; }
    float           SquaredLength   () const                     { return Dot(*this);                            }
    float           Length          () const                     { return sqrtf(SquaredLength());                }

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

typedef union xxMatrix2x2 xxMatrix2;
typedef union xxMatrix3x3 xxMatrix3;
typedef union xxMatrix4x4 xxMatrix4;

union xxCPPAPI xxMatrix2x2
{
    enum { M = 2, N = 2 };
    xxVector2 _[N];
    float f[M * N];
    struct
    {
        float m11, m12;
        float m21, m22;
    };

    typedef xxMatrix2x2 type;

    xxMatrix2x2     operator -      () const                     { type t;        for (int i = 0; i < M * N; ++i) t.f[i] =   -f[i];              return t;       }
    xxMatrix2x2&    operator +=     (const xxMatrix2x2& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] += m.f[i]; (*this) = t; return (*this); }
    xxMatrix2x2&    operator -=     (const xxMatrix2x2& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] -= m.f[i]; (*this) = t; return (*this); }
    xxMatrix2x2&    operator *=     (const xxMatrix2x2& m)       { type t(*this); t = { t * m._[0], t * m._[1] };                   (*this) = t; return (*this); }
    xxMatrix2x2&    operator *=     (float s)                    { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] *= s;      (*this) = t; return (*this); }
    xxMatrix2x2&    operator /=     (float s)                    { (*this) *= (1.0f / s);                                                        return (*this); }

    xxMatrix2x2     operator +      (const xxMatrix2x2& m) const { return xxMatrix2x2(*this) += m; }
    xxMatrix2x2     operator -      (const xxMatrix2x2& m) const { return xxMatrix2x2(*this) -= m; }
    xxMatrix2x2     operator *      (const xxMatrix2x2& m) const { return xxMatrix2x2(*this) *= m; }
    xxMatrix2x2     operator *      (float s) const              { return xxMatrix2x2(*this) *= s; }
    xxMatrix2x2     operator /      (float s) const              { return xxMatrix2x2(*this) /= s; }

    xxVector2       operator *      (const xxVector2& v) const   { return _[0] * v.x + _[1] * v.y; }

    float           Determinant     () const;
    xxMatrix2x2     Inverse         () const;
    xxMatrix2x2     Transpose       () const;

    static const xxMatrix2x2 IDENTITY;
};

union xxCPPAPI xxMatrix2x3
{
    enum { M = 2, N = 3 };
    xxVector2 _[N];
    float f[M * N];
    struct
    {
        float m11, m12;
        float m21, m22;
        float m31, m32;
    };

    typedef xxMatrix2x3 type;

    xxMatrix2x3     operator -      () const                     { type t;        for (int i = 0; i < M * N; ++i) t.f[i] =   -f[i];              return t;       }
    xxMatrix2x3&    operator +=     (const xxMatrix2x3& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] += m.f[i]; (*this) = t; return (*this); }
    xxMatrix2x3&    operator -=     (const xxMatrix2x3& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] -= m.f[i]; (*this) = t; return (*this); }
    xxMatrix2x3&    operator *=     (float s)                    { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] *= s;      (*this) = t; return (*this); }
    xxMatrix2x3&    operator /=     (float s)                    { (*this) *= (1.0f / s);                                                        return (*this); }

    xxMatrix2x3     operator +      (const xxMatrix2x3& m) const { return xxMatrix2x3(*this) += m; }
    xxMatrix2x3     operator -      (const xxMatrix2x3& m) const { return xxMatrix2x3(*this) -= m; }
    xxMatrix2x3     operator *      (float s) const              { return xxMatrix2x3(*this) *= s; }
    xxMatrix2x3     operator /      (float s) const              { return xxMatrix2x3(*this) /= s; }

    static const xxMatrix2x3 IDENTITY;
};

union xxCPPAPI xxMatrix2x4
{
    enum { M = 2, N = 4 };
    xxVector2 _[N];
    float f[M * N];
    struct
    {
        float m11, m12;
        float m21, m22;
        float m31, m32;
        float m41, m42;
    };

    typedef xxMatrix2x4 type;

    xxMatrix2x4     operator -      () const                     { type t;        for (int i = 0; i < M * N; ++i) t.f[i] =   -f[i];              return t;       }
    xxMatrix2x4&    operator +=     (const xxMatrix2x4& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] += m.f[i]; (*this) = t; return (*this); }
    xxMatrix2x4&    operator -=     (const xxMatrix2x4& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] -= m.f[i]; (*this) = t; return (*this); }
    xxMatrix2x4&    operator *=     (float s)                    { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] *= s;      (*this) = t; return (*this); }
    xxMatrix2x4&    operator /=     (float s)                    { (*this) *= (1.0f / s);                                                        return (*this); }

    xxMatrix2x4     operator +      (const xxMatrix2x4& m) const { return xxMatrix2x4(*this) += m; }
    xxMatrix2x4     operator -      (const xxMatrix2x4& m) const { return xxMatrix2x4(*this) -= m; }
    xxMatrix2x4     operator *      (float s) const              { return xxMatrix2x4(*this) *= s; }
    xxMatrix2x4     operator /      (float s) const              { return xxMatrix2x4(*this) /= s; }

    static const xxMatrix2x4 IDENTITY;
};

union xxCPPAPI xxMatrix3x2
{
    enum { M = 3, N = 2 };
    xxVector3 _[N];
    float f[M * N];
    struct
    {
        float m11, m12, m13;
        float m21, m22, m23;
    };

    typedef xxMatrix3x2 type;

    xxMatrix3x2     operator -      () const                     { type t;        for (int i = 0; i < M * N; ++i) t.f[i] =   -f[i];              return t;       }
    xxMatrix3x2&    operator +=     (const xxMatrix3x2& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] += m.f[i]; (*this) = t; return (*this); }
    xxMatrix3x2&    operator -=     (const xxMatrix3x2& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] -= m.f[i]; (*this) = t; return (*this); }
    xxMatrix3x2&    operator *=     (float s)                    { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] *= s;      (*this) = t; return (*this); }
    xxMatrix3x2&    operator /=     (float s)                    { (*this) *= (1.0f / s);                                                        return (*this); }

    xxMatrix3x2     operator +      (const xxMatrix3x2& m) const { return xxMatrix3x2(*this) += m; }
    xxMatrix3x2     operator -      (const xxMatrix3x2& m) const { return xxMatrix3x2(*this) -= m; }
    xxMatrix3x2     operator *      (float s) const              { return xxMatrix3x2(*this) *= s; }
    xxMatrix3x2     operator /      (float s) const              { return xxMatrix3x2(*this) /= s; }

    static const xxMatrix3x2 IDENTITY;
};

union xxCPPAPI xxMatrix3x3
{
    enum { M = 3, N = 3 };
    xxVector3 _[N];
    float f[M * N];
    struct
    {
        float m11, m12, m13;
        float m21, m22, m23;
        float m31, m32, m33;
    };

    typedef xxMatrix3x3 type;

    xxMatrix3x3     operator -      () const                     { type t;        for (int i = 0; i < M * N; ++i) t.f[i] =   -f[i];              return t;       }
    xxMatrix3x3&    operator +=     (const xxMatrix3x3& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] += m.f[i]; (*this) = t; return (*this); }
    xxMatrix3x3&    operator -=     (const xxMatrix3x3& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] -= m.f[i]; (*this) = t; return (*this); }
    xxMatrix3x3&    operator *=     (const xxMatrix3x3& m)       { type t(*this); t = { t * m._[0], t * m._[1], t * m._[2] };       (*this) = t; return (*this); }
    xxMatrix3x3&    operator *=     (float s)                    { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] *= s;      (*this) = t; return (*this); }
    xxMatrix3x3&    operator /=     (float s)                    { (*this) *= (1.0f / s);                                                        return (*this); }

    xxMatrix3x3     operator +      (const xxMatrix3x3& m) const { return xxMatrix3x3(*this) += m; }
    xxMatrix3x3     operator -      (const xxMatrix3x3& m) const { return xxMatrix3x3(*this) -= m; }
    xxMatrix3x3     operator *      (const xxMatrix3x3& m) const { return xxMatrix3x3(*this) *= m; }
    xxMatrix3x3     operator *      (float s) const              { return xxMatrix3x3(*this) *= s; }
    xxMatrix3x3     operator /      (float s) const              { return xxMatrix3x3(*this) /= s; }

    xxVector3       operator *      (const xxVector3& v) const   { return _[0] * v.x + _[1] * v.y + _[2] * v.z; }

    float           Determinant     () const;
    xxMatrix3x3     Inverse         () const;
    xxMatrix3x3     Transpose       () const;

    static const xxMatrix3x3 IDENTITY;
};

union xxCPPAPI xxMatrix3x4
{
    enum { M = 3, N = 4 };
    xxVector3 _[N];
    float f[M * N];
    struct
    {
        float m11, m12, m13;
        float m21, m22, m23;
        float m31, m32, m33;
        float m41, m42, m43;
    };

    typedef xxMatrix3x4 type;

    xxMatrix3x4     operator -      () const                     { type t;        for (int i = 0; i < M * N; ++i) t.f[i] =   -f[i];              return t;       }
    xxMatrix3x4&    operator +=     (const xxMatrix3x4& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] += m.f[i]; (*this) = t; return (*this); }
    xxMatrix3x4&    operator -=     (const xxMatrix3x4& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] -= m.f[i]; (*this) = t; return (*this); }
    xxMatrix3x4&    operator *=     (float s)                    { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] *= s;      (*this) = t; return (*this); }
    xxMatrix3x4&    operator /=     (float s)                    { (*this) *= (1.0f / s);                                                        return (*this); }

    xxMatrix3x4     operator +      (const xxMatrix3x4& m) const { return xxMatrix3x4(*this) += m; }
    xxMatrix3x4     operator -      (const xxMatrix3x4& m) const { return xxMatrix3x4(*this) -= m; }
    xxMatrix3x4     operator *      (float s) const              { return xxMatrix3x4(*this) *= s; }
    xxMatrix3x4     operator /      (float s) const              { return xxMatrix3x4(*this) /= s; }

    static const xxMatrix3x4 IDENTITY;
};

union xxCPPAPI xxMatrix4x2
{
    enum { M = 4, N = 2 };
    xxVector4 _[N];
    float f[M * N];
    struct
    {
        float m11, m12, m13, m14;
        float m21, m22, m23, m24;
    };

    typedef xxMatrix4x2 type;

    xxMatrix4x2     operator -      () const                     { type t;        for (int i = 0; i < M * N; ++i) t.f[i] =   -f[i];              return t;       }
    xxMatrix4x2&    operator +=     (const xxMatrix4x2& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] += m.f[i]; (*this) = t; return (*this); }
    xxMatrix4x2&    operator -=     (const xxMatrix4x2& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] -= m.f[i]; (*this) = t; return (*this); }
    xxMatrix4x2&    operator *=     (float s)                    { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] *= s;      (*this) = t; return (*this); }
    xxMatrix4x2&    operator /=     (float s)                    { (*this) *= (1.0f / s);                                                        return (*this); }

    xxMatrix4x2     operator +      (const xxMatrix4x2& m) const { return xxMatrix4x2(*this) += m; }
    xxMatrix4x2     operator -      (const xxMatrix4x2& m) const { return xxMatrix4x2(*this) -= m; }
    xxMatrix4x2     operator *      (float s) const              { return xxMatrix4x2(*this) *= s; }
    xxMatrix4x2     operator /      (float s) const              { return xxMatrix4x2(*this) /= s; }

    static const xxMatrix4x2 IDENTITY;
};

union xxCPPAPI xxMatrix4x3
{
    enum { M = 4, N = 3 };
    xxVector4 _[N];
    float f[M * N];
    struct
    {
        float m11, m12, m13, m14;
        float m21, m22, m23, m24;
        float m31, m32, m33, m34;
    };

    typedef xxMatrix4x3 type;

    xxMatrix4x3     operator -      () const                     { type t;        for (int i = 0; i < M * N; ++i) t.f[i] =   -f[i];              return t;       }
    xxMatrix4x3&    operator +=     (const xxMatrix4x3& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] += m.f[i]; (*this) = t; return (*this); }
    xxMatrix4x3&    operator -=     (const xxMatrix4x3& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] -= m.f[i]; (*this) = t; return (*this); }
    xxMatrix4x3&    operator *=     (float s)                    { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] *= s;      (*this) = t; return (*this); }
    xxMatrix4x3&    operator /=     (float s)                    { (*this) *= (1.0f / s);                                                        return (*this); }

    xxMatrix4x3     operator +      (const xxMatrix4x3& m) const { return xxMatrix4x3(*this) += m; }
    xxMatrix4x3     operator -      (const xxMatrix4x3& m) const { return xxMatrix4x3(*this) -= m; }
    xxMatrix4x3     operator *      (float s) const              { return xxMatrix4x3(*this) *= s; }
    xxMatrix4x3     operator /      (float s) const              { return xxMatrix4x3(*this) /= s; }

    static const xxMatrix4x3 IDENTITY;
};

union xxCPPAPI xxMatrix4x4
{
    enum { M = 4, N = 4 };
    xxVector4 _[N];
    float f[M * N];
    struct
    {
        float m11, m12, m13, m14;
        float m21, m22, m23, m24;
        float m31, m32, m33, m34;
        float m41, m42, m43, m44;
    };

    typedef xxMatrix4x4 type;

    xxMatrix4x4     operator -      () const                     { type t;        for (int i = 0; i < M * N; ++i) t.f[i] =   -f[i];                    return t;       }
    xxMatrix4x4&    operator +=     (const xxMatrix4x4& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] += m.f[i];       (*this) = t; return (*this); }
    xxMatrix4x4&    operator -=     (const xxMatrix4x4& m)       { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] -= m.f[i];       (*this) = t; return (*this); }
    xxMatrix4x4&    operator *=     (const xxMatrix4x4& m)       { type t(*this); t = { t * m._[0], t * m._[1], t * m._[2], t * m._[3] }; (*this) = t; return (*this); }
    xxMatrix4x4&    operator *=     (float s)                    { type t(*this); for (int i = 0; i < M * N; ++i) t.f[i] *= s;            (*this) = t; return (*this); }
    xxMatrix4x4&    operator /=     (float s)                    { (*this) *= (1.0f / s);                                                              return (*this); }

    xxMatrix4x4     operator +      (const xxMatrix4x4& m) const { return xxMatrix4x4(*this) += m; }
    xxMatrix4x4     operator -      (const xxMatrix4x4& m) const { return xxMatrix4x4(*this) -= m; }
    xxMatrix4x4     operator *      (const xxMatrix4x4& m) const { return xxMatrix4x4(*this) *= m; }
    xxMatrix4x4     operator *      (float s) const              { return xxMatrix4x4(*this) *= s; }
    xxMatrix4x4     operator /      (float s) const              { return xxMatrix4x4(*this) /= s; }

    xxVector4       operator *      (const xxVector4& v) const   { return _[0] * v.x + _[1] * v.y + _[2] * v.z + _[3] * v.w; }

    float           Determinant     () const;
    xxMatrix4x4     Inverse         () const;
    xxMatrix4x4     Transpose       () const;
    float           FastDeterminant () const;
    void            FastDecompose   (xxMatrix3& rotate, xxVector3& translate, float& scale) const;
    void            MultiplyArray   (size_t count, const xxVector4* input, int inputStride, xxVector4* output, int outputStride) const;
    void            MultiplyArray   (size_t count, const xxMatrix4* input, int inputStride, xxMatrix4* output, int outputStride) const;

    static const xxMatrix4x4 IDENTITY;
};

#if defined(_MSC_VER)
#   pragma optimize("", on)
#endif
