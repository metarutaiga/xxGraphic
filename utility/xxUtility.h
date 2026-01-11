//==============================================================================
// xxGraphic : Utility Header
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include <xxGraphic/xxSystem.h>
#include "xxMath.h"

#include <algorithm>
#include <array>
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

#ifndef xxUtilitySmartPointer
#define xxUtilitySmartPointer
typedef std::shared_ptr<class xxCamera> xxCameraPtr;
typedef std::shared_ptr<class xxMaterial> xxMaterialPtr;
typedef std::shared_ptr<class xxMesh> xxMeshPtr;
typedef std::shared_ptr<class xxModifier> xxModifierPtr;
typedef std::shared_ptr<class xxNode> xxNodePtr;
typedef std::shared_ptr<class xxTexture> xxTexturePtr;
typedef std::shared_ptr<void> xxUnknownPtr;
typedef std::weak_ptr<class xxNode> xxNodeWeakPtr;
typedef std::weak_ptr<class xxTexture> xxTextureWeakPtr;
#endif

struct xxConstantData
{
    uint64_t    device = 0;
    uint64_t    pipeline = 0;
    uint64_t    meshShader = 0;
    uint64_t    vertexShader = 0;
    uint64_t    fragmentShader = 0;
    uint64_t    meshConstant = 0;
    uint64_t    vertexConstant = 0;
    uint64_t    fragmentConstant = 0;
    int         meshConstantSize = 0;
    int         vertexConstantSize = 0;
    int         fragmentConstantSize = 0;
    int         meshTextureSlot = 0;
    int         vertexTextureSlot = 0;
    int         fragmentTextureSlot = 0;
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

template<class T>
struct xxStrideIterator
{
    xxStrideIterator(void* base, size_t stride, size_t size)
        :m_stride(stride)
        ,m_now(reinterpret_cast<char*>(base))
        ,m_begin(reinterpret_cast<char*>(base))
        ,m_end(reinterpret_cast<char*>(base) + stride * size)
    {
    }

    T& operator * () const
    {
        return reinterpret_cast<T&>(*m_now);
    }

    bool operator == (xxStrideIterator const& it) const
    {
        return m_now == it.m_now;
    }

    bool operator != (xxStrideIterator const& it) const
    {
        return m_now != it.m_now;
    }

    xxStrideIterator operator + (size_t offset) const
    {
        xxStrideIterator iterator = (*this);
        iterator.m_now = m_now + m_stride * offset;
        return iterator;
    }

    xxStrideIterator& operator ++ ()
    {
        m_now = m_now + m_stride;
        return (*this);
    }

    xxStrideIterator operator ++ (int)
    {
        xxStrideIterator iterator = (*this);
        m_now = m_now + m_stride;
        return iterator;
    }

    xxStrideIterator begin() const
    {
        xxStrideIterator iterator = (*this);
        iterator.m_now = m_begin;
        return iterator;
    }

    xxStrideIterator end() const
    {
        xxStrideIterator iterator = (*this);
        iterator.m_now = m_end;
        return iterator;
    }

private:
    char*   m_now;
    char*   m_begin;
    char*   m_end;
    size_t  m_stride;
};
