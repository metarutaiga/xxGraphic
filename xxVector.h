//==============================================================================
// xxGraphic : Vector Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

#if defined(__ARM_NEON__) || defined(__ARM_NEON) || defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
#   include <arm_neon.h>
#elif defined(__i386__) || defined(__amd64__) || defined(_M_IX86) || defined(_M_AMD64)
#   include <immintrin.h>
#endif

#if defined(__llvm__)
typedef float v4sf __attribute__((vector_size(16)));
#elif defined(__ARM_NEON__) || defined(__ARM_NEON) || defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
struct v4sf
{
    float32x4_t v;
    float   operator [] (int i) const { return v[i]; }
    v4sf    operator + (const v4sf& other) const { return v4sf{ vaddq_f32(v, other.v) }; }
    v4sf    operator - (const v4sf& other) const { return v4sf{ vsubq_f32(v, other.v) }; }
    v4sf    operator * (const v4sf& other) const { return v4sf{ vmulq_f32(v, other.v) }; }
    v4sf    operator / (const v4sf& other) const { return v4sf{ vdivq_f32(v, other.v) }; }
};
template<int x, int y, int z, int w> inline v4sf __builtin_shufflevector(const v4sf& a, const v4sf& b);
template<> inline v4sf __builtin_shufflevector<0, 0, 0, 0>(const v4sf& a, const v4sf& b) { return v4sf{ vdupq_lane_f32(vget_low_f32(a.v), 0) }; }
template<> inline v4sf __builtin_shufflevector<1, 1, 1, 1>(const v4sf& a, const v4sf& b) { return v4sf{ vdupq_lane_f32(vget_low_f32(a.v), 1) }; }
template<> inline v4sf __builtin_shufflevector<2, 2, 2, 2>(const v4sf& a, const v4sf& b) { return v4sf{ vdupq_lane_f32(vget_high_f32(a.v), 0) }; }
template<> inline v4sf __builtin_shufflevector<3, 3, 3, 3>(const v4sf& a, const v4sf& b) { return v4sf{ vdupq_lane_f32(vget_high_f32(a.v), 1) }; }
#define __builtin_shufflevector(a, b, c, d, e, f) __builtin_shufflevector<c, d, e, f>(a, b)
#elif defined(__i386__) || defined(__amd64__) || defined(_M_IX86) || defined(_M_AMD64)
struct v4sf
{
    __m128 v;
    float   operator [] (int i) const { return v[i]; }
    v4sf    operator + (const v4sf& other) const { return v4sf{ _mm_add_ps(v, other.v) }; }
    v4sf    operator - (const v4sf& other) const { return v4sf{ _mm_sub_ps(v, other.v) }; }
    v4sf    operator * (const v4sf& other) const { return v4sf{ _mm_mul_ps(v, other.v) }; }
    v4sf    operator / (const v4sf& other) const { return v4sf{ _mm_div_ps(v, other.v) }; }
};
template<int x, int y, int z, int w> inline v4sf __builtin_shufflevector(const v4sf& a, const v4sf& b);
template<> inline v4sf __builtin_shufflevector<0, 0, 0, 0>(const v4sf& a, const v4sf& b) { return v4sf{ _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(a.v), _MM_SHUFFLE(0, 0, 0, 0))) }; }
template<> inline v4sf __builtin_shufflevector<1, 1, 1, 1>(const v4sf& a, const v4sf& b) { return v4sf{ _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(a.v), _MM_SHUFFLE(1, 1, 1, 1))) }; }
template<> inline v4sf __builtin_shufflevector<2, 2, 2, 2>(const v4sf& a, const v4sf& b) { return v4sf{ _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(a.v), _MM_SHUFFLE(2, 2, 2, 2))) }; }
template<> inline v4sf __builtin_shufflevector<3, 3, 3, 3>(const v4sf& a, const v4sf& b) { return v4sf{ _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(a.v), _MM_SHUFFLE(3, 3, 3, 3))) }; }
#define __builtin_shufflevector(a, b, c, d, e, f) __builtin_shufflevector<c, d, e, f>(a, b)
#else
struct v4sf
{
    float v[4];
    float   operator [] (int i) const { return v[i]; }
    v4sf    operator + (const v4sf& other) const { return v4sf{ v[0] + other.v[0], v[1] + other.v[1], v[2] + other.v[2], v[3] + other.v[3] }; }
    v4sf    operator - (const v4sf& other) const { return v4sf{ v[0] - other.v[0], v[1] - other.v[1], v[2] - other.v[2], v[3] - other.v[3] }; }
    v4sf    operator * (const v4sf& other) const { return v4sf{ v[0] * other.v[0], v[1] * other.v[1], v[2] * other.v[2], v[3] * other.v[3] }; }
    v4sf    operator / (const v4sf& other) const { return v4sf{ v[0] / other.v[0], v[1] / other.v[1], v[2] / other.v[2], v[3] / other.v[3] }; }
};
template<int x, int y, int z, int w> inline v4sf __builtin_shufflevector(const v4sf& a, const v4sf& b);
template<> inline v4sf __builtin_shufflevector<0, 0, 0, 0>(const v4sf& a, const v4sf& b) { return v4sf{ a.v[0], a.v[0], a.v[0], a.v[0] }; }
template<> inline v4sf __builtin_shufflevector<1, 1, 1, 1>(const v4sf& a, const v4sf& b) { return v4sf{ a.v[1], a.v[1], a.v[1], a.v[1] }; }
template<> inline v4sf __builtin_shufflevector<2, 2, 2, 2>(const v4sf& a, const v4sf& b) { return v4sf{ a.v[2], a.v[2], a.v[2], a.v[2] }; }
template<> inline v4sf __builtin_shufflevector<3, 3, 3, 3>(const v4sf& a, const v4sf& b) { return v4sf{ a.v[3], a.v[3], a.v[3], a.v[3] }; }
#define __builtin_shufflevector(a, b, c, d, e, f) __builtin_shufflevector<c, d, e, f>(a, b)
#endif

inline v4sf __builtin_multiplyvector(const v4sf m[4], const v4sf& v)
{
    v4sf v0 = __builtin_shufflevector(v, v, 0, 0, 0, 0);
    v4sf v1 = __builtin_shufflevector(v, v, 1, 1, 1, 1);
    v4sf v2 = __builtin_shufflevector(v, v, 2, 2, 2, 2);
    v4sf v3 = __builtin_shufflevector(v, v, 3, 3, 3, 3);
    return m[0] * v0 + m[1] * v1 + m[2] * v2 + m[3] * v3;
}
