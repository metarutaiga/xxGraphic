//==============================================================================
// xxGraphic : Binary Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxNode.h"
#include "xxBinary.h"

#define SIGNATURE "DoubleCross"

//==============================================================================
//  Binary
//==============================================================================
xxBinary::xxBinary()
{
}
//------------------------------------------------------------------------------
xxBinary::~xxBinary()
{

}
//------------------------------------------------------------------------------
xxNodePtr xxBinary::Load(char const* path)
{
    xxNodePtr node;

    FILE* file = fopen(path, "rb");
    if (file)
    {
        handle = file;

        size_t pos;
        Path = path;
        if ((pos = Path.rfind('/')) != std::string::npos)
            Path.resize(pos + 1);
        else if ((pos = Path.rfind('\\')) != std::string::npos)
            Path.resize(pos + 1);

        char signature[12];
        if (fread(signature, 12, 1, file) == 1 &&
            fread(&version, 4, 1, file) == 1 &&
            strcmp(signature, SIGNATURE) == 0)
        {
            node = xxNode::Create();
            if (node == nullptr || node->BinaryRead(*this) == false)
            {
                node = nullptr;
            }
        }

        fclose(file);
        handle = nullptr;
    }

    return node;
}
//------------------------------------------------------------------------------
bool xxBinary::Save(char const* path, xxNodePtr const& node)
{
    bool succeed = false;

    FILE* file = fopen(path, "wb");
    if (file)
    {
        handle = file;

        char signature[12] = SIGNATURE;
        if (fwrite(signature, 12, 1, file) == 1 &&
            fwrite(&version, 4, 1, file) == 1)
        {
            succeed = node->BinaryWrite(*this);
        }

        fclose(file);
        handle = nullptr;
    }

    return succeed;
}
//------------------------------------------------------------------------------
bool xxBinary::ReadBinary(void* data, size_t size)
{
    FILE* file = reinterpret_cast<FILE*>(handle);
    if (file == nullptr || failed != 0)
        return false;
    called++;

    if (size != 0 && fread(data, size, 1, file) != 1)
    {
        failed = called;
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------
bool xxBinary::WriteBinary(void const* data, size_t size)
{
    FILE* file = reinterpret_cast<FILE*>(handle);
    if (file == nullptr || failed != 0)
        return false;
    called++;

    if (size != 0 && fwrite(data, size, 1, file) != 1)
    {
        failed = called;
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------
bool xxBinary::ReadString(std::string& string)
{
    uint16_t length = 0;
    return Read(length) && ReadArray(string.assign(length, 0).data(), length);
}
//------------------------------------------------------------------------------
bool xxBinary::WriteString(std::string const& string)
{
    uint16_t length = (uint16_t)string.size();
    return Write(length) && WriteArray(string.data(), length);
}
//==============================================================================
