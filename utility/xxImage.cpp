//==============================================================================
// xxGraphic : Image Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphic.h"
#include "xxBinary.h"
#include "xxImage.h"

void (*xxImage::ImageLoader)(xxImage& image, std::string const& path) = [](xxImage&, std::string const&){};
//==============================================================================
//  Texture
//==============================================================================
xxImage::xxImage(uint64_t format, int width, int height, int depth, int mipmap, int array)
{
    Initialize(format, width, height, depth, mipmap, array);
}
//------------------------------------------------------------------------------
xxImage::~xxImage()
{
    Invalidate();
    for (void* image : m_images)
        xxFree(image);
}
//------------------------------------------------------------------------------
void xxImage::Initialize(uint64_t format, int width, int height, int depth, int mipmap, int array)
{
    const_cast<uint64_t&>(Format) = format;
    const_cast<int&>(Width) = width;
    const_cast<int&>(Height) = height;
    const_cast<int&>(Depth) = depth;
    const_cast<int&>(Mipmap) = mipmap;
    const_cast<int&>(Array) = array;

    if (width == 0 || height == 0 || depth == 0 || mipmap == 0 || array == 0)
        return;

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
}
//------------------------------------------------------------------------------
void* xxImage::operator () (int x, int y, int z, int mipmap, int array)
{
    if (array >= Array)
        return nullptr;
    if (mipmap >= Mipmap)
        return nullptr;

    int levelWidth = (Width << mipmap);
    int levelHeight = (Height << mipmap);
    int levelDepth = (Depth << mipmap);
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

    return reinterpret_cast<char*>(image) + offset * xxSizeOf(uint32_t);
}
//------------------------------------------------------------------------------
void xxImage::Invalidate()
{
    xxDestroyTexture(m_texture);
    xxDestroySampler(m_sampler);

    m_texture = 0;
    m_sampler = 0;

    m_imageModified = true;
}
//------------------------------------------------------------------------------
void xxImage::Update(uint64_t device)
{
    m_device = device;

    if (m_texture == 0)
    {
        m_texture = xxCreateTexture(m_device, 0, Width, Height, Depth, Mipmap, Array, nullptr);
    }
    if (m_sampler == 0)
    {
        m_sampler = xxCreateSampler(m_device, ClampU, ClampV, ClampW, FilterMag, FilterMin, FilterMip, Anisotropic);
    }

    if (m_imageModified == false)
        return;
    m_imageModified = false;

    for (int array = 0; array < Array; ++array)
    {
        for (int mipmap = 0; mipmap < Mipmap; ++mipmap)
        {
            void* source = (*this)(0, 0, 0, mipmap, array);
            if (source == nullptr)
                continue;

            int stride = 0;
            void* target = xxMapTexture(m_device, m_texture, &stride, mipmap, array);
            if (target == nullptr)
                continue;

            int levelWidth = (Width >> mipmap);
            int levelHeight = (Height >> mipmap);
            int levelDepth = (Depth >> mipmap);
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
                    source = reinterpret_cast<char*>(source) + levelWidth * sizeof(int);
                    target = reinterpret_cast<char*>(target) + stride;
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
//------------------------------------------------------------------------------
xxImagePtr xxImage::Create(uint64_t format, int width, int height, int depth, int mipmap, int array)
{
    xxImagePtr image = xxImagePtr(new xxImage(format, width, height, depth, mipmap, array), [](xxImage* image) { delete image; });
    if (image == nullptr)
        return nullptr;
    if (width && height && depth && mipmap && array && image->m_images[0] == nullptr)
        return nullptr;

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
//==============================================================================
//  Binary
//==============================================================================
xxImagePtr (*xxImage::BinaryCreate)() = []() { return xxImage::Create(0, 0, 0, 0, 0, 0); };
//------------------------------------------------------------------------------
void xxImage::BinaryRead(xxBinary& binary)
{
    binary.ReadString(Name);

    binary.Read(ClampU);
    binary.Read(ClampV);
    binary.Read(ClampW);
    binary.Read(FilterMag);
    binary.Read(FilterMin);
    binary.Read(FilterMip);
    binary.Read(Anisotropic);

    ImageLoader(*this, binary.Path);
}
//------------------------------------------------------------------------------
void xxImage::BinaryWrite(xxBinary& binary) const
{
    binary.WriteString(Name);

    binary.Write(ClampU);
    binary.Write(ClampV);
    binary.Write(ClampW);
    binary.Write(FilterMag);
    binary.Write(FilterMin);
    binary.Write(FilterMip);
    binary.Write(Anisotropic);
}
//==============================================================================
