//==============================================================================
// xxGraphic : Binary Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxNode.h"
#include "xxFile.h"
#include "xxBinary.h"

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
xxNodePtr xxBinary::Load(char const* name)
{
    xxNodePtr node;

    xxFile* file = xxFile::Load(name);
    if (file)
    {
        xxBinary binary;

        binary.file = file;
        binary.reference.resize(1);
        const_cast<std::string&>(binary.Path) = xxFile::GetPath(name);

        char signature[12];
        if (file->Read(signature, 12) &&
            file->Read(const_cast<int*>(&binary.Version), 4) &&
            strcmp(signature, xxBINARY_SIGNATURE) == 0)
        {
            node = xxNode::BinaryCreate();
            if (node == nullptr || node->BinaryRead(binary) == false)
            {
                node = nullptr;
            }
        }

        delete file;
        file = nullptr;
    }

    return node;
}
//------------------------------------------------------------------------------
bool xxBinary::Save(char const* name, xxNodePtr const& node)
{
    bool succeed = false;

    xxFile* file = xxFile::Save(name);
    if (file)
    {
        xxBinary binary;

        binary.file = file;
        binary.reference.resize(1);
        const_cast<std::string&>(binary.Path) = xxFile::GetPath(name);

        char signature[12] = xxBINARY_SIGNATURE;
        if (file->Write(signature, 12) &&
            file->Write(&binary.Version, 4))
        {
            succeed = node->BinaryWrite(binary);
        }

        delete file;
        file = nullptr;
    }

    return succeed;
}
//------------------------------------------------------------------------------
bool xxBinary::Read(void* data, size_t size)
{
    if (file == nullptr || Safe == false)
        return false;
    called++;

    if (file->Read(data, size) == false)
    {
        failed = called;
        const_cast<bool&>(Safe) = false;
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------
bool xxBinary::Write(void const* data, size_t size)
{
    if (file == nullptr || Safe == false)
        return false;
    called++;

    if (file->Write(data, size) == false)
    {
        failed = called;
        const_cast<bool&>(Safe) = false;
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------
xxUnknownPtr xxBinary::ReadReferenceInternal(xxUnknownPtr (*create)(), bool(xxBinary::*read)(xxBinary&))
{
    if (Safe == false)
        return nullptr;

    size_t index = 0;
    if (ReadSize(index) == false)
        return nullptr;

    if (reference.size() < index)
        return nullptr;

    if (reference.size() == index)
    {
        xxUnknownPtr unknown = create();
        auto output = reinterpret_cast<std::shared_ptr<xxBinary>&>(unknown);
        if (output == nullptr || (output.get()->*read)(*this) == false)
        {
            const_cast<bool&>(Safe) = false;
            return nullptr;
        }
        reference.push_back(unknown);
    }

    return reinterpret_cast<xxUnknownPtr&>(reference[index]);
}
//------------------------------------------------------------------------------
bool xxBinary::WriteReferenceInternal(xxUnknownPtr const& unknown, bool(xxBinary::*write)(xxBinary&))
{
    if (Safe == false)
        return false;

    size_t index = reference.size();
    for (size_t i = 0; i < reference.size(); ++i)
    {
        if (reference[i] == unknown)
        {
            index = i;
            break;
        }
    }

    if (WriteSize(index) == false)
        return false;

    if (reference.size() == index)
    {
        auto input = reinterpret_cast<std::shared_ptr<xxBinary> const&>(unknown);
        if ((input.get()->*write)(*this) == false)
        {
            const_cast<bool&>(Safe) = false;
            return false;
        }
        reference.push_back(unknown);
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
    return ReadSize(length) && ReadArray(string.insert(string.end(), length, 0).base(), length);
}
//------------------------------------------------------------------------------
bool xxBinary::WriteString(std::string const& string)
{
    size_t length = string.size();
    return WriteSize(length) && WriteArray(string.data(), length);
}
//==============================================================================
