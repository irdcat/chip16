#pragma once

#include "Types.hpp"

class Bus
{
public:
    virtual ~Bus() = default;

    virtual void loadPalette(const Palette& palette) = 0;
};