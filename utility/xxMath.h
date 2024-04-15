//==============================================================================
// xxGraphic : Math Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"
#include "xxVector.h"

struct xxPlusAPI xxVector2
{
    enum { N = 2 };
    union
    {
        float f[N];
        struct { float x, y; };
        struct { float u, v; };
    };

    xxVector2       operator -      () const                     { xxVector2 t;     for (size_t i = 0; i < N; ++i) t.f[i] =   -f[i]; return t;       }
    xxVector2&      operator +=     (xxVector2 const& v)         {                  for (size_t i = 0; i < N; ++i)   f[i] += v.f[i]; return (*this); }
    xxVector2&      operator -=     (xxVector2 const& v)         {                  for (size_t i = 0; i < N; ++i)   f[i] -= v.f[i]; return (*this); }
    xxVector2&      operator *=     (xxVector2 const& v)         {                  for (size_t i = 0; i < N; ++i)   f[i] *= v.f[i]; return (*this); }
    xxVector2&      operator /=     (xxVector2 const& v)         {                  for (size_t i = 0; i < N; ++i)   f[i] /= v.f[i]; return (*this); }
    xxVector2&      operator *=     (float s)                    {                  for (size_t i = 0; i < N; ++i)   f[i] *= s;      return (*this); }
    xxVector2&      operator /=     (float s)                    { float r = 1 / s; for (size_t i = 0; i < N; ++i)   f[i] *= r;      return (*this); }

    bool            operator ==     (xxVector2 const& v) const   { for (size_t i = 0; i < N; ++i) if (f[i] != v.f[i]) return false; return true;  }
    bool            operator !=     (xxVector2 const& v) const   { for (size_t i = 0; i < N; ++i) if (f[i] != v.f[i]) return true;  return false; }

    xxVector2       operator +      (xxVector2 const& v) const   { return xxVector2(*this) += v; }
    xxVector2       operator -      (xxVector2 const& v) const   { return xxVector2(*this) -= v; }
    xxVector2       operator *      (xxVector2 const& v) const   { return xxVector2(*this) *= v; }
    xxVector2       operator /      (xxVector2 const& v) const   { return xxVector2(*this) /= v; }
    xxVector2       operator *      (float s) const              { return xxVector2(*this) *= s; }
    xxVector2       operator /      (float s) const              { return xxVector2(*this) /= s; }

    float&          operator []     (size_t i)                   { return f[i]; }
    float           operator []     (size_t i) const             { return f[i]; }

    float const*    Array           () const                     { return f;                      }
    float*          Array           ()                           { return f;                      }
    float           Dot             (xxVector2 const& v) const   { return x * v.x + y * v.y;      }
    float           SquaredLength   () const                     { return Dot(*this);             }
    float           Length          () const                     { return sqrtf(SquaredLength()); }

    float           Cross           (xxVector2 const& v) const   { return x * v.y - y * v.x;      }

    static const xxVector2 ZERO;
    static const xxVector2 ONE;

    static const xxVector2 X;
    static const xxVector2 Y;
};

struct xxPlusAPI xxVector3
{
    enum { N = 3 };
    union
    {
        float f[N];
        struct { float x, y, z; };
        struct { float r, g, b; };
        xxVector2 xy;
        xxVector2 rg;
    };

    xxVector3       operator -      () const                     { xxVector3 t;     for (size_t i = 0; i < N; ++i) t.f[i] =   -f[i]; return t;       }
    xxVector3&      operator +=     (xxVector3 const& v)         {                  for (size_t i = 0; i < N; ++i)   f[i] += v.f[i]; return (*this); }
    xxVector3&      operator -=     (xxVector3 const& v)         {                  for (size_t i = 0; i < N; ++i)   f[i] -= v.f[i]; return (*this); }
    xxVector3&      operator *=     (xxVector3 const& v)         {                  for (size_t i = 0; i < N; ++i)   f[i] *= v.f[i]; return (*this); }
    xxVector3&      operator /=     (xxVector3 const& v)         {                  for (size_t i = 0; i < N; ++i)   f[i] /= v.f[i]; return (*this); }
    xxVector3&      operator *=     (float s)                    {                  for (size_t i = 0; i < N; ++i)   f[i] *= s;      return (*this); }
    xxVector3&      operator /=     (float s)                    { float r = 1 / s; for (size_t i = 0; i < N; ++i)   f[i] *= r;      return (*this); }

