//==============================================================================
// xxGraphic : MSVCRT minimal Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <setjmp.h>
#include <stdio.h>
#include <intrin.h>
#include <malloc.h>

//==============================================================================
//  new / delete
//==============================================================================
void* operator new(size_t size)
{
    return _aligned_malloc(size, 16);
}
void operator delete(void* ptr, size_t size)
{
    _aligned_free(ptr);
}
//------------------------------------------------------------------------------

#ifndef _DEBUG

//==============================================================================
//  MSVCRT
//==============================================================================
extern "C" int _fltused = 1;
static HMODULE msvcrt = nullptr;
static HMODULE ntdll = nullptr;
//------------------------------------------------------------------------------
#pragma section(".CRT$XCA", long, read)
#pragma section(".CRT$XCZ", long, read)
#pragma section(".CRT$XIA", long, read)
#pragma section(".CRT$XIZ", long, read)
typedef void (*_PVFV)();
extern "C" __declspec(allocate(".CRT$XCA")) const _PVFV __xc_a = 0;
extern "C" __declspec(allocate(".CRT$XCZ")) const _PVFV __xc_z = 0;
extern "C" __declspec(allocate(".CRT$XIA")) const _PVFV __xi_a = 0;
extern "C" __declspec(allocate(".CRT$XIZ")) const _PVFV __xi_z = 0;
#pragma comment(linker, "/merge:.CRT=.rdata")
//------------------------------------------------------------------------------
static void _initterm(const _PVFV* ppfn, const _PVFV* end)
{
    do
    {
        if (_PVFV pfn = *++ppfn)
        {
            pfn();
        }
    } while (ppfn < end);
}
//------------------------------------------------------------------------------
extern "C" BOOL WINAPI _DllMainCRTStartup(HANDLE handle, DWORD reason, LPVOID preserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        if (msvcrt == nullptr)
        {
            msvcrt = LoadLibraryA("msvcrt.dll");
        }
        if (ntdll == nullptr)
        {
            ntdll = LoadLibraryA("ntdll.dll");
        }
        _initterm(&__xi_a, &__xi_z);
        _initterm(&__xc_a, &__xc_z);
        break;

    case DLL_PROCESS_DETACH:
        if (msvcrt)
        {
            FreeLibrary(msvcrt);
            msvcrt = nullptr;
        }
        if (ntdll)
        {
            FreeLibrary(ntdll);
            ntdll = nullptr;
        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
//------------------------------------------------------------------------------
static int (*__vsnprintf)(char*, size_t, char const*, va_list);
//------------------------------------------------------------------------------
static int stdio_common_vsprintf(unsigned __int64 options, char* buffer, size_t bufferCount, char const* format, _locale_t locale, va_list argList)
{
    return __vsnprintf(buffer, bufferCount, format, argList);
}
//------------------------------------------------------------------------------
static void* getFunction(const char* name)
{
    if (name == "__stdio_common_vsprintf")
    {
        if (__vsnprintf == nullptr)
        {
            (void*&)__vsnprintf = getFunction("_vsnprintf");
        }
        return stdio_common_vsprintf;
    }

    void* function = nullptr;

    function = GetProcAddress(msvcrt, name);
    if (function)
        return function;

    function = GetProcAddress(ntdll, name);
    if (function)
        return function;

    MessageBoxA(nullptr, name, "Microsoft Visual C++ Runtime Library", MB_OK);

    return nullptr;
}
//------------------------------------------------------------------------------
#if defined(_M_IX86)
#define _IMP_(v) _imp__ ## v
#elif defined(_M_AMD64)
#define _IMP_(v) __imp_ ## v
#endif
//------------------------------------------------------------------------------
extern "C" void* _IMP_(malloc)                  = getFunction("malloc");
extern "C" void* _IMP_(realloc)                 = getFunction("realloc");
extern "C" void* _IMP_(free)                    = getFunction("free");
extern "C" void* _IMP_(_aligned_malloc)         = getFunction("_aligned_malloc");
extern "C" void* _IMP_(_aligned_realloc)        = getFunction("_aligned_realloc");
extern "C" void* _IMP_(_aligned_free)           = getFunction("_aligned_free");
#if defined(_M_IX86)
extern "C" void* _IMP_(_allmul)                 = getFunction("_allmul");
extern "C" void* _IMP_(_aulldiv)                = getFunction("_aulldiv");
#endif
extern "C" void* _IMP_(fclose)                  = getFunction("fclose");
extern "C" void* _IMP_(fopen)                   = getFunction("fopen");
extern "C" void* _IMP_(fread)                   = getFunction("fread");
extern "C" void* _IMP_(fseek)                   = getFunction("fseek");
extern "C" void* _IMP_(ftell)                   = getFunction("ftell");
extern "C" void* _IMP_(getenv)                  = getFunction("getenv");
extern "C" void* _IMP_(qsort)                   = getFunction("qsort");
extern "C" void* _IMP_(longjmp)                 = getFunction("longjmp");
extern "C" void* _IMP_(_setjmp)                 = getFunction("_setjmp");
extern "C" void* _IMP_(memchr)                  = getFunction("memchr");
extern "C" void* _IMP_(memcmp)                  = getFunction("memcmp");
extern "C" void* _IMP_(memcpy)                  = getFunction("memcpy");
extern "C" void* _IMP_(memmove)                 = getFunction("memmove");
extern "C" void* _IMP_(memset)                  = getFunction("memset");
extern "C" void* _IMP_(strcmp)                  = getFunction("strcmp");
extern "C" void* _IMP_(strncmp)                 = getFunction("strncmp");
extern "C" void* _IMP_(strncpy)                 = getFunction("strncpy");
extern "C" void* _IMP_(strrchr)                 = getFunction("strrchr");
extern "C" void* _IMP_(strstr)                  = getFunction("strstr");
extern "C" void* _IMP_(strtol)                  = getFunction("strtol");
extern "C" void* _IMP_(__stdio_common_vsprintf) = getFunction("__stdio_common_vsprintf");
#if defined(_M_IX86)
extern "C" void* _IMP_(_chkstk)                 = getFunction("_chkstk");
#elif defined(_M_AMD64)
extern "C" void* _IMP_(__chkstk)                = getFunction("__chkstk");
#endif
//------------------------------------------------------------------------------
#pragma function(memchr)
extern "C" void const* memchr(void const* a, int b, size_t c)
{
    void const* (*function)(void const* a, int b, size_t c);
    (void*&)function = _IMP_(memchr);
    return function(a, b, c);
}
//------------------------------------------------------------------------------
#pragma function(memcmp)
extern "C" int memcmp(void const* a, void const* b, size_t c)
{
    int (*function)(void const* a, void const* b, size_t c);
    (void*&)function = _IMP_(memcmp);
    return function(a, b, c);
}
//------------------------------------------------------------------------------
#pragma function(memcpy)
extern "C" void* memcpy(void* a, void const* b, size_t c)
{
    void* (*function)(void* a, void const* b, size_t c);
    (void*&)function = _IMP_(memcpy);
    return function(a, b, c);
}
//------------------------------------------------------------------------------
#pragma function(memmove)
extern "C" void* memmove(void* a, void const* b, size_t c)
{
    void* (*function)(void* a, void const* b, size_t c);
    (void*&)function = _IMP_(memmove);
    return function(a, b, c);
}
//------------------------------------------------------------------------------
#pragma function(memset)
extern "C" void* memset(void* a, int b, size_t c)
{
    void* (*function)(void* a, int b, size_t c);
    (void*&)function = _IMP_(memset);
    return function(a, b, c);
}
//------------------------------------------------------------------------------
#pragma function(strcmp)
extern "C" int strcmp(char const* a, char const* b)
{
    int (*function)(char const* a, char const* b);
    (void*&)function = _IMP_(strcmp);
    return function(a, b);
}
//------------------------------------------------------------------------------
extern "C" char const* strrchr(char const* a, int b)
{
    char* (*function)(char const* a, int b);
    (void*&)function = _IMP_(strrchr);
    return function(a, b);
}
//------------------------------------------------------------------------------
extern "C" char const* strstr(char const* a, char const* b)
{
    char const* (*function)(char const* a, char const* b);
    (void*&)function = _IMP_(strstr);
    return function(a, b);
}
//------------------------------------------------------------------------------
extern "C" __declspec(noreturn) void longjmp(jmp_buf a, int b) noexcept(false)
{
    void (*function)(jmp_buf a, int b);
    (void*&)function = _IMP_(longjmp);
    function(a, b);
}
//------------------------------------------------------------------------------
extern "C" int setjmpex(jmp_buf a)
{
    int (*function)(jmp_buf a);
    (void*&)function = _IMP_(_setjmp);
    return function(a);
}
//------------------------------------------------------------------------------
#if defined(_M_IX86)
extern "C" void _chkstk()
#elif defined(_M_AMD64)
extern "C" void __chkstk()
#endif
{
    void (*function)();
#if defined(_M_IX86)
    (void*&)function = _IMP_(_chkstk);
#elif defined(_M_AMD64)
    (void*&)function = _IMP_(__chkstk);
#endif
    return function();
}
//------------------------------------------------------------------------------
#if defined(_M_IX86)
extern "C" long long __declspec(naked) _allmul(long long a, long long b)
{
    __asm jmp _imp___allmul;
}
//------------------------------------------------------------------------------
extern "C" long long __declspec(naked) _aulldiv(long long a, long long b)
{
    __asm jmp _imp___aulldiv;
}
#endif
//------------------------------------------------------------------------------

#endif
