//==============================================================================
// xxGraphic : Image Source
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphic.h"
#include "xxImage.h"

//==============================================================================
//  Texture
//==============================================================================
xxImage::xxImage(uint32_t format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmap, uint32_t array)
{
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

        size_t size = xxSizeOf(uint32_t) * width * height * depth * array;
        m_images[i].resize(size);

        width >>= 1;
        height >>= 1;
        depth >>= 1;
    }

    m_imageModified = true;

    m_clampU = true;
    m_clampV = true;
    m_clampW = true;

    m_device = 0;
    m_texture = 0;
    m_sampler = 0;
}
//------------------------------------------------------------------------------
xxImage::~xxImage()
{
    xxDestroyTexture(m_texture);
    xxDestroySampler(m_sampler);
}
//------------------------------------------------------------------------------
xxImagePtr xxImage::Create(uint32_t format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmap, uint32_t array)
{
    return xxImagePtr(new xxImage(format, width, height, depth, mipmap, array));
}
//------------------------------------------------------------------------------
xxImagePtr xxImage::Create2D(uint32_t format, uint32_t width, uint32_t height, uint32_t mipmap)
{
    return Create(format, width, height, 1, mipmap, 1);
}
//------------------------------------------------------------------------------
xxImagePtr xxImage::Create3D(uint32_t format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmap)
{
    return Create(format, width, height, depth, mipmap, 1);
}
//------------------------------------------------------------------------------
xxImagePtr xxImage::CreateCube(uint32_t format, uint32_t width, uint32_t height, uint32_t mipmap)
{
    return Create(format, width, height, 1, mipmap, 6);
}
//------------------------------------------------------------------------------
void* xxImage::operator () (uint32_t x, uint32_t y, uint32_t z, uint32_t mipmap, uint32_t array)
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
    char* ptr = image.data();

    return ptr + offset * xxSizeOf(uint32_t);
}
//------------------------------------------------------------------------------
bool xxImage::GetClampU() const
{
    return m_clampU;
}
//------------------------------------------------------------------------------
bool xxImage::GetClampV() const
{
    return m_clampV;
}
//------------------------------------------------------------------------------
bool xxImage::GetClampW() const
{
    return m_clampW;
}
//------------------------------------------------------------------------------
void xxImage::SetClampU(bool clampU)
{
    m_clampU = clampU;
}
//------------------------------------------------------------------------------
void xxImage::SetClampV(bool clampV)
{
    m_clampV = clampV;
}
//------------------------------------------------------------------------------
void xxImage::SetClampW(bool clampW)
{
    m_clampW = clampW;
}
//------------------------------------------------------------------------------
uint64_t xxImage::GetTexture() const
{
    return m_texture;
}
//------------------------------------------------------------------------------
uint64_t xxImage::GetSampler() const
{
    return m_sampler;
}
//------------------------------------------------------------------------------
void xxImage::Update(uint64_t device)
{
    m_device = device;

    if (m_texture == 0)
    {
        m_texture = xxCreateTexture(m_device, 0, m_width, m_height, m_depth, m_mipmap, m_array, nullptr);
    }
    if (m_sampler == 0)
    {
        m_sampler = xxCreateSampler(m_device, m_clampU, m_clampV, m_clampW, true, true, true, 1);
    }

    if (m_imageModified == false)
        return;
    m_imageModified = false;

    for (uint32_t array = 0; array < m_array; ++array)
    {
        for (uint32_t mipmap = 0; mipmap < m_mipmap; ++mipmap)
        {
            void* source = (*this)(0, 0, 0, mipmap, array);
            if (source == nullptr)
                continue;

            int stride = 0;
            void* target = xxMapTexture(m_device, m_texture, &stride, mipmap, array);
            if (target == nullptr)
                continue;

            uint32_t levelWidth = (m_width >> mipmap);
            uint32_t levelHeight = (m_height >> mipmap);
            uint32_t levelDepth = (m_depth >> mipmap);
            if (levelWidth == 0)
                levelWidth = 1;
            if (levelHeight == 0)
                levelHeight = 1;
            if (levelDepth == 0)
                levelDepth = 1;

            for (uint32_t depth = 0; depth < levelDepth; ++depth)
            {
                for (uint32_t height = 0; height < levelHeight; ++height)
                {
                    memcpy(target, source, levelWidth * sizeof(int));
                    source = (char*)source + levelWidth * sizeof(int);
                    target = (char*)target + stride;
                }
            }

            xxUnmapTexture(m_device, m_texture, mipmap, array);
        }
    }
}
//==============================================================================
