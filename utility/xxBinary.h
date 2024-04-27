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

    virtual bool                Read(void* data, size_t size);
    virtual bool                Write(void const* data, size_t size);

    bool                        ReadReferenceInternal(xxUnknownPtr& unknown, xxUnknownPtr (*create)(), void(xxBinary::*read)(xxBinary&));
    bool                        WriteReferenceInternal(xxUnknownPtr const& unknown, void(xxBinary::*write)(xxBinary&));

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
    virtual bool                ReadString(std::string& string);
    virtual bool                WriteString(std::string const& string);

    template<class T>
    bool ReadReference(std::shared_ptr<T>& input)
    {
        auto& unknown = reinterpret_cast<xxUnknownPtr&>(input);
        auto create = []() { auto ptr = T::BinaryCreate(); return reinterpret_cast<xxUnknownPtr&>(ptr); };
        auto read = reinterpret_cast<void(xxBinary::*)(xxBinary&)>(&T::BinaryRead);
        return ReadReferenceInternal(unknown, create, read);
    }

    template<class T>
    bool WriteReference(std::shared_ptr<T> const& input)
    {
        auto& unknown = reinterpret_cast<xxUnknownPtr const&>(input);
        auto write = reinterpret_cast<void(xxBinary::*)(xxBinary&)>(&T::BinaryWrite);
        return WriteReferenceInternal(unknown, write);
    }

    template<class T>
    bool ReadReferences(std::vector<T>& container)
    {
        size_t count = 0;
        ReadSize(count);
        container.resize(count);
        for (size_t i = 0; i < count; ++i)
        {
            ReadReference(container[i]);
        }
        return true;
    }

    template<class T>
    bool WriteReferences(std::vector<T> const& container)
    {
        size_t count = container.size();
        WriteSize(count);
        for (size_t i = 0; i < count; ++i)
        {
            WriteReference(container[i]);
        }
        return true;
    }

    std::string const           Path;
    int const                   Version = 0x20240427;
    bool const                  Safe = true;
};
