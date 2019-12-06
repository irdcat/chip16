#pragma once

#include "Types.hpp"

class Graphics
{
public:
    virtual ~Graphics() = default;

    virtual void initPalette() = 0;

    virtual void loadPalette(const Palette& palette) = 0;

    virtual const Palette& getPalette() const = 0;

    virtual u32 getColorFromPalette(unsigned index) const = 0;
};