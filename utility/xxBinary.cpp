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

        binary.m_file = file;
        binary.m_reference.resize(1);
        const_cast<std::string&>(binary.Path) = xxFile::GetPath(name);

        char signature[12];
        if (file->Read(signature, 12) == 12 &&
            file->Read(const_cast<int*>(&binary.Version), 4) == 4 &&
            strcmp(signature, xxBINARY_SIGNATURE) == 0)
        {
            node = xxNode::BinaryCreate();
            if (node)
            {
                node->BinaryRead(binary);
            }
            if (node == nullptr || binary.Safe == false)
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

        binary.m_file = file;
        binary.m_reference.resize(1);
        const_cast<std::string&>(binary.Path) = xxFile::GetPath(name);

        char signature[12] = xxBINARY_SIGNATURE;
        if (file->Write(signature, 12) == 12 &&
            file->Write(&binary.Version, 4) == 4)
        {
            if (node)
            {
                node->BinaryWrite(binary);
            }
            if (binary.Safe)
            {
                succeed = true;
            }
        }

        delete file;
        file = nullptr;
    }

    return succeed;
}
//------------------------------------------------------------------------------
bool xxBinary::Read(void* data, size_t size)
{
    if (m_file == nullptr || Safe == false)
        return false;
    m_called++;

    if (m_file->Read(data, size) != size)
    {
        m_failed = m_called;
        const_cast<bool&>(Safe) = false;
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------
bool xxBinary::Write(void const* data, size_t size)
{
    if (m_file == nullptr || Safe == false)
        return false;
    m_called++;

    if (m_file->Write(data, size) != size)
    {
        m_failed = m_called;
        const_cast<bool&>(Safe) = false;
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------
bool xxBinary::ReadReferenceInternal(xxUnknownPtr& unknown, xxUnknownPtr (*create)(), void(xxBinary::*read)(xxBinary&))
{
    if (Safe == false)
        return false;

    size_t index = 0;
    if (ReadSize(index) == false)
        return false;

    if (m_reference.size() < index)
        return false;

    if (m_reference.size() == index)
    {
        unknown = create();
        m_reference.push_back(unknown);
        auto& output = reinterpret_cast<std::shared_ptr<xxBinary>&>(unknown);
        if (output)
        {
            (output.get()->*read)(*this);
        }
        if (output == nullptr || Safe == false)
        {
            const_cast<bool&>(Safe) = false;
            return false;
        }
    }
    else
    {
        unknown = reinterpret_cast<xxUnknownPtr&>(m_reference[index]);
    }

    return true;
}
//------------------------------------------------------------------------------
bool xxBinary::WriteReferenceInternal(xxUnknownPtr const& unknown, void(xxBinary::*write)(xxBinary&))
{
    if (Safe == false)
        return false;

    size_t index = m_reference.size();
    for (size_t i = 0; i < m_reference.size(); ++i)
    {
        if (m_reference[i] == unknown)
        {
            index = i;
            break;
        }
    }

    if (WriteSize(index) == false)
        return false;

    if (m_reference.size() == index)
    {
        m_reference.push_back(unknown);
        auto& input = reinterpret_cast<std::shared_ptr<xxBinary> const&>(unknown);
        if (input)
        {
            (input.get()->*write)(*this);
        }
        if (Safe == false)
        {
            const_cast<bool&>(Safe) = false;
            return false;
        }
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
    return ReadSize(length) && ReadArray(&*string.insert(string.end(), length, 0), length);
}
//------------------------------------------------------------------------------
bool xxBinary::WriteString(std::string const& string)
{
    size_t length = string.size();
    return WriteSize(length) && WriteArray(string.data(), length);
}
//==============================================================================