    bool            operator ==     (xxVector3 const& v) const   { for (size_t i = 0; i < N; ++i) if (f[i] != v.f[i]) return false; return true;  }
    bool            operator !=     (xxVector3 const& v) const   { for (size_t i = 0; i < N; ++i) if (f[i] != v.f[i]) return true;  return false; }

    xxVector3       operator +      (xxVector3 const& v) const   { return xxVector3(*this) += v; }
    xxVector3       operator -      (xxVector3 const& v) const   { return xxVector3(*this) -= v; }
    xxVector3       operator *      (xxVector3 const& v) const   { return xxVector3(*this) *= v; }
    xxVector3       operator /      (xxVector3 const& v) const   { return xxVector3(*this) /= v; }
    xxVector3       operator *      (float s) const              { return xxVector3(*this) *= s; }
    xxVector3       operator /      (float s) const              { return xxVector3(*this) /= s; }

    float&          operator []     (size_t i)                   { return f[i]; }
    float           operator []     (size_t i) const             { return f[i]; }

    float const*    Array           () const                     { return f;                           }
    float*          Array           ()                           { return f;                           }
    float           Dot             (xxVector3 const& v) const   { return x * v.x + y * v.y + z * v.z; }
    float           SquaredLength   () const                     { return Dot(*this);                  }
    float           Length          () const                     { return sqrtf(SquaredLength());      }

