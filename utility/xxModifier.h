//==============================================================================
// xxGraphic : Modifier Header
//
// Copyright (c) 2019-2026 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxModifier
{
public:
    typedef void  (xxModifier::*UpdateDeclaration)(void*, xxModifierData*, float);
    void                        UpdateDummy(void* target, xxModifierData* data, float time) {};
    UpdateDeclaration const     UpdateFunction = &xxModifier::UpdateDummy;

public:
    void                        Update(void* target, xxModifierData* data, float time) { ((*this).*UpdateFunction)(target, data, time); }

    static xxModifierPtr        Create(size_t size = 0);

    static xxModifierPtr      (*BinaryCreate)();
    virtual void                BinaryRead(xxBinary& binary);
    virtual void                BinaryWrite(xxBinary& binary) const;

    static void               (*Loader)(xxModifier& modifier, size_t type);

protected:
    xxModifier();
    virtual ~xxModifier();

public:
    size_t const                DataType = 0;
    std::vector<char>           Data;
    std::vector<xxModifierPtr>  Modifiers;
};
