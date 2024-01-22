//==============================================================================
// xxGraphic : Binary Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxBinary
{
public:
    static xxNodePtr            Load(char const* name);
    static bool                 Save(char const* name, xxNodePtr const& node);

protected:
    xxBinary();
    virtual ~xxBinary();

    bool                        Read(void* data, size_t size);
    bool                        Write(void const* data, size_t size);

    xxUnknownPtr                ReadReferenceInternal(xxUnknownPtr (*create)(), bool(xxBinary::*read)(xxBinary&));
    bool                        WriteReferenceInternal(xxUnknownPtr const& unknown, bool(xxBinary::*write)(xxBinary&));

    xxFile*                     m_file = nullptr;
    std::vector<xxUnknownPtr>   m_reference;
    int                         m_called = 0;
    int                         m_failed = 0;

public:
    template<typename T> bool   Read(T& data)                           { return Read(&data, sizeof(T)); }
    template<typename T> bool   Write(T const& data)                    { return Write(&data, sizeof(T)); }
    template<typename T> bool   ReadArray(T* data, size_t size)         { return Read(data, sizeof(T) * size); }
    template<typename T> bool   WriteArray(T const* data, size_t size)  { return Write(data, sizeof(T) * size); }
    bool                        ReadSize(size_t& size);
    bool                        WriteSize(size_t size);
    bool                        ReadString(std::string& string);
    bool                        WriteString(std::string const& string);

    template<class T>
    std::shared_ptr<T> ReadReference()
    {
        auto create = []() { auto ptr = T::BinaryCreate(); return reinterpret_cast<xxUnknownPtr&>(ptr); };
        auto read = reinterpret_cast<bool(xxBinary::*)(xxBinary&)>(&T::BinaryRead);
        auto unknown = ReadReferenceInternal(create, read);
        return reinterpret_cast<std::shared_ptr<T>&>(unknown);
    }

    template<class T>
    bool WriteReference(std::shared_ptr<T> const& input)
    {
        auto unknown = reinterpret_cast<xxUnknownPtr const&>(input);
        auto write = reinterpret_cast<bool(xxBinary::*)(xxBinary&)>(&T::BinaryWrite);
        return WriteReferenceInternal(unknown, write);
    }

    std::string const           Path;
    int const                   Version = 0x20240121;
    bool const                  Safe = true;
};
