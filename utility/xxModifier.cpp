//==============================================================================
// xxGraphic : Modifier Source
//
// Copyright (c) 2019-2026 TAiGA
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
xxModifierPtr xxModifier::Create(size_t size)
{
    xxModifierPtr modifier = xxModifier::BinaryCreate();
    if (modifier == nullptr)
        return nullptr;

    modifier->Data.resize(size);
    return modifier;
}
//------------------------------------------------------------------------------
void (*xxModifier::Loader)(xxModifier& modifier, size_t type) = [](xxModifier&, size_t)
{
};
//==============================================================================
//  Binary
//==============================================================================
xxModifierPtr (*xxModifier::BinaryCreate)() = []() -> xxModifierPtr
{
    xxModifierPtr modifier = xxModifierPtr(new xxModifier(), [](xxModifier* Modifier) { delete Modifier; });
    if (modifier == nullptr)
        return nullptr;

    return modifier;
};
//------------------------------------------------------------------------------
void xxModifier::BinaryRead(xxBinary& binary)
{
    binary.Read(const_cast<size_t&>(DataType));

    size_t length = 0;
    binary.ReadSize(length);
    binary.ReadArray(&*Data.insert(Data.end(), length, 0), length);
    binary.ReadReferences(Modifiers);

    Loader(*this, DataType);
}
//------------------------------------------------------------------------------
void xxModifier::BinaryWrite(xxBinary& binary) const
{
    binary.Write(DataType);

    size_t length = Data.size();
    binary.WriteSize(length);
    binary.WriteArray(Data.data(), length);
    binary.WriteReferences(Modifiers);
}
//==============================================================================
