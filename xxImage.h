//==============================================================================
// xxGraphic : Image Header
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"
#include "xxVector.h"

#include <memory>
#include <vector>

class xxImage;
typedef std::shared_ptr<xxImage> xxImagePtr;

class xxCPPAPI xxImage
{
public:
    virtual ~xxImage();

    void Create(uint32_t format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmap, uint32_t array);
    void Create2D(uint32_t format, uint32_t width, uint32_t height, uint32_t mipmap);
    void Create3D(uint32_t format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmap);
    void CreateCube(uint32_t format, uint32_t width, uint32_t height, uint32_t mipmap);

    void* operator () (uint32_t x, uint32_t y, uint32_t z, uint32_t mipmap, uint32_t array);

protected:
    xxImage();

    std::vector<char>   m_images[16];

    uint32_t            m_format;
    uint32_t            m_width;
    uint32_t            m_height;
    uint32_t            m_depth;
    uint32_t            m_mipmap;
    uint32_t            m_array;

    uint64_t            m_device;
    uint64_t            m_texture;
    uint64_t            m_sampler;
};
