//==============================================================================
// xxGraphic : File Header
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxFile
{
public:
    xxFile(void* file);
    virtual ~xxFile();

    virtual size_t      Read(void* data, size_t size);
    virtual size_t      Write(void const* data, size_t size);

    virtual size_t      Position() const;
    virtual size_t      Size() const;

    static xxFile*    (*Load)(char const* name);
    static xxFile*    (*Save)(char const* name);

    static std::string  GetName(char const* name, bool extension = false);
    static std::string  GetPath(char const* name);

protected:
    void*               m_handle = nullptr;
};
