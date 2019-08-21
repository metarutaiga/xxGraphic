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
#   pragma warning(disable:4201)
#endif

#ifndef xxEXTERN
#   if defined(__cplusplus)
#       define xxEXTERN extern "C"
#   else
#       define xxEXTERN extern
#   endif
#endif

#ifndef xxAPI
#   if defined(_WINDLL)
#       define xxAPI xxEXTERN __declspec(dllexport)
#   elif defined(_MSC_VER)
#       define xxAPI xxEXTERN __declspec(dllimport)
#   else
#       define xxAPI xxEXTERN
#   endif
#endif

#if defined(__GNUC__)
#   define xxLikely(x)      __builtin_expect((x), 1)
#   define xxUnlikely(x)    __builtin_expect((x), 0)
#else
#   define xxLikely(x)      (x)
#   define xxUnlikely(x)    (x)
#endif

#define xxSizeOf(var)       (sizeof(var))
#define xxCountOf(var)      (sizeof(var) / sizeof(*var))
#define xxOffsetOf(st, m)   (offsetof(st, m))

#define xxAlloc(T, count)   (T*)malloc(sizeof(T) * count)
#define xxFree(ptr)         free(ptr)
//==============================================================================
//  TSC
//==============================================================================
xxAPI uint64_t xxTSC();
xxAPI float xxGetCurrentTime();
//==============================================================================
//  Process / Thread ID
//==============================================================================
xxAPI uint64_t xxGetCurrentProcessId();
xxAPI uint64_t xxGetCurrentThreadId();
xxAPI int xxGetIncrementThreadId();
