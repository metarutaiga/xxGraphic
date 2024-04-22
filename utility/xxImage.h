//==============================================================================
// xxGraphic : Image Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxImage
{
public:
    void*               operator () (int x = 0, int y = 0, int z = 0, int mipmap = 0, int array = 0);

    void                Invalidate();
    void                Update(uint64_t device);

    uint64_t            GetTexture() const;
    uint64_t            GetSampler() const;

    static xxImagePtr   Create(uint64_t format, int width, int height, int depth, int mipmap, int array);
    static xxImagePtr   Create2D(uint64_t format, int width, int height, int mipmap);
    static xxImagePtr   Create3D(uint64_t format, int width, int height, int depth, int mipmap);
    static xxImagePtr   CreateCube(uint64_t format, int width, int height, int mipmap);

    static xxImagePtr (*BinaryCreate)();
    virtual void        BinaryRead(xxBinary& binary);
    virtual void        BinaryWrite(xxBinary& binary) const;

    static void       (*Loader)(xxImage& image, std::string const& path);

protected:
    xxImage(uint64_t format, int width, int height, int depth, int mipmap, int array);
    virtual ~xxImage();

    void                Initialize(uint64_t format, int width, int height, int depth, int mipmap, int array);

    uint64_t            m_device = 0;
    uint64_t            m_texture = 0;
    uint64_t            m_sampler = 0;

    void*               m_images[16] = {};
    bool                m_imageModified = true;

public:
    std::string         Name = "";

    uint64_t const      Format = 0;
    int const           Width = 0;
    int const           Height = 0;
    int const           Depth = 0;
    int const           Mipmap = 0;
    int const           Array = 0;

    bool                ClampU = true;
    bool                ClampV = true;
    bool                ClampW = true;
    bool                FilterMag = true;
    bool                FilterMin = true;
    bool                FilterMip = true;
    char                Anisotropic = 1;
};
