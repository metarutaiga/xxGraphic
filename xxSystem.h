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

#define MSEC_PER_SEC    1000ull
#define USEC_PER_SEC    1000000ull
#define NSEC_PER_SEC    1000000000ull
#define USEC_PER_MSEC   1000ull
#define NSEC_PER_MSEC   1000000ull
#define NSEC_PER_USEC   1000ull

#if defined(__APPLE__)
#   include <TargetConditionals.h>
#   if TARGET_OS_IPHONE
#       define xxIOS 1
#   endif
#   if TARGET_OS_OSX
#       define xxMACOS 1
#       if TARGET_CPU_X86_64
#           define xxMACOS_INTEL 1
#       endif
#       if TARGET_CPU_ARM64
#           define xxMACOS_ARM 1
#       endif
#   endif
#   if TARGET_OS_MACCATALYST
#       define xxMACCATALYST 1
#   endif
#   define objc_msgSend objc_msgSend_unused
#   include <objc/message.h>
#   undef objc_msgSend
    OBJC_EXPORT id objc_msgSend(id self, SEL op, ...);
#endif

#if defined(__linux__)
#   define xxLINUX 1
#   if defined(__ANDROID__)
#       define xxANDROID 1
#   endif
#endif

#if defined(_MSC_VER)
#   include <intrin.h>
#   define NOMINMAX
#   define WIN32_LEAN_AND_MEAN
#   define ChoosePixelFormat ChoosePixelFormat_unused
#   define SetPixelFormat SetPixelFormat_unused
#   define SwapBuffers SwapBuffers_unused
#   define wglCreateContext wglCreateContext_unused
#   define wglDeleteContext wglDeleteContext_unused
#   define wglGetCurrentContext wglGetCurrentContext_unused
#   define wglGetCurrentDC wglGetCurrentDC_unused
#   define wglGetProcAddress wglGetProcAddress_unused
#   define wglMakeCurrent wglMakeCurrent_unused
#   define wglShareLists wglShareLists_unused
#   include <windows.h>
#   undef ChoosePixelFormat
#   undef SetPixelFormat
#   undef SwapBuffers
#   undef wglCreateContext
#   undef wglDeleteContext
#   undef wglGetCurrentContext
#   undef wglGetCurrentDC
#   undef wglGetProcAddress
#   undef wglMakeCurrent
#   undef wglShareLists
#   pragma warning(disable:4201)
#   pragma warning(disable:4251)
#   pragma warning(disable:4482)
#   define xxWINDOWS 1
#endif

#ifndef nullptr
#   if defined(__cplusplus)
#   else
#       define nullptr              NULL
#   endif
#endif

#ifndef xxEXTERN
#   if defined(_MSC_VER)
#       if defined(__cplusplus)
#           define xxEXTERN         extern "C"
#       else
#           define xxEXTERN         extern
#       endif
#   else
#       if defined(__cplusplus)
#           define xxEXTERN         __attribute__((visibility("default"))) extern "C"
#       else
#           define xxEXTERN         __attribute__((visibility("default"))) extern
#       endif
#   endif
#endif

#ifndef xxAPI
#   if defined(_MSC_VER) && defined(XX_BUILD_LIBRARY)
#       define xxAPI xxEXTERN       __declspec(dllexport)
#   elif defined(_MSC_VER)
#       define xxAPI xxEXTERN       __declspec(dllimport)
#   else
#       define xxAPI xxEXTERN
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

#if defined(__GNUC__) || defined(__llvm__)
#   define xxLikely(x)              __builtin_expect((x), 1)
#   define xxUnlikely(x)            __builtin_expect((x), 0)
#else
#   define xxLikely(x)              (x)
#   define xxUnlikely(x)            (x)
#endif

#if defined(__cplusplus)
#   define xxInline                 inline
#   define xxConstexpr              constexpr
#   define xxDefaultArgument(value) = value
#elif defined(__GNUC__) || defined(__llvm__)
#   define xxInline                 __inline__
#   define xxConstexpr              const
#   define xxDefaultArgument(value)
#elif defined(_MSC_VER)
#   define xxInline                 __inline
#   define xxConstexpr              const
#   define xxDefaultArgument(value)
#endif

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

//==============================================================================
//  OS Dependency
//==============================================================================
#if defined(xxANDROID)
xxEXTERN struct _JavaVM* xxAndroidJavaVM;
xxEXTERN struct _JNIEnv* xxAndroidJNIEnv;
xxEXTERN class _jobject* xxAndroidContext;
xxEXTERN int xxAndroidJNIVersion;
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
    return (*key) ? xxHash(key + 1, (hash << 5) ^ (*key)) : hash;
}
//==============================================================================
//  Bitwise
//==============================================================================
xxAPI xxInline int xxCountLeadingZeros(int value)
{
#if defined(__GNUC__) || defined(__llvm__)
    return __builtin_clz(value);
#elif defined(_M_IX86) || defined(_M_AMD64)
    return __lzcnt(value);
#elif defined(_M_ARM) || defined(_M_ARM64)
    return _CountLeadingZeros(value);
#endif
}
