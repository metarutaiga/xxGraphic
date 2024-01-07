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
    xxBinary();
    virtual ~xxBinary();

    virtual xxNodePtr                   Load(char const* path);
    virtual bool                        Save(char const* path, xxNodePtr const& node);

protected:
    virtual bool                        ReadBinary(void* data, size_t size);
    virtual bool                        WriteBinary(void const* data, size_t size);

    void*                               handle = nullptr;
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

    std::string                         Path;
    int                                 Version = 0x20240108;
    bool                                Safe = true;

    template<class T>
    std::shared_ptr<T> ReadReference()
    {
        size_t index = 0;
        if (ReadSize(index) == false)
            return nullptr;

        if (reference.size() < index)
            return nullptr;

        if (reference.size() == index)
        {
            std::shared_ptr<T> output = T::Create();
            if (output == nullptr || output->BinaryRead(*this) == false)
                return nullptr;
            reference.push_back(output);
        }

        return reinterpret_cast<std::shared_ptr<T>&>(reference[index]);
    }

    template<class T>
    bool WriteReference(std::shared_ptr<T>& input)
    {
        size_t index = reference.size();
        for (size_t i = 0; i < reference.size(); ++i)
        {
            if (reference[i].get() == input.get())
            {
                index = i;
                break;
            }
        }

        if (WriteSize(index) == false)
            return false;

        if (reference.size() == index)
        {
            if (input->BinaryWrite(*this) == false)
                return false;
            reference.push_back(reinterpret_cast<std::shared_ptr<void>&>(input));
        }

        return true;
    }
};
