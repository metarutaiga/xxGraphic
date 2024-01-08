//==============================================================================
// xxGraphic : File Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxFile.h"

//==============================================================================
//  Binary
//==============================================================================
xxFile::xxFile(void* file)
{
    handle = file;
}
//------------------------------------------------------------------------------
xxFile::~xxFile()
{
    FILE* file = reinterpret_cast<FILE*>(handle);
    if (file)
    {
        fclose(file);
    }
}
//------------------------------------------------------------------------------
bool xxFile::Read(void* data, size_t size)
{
    FILE* file = reinterpret_cast<FILE*>(handle);
    if (file == nullptr)
        return false;

    return (size == 0) || (fread(data, size, 1, file) == 1);
}
//------------------------------------------------------------------------------
bool xxFile::Write(void const* data, size_t size)
{
    FILE* file = reinterpret_cast<FILE*>(handle);
    if (file == nullptr)
        return false;

    return (size == 0) || (fwrite(data, size, 1, file) == 1);
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
std::string xxFile::GetName(char const* name)
{
    const char* leftName = strrchr(name, '/');
    if (leftName == nullptr)
        leftName = strrchr(name, '\\');

    if (leftName)
        leftName++;
    else if (leftName == nullptr)
        leftName = name;

    const char* rightName = strrchr(name, '.');
    if (rightName == nullptr)
        rightName = name + strlen(name);

    return std::string(leftName, rightName);
}
//------------------------------------------------------------------------------
std::string xxFile::GetPath(char const* name)
{
    size_t pos;
    std::string output = name;
    if ((pos = output.rfind('/')) != std::string::npos)
        output.resize(pos + 1);
    else if ((pos = output.rfind('\\')) != std::string::npos)
        output.resize(pos + 1);
    return output;
}
//==============================================================================
