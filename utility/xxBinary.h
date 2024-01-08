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
    static xxNodePtr                    Load(char const* name);
    static bool                         Save(char const* name, xxNodePtr const& node);

protected:
    xxBinary();
    virtual ~xxBinary();

    bool                                ReadBinary(void* data, size_t size);
    bool                                WriteBinary(void const* data, size_t size);

    std::shared_ptr<xxBinary>           ReadReferenceInternal(std::shared_ptr<xxBinary>(*create)(), bool(xxBinary::*read)(xxBinary&));
    bool                                WriteReferenceInternal(std::shared_ptr<xxBinary>& input, bool(xxBinary::*write)(xxBinary&));

    xxFile*                             file = nullptr;
    std::vector<std::shared_ptr<void>>  reference;
    int                                 called = 0;
    int                                 failed = 0;

public:
    template<typename T> bool           Read(T& data)                           { return ReadBinary(&data, sizeof(T)); }
    template<typename T> bool           Write(T const& data)                    { return WriteBinary(&data, sizeof(T)); }
    template<typename T> bool           ReadArray(T* data, size_t size)         { return ReadBinary(data, sizeof(T) * size); }
    template<typename T> bool           WriteArray(T const* data, size_t size)  { return WriteBinary(data, sizeof(T) * size); }
    bool                                ReadSize(size_t& size);
    bool                                WriteSize(size_t size);
    bool                                ReadString(std::string& string);
    bool                                WriteString(std::string const& string);

    template<class T>
    std::shared_ptr<T> ReadReference()
    {
        auto create = reinterpret_cast<std::shared_ptr<xxBinary>(*)()>(&T::Create);
        auto read = reinterpret_cast<bool(xxBinary::*)(xxBinary&)>(&T::BinaryRead);
        std::shared_ptr<xxBinary> pointer = ReadReferenceInternal(create, read);
        return reinterpret_cast<std::shared_ptr<T>&>(pointer);
    }

    template<class T>
    bool WriteReference(std::shared_ptr<T>& input)
    {
        auto pointer = reinterpret_cast<std::shared_ptr<xxBinary>&>(input);
        auto write = reinterpret_cast<bool(xxBinary::*)(xxBinary&)>(&T::BinaryWrite);
        return WriteReferenceInternal(pointer, write);
    }

    std::string const                   Path;
    int const                           Version = 0x20240108;
    bool const                          Safe = true;
};
