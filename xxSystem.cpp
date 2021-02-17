//==============================================================================
// xxGraphic : System Source
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxSystem.h"

#if defined(__APPLE__)
#   include <dirent.h>
#   include <dlfcn.h>
#   include <mach/mach_time.h>
#   include <mach-o/dyld.h>
#   include <pthread.h>
#   include <sys/sysctl.h>
#   include <sys/syscall.h>
#   include <sys/time.h>
#   include <unistd.h>
#endif

#if defined(__linux__)
#   include <dirent.h>
#   include <dlfcn.h>
#   include <sys/time.h>
#   include <sys/types.h>
#   include <unistd.h>
#   if defined(__ANDROID__)
#       include <android/log.h>
#   endif
#endif

#if defined(_MSC_VER)
#   include <intrin.h>
#   include <malloc.h>
#   undef ARM64_CNTFRQ_EL0
#   undef ARM64_CNTPCT_EL0
#   undef ARM64_CNTVCT_EL0
#   define ARM64_CNTFRQ_EL0 ARM64_SYSREG(3,3,14,0,0)    // Counter-timer Frequency register
#   define ARM64_CNTPCT_EL0 ARM64_SYSREG(3,3,14,0,1)    // Counter-timer Physical Count register
#   define ARM64_CNTVCT_EL0 ARM64_SYSREG(3,3,14,0,2)    // Counter-timer Virtual Count register
#   if defined(_WIN32)
#       include <timeapi.h>
#       pragma comment(lib, "winmm")
#   endif
#endif

