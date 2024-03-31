//==============================================================================
// xxGraphic : Modifier Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxBinary.h"
#include "xxModifier.h"

//==============================================================================
//  Modifier
//==============================================================================
xxModifier::xxModifier()
{
}
//------------------------------------------------------------------------------
xxModifier::~xxModifier()
{
}
//------------------------------------------------------------------------------
xxModifierPtr xxModifier::Create()
{
    xxModifierPtr Modifier = xxModifierPtr(new xxModifier(), [](xxModifier* Modifier) { delete Modifier; });
    if (Modifier == nullptr)
        return nullptr;

    return Modifier;
}
//==============================================================================
//  Binary
//==============================================================================
xxModifierPtr (*xxModifier::BinaryCreate)() = xxModifier::Create;
//------------------------------------------------------------------------------
void xxModifier::BinaryRead(xxBinary& binary)
{
    binary.ReadString(Name);

    binary.Read(DataType);

    size_t length = 0;
    binary.ReadSize(length);
    binary.ReadArray(Data.insert(Data.end(), length, 0).base(), length);
}
//------------------------------------------------------------------------------
void xxModifier::BinaryWrite(xxBinary& binary) const
{
    binary.WriteString(Name);

    binary.Write(DataType);

    size_t length = Data.size();
    binary.WriteSize(length);
    binary.WriteArray(Data.data(), length);
}
//==============================================================================
