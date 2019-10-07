//==============================================================================
// xxGraphic : MSVCRT minimal Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#ifndef _INC_SETJMPEX
#define _INC_SETJMPEX
#endif
#include <Windows.h>
#include <float.h>
#include <setjmp.h>
#include <stdio.h>
#include <intrin.h>
#include <limits.h>
#include <malloc.h>
#include <math.h>

#pragma comment(lib, "int64")

#ifndef _DEBUG

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
//==============================================================================
//  MSVCRT
//==============================================================================
extern "C" int _fltused = 1;
static HMODULE msvcrt = nullptr;
static HMODULE user32 = nullptr;
static HMODULE unicows = nullptr;
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
            msvcrt = LoadLibraryA("msvcrt20.dll");
        }
        if (user32 == nullptr)
        {
            user32 = LoadLibraryA("user32.dll");
        }
        if (unicows == nullptr)
        {
            unicows = LoadLibraryA("unicows.dll");
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
        if (user32)
        {
            FreeLibrary(user32);
            user32 = nullptr;
        }
        if (unicows)
        {
            FreeLibrary(unicows);
            unicows = nullptr;
        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
//------------------------------------------------------------------------------
static FILE** __iob;
static int (*__vfprintf)(FILE*, char const*, va_list);
static int (*__vsnprintf)(char*, size_t, char const*, va_list);
static int (*__vsscanf)(char const*, char const*, va_list);
//------------------------------------------------------------------------------
static FILE* acrt_iob_func(unsigned int i)
{
    return __iob[i];
}
//------------------------------------------------------------------------------
static int stdio_common_vfprintf(unsigned __int64 options, FILE* stream, char const* format, _locale_t locale, va_list argList)
{
    return __vfprintf(stream, format, argList);
}
//------------------------------------------------------------------------------
static int stdio_common_vsprintf(unsigned __int64 options, char* buffer, size_t bufferCount, char const* format, _locale_t locale, va_list argList)
{
    return __vsnprintf(buffer, bufferCount, format, argList);
}
//------------------------------------------------------------------------------
static int stdio_common_vsscanf(unsigned __int64 options, char const* buffer, size_t bufferCount, char const* format, _locale_t locale, va_list argList)
{
    return __vsscanf(buffer, format, argList);
}
//------------------------------------------------------------------------------
static void* aligned_malloc(size_t size, size_t alignment)
{
    size_t unalignedPtr = reinterpret_cast<size_t>(malloc(size + alignment));
    if (unalignedPtr == 0)
        return nullptr;
    char* alignedPtr = (char*)nullptr + ((unalignedPtr + alignment) & ~(alignment - 1));
    memcpy(alignedPtr - sizeof(size_t), &unalignedPtr, sizeof(size_t));
    return alignedPtr;
}
//------------------------------------------------------------------------------
static void* aligned_realloc(void* ptr, size_t size, size_t alignment)
{
    char* alignedPtr = (char*)ptr;
    if (alignedPtr == nullptr)
        return aligned_malloc(size, alignment);
    size_t unalignedPtr;
    memcpy(&unalignedPtr, alignedPtr - sizeof(size_t), sizeof(size_t));
    unalignedPtr = reinterpret_cast<size_t>(realloc((char*)nullptr + unalignedPtr, size + alignment));
    if (unalignedPtr == 0)
        return nullptr;
    alignedPtr = (char*)nullptr + ((unalignedPtr + alignment) & ~(alignment - 1));
    memcpy(alignedPtr - sizeof(size_t), &unalignedPtr, sizeof(size_t));
    return alignedPtr;
}
//------------------------------------------------------------------------------
static void aligned_free(void* ptr)
{
    char* alignedPtr = (char*)ptr;
    if (alignedPtr == nullptr)
        return;
    size_t unalignedPtr;
    memcpy(&unalignedPtr, alignedPtr - sizeof(size_t), sizeof(size_t));
    free((char*)nullptr + unalignedPtr);
}
//------------------------------------------------------------------------------
static void* getFunction(const char* name)
{
    if (name == "__acrt_iob_func")
    {
        if (__iob == nullptr)
        {
            (void*&)__iob = getFunction("_iob");
        }
        return acrt_iob_func;
    }
    if (name == "__stdio_common_vfprintf")
    {
        if (__vfprintf == nullptr)
        {
            (void*&)__vfprintf = getFunction("vfprintf");
        }
        return stdio_common_vfprintf;
    }
    if (name == "__stdio_common_vsprintf")
    {
        if (__vsnprintf == nullptr)
        {
            (void*&)__vsnprintf = getFunction("_vsnprintf");
        }
        return stdio_common_vsprintf;
    }
    if (name == "__stdio_common_vsscanf")
    {
        if (__vsnprintf == nullptr)
        {
            (void*&)__vsscanf = getFunction("vsscanf");
        }
        return stdio_common_vsscanf;
    }

    if (name == "_aligned_malloc")
        return aligned_malloc;
    if (name == "_aligned_realloc")
        return aligned_realloc;
    if (name == "_aligned_free")
        return aligned_free;

    void* function = nullptr;

    function = GetProcAddress(msvcrt, name);
    if (function)
        return function;

    static int (WINAPI * MessageBoxA)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
    if (MessageBoxA == nullptr)
    {
        (void*&)MessageBoxA = GetProcAddress(user32, "MessageBoxA");
    }
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
extern "C" void* _IMP_(atexit)                  = getFunction("atexit");
extern "C" void* _IMP_(atof)                    = getFunction("atof");
extern "C" void* _IMP_(ceil)                    = getFunction("ceil");
extern "C" void* _IMP_(floor)                   = getFunction("floor");
extern "C" void* _IMP_(calloc)                  = getFunction("calloc");
extern "C" void* _IMP_(malloc)                  = getFunction("malloc");
extern "C" void* _IMP_(realloc)                 = getFunction("realloc");
extern "C" void* _IMP_(free)                    = getFunction("free");
extern "C" void* _IMP_(_aligned_malloc)         = getFunction("_aligned_malloc");
extern "C" void* _IMP_(_aligned_realloc)        = getFunction("_aligned_realloc");
extern "C" void* _IMP_(_aligned_free)           = getFunction("_aligned_free");
extern "C" void* _IMP_(fclose)                  = getFunction("fclose");
extern "C" void* _IMP_(fflush)                  = getFunction("fflush");
extern "C" void* _IMP_(fopen)                   = getFunction("fopen");
extern "C" void* _IMP_(fread)                   = getFunction("fread");
extern "C" void* _IMP_(fseek)                   = getFunction("fseek");
extern "C" void* _IMP_(ftell)                   = getFunction("ftell");
extern "C" void* _IMP_(fwrite)                  = getFunction("fwrite");
extern "C" void* _IMP_(getenv)                  = getFunction("getenv");
extern "C" void* _IMP_(qsort)                   = getFunction("qsort");
extern "C" void* _IMP_(longjmp)                 = getFunction("longjmp");
extern "C" void* _IMP_(_setjmp)                 = getFunction("_setjmp");
extern "C" void* _IMP_(memchr)                  = getFunction("memchr");
extern "C" void* _IMP_(memcmp)                  = getFunction("memcmp");
extern "C" void* _IMP_(memcpy)                  = getFunction("memcpy");
extern "C" void* _IMP_(memmove)                 = getFunction("memmove");
extern "C" void* _IMP_(memset)                  = getFunction("memset");
extern "C" void* _IMP_(strchr)                  = getFunction("strchr");
extern "C" void* _IMP_(strcmp)                  = getFunction("strcmp");
extern "C" void* _IMP_(strncmp)                 = getFunction("strncmp");
extern "C" void* _IMP_(strncpy)                 = getFunction("strncpy");
extern "C" void* _IMP_(strrchr)                 = getFunction("strrchr");
extern "C" void* _IMP_(strstr)                  = getFunction("strstr");
extern "C" void* _IMP_(strtol)                  = getFunction("strtol");
extern "C" void* _IMP_(toupper)                 = getFunction("toupper");
extern "C" void* _IMP_(_wfopen)                 = getFunction("_wfopen");
extern "C" void* _IMP_(_CIfmod)                 = getFunction("_CIfmod");
extern "C" void* _IMP_(_CIacos)                 = getFunction("_CIacos");
extern "C" void* _IMP_(_CIpow)                  = getFunction("_CIpow");
extern "C" void* _IMP_(__acrt_iob_func)         = getFunction("__acrt_iob_func");
extern "C" void* _IMP_(__stdio_common_vfprintf) = getFunction("__stdio_common_vfprintf");
extern "C" void* _IMP_(__stdio_common_vsprintf) = getFunction("__stdio_common_vsprintf");
extern "C" void* _IMP_(__stdio_common_vsscanf)  = getFunction("__stdio_common_vsscanf");
//------------------------------------------------------------------------------
extern "C" int atexit(void (*f)(void))
{
    int (*function)(void (*f)(void));
    (void*&)function = _IMP_(atexit);
    return function(f);
}
//------------------------------------------------------------------------------
#pragma function(ceil)
extern "C" double ceil(double f)
{
    double (*function)(double f);
    (void*&)function = _IMP_(ceil);
    return function(f);
}
//------------------------------------------------------------------------------
#pragma function(floor)
extern "C" double floor(double f)
{
    double (*function)(double f);
    (void*&)function = _IMP_(floor);
    return function(f);
}
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
extern "C" char const* strchr(char const* a, int b)
{
    char* (*function)(char const* a, int b);
    (void*&)function = _IMP_(strchr);
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
extern "C" int setjmp(jmp_buf a)
{
    int (*function)(jmp_buf a);
    (void*&)function = _IMP_(_setjmp);
    return function(a);
}
//------------------------------------------------------------------------------
#if defined(_M_IX86)
extern "C" __declspec(naked) void _chkstk()
{
    __asm neg       eax
    __asm add       eax, esp
    __asm xchg      eax, esp
    __asm mov       eax, [eax]
    __asm mov       [esp], eax
    __asm ret
}
//------------------------------------------------------------------------------
extern "C" __declspec(naked) void _CIfmod()
{
    __asm jmp _IMP_(_CIfmod);
}
//------------------------------------------------------------------------------
extern "C" __declspec(naked) void _CIacos()
{
    __asm jmp _IMP_(_CIacos);
}
//------------------------------------------------------------------------------
extern "C" __declspec(naked) void* _CIpow()
{
    __asm jmp _IMP_(_CIpow);
}
//------------------------------------------------------------------------------
extern "C" __declspec(naked) void* _ftol2()
{
    __asm sub       esp, 12
    __asm fnstcw    [esp]
    __asm mov       ax, [esp]
    __asm or        ax, 0C00h
    __asm mov       [esp + 2], ax
    __asm fldcw     [esp + 2]
    __asm fistp     qword ptr [esp + 4]
    __asm fldcw     [esp]
    __asm pop       eax
    __asm pop       eax
    __asm pop       edx
    __asm ret
}
//------------------------------------------------------------------------------
extern "C" __declspec(naked) void* _ftol2_sse()
{
    __asm sub       esp, 12
    __asm fnstcw    [esp]
    __asm mov       ax, [esp]
    __asm or        ax, 0C00h
    __asm mov       [esp + 2], ax
    __asm fldcw     [esp + 2]
    __asm fistp     qword ptr [esp + 4]
    __asm fldcw     [esp]
    __asm pop       eax
    __asm pop       eax
    __asm pop       edx
    __asm ret
}
//------------------------------------------------------------------------------
extern "C" __declspec(naked) void _alloca_probe_16()
{
    __asm push      eax
    __asm lea       eax, [esp + 8]
    __asm sub       eax, [esp]
    __asm and       eax, 0Fh
    __asm add       eax, [esp]
    __asm add       esp, 4
    __asm jmp       _chkstk
}
#elif defined(_M_AMD64)
extern "C" void __chkstk()
{
}
#endif
//------------------------------------------------------------------------------

#endif
