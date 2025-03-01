//==============================================================================
// xxGraphic : File Source
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxFile.h"

//==============================================================================
//  Binary
//==============================================================================
xxFile::xxFile(void* file)
{
    m_handle = file;
}
//------------------------------------------------------------------------------
xxFile::~xxFile()
{
    FILE* file = reinterpret_cast<FILE*>(m_handle);
    if (file)
    {
        fclose(file);
    }
}
//------------------------------------------------------------------------------
size_t xxFile::Read(void* data, size_t size)
{
    FILE* file = reinterpret_cast<FILE*>(m_handle);
    if (file == nullptr)
        return 0;

    return fread(data, 1, size, file);
}
//------------------------------------------------------------------------------
size_t xxFile::Write(void const* data, size_t size)
{
    FILE* file = reinterpret_cast<FILE*>(m_handle);
    if (file == nullptr)
        return 0;

    return fwrite(data, 1, size, file);;
}
//------------------------------------------------------------------------------
size_t xxFile::Position() const
{
    FILE* file = reinterpret_cast<FILE*>(m_handle);
    if (file == nullptr)
        return 0;

    size_t position = ftell(file);
    return position;
}
//------------------------------------------------------------------------------
size_t xxFile::Size() const
{
    FILE* file = reinterpret_cast<FILE*>(m_handle);
    if (file == nullptr)
        return 0;

    long position = ftell(file);
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, position, SEEK_SET);
    return size;
}
//------------------------------------------------------------------------------
xxFile* (*xxFile::Load)(char const* name) = [](char const* name) -> xxFile*
{
    FILE* file = fopen(name, "rb");
    if (file == nullptr)
        return nullptr;

    xxFile* output = new xxFile(file);
    if (output == nullptr)
    {
        fclose(file);
        return nullptr;
    }
    return output;
};
//------------------------------------------------------------------------------
xxFile* (*xxFile::Save)(char const* name) = [](char const* name) -> xxFile*
{
    FILE* file = fopen(name, "wb");
    if (file == nullptr)
        return nullptr;

    xxFile* output = new xxFile(file);
    if (output == nullptr)
    {
        fclose(file);
        return nullptr;
    }
    return output;
};
//------------------------------------------------------------------------------
std::string xxFile::GetName(char const* name, bool extension)
{
    size_t dot = std::string::npos;
    std::string output;
    for (char c; (c = *name); ++name)
    {
        if (c == '/' || c == '\\')
        {
            dot = std::string::npos;
            output.clear();
            continue;
        }
        if (c == '.' && extension == false)
        {
            dot = output.size();
        }
        output.push_back(c);
    }
    if (dot != std::string::npos)
        output.resize(dot);
    return output;
}
//------------------------------------------------------------------------------
std::string xxFile::GetPath(char const* name)
{
    size_t slash = std::string::npos;
    std::string output;
    for (char c; (c = *name); ++name)
    {
        if (c == '/' || c == '\\')
        {
            slash = output.size();
        }
        output.push_back(c);
    }
    if (slash != std::string::npos)
        output.resize(slash);
    return output;
}
//==============================================================================
