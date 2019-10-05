//==============================================================================
// xxGraphic : Direct3D Common Function
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

template <class T>
inline ULONG SafeRelease(T*& ptr)
{
    ULONG ref = 0;
    if (ptr)
    {
        ref = ptr->Release();
        ptr = nullptr;
    }
    return ref;
}

inline void PatchD3DIM(const char* name)
{
    void* d3dim = xxLoadLibrary(name);
    if (d3dim == nullptr)
        return;

    // https://github.com/UCyborg/LegacyD3DResolutionHack
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)d3dim;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((char*)dosHeader + dosHeader->e_lfanew);
    char* codeBase = (char*)d3dim + ntHeader->OptionalHeader.BaseOfCode;
    DWORD codeSize = ntHeader->OptionalHeader.SizeOfCode;

    const BYTE wantedBytes[] = { 0xB8, 0x00, 0x08, 0x00, 0x00, 0x39 };
    for (DWORD i = 0, size = codeSize - sizeof(wantedBytes); i < size; ++i)
    {
        if (memcmp(codeBase + i, wantedBytes, sizeof(wantedBytes)) == 0)
        {
            DWORD oldProtect;
            VirtualProtect(&codeBase[i + 1], 4, PAGE_EXECUTE_READWRITE, &oldProtect);
            codeBase[i + 1] = -1;
            codeBase[i + 2] = -1;
            codeBase[i + 3] = -1;
            codeBase[i + 4] = -1;
            VirtualProtect(&codeBase[i + 1], 4, oldProtect, &oldProtect);
        }
    }

    xxFreeLibrary(d3dim);
}
