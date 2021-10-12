//==============================================================================
// xxGraphic : System Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#ifndef _HAS_EXCEPTIONS
#   define _HAS_EXCEPTIONS 0
#endif

#ifndef _CRT_NONSTDC_NO_WARNINGS
#   define _CRT_NONSTDC_NO_WARNINGS
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#   define _CRT_SECURE_NO_WARNINGS
#endif

#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__APPLE__)
#   include <TargetConditionals.h>
#   define objc_msgSend objc_msgSend_unused
#   include <objc/message.h>
#   undef objc_msgSend
    OBJC_EXPORT id objc_msgSend(id self, SEL op, ...);
#   if TARGET_OS_IPHONE
#       define xxIOS 1
#       if TARGET_CPU_X86_64
#           define xxIOS_LEGACY 1
#       endif
#   endif
#   if TARGET_OS_OSX
#       define xxMACOS 1
#       if TARGET_CPU_X86_64
#           define xxMACOS_LEGACY 1
#       endif
#   endif
#   if TARGET_OS_MACCATALYST
#       define xxMACCATALYST 1
#   endif
#endif

#if defined(__linux__)
#   define xxLINUX 1
#   if defined(__ANDROID__)
#       if defined(__cplusplus)
            extern "C"
            {
#       endif
        extern struct _JavaVM* xxAndroidJavaVM;
        extern struct _JNIEnv* xxAndroidJNIEnv;
        extern class _jobject* xxAndroidContext;
        extern int xxAndroidJNIVersion;
#       if defined(__cplusplus)
            };
#       endif
#       define xxANDROID 1
#   endif
#endif

#if defined(_MSC_VER)
#   include <intrin.h>
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   define xxWINDOWS 1
#endif

#ifndef xxAPI
#   if defined(__cplusplus)
#       if defined(_MSC_VER) && defined(XX_BUILD_LIBRARY)
#           define xxAPI            __declspec(dllexport) extern "C"
#       elif defined(_MSC_VER)
#           define xxAPI            __declspec(dllimport) extern "C"
#       else
#           define xxAPI            __attribute__((visibility("default"))) extern "C"
#       endif
#   else
#       if defined(_MSC_VER) && defined(XX_BUILD_LIBRARY)
#           define xxAPI            __declspec(dllexport) extern
#       elif defined(_MSC_VER)
#           define xxAPI            __declspec(dllimport) extern
#       else
#           define xxAPI            __attribute__((visibility("default"))) extern
#       endif
#   endif
#endif

#ifndef xxPlusAPI
#   if defined(_MSC_VER) && defined(XXPLUS_BUILD_LIBRARY)
#       define xxPlusAPI            __declspec(dllexport)
#   elif defined(_MSC_VER)
#       define xxPlusAPI            __declspec(dllimport)
#   else
#       define xxPlusAPI            __attribute__((visibility("default")))
#   endif
#endif

#undef MSEC_PER_SEC
#undef USEC_PER_SEC
#undef NSEC_PER_SEC
#undef USEC_PER_MSEC
#undef NSEC_PER_MSEC
#undef NSEC_PER_USEC
#define MSEC_PER_SEC                1000ull
#define USEC_PER_SEC                1000000ull
#define NSEC_PER_SEC                1000000000ull
#define USEC_PER_MSEC               1000ull
#define NSEC_PER_MSEC               1000000ull
#define NSEC_PER_USEC               1000ull

#define xxSizeOf(var)               (sizeof(var))
#define xxCountOf(var)              (sizeof(var) / sizeof(*var))
#define xxOffsetOf(s, m)            ((char*)&((s*)nullptr)->m - (char*)nullptr)

#define xxRotateLeft(v, s)          ((v << s) | (v >> (sizeof(v) * 8 - s)))
#define xxRotateRight(v, s)         ((v >> s) | (v << (sizeof(v) * 8 - s)))

#define xxLocalBreak()              switch (0) case 0:

#define xxStringify_(v)             #v
#define xxStringify(v)              xxStringify_(v)

#define xxConcate_(x, y)            x ## y
#define xxConcate(m, n)             xxConcate_(m, n)

#define xxLikely(x)                 (x)
#define xxUnlikely(x)               (x)

#define xxConstexpr                 const
#define xxDefaultArgument(value)
#define xxInline                    __inline__

