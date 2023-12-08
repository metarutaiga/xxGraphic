//==============================================================================
// xxGraphic : Image Header
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxImage
{
public:
    virtual ~xxImage();

    static xxImagePtr Create(uint64_t format, int width, int height, int depth, int mipmap, int array);
    static xxImagePtr Create2D(uint64_t format, int width, int height, int mipmap);
    static xxImagePtr Create3D(uint64_t format, int width, int height, int depth, int mipmap);
    static xxImagePtr CreateCube(uint64_t format, int width, int height, int mipmap);

    void* operator () (int x, int y, int z, int mipmap, int array);

    void        Update(uint64_t device);

    uint64_t    GetTexture() const;
    uint64_t    GetSampler() const;

protected:
    xxImage(uint64_t format, int width, int height, int depth, int mipmap, int array);

    uint64_t    m_device = 0;
    uint64_t    m_texture = 0;
    uint64_t    m_sampler = 0;

    uint64_t    m_format = 0;
    int         m_width = 0;
    int         m_height = 0;
    int         m_depth = 0;
    int         m_mipmap = 0;
    int         m_array = 0;

    void*       m_images[16] = {};
    bool        m_imageModified = true;

public:
    bool        m_clampU = true;
    bool        m_clampV = true;
    bool        m_clampW = true;
    bool        m_filterMag = true;
    bool        m_filterMin = true;
    bool        m_filterMip = true;
    uint8_t     m_anisotropic = 1;
};
