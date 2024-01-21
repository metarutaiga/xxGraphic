//==============================================================================
// xxGraphic : Modifier Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxModifier
{
public:
    static xxModifierPtr    Create();

    void                  (*Update)(float time, void* target) = [](float, void*){};

    static xxModifierPtr  (*BinaryCreate)();
    virtual bool            BinaryRead(xxBinary& binary);
    virtual bool            BinaryWrite(xxBinary& binary) const;

protected:
    xxModifier();
    virtual ~xxModifier();

public:
    std::string             Name = "";

    size_t                  DataType = 0;
    std::vector<char>       Data;
};
