#include "xxSystem.h"

//------------------------------------------------------------------------------
//  TSC
//------------------------------------------------------------------------------
uint64_t xxTSC()
{
    return __rdtsc();
}
//------------------------------------------------------------------------------
uint64_t xxTSCFrequencyImpl()
{
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

    uint64_t frequency = (tscEnd - tscBegin) * 1000 / performanceDelta.QuadPart;

    float mhz = frequency / 1000000.0f;
    frequency = llroundf(mhz / 100.0f) * 100 * 1000000;

    return frequency;
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
//------------------------------------------------------------------------------
//  Process / Thread ID
//------------------------------------------------------------------------------
uint64_t xxGetCurrentProcessId()
{
#if defined(_M_IX86)
    return __readfsdword(0x20);
#elif defined(_M_AMD64)
    return __readgsqword(0x40);
#endif
    return GetCurrentProcessId();
}
//------------------------------------------------------------------------------
uint64_t xxGetCurrentThreadId()
{
#if defined(_M_IX86)
    return __readfsdword(0x24);
#elif defined(_M_AMD64)
    return __readgsqword(0x48);
#endif
    return GetCurrentThreadId();
}
//------------------------------------------------------------------------------
int xxGetIncrementThreadId()
{
    static int increment;
    static int __declspec(thread) threadId;
    if (xxUnlikely(threadId == 0))
    {
        threadId = _InterlockedIncrement((unsigned int*)&increment);
    }
    return threadId - 1;
}
//------------------------------------------------------------------------------
