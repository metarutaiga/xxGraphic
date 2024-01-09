//==============================================================================
// xxGraphic : Utility Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"
#include "xxMath.h"

#include <memory>
#include <string>
#include <vector>

#define xxBINARY_SIGNATURE "DoubleCross"

class xxBinary;
class xxFile;

struct xxConstantData;
struct xxDrawData;

typedef std::shared_ptr<class xxCamera> xxCameraPtr;
typedef std::shared_ptr<class xxImage> xxImagePtr;
typedef std::shared_ptr<class xxMaterial> xxMaterialPtr;
typedef std::shared_ptr<class xxMesh> xxMeshPtr;
typedef std::shared_ptr<class xxNode> xxNodePtr;
typedef std::shared_ptr<void> xxUnknownPtr;
