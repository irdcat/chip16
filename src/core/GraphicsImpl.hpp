#pragma once

#include "Graphics.hpp"
#include "../log/Logger.hpp"
#include "../log/HexModificator.hpp"
#include "../log/NumberModificator.hpp"

class GraphicsImpl : public Graphics
{
public:
    GraphicsImpl();

    ~GraphicsImpl() = default;

    void initPalette() override;

    void loadPalette(const Palette& pal) override;

    const Palette& getPalette() const override;

    u32 getColorFromPalette(unsigned index) const override;

    void clearScreen() override;

    const std::vector<u8>& getScreenBuffer() const override;

    void setBackgroundColorIndex(u8 index) override;

    void setSpriteDimensions(u8 width, u8 height) override;

    bool drawSprite(u16 x, u16 y, std::vector<u8>::const_iterator start) override;

    void setHFlip(bool flip) override;

    void setVFlip(bool flip) override;

    bool isVBlank() const override;

    struct Registers
    {
        u8 bg;
        u8 spritew;
        u8 spriteh;
        bool hflip;
        bool vflip;
    };
    bool vblank;
    Registers& getRegisters();

private:
    static constexpr u16 PIXELS_PER_BYTE = 2;
    static constexpr u16 BITS_PER_PIXEL = 4;
    static constexpr u16 SCREEN_WIDTH = 320;
    static constexpr u16 SCREEN_HEIGHT = 240;

    std::vector<u8> buffer;
    Palette palette;
    Registers registers;

    static Logger LOG;
};