#pragma once

#include "Types.hpp"
#include <vector>

class Graphics
{
public:
    virtual ~Graphics() = default;

    /**
     * Initializes graphics unit with default palette.
     */
    virtual void initPalette() = 0;

    /**
     * Overrides current palette with given custom one.
     *
     * @param palette Custom palette.
     */
    virtual void loadPalette(const Palette& palette) = 0;

    /**
     * Returns current palette.
     * 
     * @return Current palette.
     */
    virtual const Palette& getPalette() const = 0;

    /**
     * Returns color from current palette at given index.
     * 
     * @param index Index of the color.
     * @return Color from palette.
     */
    virtual u32 getColorFromPalette(unsigned index) const = 0;

    /**
     * Clears background and foreground layer.
     */
    virtual void clearScreen() = 0;

    /**
     * Returns screen buffer contents.
     *
     * @return Screen buffer.
     */
    virtual const std::vector<u8>& getScreenBuffer() const = 0;

    /**
     * Sets background color index corresponding to the color in palette.
     * 
     * @param index Index of the color.
     */
    virtual void setBackgroundColorIndex(u8 index) = 0;

    /**
     * Gets currently set background color index.
     * 
     * @return Background Color Index.
     */
    virtual u8 getBackgroundColorIndex() = 0;

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
     * @param start iterator of the first element from memory.
     * @return True or false if any current pixel collided with one from sprite.
     */
    virtual bool drawSprite(u16 x, u16 y, std::vector<u8>::const_iterator start) = 0;

    /**
     * Set horizontal flip value.
     * 
     * @param flip Horizontal flip value.
     */
    virtual void setHFlip(bool flip) = 0;

    /**
     * Set vertical flip value.
     *
     * @param flip Vertical flip value.
     */
    virtual void setVFlip(bool flip) = 0;

    /**
     * Sets VBLANK flag value.
     * 
     * @param value VBLANK value.
     */
    virtual void setVBlank(bool value) = 0;

    /**
     * Returns VBlank flag status.
     *
     * @return True or false depending on VBlank status.
     */
    virtual bool isVBlank() const = 0;
};