#pragma once

#include <vector>
#include "Types.hpp"

class Bus
{
public:
    virtual ~Bus() = default;

    /**
     * Loads given palette to graphics unit.
     *
     * @param palette Palette to load.
     */
    virtual void loadPalette(const Palette& palette) = 0;

    /**
     * Clears screen buffer.
     */
    virtual void clearScreen() = 0;

    /**
     * Sets background color index.
     *
     * @param index Color index.
     */
    virtual void setBackgroundColorIndex(u8 index) = 0;

    /**
     * Sets sprite dimensions.
     *
     * @param width Width of the sprite.
     * @param height Height of the sprite.
     */
    virtual void setSpriteDimensions(u8 width, u8 height) = 0;

    /**
     * Draws sprite at given position.
     *
     * @param x Position x.
     * @param y Position y.
     * @param start Iterator to first byte containing sprite data.
     * @return True if any pixel from drawed sprite collides with existing one, otherwise false.
     */
    virtual bool drawSprite(u16 x, u16 y, std::vector<u8>::const_iterator start) = 0;

    /**
     * Sets HFlip flag.
     *
     * @param flip Flip flag value.
     */
    virtual void setHFlip(bool flip) = 0;

    /**
     * Sets VFlip flag.
     *
     * @param flip Flip flag value.
     */
    virtual void setVFlip(bool flip) = 0;

    /**
     * Returns current VBlank flag value.
     *
     * @return True or false depending on VBlank status.
     */
    virtual bool isVBlank() const = 0;

    /**
     * Sets VBlank flag value.
     * 
     * @param value VBlank flag value to set.
     */
    virtual void setVBlank(bool value) const = 0;
};