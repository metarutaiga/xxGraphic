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
xxImage::xxImage(uint64_t format, int width, int height, int depth, int mipmap, int array)
{
    m_format = format;
    m_width = width;
    m_height = height;
    m_depth = depth;
    m_mipmap = mipmap;
    m_array = array;

    for (int i = 0; i < mipmap; ++i)
    {
        if (width == 0)
            width = 1;
        if (height == 0)
            height = 1;
        if (depth == 0)
            depth = 1;

        xxFree(m_images[i]);
        size_t size = xxSizeOf(uint32_t) * width * height * depth * array;
        m_images[i] = xxAlloc(char, size);

        width >>= 1;
        height >>= 1;
        depth >>= 1;
    }

    m_imageModified = true;
}
//------------------------------------------------------------------------------
xxImage::~xxImage()
{
    xxDestroyTexture(m_texture);
    xxDestroySampler(m_sampler);
    for (void* image : m_images)
    {
        xxFree(image);
    }
}
//------------------------------------------------------------------------------
xxImagePtr xxImage::Create(uint64_t format, int width, int height, int depth, int mipmap, int array)
{
    xxImagePtr image = xxImagePtr(new xxImage(format, width, height, depth, mipmap, array));
    if (image == nullptr)
        return xxImagePtr();
    if (image->m_images[0] == nullptr)
        return xxImagePtr();

    return image;
}
//------------------------------------------------------------------------------
xxImagePtr xxImage::Create2D(uint64_t format, int width, int height, int mipmap)
{
    return Create(format, width, height, 1, mipmap, 1);
}
//------------------------------------------------------------------------------
xxImagePtr xxImage::Create3D(uint64_t format, int width, int height, int depth, int mipmap)
{
    return Create(format, width, height, depth, mipmap, 1);
}
//------------------------------------------------------------------------------
xxImagePtr xxImage::CreateCube(uint64_t format, int width, int height, int mipmap)
{
    return Create(format, width, height, 1, mipmap, 6);
}
//------------------------------------------------------------------------------
void* xxImage::operator () (int x, int y, int z, int mipmap, int array)
{
    if (array >= m_array)
        return nullptr;
    if (mipmap >= m_mipmap)
        return nullptr;

    int levelWidth = (m_width << mipmap);
    int levelHeight = (m_height << mipmap);
    int levelDepth = (m_depth << mipmap);
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

    int offset = 0;
    offset += z * (levelWidth * levelHeight);
    offset += y * (levelWidth);
    offset += x;

    void* image = m_images[mipmap];
    if (image == nullptr)
        return nullptr;

    return (char*)image + offset * xxSizeOf(uint32_t);
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
        m_sampler = xxCreateSampler(m_device, ClampU, ClampV, ClampW, FilterMag, FilterMin, FilterMip, Anisotropic);
    }

    if (m_imageModified == false)
        return;
    m_imageModified = false;

    for (int array = 0; array < m_array; ++array)
    {
        for (int mipmap = 0; mipmap < m_mipmap; ++mipmap)
        {
            void* source = (*this)(0, 0, 0, mipmap, array);
            if (source == nullptr)
                continue;

            int stride = 0;
            void* target = xxMapTexture(m_device, m_texture, &stride, mipmap, array);
            if (target == nullptr)
                continue;

            int levelWidth = (m_width >> mipmap);
            int levelHeight = (m_height >> mipmap);
            int levelDepth = (m_depth >> mipmap);
            if (levelWidth == 0)
                levelWidth = 1;
            if (levelHeight == 0)
                levelHeight = 1;
            if (levelDepth == 0)
                levelDepth = 1;

            for (int depth = 0; depth < levelDepth; ++depth)
            {
                for (int height = 0; height < levelHeight; ++height)
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
//==============================================================================