#if defined(__GNUC__) || defined(__llvm__)
#   undef xxLikely
#   undef xxUnlikely
#   define xxLikely(x)              __builtin_expect((x), 1)
#   define xxUnlikely(x)            __builtin_expect((x), 0)
#endif

#if (__cplusplus >= 201103L) || (_MSC_VER >= 1600)
#   undef xxConstexpr
#   define xxConstexpr              constexpr
#endif

#if defined(__cplusplus)
#   undef xxDefaultArgument
#   undef xxInline
#   define xxDefaultArgument(value) = value
#   define xxInline                 inline
#elif defined(_MSC_VER)
#   undef xxInline
#   define xxInline                 __inline
#endif

//==============================================================================
//  Allocator
//==============================================================================
#if INTPTR_MAX == INT64_MAX
#   define xxAlloc(T, ...)          (T*)malloc(sizeof(T) * (__VA_ARGS__ + 1 != 1 ? __VA_ARGS__ + 0 : 1))
#   define xxRealloc(ptr, T, count) (T*)realloc(ptr, sizeof(T) * count)
#   define xxFree(ptr)              free(ptr)
#else
#   define xxAlloc(T, ...)          (T*)xxAlignedAlloc(sizeof(T) * (__VA_ARGS__ + 1 != 1 ? __VA_ARGS__ + 0 : 1), 16)
#   define xxRealloc(ptr, T, count) (T*)xxAlignedRealloc(ptr, sizeof(T) * count, 16)
#   define xxFree(ptr)              xxAlignedFree(ptr)
#endif
xxAPI void* xxAlignedAlloc(size_t size, size_t alignment);
xxAPI void* xxAlignedRealloc(void* ptr, size_t size, size_t alignment);
xxAPI void xxAlignedFree(void* ptr);
//==============================================================================
//  Library
//==============================================================================
xxAPI void* xxLoadLibrary(const char* name);
xxAPI void* xxGetProcAddress(void* library, const char* name);
xxAPI void xxFreeLibrary(void* library);
//==============================================================================
//  TSC
//==============================================================================
xxAPI uint64_t xxTSC();
xxAPI uint64_t xxTSCFrequency();
xxAPI float xxGetCurrentTime();
xxAPI void xxSleep(unsigned int sleepMs);
//==============================================================================
//  Process / Thread ID
//==============================================================================
xxAPI uint64_t xxGetCurrentProcessId();
xxAPI uint64_t xxGetCurrentThreadId();
xxAPI int xxGetIncrementThreadId();
//==============================================================================
//  Path
//==============================================================================
xxAPI const char* xxGetExecutablePath();
xxAPI char* xxOpenDirectory(uint64_t* handle, const char* path, ...);
xxAPI void xxCloseDirectory(uint64_t* handle);
//==============================================================================
//  Logger
//==============================================================================
xxAPI void xxLog(const char* tag, const char* format, ...);
//==============================================================================
//  MD5
//==============================================================================
xxAPI const uint8_t* xxMD5(const void* data, int len, uint8_t* digest);
xxAPI const uint8_t* xxDXBCChecksum(const void* data, int len, uint8_t* digest);
//==============================================================================
//  Hash
//==============================================================================
xxAPI xxInline xxConstexpr unsigned int xxHash(const char* key, const unsigned int hash xxDefaultArgument(0))
{
    return (*key) ? xxHash(key + 1, (hash * 16777219) ^ (*key)) : hash;
}
//==============================================================================
//  Bitwise
//==============================================================================
#if defined(__GNUC__) || defined(__llvm__)
#define xxFindFirstSet          __builtin_ffs
#define xxCountLeadingZeros     __builtin_clz
#define xxCountTrailingZeros    __builtin_ctz
#define xxPopulationCount       __builtin_popcount
#elif defined(_M_IX86) || defined(_M_AMD64)
#define xxFindFirstSet          1 + __lzcnt
#define xxCountLeadingZeros     __lzcnt
#define xxCountTrailingZeros    __tzcnt
#define xxPopulationCount       __popcnt
#elif defined(_M_ARM) || defined(_M_ARM64)
#define xxFindFirstSet          1 + _CountLeadingZeros
#define xxCountLeadingZeros     _CountLeadingZeros
#define xxCountTrailingZeros    _CountTrailinZeros
#define xxPopulationCount       _CountOneBits
#endif
