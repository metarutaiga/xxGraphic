#pragma once

#ifndef _HAS_EXCEPTIONS
#   define _HAS_EXCEPTIONS 0
#endif

#include <float.h>
#include <intrin.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>

#if defined(_MSC_VER)
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#ifndef xxGL_EXTERN
#   if defined(__cplusplus)
#       define xxGL_EXTERN extern "C"
#   else
#       define xxGL_EXTERN extern
#   endif
#endif

#ifndef xxGL_API
#   if defined(_WINDLL)
#       define xxGL_API xxGL_EXTERN __declspec(dllexport)
#   elif defined(_MSC_VER)
#       define xxGL_API xxGL_EXTERN __declspec(dllimport)
#   else
#       define xxGL_API xxGL_EXTERN
#   endif
#endif

#if defined(__GNUC__)
#   define xxLikely(x)      __builtin_expect((x), 1)
#   define xxUnlikely(x)    __builtin_expect((x), 0)
#else
#   define xxLikely(x)      (x)
#   define xxUnlikely(x)    (x)
#endif

#define xxSizeOf(var)       sizeof(var)
#define xxCountOf(var)      (sizeof(var) / sizeof(*var))

#define xxAlloc(T, count)   (T*)malloc(sizeof(T) * count)
#define xxFree(ptr)         free(ptr)

xxGL_API uint64_t xxTSCFrequency;
xxGL_API uint64_t xxTSC();
xxGL_API float xxGetCurrentTime();

xxGL_API uint64_t xxGetCurrentProcessId();
xxGL_API uint64_t xxGetCurrentThreadId();
xxGL_API int xxGetIncrementThreadId();
