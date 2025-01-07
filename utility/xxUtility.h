//==============================================================================
// xxGraphic : Utility Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include <xxGraphic/xxSystem.h>
#include "xxMath.h"

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <vector>

#if defined(_MSC_VER)
#pragma warning(disable:4251)
#endif

#define xxBINARY_SIGNATURE "DoubleCross"

class xxBinary;
class xxFile;

typedef std::shared_ptr<class xxCamera> xxCameraPtr;
typedef std::shared_ptr<class xxMaterial> xxMaterialPtr;
typedef std::shared_ptr<class xxMesh> xxMeshPtr;
typedef std::shared_ptr<class xxModifier> xxModifierPtr;
typedef std::shared_ptr<class xxNode> xxNodePtr;
typedef std::shared_ptr<class xxTexture> xxTexturePtr;
typedef std::shared_ptr<void> xxUnknownPtr;

typedef std::weak_ptr<class xxNode> xxNodeWeakPtr;
typedef std::weak_ptr<class xxTexture> xxTextureWeakPtr;

struct xxConstantData
{
    uint64_t    device = 0;
    uint64_t    pipeline = 0;
    uint64_t    vertexConstant = 0;
    uint64_t    fragmentConstant = 0;
    int         vertexConstantSize = 0;
    int         fragmentConstantSize = 0;
    int         ready = 0;
};

struct xxDrawData
{
    uint64_t                device = 0;
    uint64_t                commandEncoder = 0;
    xxCamera*               camera = nullptr;
    xxMatrix4x2*            frustum = nullptr;
    mutable xxConstantData* constantData = nullptr;
    mutable xxMesh*         mesh = nullptr;
    mutable xxNode*         node = nullptr;
    unsigned int            materialIndex = 0;
};

struct xxModifierData
{
    xxModifierPtr   modifier;
    float           time = FLT_MIN;
    size_t          index = 0;
};