    xxVector3       Cross           (xxVector3 const& v) const   { return xxVector3{ y, z, x } * xxVector3{ v.z, v.x, v.y } -
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

struct xxPlusAPI xxVector4
{
    enum { N = 4 };
    union
    {
        v4sf v;
        float f[N];
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
        xxVector2 xy;
        xxVector2 rg;
        xxVector3 xyz;
        xxVector3 rgb;
    };

    xxVector4       operator -      () const                     { xxVector4 t;     for (size_t i = 0; i < N; ++i) t.f[i] =   -f[i]; return t;       }
    xxVector4&      operator +=     (xxVector4 const& v)         {                  for (size_t i = 0; i < N; ++i)   f[i] += v.f[i]; return (*this); }
    xxVector4&      operator -=     (xxVector4 const& v)         {                  for (size_t i = 0; i < N; ++i)   f[i] -= v.f[i]; return (*this); }
    xxVector4&      operator *=     (xxVector4 const& v)         {                  for (size_t i = 0; i < N; ++i)   f[i] *= v.f[i]; return (*this); }
    xxVector4&      operator /=     (xxVector4 const& v)         {                  for (size_t i = 0; i < N; ++i)   f[i] /= v.f[i]; return (*this); }
    xxVector4&      operator *=     (float s)                    {                  for (size_t i = 0; i < N; ++i)   f[i] *= s;      return (*this); }
    xxVector4&      operator /=     (float s)                    { float r = 1 / s; for (size_t i = 0; i < N; ++i)   f[i] *= r;      return (*this); }

    bool            operator ==     (xxVector4 const& v) const   { for (size_t i = 0; i < N; ++i) if (f[i] != v.f[i]) return false; return true;  }
    bool            operator !=     (xxVector4 const& v) const   { for (size_t i = 0; i < N; ++i) if (f[i] != v.f[i]) return true;  return false; }

    xxVector4       operator +      (xxVector4 const& v) const   { return xxVector4(*this) += v; }
    xxVector4       operator -      (xxVector4 const& v) const   { return xxVector4(*this) -= v; }
    xxVector4       operator *      (xxVector4 const& v) const   { return xxVector4(*this) *= v; }
    xxVector4       operator /      (xxVector4 const& v) const   { return xxVector4(*this) /= v; }
    xxVector4       operator *      (float s) const              { return xxVector4(*this) *= s; }
    xxVector4       operator /      (float s) const              { return xxVector4(*this) /= s; }

    float&          operator []     (size_t i)                   { return f[i]; }
    float           operator []     (size_t i) const             { return f[i]; }

    float const*    Array           () const                     { return f;                                     }
    float*          Array           ()                           { return f;                                     }
    float           Dot             (xxVector4 const& v) const   { return x * v.x + y * v.y + z * v.z + w * v.w; }
    float           SquaredLength   () const                     { return Dot(*this);                            }
    float           Length          () const                     { return sqrtf(SquaredLength());                }

    static xxVector4    FromInteger (uint32_t v)                 { xxVector4 t;    for (size_t i = 0; i < N; ++i) t.f[i] = (v >> (i * 8) & 0xFF) / 255.0f; return t; }
           uint32_t     ToInteger   () const                     { uint32_t t = 0; for (size_t i = 0; i < N; ++i) t |= uint32_t(f[i] * 255.0f) << (i * 8); return t; }

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

typedef struct xxMatrix2x2 xxMatrix2;
typedef struct xxMatrix3x3 xxMatrix3;
typedef struct xxMatrix4x4 xxMatrix4;

struct xxPlusAPI xxMatrix2x2
{
    enum { M = 2, N = 2 };
    union
    {
        xxVector2 v[N];
        float f[M * N];
        struct
        {
            float m11, m12;
            float m21, m22;
        };
    };

    xxMatrix2x2     operator -      () const                     { xxMatrix2x2 t;   for (size_t i = 0; i < N; ++i) t.v[i]  =  -v[i]; return t;       }
    xxMatrix2x2&    operator +=     (xxMatrix2x2 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] += m.v[i]; return (*this); }
    xxMatrix2x2&    operator -=     (xxMatrix2x2 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] -= m.v[i]; return (*this); }
    xxMatrix2x2&    operator *=     (xxMatrix2x2 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i]  = m*v[i]; return (*this); }
    xxMatrix2x2&    operator *=     (float s)                    {                  for (size_t i = 0; i < N; ++i)   v[i] *= s;      return (*this); }
    xxMatrix2x2&    operator /=     (float s)                    { float r = 1 / s; for (size_t i = 0; i < N; ++i)   v[i] *= r;      return (*this); }

    bool            operator ==     (xxMatrix2x2 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return false; return true;  }
    bool            operator !=     (xxMatrix2x2 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return true;  return false; }

    xxMatrix2x2     operator +      (xxMatrix2x2 const& m) const { return xxMatrix2x2(*this) += m; }
    xxMatrix2x2     operator -      (xxMatrix2x2 const& m) const { return xxMatrix2x2(*this) -= m; }
    xxMatrix2x2     operator *      (xxMatrix2x2 const& m) const { return xxMatrix2x2(*this) *= m; }
    xxMatrix2x2     operator *      (float s) const              { return xxMatrix2x2(*this) *= s; }
    xxMatrix2x2     operator /      (float s) const              { return xxMatrix2x2(*this) /= s; }

    xxVector2       operator *      (xxVector2 const& c) const   { return v[0] * c.x + v[1] * c.y; }

    xxVector2&      operator []     (size_t i)                   { return v[i]; }
    xxVector2       operator []     (size_t i) const             { return v[i]; }

    float           Determinant     () const;
    xxMatrix2x2     Inverse         () const;
    xxMatrix2x2     Transpose       () const;

    static const xxMatrix2x2 IDENTITY;
};

struct xxPlusAPI xxMatrix2x3
{
    enum { M = 2, N = 3 };
    union
    {
        xxVector2 v[N];
        float f[M * N];
        struct
        {
            float m11, m12;
            float m21, m22;
            float m31, m32;
        };
    };