//==============================================================================
//  OS Dependency
//==============================================================================
#if defined(xxANDROID)
struct _JavaVM* xxAndroidJavaVM;
struct _JNIEnv* xxAndroidJNIEnv;
class _jobject* xxAndroidContext;
int xxAndroidJNIVersion;
#endif
//==============================================================================
//  Allocator
//==============================================================================
void* xxAlignedAlloc(size_t size, size_t alignment)
{
#if defined(_MSC_VER)
    return _aligned_malloc(size, alignment);
#else
    void* ptr = nullptr;
    posix_memalign(&ptr, alignment, size);
    return ptr;
#endif
}
//------------------------------------------------------------------------------
void* xxAlignedRealloc(void* ptr, size_t size, size_t alignment)
{
#if defined(_MSC_VER)
    return _aligned_realloc(ptr, size, alignment);
#else
    void* newPtr = realloc(ptr, size);
    if ((reinterpret_cast<size_t>(newPtr) & (alignment - 1)) != 0)
    {
        void* alignedPtr = xxAlignedAlloc(size, alignment);
        if (alignedPtr != nullptr)
        {
            memcpy(alignedPtr, newPtr, size);
        }
        free(newPtr);
        newPtr = alignedPtr;
    }
    return newPtr;
#endif
}
//------------------------------------------------------------------------------
void xxAlignedFree(void* ptr)
{
#if defined(_MSC_VER)
    return _aligned_free(ptr);
#else
    return free(ptr);
#endif
}
//==============================================================================
//  Library
//==============================================================================
void* xxLoadLibrary(const char* name)
{
#if defined(_MSC_VER) && defined(_UNICODE)
    wchar_t temp[MAX_PATH];
    ::MultiByteToWideChar(CP_UTF8, 0, name, -1, temp, MAX_PATH);
    void* library = LoadLibraryW(temp);
    if (library == nullptr)
        library = LoadLibraryExW(temp, nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    return library;
#elif defined(_MSC_VER)
    return LoadLibraryA(name);
#else
    return dlopen(name, RTLD_LAZY);
#endif
}
//------------------------------------------------------------------------------
void* xxGetProcAddress(void* library, const char* name)
{
#if defined(_MSC_VER)
    return GetProcAddress((HMODULE)library, name);
#else
    return dlsym(library, name);
#endif
}
//------------------------------------------------------------------------------
void xxFreeLibrary(void* library)
{
#if defined(_MSC_VER)
    FreeLibrary((HMODULE)library);
#else
    dlclose(library);
#endif
}
//==============================================================================
//  TSC
//==============================================================================
uint64_t xxTSC()
{
#if defined(__aarch64__)
    uint64_t cntpct;
#if defined(__ANDROID__)
    asm volatile("mrs %0, cntvct_el0" : "=r" (cntpct));
#else
    asm volatile("mrs %0, cntpct_el0" : "=r" (cntpct));
#endif
    return cntpct;
#elif defined(_M_IX86) || defined(_M_AMD64) || defined(__i386__) || defined(__amd64__)
    return __rdtsc();
#elif defined(_M_ARM)
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
//  return (uint64_t)_MoveFromCoprocessor(15, 0, 9, 13, 0) * 64;
#elif defined(_M_ARM64)
    return _ReadStatusReg(ARM64_CNTPCT_EL0);
#elif defined(__APPLE__)
    return mach_absolute_time();
#else
    timeval tm;
    gettimeofday(&tm, nullptr);
    return (tm.tv_sec * 1000000 + tm.tv_usec) * 1000;
#endif
}
//------------------------------------------------------------------------------
static uint64_t xxTSCFrequencyImpl()
{
    int64_t frequency = 0;

    // Hardware
    if (frequency == 0)
    {
#if defined(__aarch64__)
        uint64_t cntfrq;
        asm volatile("mrs %0, cntfrq_el0" : "=r" (cntfrq));
        frequency = cntfrq;
#elif defined(_M_ARM)
        LARGE_INTEGER counter;
        QueryPerformanceFrequency(&counter);
        frequency = counter.QuadPart;
//      frequency = _MoveFromCoprocessor(15, 0, 14, 0, 0);
#elif defined(_M_ARM64)
        frequency = _ReadStatusReg(ARM64_CNTFRQ_EL0);
#elif defined(xxMACOS)
        size_t sz = sizeof(frequency);
        sysctlbyname("machdep.tsc.frequency", &frequency, &sz, nullptr, 0);
#endif
    }

    // Measure
    if (frequency == 0)
    {
#if defined(xxWINDOWS)
        LARGE_INTEGER performanceBegin;
        LARGE_INTEGER performanceEnd;

        LARGE_INTEGER performanceFrequency;
        QueryPerformanceFrequency(&performanceFrequency);
        if (performanceFrequency.QuadPart == 0)
            performanceFrequency.QuadPart = 1;

        QueryPerformanceCounter(&performanceBegin);
        uint64_t tscBegin = xxTSC();
        Sleep(100);
        uint64_t tscEnd = xxTSC();
        QueryPerformanceCounter(&performanceEnd);

        double delta = ((performanceEnd.QuadPart - performanceBegin.QuadPart) * MSEC_PER_SEC) / double(performanceFrequency.QuadPart);
#else
        timeval tmBegin;
        timeval tmEnd;

        timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 100 * NSEC_PER_MSEC;

        gettimeofday(&tmBegin, nullptr);
        uint64_t tscBegin = xxTSC();
        nanosleep(&ts, nullptr);
        uint64_t tscEnd = xxTSC();
        gettimeofday(&tmEnd, nullptr);

        double delta = (((tmEnd.tv_sec - tmBegin.tv_sec) * USEC_PER_SEC + (tmEnd.tv_usec - tmBegin.tv_usec)) * NSEC_PER_USEC) / double(NSEC_PER_MSEC);
#endif
        if (delta == 0.0)
            delta = 100.0;

        int64_t counter = int64_t((tscEnd - tscBegin) * MSEC_PER_SEC / delta);
        double mhz = counter / double(NSEC_PER_MSEC);
        frequency = int64_t(int64_t(mhz + 0.5) * NSEC_PER_MSEC);
    }

    xxLog("xxSystem", "Frequency : %llu", frequency);

    return frequency;
}
//------------------------------------------------------------------------------
static uint64_t xxTSCFrequencyInteger = xxTSCFrequencyImpl();
static uint64_t xxTSCInitialized = xxTSC();
static float xxTSCInvFrequencyFloat = 1.0f / xxTSCFrequencyInteger;
//------------------------------------------------------------------------------
uint64_t xxTSCFrequency()
{
    return xxTSCFrequencyInteger;
}
//------------------------------------------------------------------------------
float xxGetCurrentTime()
{
    return (float)(xxTSC() - xxTSCInitialized) * xxTSCInvFrequencyFloat;
}
//------------------------------------------------------------------------------
void xxSleep(unsigned int sleepMs)
{
#if defined(_MSC_VER)
    timeBeginPeriod(1);
    Sleep(sleepMs);
    timeEndPeriod(1);
#else
    struct timespec timespec;
    timespec.tv_sec = (sleepMs / 1000);
    timespec.tv_nsec = (sleepMs % 1000) * 1000000;
    nanosleep(&timespec, nullptr);
#endif
}
//==============================================================================
//  Process / Thread ID
//==============================================================================
#if defined(_MSC_VER) && defined(__llvm__)
static DWORD tlsIndexThreadId = TlsAlloc();
#endif
//------------------------------------------------------------------------------
uint64_t xxGetCurrentProcessId()
{
#if defined(_M_IX86)
    return __readfsdword(0x20);
#elif defined(_M_AMD64)
    return __readgsqword(0x40);
#elif defined(xxWINDOWS)
    return GetCurrentProcessId();
#else
    return getpid();
#endif
}
//------------------------------------------------------------------------------
uint64_t xxGetCurrentThreadId()
{
#if defined(_M_IX86)
    return __readfsdword(0x24);
#elif defined(_M_AMD64)
    return __readgsqword(0x48);
#elif defined(xxWINDOWS)
    return GetCurrentThreadId();
#elif defined(__APPLE__)
    uint64_t tid;
    pthread_threadid_np(NULL, &tid);
    return tid;
#else
    return gettid();
#endif
}
//------------------------------------------------------------------------------
int xxGetIncrementThreadId()
{
    static int thread_local threadId;
    if (xxUnlikely(threadId == 0))
    {
        static int increment = 0;
#if defined(__GNUC__) || defined(__llvm__)
        threadId = __sync_fetch_and_add(&increment, 1);
#elif defined(_MSC_VER)
        threadId = _InterlockedIncrement((long*)&increment);
#endif
    }
    return threadId - 1;
}
//==============================================================================
//  Path
//==============================================================================
const char* xxGetExecutablePath()
{
    static char path[4096];
    if (path[0] != '\0')
        return path;

#if defined(xxWINDOWS)
    GetModuleFileNameA(nullptr, path, sizeof(path));
    if (char* slash = strrchr(path, '\\'))
        (*slash) = '\0';
#elif defined(xxMACOS) || defined(xxIOS)
    unsigned int pathSize = sizeof(path);
    _NSGetExecutablePath(path, &pathSize);
    if (char* slash = strrchr(path, '/'))
        (*slash) = '\0';
#elif defined(xxANDROID)
    FILE* maps = fopen("/proc/self/maps", "r");
    if (maps)
    {
        while (fgets(path, sizeof(path), maps))
        {
            const char* temp = nullptr;
            if (temp == nullptr)
                temp = strstr(path, "/data/app/");
            if (temp == nullptr)
                temp = strstr(path, "/data/app-lib/");
            if (temp && strstr(path, ".so"))
            {
                strcpy(path, temp);
                break;
            }
        }
        fclose(maps);
    }
    else
    {
        strcpy(path, ".");
    }

    if (char* slash = strrchr(path, '/'))
        (*slash) = '\0';
#endif

    return path;
}
//------------------------------------------------------------------------------
char* xxOpenDirectory(uint64_t* handle, const char* path, ...)
{
    if (handle == nullptr)
        return nullptr;

    const char* filename;
#if defined(xxWINDOWS)
    WIN32_FIND_DATAA data;

    HANDLE* dir = (HANDLE*)handle;
    if ((*dir) == nullptr)
    {
        char temp[MAX_PATH];
        snprintf(temp, MAX_PATH, "%s\\%s", path, "*.*");
        (*dir) = FindFirstFileA(temp, &data);
        if ((*dir) == INVALID_HANDLE_VALUE)
            return nullptr;
    }
    else
    {
        if ((*dir) == INVALID_HANDLE_VALUE)
            return nullptr;
        if (FindNextFileA((*dir), &data) == FALSE)
            return nullptr;
    }

    filename = data.cFileName;
#else
    DIR** dir = (DIR**)handle;
    if ((*dir) == nullptr)
        (*dir) = opendir(path);
    if ((*dir) == nullptr)
        return nullptr;

    dirent* dirent = readdir(*dir);
    if (dirent == nullptr)
        return nullptr;

    filename = dirent->d_name;
#endif

    char* result = nullptr;
    for (;;)
    {
        va_list args;
        va_start(args, path);
        while (const char* match = va_arg(args, const char*))
        {
#if defined(_MSC_VER)
            auto strcasestr = [](const char* p1, const char* p2) -> const char*
            {
                for (size_t p2Length = strlen(p2); (*p1); p1++)
                    if (strnicmp(p1, p2, p2Length) == 0)
                        return p1;
                return nullptr;
            };
#endif

            if (match[0] && strcasestr(filename, match) == nullptr)
            {
                filename = nullptr;
                break;
            }
        }
        va_end(args);

        if (filename)
        {
            result = strdup(filename);
            break;
        }
#if defined(xxWINDOWS)
        if (FindNextFileA((*dir), &data) == FALSE)
            return nullptr;

        filename = data.cFileName;
#else
        dirent = readdir(*dir);
        if (dirent == nullptr)
            return nullptr;

        filename = dirent->d_name;
#endif
        if (filename == nullptr)
            break;
    }

    return result;
}
//------------------------------------------------------------------------------
void xxCloseDirectory(uint64_t* handle)
{
    if (handle == nullptr)
        return;

#if defined(xxWINDOWS)
    HANDLE* dir = (HANDLE*)handle;
    if ((*dir) == nullptr)
        return;
    if ((*dir) == INVALID_HANDLE_VALUE)
        return;

    FindClose((*dir));
#else
    DIR** dir = (DIR**)handle;
    if ((*dir) == nullptr)
        return;

    closedir(*dir);
#endif
    (*dir) = nullptr;
}
//==============================================================================
//  Logger
//==============================================================================
void xxLog(const char* tag, const char* format, ...)
{
    va_list va;
    va_start(va, format);
#if defined(xxANDROID)
    __android_log_vprint(ANDROID_LOG_INFO, tag, format, va);
#else
    char fmt[256];
    snprintf(fmt, 256, "[%s] %s\n", tag, format);
#if defined(xxWINDOWS)
    int length = vsnprintf(nullptr, 0, fmt, va);
    char* buffer = nullptr;
    if (length > 1024)
        buffer = (char*)malloc(length + 2);
    if (buffer == nullptr)
        buffer = (char*)alloca(length + 2);
    vsnprintf(buffer, length + 1, fmt, va);
    OutputDebugStringA(buffer);
    if (length > 1024)
        free(buffer);
#else
    vprintf(fmt, va);
#endif
#endif
    va_end(va);
}
//==============================================================================
//  MD5 - omaha
//  Copyright 2007-2009 Google Inc.
//==============================================================================
static const char Kr[64] =
{
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};
//------------------------------------------------------------------------------
static const unsigned int KK[64] =
{
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};
//------------------------------------------------------------------------------
struct MD5_CTX
{
    uint8_t     buf[64];
    uint32_t    state[4];
    uint32_t    count;
    enum { MD5_DIGEST_SIZE = 16 };
};
//------------------------------------------------------------------------------
static void MD5_transform(MD5_CTX* ctx)
{
    uint32_t W[64];
    uint32_t A, B, C, D;
    uint8_t* p = ctx->buf;
    int t;

    for (t = 0; t < 16; ++t)
    {
        uint32_t tmp = *p++;
        tmp |= *p++ << 8;
        tmp |= *p++ << 16;
        tmp |= *p++ << 24;
        W[t] = tmp;
    }

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];

    for (t = 0; t < 64; t++)
    {
        uint32_t f, tmp;
        int g;

        if (t < 16)
        {
            f = (D ^ (B & (C ^ D)));
            g = t;
        }
        else if (t < 32)
        {
            f = (C ^ (D & (B ^ C)));
            g = (5 * t + 1) & 15;
        }
        else if (t < 48)
        {
            f = (B ^ C ^ D);
            g = (3 * t + 5) & 15;
        }
        else
        {
            f = (C ^ (B | (~D)));
            g = (7 * t) & 15;
        }

        tmp = D;
        D = C;
        C = B;
        B = B + xxRotateLeft((A + f + KK[t] + W[g]), Kr[t]);
        A = tmp;
    }

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
}
//------------------------------------------------------------------------------
void MD5_init(MD5_CTX* ctx)
{
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->count = 0;
}
//------------------------------------------------------------------------------
void MD5_update(MD5_CTX* ctx, const void* data, int len)
{
    int i = ctx->count & 63;
    const uint8_t* p = (const uint8_t*)data;

    ctx->count += len;

    while (len--)
    {
        ctx->buf[i++] = *p++;
        if (i == 64)
        {
            MD5_transform(ctx);
            i = 0;
        }
    }
}
//------------------------------------------------------------------------------
const uint8_t* MD5_final(MD5_CTX* ctx)
{
    uint8_t* p = ctx->buf;
    uint64_t cnt = ctx->count * 8;
    int i;

    MD5_update(ctx, (uint8_t*)"\x80", 1);
    while ((ctx->count & 63) != 56)
    {
        MD5_update(ctx, (uint8_t*)"\0", 1);
    }
    for (i = 0; i < 8; ++i)
    {
        uint8_t* tmp = (uint8_t*)&cnt;
        MD5_update(ctx, &tmp[i], 1);
    }

    for (i = 0; i < 4; i++)
    {
        uint32_t tmp = ctx->state[i];
        *p++ = tmp;
        *p++ = tmp >> 8;
        *p++ = tmp >> 16;
        *p++ = tmp >> 24;
    }

    return ctx->buf;
}
//------------------------------------------------------------------------------
const uint8_t* xxMD5(const void* data, int len, uint8_t* digest)
{
    MD5_CTX ctx;
    MD5_init(&ctx);
    MD5_update(&ctx, data, len);
    memcpy(digest, MD5_final(&ctx), MD5_CTX::MD5_DIGEST_SIZE);
    return digest;
}
//------------------------------------------------------------------------------
// Copyright 2008-2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//------------------------------------------------------------------------------
const uint8_t* xxDXBCChecksum(const void* data, int len, uint8_t* digest)
{
    MD5_CTX ctx;
    MD5_init(&ctx);

    // Skip the start of the shader header
    int hashOffset = 0x14;
    len = len - hashOffset;
    data = (char*)data + hashOffset;

    int numberOfBits = len * 8;

    // First we hash all the full chunks available
    int fullChunksSize = len & 0xffffffc0;
    MD5_update(&ctx, data, fullChunksSize);

    int lastChunkSize = len - fullChunksSize;
    int paddingSize = 64 - lastChunkSize;
    void* lastChunkData = (char*)data + fullChunksSize;

    if (lastChunkSize >= 56)
    {
        MD5_update(&ctx, lastChunkData, lastChunkSize);

        /* Pad out to 56 mod 64 */
        static const unsigned char MD5_PADDING[64] = { 0x80 };
        MD5_update(&ctx, MD5_PADDING, paddingSize);

        // Pass in the number of bits
        memset(ctx.buf, 0, sizeof(ctx.buf));
        uint32_t* in = (uint32_t*)ctx.buf;
        in[0] = numberOfBits;
        in[15] = (numberOfBits >> 2) | 1;

        MD5_transform(&ctx);
    }
    else
    {
        // Pass in the number of bits
        MD5_update(&ctx, &numberOfBits, 4);

        if (lastChunkSize)
        {
            MD5_update(&ctx, lastChunkData, lastChunkSize);
        }

        // Adjust for the space used for dwNumberOfBits
        lastChunkSize += sizeof(uint32_t);
        paddingSize -= sizeof(uint32_t);

        /* Pad out to 56 mod 64 */
        static const unsigned char MD5_PADDING[64] = { 0x80 };
        memcpy(&ctx.buf[lastChunkSize], MD5_PADDING, paddingSize);

        // Pass in the number of bits
        uint32_t* in = (uint32_t*)ctx.buf;
        in[15] = (numberOfBits >> 2) | 1;

        MD5_transform(&ctx);
    }

    memcpy(digest, ctx.state, MD5_CTX::MD5_DIGEST_SIZE);
    return digest;
}
//------------------------------------------------------------------------------
