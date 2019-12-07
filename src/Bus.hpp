#pragma once

#include <vector>
#include "Types.hpp"

class Bus
{
public:
    virtual ~Bus() = default;

    virtual void loadPalette(const Palette& palette) = 0;

    virtual void clearScreen() = 0;

    virtual void setBackgroundColorIndex(u8 index) = 0;

    virtual void setSpriteDimensions(u8 width, u8 height) = 0;

    virtual bool drawSprite(u16 x, u16 y, std::vector<u8>::const_iterator start) = 0;

    virtual void setHFlip(bool flip) = 0;

    virtual void setVFlip(bool flip) = 0;
};