//==============================================================================
// xxGraphic : Vector Header
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

#if defined(__ARM_NEON__) || defined(__ARM_NEON) || defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
#   include <arm_neon.h>
#   if defined(_M_ARM)
#       define vdupq_laneq_f32(v, i) vdupq_lane_f32((i < 2 ? vget_low_f32(v) : vget_high_f32(v)), i < 2 ? i : i - 2)
#       define vuzp1q_f32(x, y) vuzpq_f32(x, y).val[0] 
#   endif
#elif defined(__i386__) || defined(__amd64__) || defined(_M_IX86) || defined(_M_AMD64)
#   include <immintrin.h>
#   undef _mm_loadu_si64
#   define _mm_loadu_si64(v) _mm_castpd_si128(_mm_load_sd((double*)v))
#endif

#if defined(__llvm__)
typedef int16_t v4hi __attribute__((vector_size(8), aligned(4)));
typedef float v4sf __attribute__((vector_size(16), aligned(4)));
#elif defined(__ARM_NEON__) || defined(__ARM_NEON) || defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
union v4hi
{
    struct { int16_t _[4]; };
    int16x4_t v;
    int16_t operator [] (int i) const { return v.n64_i16[i]; }
};
union v4sf
{
    struct { float _[4]; };
    float32x4_t v;
    float   operator [] (int i) const { return v.n128_f32[i]; }
    v4sf    operator + (v4sf const& other) const { return v4sf{ .v = vaddq_f32(v, other.v) }; }
    v4sf    operator - (v4sf const& other) const { return v4sf{ .v = vsubq_f32(v, other.v) }; }
    v4sf    operator * (v4sf const& other) const { return v4sf{ .v = vmulq_f32(v, other.v) }; }
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
    v4sf    operator / (v4sf const& other) const { return v4sf{ .v = vdivq_f32(v, other.v) }; }
#else
    v4sf    operator / (v4sf const& other) const { return v4sf{ v.n128_f32[0] / other.v.n128_f32[0],
                                                                v.n128_f32[1] / other.v.n128_f32[1],
                                                                v.n128_f32[2] / other.v.n128_f32[2],
                                                                v.n128_f32[3] / other.v.n128_f32[3] }; }
#endif
};
template<typename O, typename I> inline O __builtin_convertvector(I const& a);
template<> inline v4hi __builtin_convertvector<v4hi, v4sf>(v4sf const& a) { return v4hi{ .v = vmovn_s32(vcvtq_s32_f32(a.v)) }; };
template<> inline v4sf __builtin_convertvector<v4sf, v4hi>(v4hi const& a) { return v4sf{ .v = vcvtq_f32_s32(vmovl_s16(a.v)) }; };
#define __builtin_convertvector(a, b) __builtin_convertvector<b, std::remove_reference<decltype(a)>::type>(a)
template<int x, int y, int z, int w> inline v4sf __builtin_shufflevector(v4sf const& a, v4sf const& b);
template<> inline v4sf __builtin_shufflevector<0, 0, 0, 0>(v4sf const& a, v4sf const& b) { return v4sf{ .v = vdupq_laneq_f32(a.v, 0) }; }
template<> inline v4sf __builtin_shufflevector<1, 1, 1, 1>(v4sf const& a, v4sf const& b) { return v4sf{ .v = vdupq_laneq_f32(a.v, 1) }; }
template<> inline v4sf __builtin_shufflevector<1, 2, 0, 1>(v4sf const& a, v4sf const& b) { return v4sf{ .v = vextq_f32(vextq_f32(a.v, a.v, 3), a.v, 2) }; }
template<> inline v4sf __builtin_shufflevector<2, 0, 1, 2>(v4sf const& a, v4sf const& b) { return v4sf{ .v = vextq_f32(vuzp1q_f32(a.v, a.v), a.v, 3) }; }
template<> inline v4sf __builtin_shufflevector<2, 2, 2, 2>(v4sf const& a, v4sf const& b) { return v4sf{ .v = vdupq_laneq_f32(a.v, 2) }; }
template<> inline v4sf __builtin_shufflevector<3, 3, 3, 3>(v4sf const& a, v4sf const& b) { return v4sf{ .v = vdupq_laneq_f32(a.v, 3) }; }
#define __builtin_shufflevector(a, b, c, d, e, f) __builtin_shufflevector<c, d, e, f>(a, b)
#elif defined(__i386__) || defined(__amd64__) || defined(_M_IX86) || defined(_M_AMD64)
struct v4hi
{
    int64_t v;
    int16_t operator [] (int i) const { return ((int16_t*)&v)[i]; }
};
struct v4sf
{
    __m128 v;
    float   operator [] (int i) const { return v.m128_f32[i]; }
    v4sf    operator + (v4sf const& other) const { return { _mm_add_ps(v, other.v) }; }
    v4sf    operator - (v4sf const& other) const { return { _mm_sub_ps(v, other.v) }; }
    v4sf    operator * (v4sf const& other) const { return { _mm_mul_ps(v, other.v) }; }
    v4sf    operator / (v4sf const& other) const { return { _mm_div_ps(v, other.v) }; }
};
template<typename O, typename I> inline O __builtin_convertvector(I const& a);
template<> inline v4hi __builtin_convertvector<v4hi, v4sf>(v4sf const& a) { __m128i t = _mm_cvtps_epi32(a.v); v4hi b; _mm_storeu_si64(&b.v, _mm_packs_epi32(t, t)); return b; }
template<> inline v4sf __builtin_convertvector<v4sf, v4hi>(v4hi const& a) { __m128i t = _mm_loadu_si64(&a.v); return { _mm_cvtepi32_ps(_mm_srai_epi32(_mm_unpacklo_epi16(t, t), 16)) }; };
#define __builtin_convertvector(a, b) __builtin_convertvector<b, std::remove_reference<decltype(a)>::type>(a)
template<int x, int y, int z, int w> inline v4sf __builtin_shufflevector(v4sf const& a, v4sf const& b);
template<> inline v4sf __builtin_shufflevector<0, 0, 0, 0>(v4sf const& a, v4sf const& b) { return { _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(a.v), _MM_SHUFFLE(0, 0, 0, 0))) }; }
template<> inline v4sf __builtin_shufflevector<1, 1, 1, 1>(v4sf const& a, v4sf const& b) { return { _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(a.v), _MM_SHUFFLE(1, 1, 1, 1))) }; }
template<> inline v4sf __builtin_shufflevector<1, 2, 0, 1>(v4sf const& a, v4sf const& b) { return { _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(a.v), _MM_SHUFFLE(1, 0, 2, 1))) }; }
template<> inline v4sf __builtin_shufflevector<2, 0, 1, 2>(v4sf const& a, v4sf const& b) { return { _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(a.v), _MM_SHUFFLE(2, 1, 0, 2))) }; }
template<> inline v4sf __builtin_shufflevector<2, 2, 2, 2>(v4sf const& a, v4sf const& b) { return { _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(a.v), _MM_SHUFFLE(2, 2, 2, 2))) }; }
template<> inline v4sf __builtin_shufflevector<3, 3, 3, 3>(v4sf const& a, v4sf const& b) { return { _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(a.v), _MM_SHUFFLE(3, 3, 3, 3))) }; }
#define __builtin_shufflevector(a, b, c, d, e, f) __builtin_shufflevector<c, d, e, f>(a, b)
#else
struct v4hi
{
    int16_t v[4];
    int16_t operator [] (int i) const { return v[i]; }
};
struct v4sf
{
    float v[4];
    float   operator [] (int i) const { return v[i]; }
    v4sf    operator + (v4sf const& other) const { return { v[0] + other.v[0], v[1] + other.v[1], v[2] + other.v[2], v[3] + other.v[3] }; }
    v4sf    operator - (v4sf const& other) const { return { v[0] - other.v[0], v[1] - other.v[1], v[2] - other.v[2], v[3] - other.v[3] }; }
    v4sf    operator * (v4sf const& other) const { return { v[0] * other.v[0], v[1] * other.v[1], v[2] * other.v[2], v[3] * other.v[3] }; }
    v4sf    operator / (v4sf const& other) const { return { v[0] / other.v[0], v[1] / other.v[1], v[2] / other.v[2], v[3] / other.v[3] }; }
};
template<typename O, typename I> inline O __builtin_convertvector(I const& a);
template<> inline v4hi __builtin_convertvector<v4hi, v4sf>(v4sf const& a) { return { (int16_t)a.v[0], (int16_t)a.v[1], (int16_t)a.v[2], (int16_t)a.v[3] }; };
template<> inline v4sf __builtin_convertvector<v4sf, v4hi>(v4hi const& a) { return { (float)a.v[0], (float)a.v[1], (float)a.v[2], (float)a.v[3] }; };
#define __builtin_convertvector(a, b) __builtin_convertvector<b, std::remove_reference<decltype(a)>::type>(a)
template<int x, int y, int z, int w> inline v4sf __builtin_shufflevector(v4sf const& a, v4sf const& b);
template<> inline v4sf __builtin_shufflevector<0, 0, 0, 0>(v4sf const& a, v4sf const& b) { return { a.v[0], a.v[0], a.v[0], a.v[0] }; }
template<> inline v4sf __builtin_shufflevector<1, 1, 1, 1>(v4sf const& a, v4sf const& b) { return { a.v[1], a.v[1], a.v[1], a.v[1] }; }
template<> inline v4sf __builtin_shufflevector<1, 2, 0, 1>(v4sf const& a, v4sf const& b) { return { a.v[1], a.v[2], a.v[0], a.v[1] }; }
template<> inline v4sf __builtin_shufflevector<2, 0, 1, 2>(v4sf const& a, v4sf const& b) { return { a.v[2], a.v[0], a.v[1], a.v[2] }; }
template<> inline v4sf __builtin_shufflevector<2, 2, 2, 2>(v4sf const& a, v4sf const& b) { return { a.v[2], a.v[2], a.v[2], a.v[2] }; }
template<> inline v4sf __builtin_shufflevector<3, 3, 3, 3>(v4sf const& a, v4sf const& b) { return { a.v[3], a.v[3], a.v[3], a.v[3] }; }
#define __builtin_shufflevector(a, b, c, d, e, f) __builtin_shufflevector<c, d, e, f>(a, b)
#endif

inline v4sf __builtin_vectormultiply(v4sf const& v, v4sf const m[4])
{
    v4sf v0 = __builtin_shufflevector(v, v, 0, 0, 0, 0);
    v4sf v1 = __builtin_shufflevector(v, v, 1, 1, 1, 1);
    v4sf v2 = __builtin_shufflevector(v, v, 2, 2, 2, 2);
    v4sf v3 = __builtin_shufflevector(v, v, 3, 3, 3, 3);
    return m[0] * v0 + m[1] * v1 + m[2] * v2 + m[3] * v3;
}
