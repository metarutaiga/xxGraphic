//==============================================================================
// xxGraphic : Texture Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphic.h"
#include "xxTexture.h"

//==============================================================================
//  Texture
//==============================================================================
xxTexture::xxTexture()
{
    m_format = 0;
    m_width = 0;
    m_height = 0;
    m_depth = 0;
    m_mipmap = 0;
    m_array = 0;

    m_device = 0;
    m_texture = 0;
    m_sampler = 0;
}
//------------------------------------------------------------------------------
xxTexture::~xxTexture()
{
    xxDestroyTexture(m_texture);
    xxDestroySampler(m_sampler);
}
//------------------------------------------------------------------------------
void xxTexture::Create(uint32_t format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmap, uint32_t array)
{
    for (uint32_t i = 0; i < xxCountOf(m_images); ++i)
        m_images[i] = std::vector<char>();

    m_format = format;
    m_width = width;
    m_height = height;
    m_depth = depth;
    m_mipmap = mipmap;
    m_array = array;

    for (uint32_t i = 0; i < mipmap; ++i)
    {
        if (width == 0)
            width = 1;
        if (height == 0)
            height = 1;
        if (depth == 0)
            depth = 1;

        size_t size = width * height * depth * array * xxSizeOf(uint32_t);
        m_images[i].resize(size);

        width <<= 1;
        height <<= 1;
        depth <<= 1;
    }
}
//------------------------------------------------------------------------------
void xxTexture::Create2D(uint32_t format, uint32_t width, uint32_t height, uint32_t mipmap)
{
    Create(format, width, height, 1, mipmap, 1);
}
//------------------------------------------------------------------------------
void xxTexture::Create3D(uint32_t format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmap)
{
    Create(format, width, height, depth, mipmap, 1);
}
//------------------------------------------------------------------------------
void xxTexture::CreateCube(uint32_t format, uint32_t width, uint32_t height, uint32_t mipmap)
{
    Create(format, width, height, 1, mipmap, 6);
}
//------------------------------------------------------------------------------
void* xxTexture::operator () (uint32_t x, uint32_t y, uint32_t z, uint32_t mipmap, uint32_t array)
{
    if (array >= m_array)
        return nullptr;
    if (mipmap >= m_mipmap)
        return nullptr;

    uint32_t levelWidth = (m_width << mipmap);
    uint32_t levelHeight = (m_height << mipmap);
    uint32_t levelDepth = (m_depth << mipmap);
    if (levelWidth == 0)
        levelWidth = 1;
    if (levelHeight == 0)
        levelHeight = 1;
    if (levelDepth == 0)
        levelDepth = 1;
    if (x >= levelWidth)
        return nullptr;
    if (y >= levelHeight)
        return nullptr;
    if (z >= levelDepth)
        return nullptr;

    uint32_t offset = 0;
    offset += z * (levelWidth * levelHeight);
    offset += y * (levelWidth);
    offset += x;

    if (mipmap >= xxCountOf(m_images))
        return nullptr;
    std::vector<char>& image = m_images[mipmap];
    if (image.empty())
        return nullptr;
    char* ptr = &image.front();

    return ptr + offset * xxSizeOf(uint32_t);
}
//==============================================================================
