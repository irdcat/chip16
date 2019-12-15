#pragma once

#include "Bus.hpp"
#include "Graphics.hpp"
#include "log/Logger.hpp"

class BusImpl : public Bus
{
public:
    BusImpl() = default;

    BusImpl(const std::shared_ptr<Graphics>& graphics);

    ~BusImpl() = default;

    void loadPalette(const Palette& palette) override;

    void clearScreen() override;

    void setBackgroundColorIndex(u8 index) override;

    void setSpriteDimensions(u8 width, u8 height) override;

    bool drawSprite(u16 x, u16 y, std::vector<u8>::const_iterator) override;

    void setHFlip(bool flip) override;

    void setVFlip(bool flip) override;

    bool isVBlank() const override;

private:
    std::shared_ptr<Graphics> graphics;

    static Logger LOG;
};