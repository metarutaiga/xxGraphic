//==============================================================================
// xxGraphic : File Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxFile
{
public:
    xxFile(void* file);
    virtual ~xxFile();

    virtual bool        Read(void* data, size_t size);
    virtual bool        Write(void const* data, size_t size);

    static xxFile*    (*Load)(char const* name);
    static xxFile*    (*Save)(char const* name);

    static std::string  GetName(char const* name);
    static std::string  GetPath(char const* name);

protected:
    void*               m_handle = nullptr;
};
