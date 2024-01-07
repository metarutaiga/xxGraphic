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
        reference.resize(1);

        size_t pos;
        Path = path;
        if ((pos = Path.rfind('/')) != std::string::npos)
            Path.resize(pos + 1);
        else if ((pos = Path.rfind('\\')) != std::string::npos)
            Path.resize(pos + 1);

        char signature[12];
        if (fread(signature, 12, 1, file) == 1 &&
            fread(&Version, 4, 1, file) == 1 &&
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
        reference.resize(1);

        char signature[12] = SIGNATURE;
        if (fwrite(signature, 12, 1, file) == 1 &&
            fwrite(&Version, 4, 1, file) == 1)
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
    if (file == nullptr || Safe == false)
        return false;
    called++;

    if (size != 0 && fread(data, size, 1, file) != 1)
    {
        failed = called;
        Safe = false;
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------
bool xxBinary::WriteBinary(void const* data, size_t size)
{
    FILE* file = reinterpret_cast<FILE*>(handle);
    if (file == nullptr || Safe == false)
        return false;
    called++;

    if (size != 0 && fwrite(data, size, 1, file) != 1)
    {
        failed = called;
        Safe = false;
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------
bool xxBinary::ReadSize(size_t& size)
{
    size = 0;
    size_t shift = 0;
    for (size_t i = 0; i < sizeof(size_t); ++i)
    {
        uint8_t byte = 0;
        if (Read(byte) == false)
            return false;
        size |= (byte & 0x7F) << shift;
        if ((byte & 0x80) == 0)
            break;
        shift += 7;
    }
    return true;
}
//------------------------------------------------------------------------------
bool xxBinary::WriteSize(size_t size)
{
    for (size_t i = 0; i < sizeof(size_t); ++i)
    {
        uint8_t byte = size & 0x7F;
        size >>= 7;
        if (size)
            byte |= 0x80;
        if (Write(byte) == false)
            return false;
        if ((byte & 0x80) == 0)
            break;
    }
    return true;
}
//------------------------------------------------------------------------------
bool xxBinary::ReadString(std::string& string)
{
    size_t length = 0;
    return ReadSize(length) && ReadArray(string.assign(length, 0).data(), length);
}
//------------------------------------------------------------------------------
bool xxBinary::WriteString(std::string const& string)
{
    size_t length = string.size();
    return WriteSize(length) && WriteArray(string.data(), length);
}
//==============================================================================
