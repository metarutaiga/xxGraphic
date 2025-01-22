//==============================================================================
// xxGraphic : System Header
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#if defined(__APPLE__)
#   include <TargetConditionals.h>
#   define objc_retain(self) (__bridge_retained void*)self
#   define objc_release(self) (__bridge_transfer id)(__bridge void*)self
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
        extern __attribute__((visibility("default"))) struct _JavaVM* xxAndroidJavaVM;
        extern __attribute__((visibility("default"))) struct _JNIEnv* xxAndroidJNIEnv;
        extern __attribute__((visibility("default"))) class _jobject* xxAndroidContext;
        extern __attribute__((visibility("default"))) int xxAndroidJNIVersion;
#       if defined(__cplusplus)
            };
#       endif
#       define xxANDROID 1
#   endif
#endif

#if defined(_MSC_VER)
#   ifndef _HAS_EXCEPTIONS
#       define _HAS_EXCEPTIONS 0
#   endif
#   ifndef _CRT_NONSTDC_NO_WARNINGS
#       define _CRT_NONSTDC_NO_WARNINGS
#   endif
#   ifndef _CRT_SECURE_NO_WARNINGS
#       define _CRT_SECURE_NO_WARNINGS
#   endif
#   include <intrin.h>
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   define _USE_MATH_DEFINES
#   define xxWINDOWS 1
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

#ifndef xxAPI
#   if defined(__cplusplus)
#       if defined(_MSC_VER) && defined(XX_BUILD_LIBRARY)
#           define xxAPI            extern "C" __declspec(dllexport) 
#       elif defined(_MSC_VER)
#           define xxAPI            extern "C" __declspec(dllimport) 
#       else
#           define xxAPI            extern "C" __attribute__((visibility("default")))
#       endif
#   else
#       if defined(_MSC_VER) && defined(XX_BUILD_LIBRARY)
#           define xxAPI            extern __declspec(dllexport)
#       elif defined(_MSC_VER)
#           define xxAPI            extern __declspec(dllimport)
#       else
#           define xxAPI            extern __attribute__((visibility("default")))
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
#define xxOffsetOf(s, m)            ((int)((char*)&((s*)nullptr)->m - (char*)nullptr))

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

#if defined(__cplusplus) && ((__cplusplus >= 201103L) || (_MSC_VER >= 1600))
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
    xxAPI size_t xxAllocSize(const void* ptr);
#else
#   define xxAlloc(T, ...)          (T*)xxAlignedAlloc(sizeof(T) * (__VA_ARGS__ + 1 != 1 ? __VA_ARGS__ + 0 : 1), 16)
#   define xxRealloc(ptr, T, count) (T*)xxAlignedRealloc(ptr, sizeof(T) * count, 16)
#   define xxFree(ptr)              xxAlignedFree(ptr)
#   define xxAllocSize(ptr)         xxAlignedAllocSize(ptr, 16)
#endif
xxAPI void* xxAlignedAlloc(size_t size, size_t alignment);
xxAPI void* xxAlignedRealloc(void* ptr, size_t size, size_t alignment);
xxAPI void xxAlignedFree(void* ptr);
xxAPI size_t xxAlignedAllocSize(const void* ptr, size_t alignment);
//==============================================================================
//  Library
//==============================================================================
xxAPI void* xxLoadLibrary(char const* name);
xxAPI void* xxGetProcAddress(void* library, char const* name);
xxAPI void xxFreeLibrary(void* library);
//==============================================================================
//  TSC
//==============================================================================
xxAPI uint64_t xxTSC();
xxAPI uint64_t xxTSCFrequency();
xxAPI float xxGetCurrentTime(double* fp64 = nullptr);
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
xxAPI char const* xxGetDocumentPath();
xxAPI char const* xxGetExecutablePath();
xxAPI char* xxOpenDirectory(uint64_t* handle, char const* path, ...);
xxAPI void xxCloseDirectory(uint64_t* handle);
//==============================================================================
//  Logger
//==============================================================================
xxAPI void (*xxLog)(char const* tag, char const* format, ...);
//==============================================================================
//  MD5
//==============================================================================
xxAPI const uint8_t* xxMD5(void const* data, int len, uint8_t* digest);
xxAPI const uint8_t* xxDXBCChecksum(void const* data, int len, uint8_t* digest);
//==============================================================================
//  Hash
//==============================================================================
xxAPI xxInline xxConstexpr unsigned int xxHash(char const* key, size_t count xxDefaultArgument(~size_t(0)), unsigned int hash xxDefaultArgument(0))
{
    for (char c; (c = (*key)) && count; key++, count--)
        hash = (hash * 16777619) ^ c;
    return hash;
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
#define xxFindFirstSet          1 + _lzcnt_u32
#define xxCountLeadingZeros     _lzcnt_u32
#define xxCountTrailingZeros    _tzcnt_u32
#define xxPopulationCount       __popcnt
#elif defined(_M_ARM) || defined(_M_ARM64)
#define xxFindFirstSet          1 + _CountLeadingZeros
#define xxCountLeadingZeros     _CountLeadingZeros
#define xxCountTrailingZeros    _CountTrailingZeros
#define xxPopulationCount       _CountOneBits
#endif
//==============================================================================
//  Byte order
//==============================================================================
#if defined(__llvm__)
#define xxHTONS                 __builtin_bswap16
#define xxHTONL                 __builtin_bswap32
#define xxNTOHS                 __builtin_bswap16
#define xxNTOHL                 __builtin_bswap32
#elif defined(_WIN32)
#define xxHTONS                 _byteswap_ushort
#define xxHTONL                 _byteswap_ulong
#define xxNTOHS                 _byteswap_ushort
#define xxNTOHL                 _byteswap_ulong
#endif
//==============================================================================
//  String
//==============================================================================
#if defined(_WIN32)
xxAPI xxInline char* strcasestr(char const* haystack, char const* needle)
{
    for (size_t length = strlen(needle); (*haystack); haystack++)
        if (_strnicmp(haystack, needle, length) == 0)
            return (char*)haystack;
    return NULL;
}
xxAPI xxInline char* strsep(char** stringp, char const* delim)
{
    if (**stringp == '\0') return NULL;
    char* s = *stringp;
    char* p = s + strcspn(s, delim);
    if (*p != '\0') *p++ = '\0';
    *stringp = p;
    return s;
}
#endif
