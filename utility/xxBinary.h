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
    int                                 version = 0x20240107;
    int                                 called = 0;
    int                                 failed = 0;

public:
    template<typename T> bool           Read(T& data)                           { return ReadBinary(&data, sizeof(T)); }
    template<typename T> bool           Write(T const& data)                    { return WriteBinary(&data, sizeof(T)); }
    template<typename T> bool           ReadArray(T* data, size_t size)         { return ReadBinary(data, sizeof(T) * size); }
    template<typename T> bool           WriteArray(T const* data, size_t size)  { return WriteBinary(data, sizeof(T) * size); }
    bool                                ReadString(std::string& string);
    bool                                WriteString(std::string const& string);

    std::string                         Path;

    template<class T>
    std::shared_ptr<T> ReadReference()
    {
        int16_t id = 0;
        if (Read(id) == false)
            return nullptr;

        size_t index = id;
        if (reference.size() < index || index == SIZE_T_MAX)
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
        size_t index = input ? reference.size() : SIZE_T_MAX;
        for (size_t i = 0; i < reference.size(); ++i)
        {
            if (reference[i].get() == input.get())
            {
                index = i;
                break;
            }
        }

        int16_t id = (int16_t)index;
        if (Write(id) == false)
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
