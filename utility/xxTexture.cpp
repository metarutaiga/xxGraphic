//==============================================================================
// xxGraphic : Texture Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include <xxGraphic/xxGraphic.h>
#include "xxBinary.h"
#include "xxTexture.h"

//==============================================================================
//  Texture
//==============================================================================
xxTexture::xxTexture(uint64_t format, int width, int height, int depth, int mipmap, int array)
{
    if (format && width && height && depth && mipmap && array)
    {
        Initialize(format, width, height, depth, mipmap, array);
    }
}
//------------------------------------------------------------------------------
xxTexture::~xxTexture()
{
    Invalidate();
    for (void* image : m_images)
        xxFree(image);
}
//------------------------------------------------------------------------------
void xxTexture::Initialize(uint64_t format, int width, int height, int depth, int mipmap, int array)
{
    const_cast<uint64_t&>(Format) = format;
    const_cast<int&>(Width) = width;
    const_cast<int&>(Height) = height;
    const_cast<int&>(Depth) = depth;
    const_cast<int&>(Mipmap) = mipmap;
    const_cast<int&>(Array) = array;

    if (width == 0 || height == 0 || depth == 0 || mipmap == 0 || array == 0)
    {
        for (void*& image : m_images)
        {
            xxFree(image);
            image = nullptr;
        }
        return;
    }

    for (int i = 0; i < mipmap; ++i)
    {
        if (width == 0)
            width = 1;
        if (height == 0)
            height = 1;
        if (depth == 0)
            depth = 1;

        xxFree(m_images[i]);
        size_t size = Calculate(format, width, height, depth) * array;
        m_images[i] = xxAlloc(char, size);

        width >>= 1;
        height >>= 1;
        depth >>= 1;
    }

    Dirty = true;
}
//------------------------------------------------------------------------------
void* xxTexture::operator () (int x, int y, int z, int mipmap, int array)
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

    size_t offset = 0;
    offset += Calculate(Format, levelWidth, levelHeight, z);
    offset += Calculate(Format, levelWidth, y, 1);
    offset += Calculate(Format, x, 1, 1);

    void* image = m_images[mipmap];
    if (image == nullptr)
        return nullptr;

    return reinterpret_cast<char*>(image) + offset;
}
//------------------------------------------------------------------------------
void xxTexture::Invalidate()
{
    xxDestroyTexture(Texture);
    xxDestroySampler(Sampler);

    const_cast<uint64_t&>(Texture) = 0;
    const_cast<uint64_t&>(Sampler) = 0;

    Dirty = true;
}
//------------------------------------------------------------------------------
void xxTexture::Update(uint64_t device)
{
    const_cast<uint64_t&>(Device) = device;

    bool destroy = false;
    if (Texture == 0)
    {
        if (Format == 0)
        {
            destroy = true;
            Reader((xxTexturePtr&)m_this);
        }
        const_cast<uint64_t&>(Texture) = xxCreateTexture(device, Format, Width, Height, Depth, Mipmap, Array, nullptr);
    }
    if (Sampler == 0)
    {
        const_cast<uint64_t&>(Sampler) = xxCreateSampler(device, ClampU, ClampV, ClampW, FilterMag, FilterMin, FilterMip, Anisotropic);
    }

    if (Dirty == false)
        return;
    Dirty = false;

    for (int array = 0; array < Array; ++array)
    {
        for (int mipmap = 0; mipmap < Mipmap; ++mipmap)
        {
            void* source = (*this)(0, 0, 0, mipmap, array);
            if (source == nullptr)
                continue;

            int stride = 0;
            void* target = xxMapTexture(device, Texture, &stride, mipmap, array);
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

            size_t line = Calculate(Format, levelWidth, 1, 1);
            size_t face = Calculate(Format, levelWidth, levelHeight, 1);
            for (int depth = 0; depth < levelDepth; ++depth)
            {
                for (size_t offset = 0; offset < face; offset += line)
                {
                    memcpy(target, source, line);
                    source = reinterpret_cast<char*>(source) + line;
                    target = reinterpret_cast<char*>(target) + stride;
                }
            }

            xxUnmapTexture(device, Texture, mipmap, array);
        }
    }

    if (destroy)
    {
        for (void*& image : m_images)
        {
            xxFree(image);
            image = nullptr;
        }
    }
}
//------------------------------------------------------------------------------
xxTexturePtr xxTexture::Create(uint64_t format, int width, int height, int depth, int mipmap, int array)
{
    xxTexturePtr texture = xxTexturePtr(new xxTexture(format, width, height, depth, mipmap, array), [](xxTexture* texture) { delete texture; });
    if (texture == nullptr)
        return nullptr;
    if (width && height && depth && mipmap && array && texture->m_images[0] == nullptr)
        return nullptr;

    texture->m_this = texture;
    return texture;
}
//------------------------------------------------------------------------------
xxTexturePtr xxTexture::Create2D(uint64_t format, int width, int height, int mipmap)
{
    return Create(format, width, height, 1, mipmap, 1);
}
//------------------------------------------------------------------------------
xxTexturePtr xxTexture::Create3D(uint64_t format, int width, int height, int depth, int mipmap)
{
    return Create(format, width, height, depth, mipmap, 1);
}
//------------------------------------------------------------------------------
xxTexturePtr xxTexture::CreateCube(uint64_t format, int width, int height, int mipmap)
{
    return Create(format, width, height, 1, mipmap, 6);
}
//------------------------------------------------------------------------------
size_t (*xxTexture::Calculate)(uint64_t format, int width, int height, int depth) = [](uint64_t format, int width, int height, int depth)
{
    return width * height * depth * sizeof(uint32_t);
};
//------------------------------------------------------------------------------
void (*xxTexture::Loader)(xxTexturePtr& texture, std::string const& path) = [](xxTexturePtr&, std::string const&) {};
void (*xxTexture::Reader)(xxTexturePtr const& texture) = [](xxTexturePtr const&) {};
//==============================================================================
//  Binary
//==============================================================================
xxTexturePtr (*xxTexture::BinaryCreate)() = []() { return xxTexture::Create(); };
//------------------------------------------------------------------------------
void xxTexture::BinaryRead(xxBinary& binary)
{
    binary.ReadString(Name);

    binary.Read(ClampU);
    binary.Read(ClampV);
    binary.Read(ClampW);
    binary.Read(FilterMag);
    binary.Read(FilterMin);
    binary.Read(FilterMip);
    binary.Read(Anisotropic);
}
//------------------------------------------------------------------------------
void xxTexture::BinaryWrite(xxBinary& binary) const
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
