#pragma once

#include <vector>

#include "../core/Types.hpp"

template <class GraphicsBuffer>
class GraphicsService
{
public:
    virtual ~GraphicsService() = default;

    virtual void convertFromChip16Buffer(const std::vector<u8> &chip16Buffer, GraphicsBuffer &graphicsBuffer, const Palette &palette, const unsigned bgColorIndex) = 0;
};