    xxMatrix2x3     operator -      () const                     { xxMatrix2x3 t;   for (size_t i = 0; i < N; ++i) t.v[i]  =  -v[i]; return t;       }
    xxMatrix2x3&    operator +=     (xxMatrix2x3 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] += m.v[i]; return (*this); }
    xxMatrix2x3&    operator -=     (xxMatrix2x3 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] -= m.v[i]; return (*this); }
    xxMatrix2x3&    operator *=     (float s)                    {                  for (size_t i = 0; i < N; ++i)   v[i] *= s;      return (*this); }
    xxMatrix2x3&    operator /=     (float s)                    { float r = 1 / s; for (size_t i = 0; i < N; ++i)   v[i] *= r;      return (*this); }

    bool            operator ==     (xxMatrix2x3 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return false; return true;  }
    bool            operator !=     (xxMatrix2x3 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return true;  return false; }

    xxMatrix2x3     operator +      (xxMatrix2x3 const& m) const { return xxMatrix2x3(*this) += m; }
    xxMatrix2x3     operator -      (xxMatrix2x3 const& m) const { return xxMatrix2x3(*this) -= m; }
    xxMatrix2x3     operator *      (float s) const              { return xxMatrix2x3(*this) *= s; }
    xxMatrix2x3     operator /      (float s) const              { return xxMatrix2x3(*this) /= s; }

    xxVector2&      operator []     (size_t i)                   { return v[i]; }
    xxVector2       operator []     (size_t i) const             { return v[i]; }

    static const xxMatrix2x3 IDENTITY;
};

struct xxPlusAPI xxMatrix2x4
{
    enum { M = 2, N = 4 };
    union
    {
        xxVector2 v[N];
        float f[M * N];
        struct
        {
            float m11, m12;
            float m21, m22;
            float m31, m32;
            float m41, m42;
        };
    };

    xxMatrix2x4     operator -      () const                     { xxMatrix2x4 t;   for (size_t i = 0; i < N; ++i) t.v[i]  =  -v[i]; return t;       }
    xxMatrix2x4&    operator +=     (xxMatrix2x4 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] += m.v[i]; return (*this); }
    xxMatrix2x4&    operator -=     (xxMatrix2x4 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] -= m.v[i]; return (*this); }
    xxMatrix2x4&    operator *=     (float s)                    {                  for (size_t i = 0; i < N; ++i)   v[i] *= s;      return (*this); }
    xxMatrix2x4&    operator /=     (float s)                    { float r = 1 / s; for (size_t i = 0; i < N; ++i)   v[i] *= r;      return (*this); }

    bool            operator ==     (xxMatrix2x4 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return false; return true;  }
    bool            operator !=     (xxMatrix2x4 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return true;  return false; }

    xxMatrix2x4     operator +      (xxMatrix2x4 const& m) const { return xxMatrix2x4(*this) += m; }
    xxMatrix2x4     operator -      (xxMatrix2x4 const& m) const { return xxMatrix2x4(*this) -= m; }
    xxMatrix2x4     operator *      (float s) const              { return xxMatrix2x4(*this) *= s; }
    xxMatrix2x4     operator /      (float s) const              { return xxMatrix2x4(*this) /= s; }

    xxVector2&      operator []     (size_t i)                   { return v[i]; }
    xxVector2       operator []     (size_t i) const             { return v[i]; }

    static const xxMatrix2x4 IDENTITY;
};

struct xxPlusAPI xxMatrix3x2
{
    enum { M = 3, N = 2 };
    union
    {
        xxVector3 v[N];
        float f[M * N];
        struct
        {
            float m11, m12, m13;
            float m21, m22, m23;
        };
    };

