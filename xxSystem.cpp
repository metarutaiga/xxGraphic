#include "xxSystem.h"

#if defined(__APPLE__)
#   include <mach/mach_time.h>
#endif

//==============================================================================
//  TSC
//==============================================================================
uint64_t xxTSC()
{
#if defined(__aarch64__)
    unsigned long cntpct;
    asm volatile("mrs %0, cntpct_el0" : "=r" (cntpct));
    return cntpct;
#elif defined(_M_IX86) || defined(_M_AMD64) || defined(__i386__) || defined(__amd64__)
    return __rdtsc();
#elif defined(__APPLE__)
    return mach_absolute_time();
#else
    timeval tm;
    gettimeofday(&tm, nullptr);
    return (tm.tv_sec * 1000000 + tm.tv_usec) * 1000;
#endif
}
//------------------------------------------------------------------------------
uint64_t xxTSCFrequencyImpl()
{
#if defined(__aarch64__)
    unsigned long cntfrq;
    asm volatile("mrs %0, cntfrq_el0" : "=r" (cntfrq));
    return cntfrq;
#else
#if defined(xxWINDOWS)
    LARGE_INTEGER performanceBegin;
    LARGE_INTEGER performanceDelta;
    LARGE_INTEGER performanceEnd;
    LARGE_INTEGER performanceFrequency;

    QueryPerformanceCounter(&performanceBegin);
    uint64_t tscBegin = xxTSC();
    Sleep(100);
    uint64_t tscEnd = xxTSC();
    QueryPerformanceCounter(&performanceEnd);

    QueryPerformanceFrequency(&performanceFrequency);
    if (performanceFrequency.QuadPart == 0)
        performanceFrequency.QuadPart = 1;
    performanceDelta.QuadPart = (performanceEnd.QuadPart - performanceBegin.QuadPart) * 1000 / performanceFrequency.QuadPart;
    if (performanceDelta.QuadPart == 0)
        performanceDelta.QuadPart = 100;

    uint64_t counter = (tscEnd - tscBegin) * 1000 / performanceDelta.QuadPart;
#else
    timeval tmBegin;
    timeval tmDelta;
    timeval tmEnd;

    timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 100 * 1000000;

    gettimeofday(&tmBegin, nullptr);
    uint64_t tscBegin = xxTSC();
    nanosleep(&ts, nullptr);
    uint64_t tscEnd = xxTSC();
    gettimeofday(&tmEnd, nullptr);

    uint64_t frequency = 1000000;
    tmDelta.tv_sec = tmEnd.tv_sec - tmBegin.tv_sec;
    tmDelta.tv_usec = tmEnd.tv_usec - tmBegin.tv_usec;
    uint64_t delta = (tmDelta.tv_sec * 1000000 + tmDelta.tv_usec) * 1000 / frequency;
    if (delta == 0)
        delta = 100;

    uint64_t counter = (tscEnd - tscBegin) * 1000 / delta;
#endif

    float mhz = counter / 1000000.0f;
    counter = llroundf(mhz / 100.0f) * 100 * 1000000;

    return counter;
#endif
}
//------------------------------------------------------------------------------
uint64_t xxTSCFrequency = xxTSCFrequencyImpl();
uint64_t xxTSCInitialized = xxTSC();
float xxTSCInvFrequencyFloat = 1.0f / xxTSCFrequency;
//------------------------------------------------------------------------------
float xxGetCurrentTime()
{
    return (float)(xxTSC() - xxTSCInitialized) * xxTSCInvFrequencyFloat;
}
//==============================================================================
//  Process / Thread ID
//==============================================================================
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
#if defined(__GNUC__)
        threadId = __sync_fetch_and_add(&increment, 1);
#elif defined(_MSC_VER)
        threadId = _InterlockedIncrement((unsigned int*)&increment);
#endif
    }
    return threadId - 1;
}
//==============================================================================
//  Logger
//==============================================================================
int xxLog(const char* tag, const char* format, ...)
{
    va_list first;
    va_list second;

    va_start(first, format);
    va_copy(second, first);
    int tagLength = snprintf(nullptr, 0, "[%s]", tag) + 1;
    int formatLength = vsnprintf(nullptr, 0, format, first) + 1;
    va_end(first);

    char* buffer = xxAlloc(char, tagLength + formatLength);
    if (buffer)
    {
        snprintf(buffer, tagLength + formatLength, "[%s]", tag);
        vsnprintf(buffer + tagLength, formatLength, format, second);
        buffer[tagLength - 1] = ' ';
#if defined(xxWINDOWS)
        OutputDebugStringA(buffer);
        OutputDebugStringA("\n");
#else
        printf("%s\n", buffer);
#endif
        xxFree(buffer);
    }
    va_end(second);

    return formatLength - 1;
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
        uint8_t tmp = (uint8_t)(cnt >> (i * 8));
        MD5_update(ctx, &tmp, 1);
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
