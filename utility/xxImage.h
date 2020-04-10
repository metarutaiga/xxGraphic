//==============================================================================
// xxGraphic : Image Header
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"
#include "xxMath.h"

#include <memory>
#include <vector>

class xxImage;
typedef std::shared_ptr<xxImage> xxImagePtr;

class xxCPPAPI xxImage
{
public:
    virtual ~xxImage();

    static xxImagePtr Create(uint32_t format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmap, uint32_t array);
    static xxImagePtr Create2D(uint32_t format, uint32_t width, uint32_t height, uint32_t mipmap);
    static xxImagePtr Create3D(uint32_t format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmap);
    static xxImagePtr CreateCube(uint32_t format, uint32_t width, uint32_t height, uint32_t mipmap);

    void* operator () (uint32_t x, uint32_t y, uint32_t z, uint32_t mipmap, uint32_t array);

    uint64_t GetTexture() const;
    uint64_t GetSampler() const;

    void Update(uint64_t device);

protected:
    xxImage(uint32_t format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmap, uint32_t array);

    std::vector<char>   m_images[16];

    uint32_t            m_format;
    uint32_t            m_width;
    uint32_t            m_height;
    uint32_t            m_depth;
    uint32_t            m_mipmap;
    uint32_t            m_array;

    bool                m_imageModified;

    uint64_t            m_device;
    uint64_t            m_texture;
    uint64_t            m_sampler;
};
