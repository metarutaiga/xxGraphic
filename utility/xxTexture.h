//==============================================================================
// xxGraphic : Texture Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxUtility.h"

class xxPlusAPI xxTexture
{
public:
    void                    Initialize(uint64_t format, int width, int height, int depth, int mipmap, int array);

    void*                   operator () (int x = 0, int y = 0, int z = 0, int mipmap = 0, int array = 0);

    void                    Invalidate();
    void                    Update(uint64_t device);

    static xxTexturePtr     Create(uint64_t format = 0, int width = 0, int height = 0, int depth = 0, int mipmap = 0, int array = 0);
    static xxTexturePtr     Create2D(uint64_t format, int width, int height, int mipmap);
    static xxTexturePtr     Create3D(uint64_t format, int width, int height, int depth, int mipmap);
    static xxTexturePtr     CreateCube(uint64_t format, int width, int height, int mipmap);

    static xxTexturePtr   (*BinaryCreate)();
    virtual void            BinaryRead(xxBinary& binary);
    virtual void            BinaryWrite(xxBinary& binary) const;

    static size_t         (*Calculate)(uint64_t format, int width, int height, int depth);
    static void           (*Loader)(xxTexturePtr& texture, std::string const& path);
    static void           (*Reader)(xxTexturePtr const& texture);

protected:
    xxTexture(uint64_t format, int width, int height, int depth, int mipmap, int array);
    virtual ~xxTexture();

    xxTextureWeakPtr        m_this;
    void*                   m_images[16] = {};
    bool                    m_imageModified = false;

public:
    std::string             Name = "";
    std::string             Path = "";

    uint64_t const          Format = 0;
    int const               Width = 0;
    int const               Height = 0;
    int const               Depth = 0;
    int const               Mipmap = 0;
    int const               Array = 0;

    bool                    ClampU = true;
    bool                    ClampV = true;
    bool                    ClampW = true;
    bool                    FilterMag = true;
    bool                    FilterMin = true;
    bool                    FilterMip = true;
    char                    Anisotropic = 1;

    uint64_t const          Device = 0;
    uint64_t const          Texture = 0;
    uint64_t const          Sampler = 0;
};