    xxMatrix3x2     operator -      () const                     { xxMatrix3x2 t;   for (size_t i = 0; i < N; ++i) t.v[i]  =  -v[i]; return t;       }
    xxMatrix3x2&    operator +=     (xxMatrix3x2 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] += m.v[i]; return (*this); }
    xxMatrix3x2&    operator -=     (xxMatrix3x2 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] -= m.v[i]; return (*this); }
    xxMatrix3x2&    operator *=     (float s)                    {                  for (size_t i = 0; i < N; ++i)   v[i] *= s;      return (*this); }
    xxMatrix3x2&    operator /=     (float s)                    { float r = 1 / s; for (size_t i = 0; i < N; ++i)   v[i] *= r;      return (*this); }

    bool            operator ==     (xxMatrix3x2 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return false; return true;  }
    bool            operator !=     (xxMatrix3x2 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return true;  return false; }

    xxMatrix3x2     operator +      (xxMatrix3x2 const& m) const { return xxMatrix3x2(*this) += m; }
    xxMatrix3x2     operator -      (xxMatrix3x2 const& m) const { return xxMatrix3x2(*this) -= m; }
    xxMatrix3x2     operator *      (float s) const              { return xxMatrix3x2(*this) *= s; }
    xxMatrix3x2     operator /      (float s) const              { return xxMatrix3x2(*this) /= s; }

    xxVector3&      operator []     (size_t i)                   { return v[i]; }
    xxVector3       operator []     (size_t i) const             { return v[i]; }

    static const xxMatrix3x2 IDENTITY;
};

struct xxPlusAPI xxMatrix3x3
{
    enum { M = 3, N = 3 };
    union
    {
        xxVector3 v[N];
        float f[M * N];
        struct
        {
            float m11, m12, m13;
            float m21, m22, m23;
            float m31, m32, m33;
        };
    };

    xxMatrix3x3     operator -      () const                     { xxMatrix3x3 t;   for (size_t i = 0; i < N; ++i) t.v[i]  =  -v[i]; return t;       }
    xxMatrix3x3&    operator +=     (xxMatrix3x3 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] += m.v[i]; return (*this); }
    xxMatrix3x3&    operator -=     (xxMatrix3x3 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] -= m.v[i]; return (*this); }
    xxMatrix3x3&    operator *=     (xxMatrix3x3 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i]  = m*v[i]; return (*this); }
    xxMatrix3x3&    operator *=     (float s)                    {                  for (size_t i = 0; i < N; ++i)   v[i] *= s;      return (*this); }
    xxMatrix3x3&    operator /=     (float s)                    { float r = 1 / s; for (size_t i = 0; i < N; ++i)   v[i] *= r;      return (*this); }

    bool            operator ==     (xxMatrix3x3 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return false; return true;  }
    bool            operator !=     (xxMatrix3x3 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return true;  return false; }

    xxMatrix3x3     operator +      (xxMatrix3x3 const& m) const { return xxMatrix3x3(*this) += m; }
    xxMatrix3x3     operator -      (xxMatrix3x3 const& m) const { return xxMatrix3x3(*this) -= m; }
    xxMatrix3x3     operator *      (xxMatrix3x3 const& m) const { return xxMatrix3x3(*this) *= m; }
    xxMatrix3x3     operator *      (float s) const              { return xxMatrix3x3(*this) *= s; }
    xxMatrix3x3     operator /      (float s) const              { return xxMatrix3x3(*this) /= s; }

    xxVector3       operator *      (xxVector3 const& c) const   { return v[0] * c.x + v[1] * c.y + v[2] * c.z; }

    xxVector3&      operator []     (size_t i)                   { return v[i]; }
    xxVector3       operator []     (size_t i) const             { return v[i]; }

    float           Determinant     () const;
    xxMatrix3x3     Inverse         () const;
    xxMatrix3x3     Transpose       () const;

    static const xxMatrix3x3 IDENTITY;
};

struct xxPlusAPI xxMatrix3x4
{
    enum { M = 3, N = 4 };
    union
    {
        xxVector3 v[N];
        float f[M * N];
        struct
        {
            float m11, m12, m13;
            float m21, m22, m23;
            float m31, m32, m33;
            float m41, m42, m43;
        };
    };

    xxMatrix3x4     operator -      () const                     { xxMatrix3x4 t;   for (size_t i = 0; i < N; ++i) t.v[i]  =  -v[i]; return t;       }
    xxMatrix3x4&    operator +=     (xxMatrix3x4 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] += m.v[i]; return (*this); }
    xxMatrix3x4&    operator -=     (xxMatrix3x4 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] -= m.v[i]; return (*this); }
    xxMatrix3x4&    operator *=     (float s)                    {                  for (size_t i = 0; i < N; ++i)   v[i] *= s;      return (*this); }
    xxMatrix3x4&    operator /=     (float s)                    { float r = 1 / s; for (size_t i = 0; i < N; ++i)   v[i] *= r;      return (*this); }

    bool            operator ==     (xxMatrix3x4 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return false; return true;  }
    bool            operator !=     (xxMatrix3x4 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return true;  return false; }

    xxMatrix3x4     operator +      (xxMatrix3x4 const& m) const { return xxMatrix3x4(*this) += m; }
    xxMatrix3x4     operator -      (xxMatrix3x4 const& m) const { return xxMatrix3x4(*this) -= m; }
    xxMatrix3x4     operator *      (float s) const              { return xxMatrix3x4(*this) *= s; }
    xxMatrix3x4     operator /      (float s) const              { return xxMatrix3x4(*this) /= s; }

    xxVector3&      operator []     (size_t i)                   { return v[i]; }
    xxVector3       operator []     (size_t i) const             { return v[i]; }

    static const xxMatrix3x4 IDENTITY;
};

struct xxPlusAPI xxMatrix4x2
{
    enum { M = 4, N = 2 };
    union
    {
        xxVector4 v[N];
        float f[M * N];
        struct
        {
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
        };
    };

    xxMatrix4x2     operator -      () const                     { xxMatrix4x2 t;   for (size_t i = 0; i < N; ++i) t.v[i]  =  -v[i]; return t;       }
    xxMatrix4x2&    operator +=     (xxMatrix4x2 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] += m.v[i]; return (*this); }
    xxMatrix4x2&    operator -=     (xxMatrix4x2 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] -= m.v[i]; return (*this); }
    xxMatrix4x2&    operator *=     (float s)                    {                  for (size_t i = 0; i < N; ++i)   v[i] *= s;      return (*this); }
    xxMatrix4x2&    operator /=     (float s)                    { float r = 1 / s; for (size_t i = 0; i < N; ++i)   v[i] *= r;      return (*this); }

    bool            operator ==     (xxMatrix4x2 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return false; return true;  }
    bool            operator !=     (xxMatrix4x2 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return true;  return false; }

    xxMatrix4x2     operator +      (xxMatrix4x2 const& m) const { return xxMatrix4x2(*this) += m; }
    xxMatrix4x2     operator -      (xxMatrix4x2 const& m) const { return xxMatrix4x2(*this) -= m; }
    xxMatrix4x2     operator *      (float s) const              { return xxMatrix4x2(*this) *= s; }
    xxMatrix4x2     operator /      (float s) const              { return xxMatrix4x2(*this) /= s; }

    xxVector4&      operator []     (size_t i)                   { return v[i]; }
    xxVector4       operator []     (size_t i) const             { return v[i]; }

    static const xxMatrix4x2 IDENTITY;
};

struct xxPlusAPI xxMatrix4x3
{
    enum { M = 4, N = 3 };
    union
    {
        xxVector4 v[N];
        float f[M * N];
        struct
        {
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
        };
    };

    xxMatrix4x3     operator -      () const                     { xxMatrix4x3 t;   for (size_t i = 0; i < N; ++i) t.v[i]  =  -v[i]; return t;       }
    xxMatrix4x3&    operator +=     (xxMatrix4x3 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] += m.v[i]; return (*this); }
    xxMatrix4x3&    operator -=     (xxMatrix4x3 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] -= m.v[i]; return (*this); }
    xxMatrix4x3&    operator *=     (float s)                    {                  for (size_t i = 0; i < N; ++i)   v[i] *= s;      return (*this); }
    xxMatrix4x3&    operator /=     (float s)                    { float r = 1 / s; for (size_t i = 0; i < N; ++i)   v[i] *= r;      return (*this); }

    bool            operator ==     (xxMatrix4x3 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return false; return true;  }
    bool            operator !=     (xxMatrix4x3 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return true;  return false; }

    xxMatrix4x3     operator +      (xxMatrix4x3 const& m) const { return xxMatrix4x3(*this) += m; }
    xxMatrix4x3     operator -      (xxMatrix4x3 const& m) const { return xxMatrix4x3(*this) -= m; }
    xxMatrix4x3     operator *      (float s) const              { return xxMatrix4x3(*this) *= s; }
    xxMatrix4x3     operator /      (float s) const              { return xxMatrix4x3(*this) /= s; }

    xxVector4&      operator []     (size_t i)                   { return v[i]; }
    xxVector4       operator []     (size_t i) const             { return v[i]; }

    static xxMatrix4x3  FromMatrix4 (xxMatrix4 const& m);
           xxMatrix4    ToMatrix4   () const;

    static const xxMatrix4x3 IDENTITY;
};

struct xxPlusAPI xxMatrix4x4
{
    enum { M = 4, N = 4 };
    union
    {
        xxVector4 v[N];
        float f[M * N];
        struct
        {
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;
        };
    };

    xxMatrix4x4     operator -      () const                     { xxMatrix4x4 t;   for (size_t i = 0; i < N; ++i) t.v[i]  =  -v[i]; return t;       }
    xxMatrix4x4&    operator +=     (xxMatrix4x4 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] += m.v[i]; return (*this); }
    xxMatrix4x4&    operator -=     (xxMatrix4x4 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i] -= m.v[i]; return (*this); }
    xxMatrix4x4&    operator *=     (xxMatrix4x4 const& m)       {                  for (size_t i = 0; i < N; ++i)   v[i]  = m*v[i]; return (*this); }
    xxMatrix4x4&    operator *=     (float s)                    {                  for (size_t i = 0; i < N; ++i)   v[i] *= s;      return (*this); }
    xxMatrix4x4&    operator /=     (float s)                    { float r = 1 / s; for (size_t i = 0; i < N; ++i)   v[i] *= r;      return (*this); }

    bool            operator ==     (xxMatrix4x4 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return false; return true;  }
    bool            operator !=     (xxMatrix4x4 const& m) const { for (size_t i = 0; i < N; ++i) if (v[i] != m.v[i]) return true;  return false; }

    xxMatrix4x4     operator +      (xxMatrix4x4 const& m) const { return xxMatrix4x4(*this) += m; }
    xxMatrix4x4     operator -      (xxMatrix4x4 const& m) const { return xxMatrix4x4(*this) -= m; }
    xxMatrix4x4     operator *      (xxMatrix4x4 const& m) const { return xxMatrix4x4(*this) *= m; }
    xxMatrix4x4     operator *      (float s) const              { return xxMatrix4x4(*this) *= s; }
    xxMatrix4x4     operator /      (float s) const              { return xxMatrix4x4(*this) /= s; }

    xxVector4       operator *      (xxVector4 const& c) const   { return v[0] * c.x + v[1] * c.y + v[2] * c.z + v[3] * c.w; }

    xxVector4&      operator []     (size_t i)                   { return v[i]; }
    xxVector4       operator []     (size_t i) const             { return v[i]; }

    float           Determinant     () const;
    xxMatrix4x4     Inverse         () const;
    xxMatrix4x4     Transpose       () const;
    float           FastDeterminant () const;
    void            FastDecompose   (xxMatrix3& rotate, xxVector3& translate, float& scale) const;
    void            MultiplyArray   (size_t count, xxVector4 const* input, int inputStride, xxVector4* output, int outputStride) const;
    void            MultiplyArray   (size_t count, xxMatrix4 const* input, int inputStride, xxMatrix4* output, int outputStride) const;
    void            MultiplyLink    (size_t count, xxMatrix4 const* input, int inputStride, xxMatrix4* output, int outputStride) const;

    static const xxMatrix4x4 IDENTITY;
};
