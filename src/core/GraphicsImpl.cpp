#include "GraphicsImpl.hpp"

Logger GraphicsImpl::LOG(STRINGIFY(GraphicsImpl));

GraphicsImpl::GraphicsImpl()
    : buffer(SCREEN_WIDTH * SCREEN_HEIGHT / PIXELS_PER_BYTE)
    , palette()
    , registers()
    , vblank(false)
{
    initPalette();
}

void GraphicsImpl::initPalette()
{
    LOG.debug("Initializing default palette");
    static const Palette defaultPalette = {
        0x00000000, 0x000000FF, 0x888888FF, 0xBF3932FF,
        0xDE7AAEFF, 0x4C3D21FF, 0x905F25FF, 0xE49452FF,
        0xEAD979FF, 0x537A3BFF, 0xABD54AFF, 0x252E38FF,
        0x00467FFF, 0x68ABCCFF, 0xBCDEE4FF, 0xFFFFFFFF
    };
    std::copy(defaultPalette.begin(), defaultPalette.end(), palette.begin());
}

void GraphicsImpl::loadPalette(const Palette& pal)
{
    LOG.debug("Loading palette.");
    std::copy(pal.begin(), pal.end(), palette.begin());
}

const Palette& GraphicsImpl::getPalette() const
{
    return palette;
}

u32 GraphicsImpl::getColorFromPalette(unsigned index) const
{
    if (index > palette.size() - 1)
        index = palette.size() - 1;

    return palette[index];
}

void GraphicsImpl::clearScreen()
{
    LOG.debug("Clearing screen.");
    for (auto byte : buffer)
        byte = 0;

    registers.bg = 0;
}

const std::vector<u8>& GraphicsImpl::getScreenBuffer() const
{
    return buffer;
}

void GraphicsImpl::setBackgroundColorIndex(u8 index)
{
    LOG.debug("Setting background color index to ", logHex(index));
    registers.bg = index;
}

u8 GraphicsImpl::getBackgroundColorIndex()
{
    return registers.bg;
}

void GraphicsImpl::setSpriteDimensions(u8 width, u8 height)
{
    LOG.debug("Setting sprite dimensions to [", logNumber(width), ",", logNumber(height), "]");
    registers.spritew = width;
    registers.spriteh = height;
}

bool GraphicsImpl::drawSprite(u16 x, u16 y, std::vector<u8>::const_iterator start)
{
    LOG.debug("Drawing sprite at position [", logNumber(x), ",", logNumber(y), "]");
    
    bool collision = false;
    const auto bytesToPut = registers.spritew * registers.spriteh;
    const auto spriteWidthInPixels = registers.spritew * PIXELS_PER_BYTE;
    auto put = [this](const auto addr, u8 data) {
        bool collision = false;
        static const auto LEFT_PIXEL_MASK = 0xF0;
        static const auto RIGHT_PIXEL_MASK = 0xF;
        u8 leftPixel = data & LEFT_PIXEL_MASK;
        u8 rightPixel = data & RIGHT_PIXEL_MASK;
        auto& memoryCell = buffer[addr];

        if ((leftPixel != 0 && rightPixel != 0))
        {
            collision = buffer[addr] != 0;
            memoryCell = leftPixel | rightPixel;
        }
        else if (leftPixel != 0 && rightPixel == 0)
        {
            const auto CURRENT_PIXEL_MASK = LEFT_PIXEL_MASK;
            const auto PIXEL_TO_ERASE_MASK = RIGHT_PIXEL_MASK;
            collision = (buffer[addr] & CURRENT_PIXEL_MASK) > 0;
            memoryCell &= PIXEL_TO_ERASE_MASK;
            memoryCell |= leftPixel;
        }
        else if (leftPixel == 0 && rightPixel != 0)
        {
            const auto CURRENT_PIXEL_MASK = RIGHT_PIXEL_MASK;
            const auto PIXEL_TO_ERASE_MASK = LEFT_PIXEL_MASK;
            collision = (buffer[addr] & CURRENT_PIXEL_MASK) > 0;
            memoryCell &= PIXEL_TO_ERASE_MASK;
            memoryCell |= rightPixel;
        }
        return collision;
    };

    auto incrementAndNormalizeOverflow = [](auto& value, auto incrementor, auto max) {
        value = (value + incrementor) % max;
    };

    auto decrementAndNormalizeUnderflow = [](auto& value, auto decrementor, auto max) {
        value = (decrementor > value ? max - (decrementor - value) : value - decrementor);
    };

    const u16 X_START = x, X_END = x + spriteWidthInPixels - 2;
    const u16 Y_START = y, Y_END = y + registers.spriteh - 1;
    const auto START_XPOS = (registers.hflip ? X_END : X_START);
    const auto START_YPOS = (registers.vflip ? Y_END : Y_START);
    for (u16 i = 0u, xPos = START_XPOS, yPos = START_YPOS; i < bytesToPut; i++, start++)
    {
        const u16 addr1 = (((xPos + 0) % SCREEN_WIDTH) + ((yPos % SCREEN_HEIGHT) * SCREEN_WIDTH)) / PIXELS_PER_BYTE;
        const u16 addr2 = (((xPos + 1) % SCREEN_WIDTH) + ((yPos % SCREEN_HEIGHT) * SCREEN_WIDTH)) / PIXELS_PER_BYTE;

        u8 pixelData = 0;
        if (registers.hflip)
            pixelData = ((*start & 0xF) << 4) | ((*start & 0xF0) >> 4);
        else
            pixelData = *start;

        const u8 data1 = pixelData >> (xPos % PIXELS_PER_BYTE) * BITS_PER_PIXEL;
        const u8 data2 = pixelData << (PIXELS_PER_BYTE - (xPos % PIXELS_PER_BYTE)) * BITS_PER_PIXEL;
        
        collision |= put(addr1, data1);
        collision |= put(addr2, data2);

        if (!registers.hflip)
        {
            incrementAndNormalizeOverflow(xPos, 2, SCREEN_WIDTH);
        }
        else
        {
            decrementAndNormalizeUnderflow(xPos, 2, SCREEN_WIDTH);
        }

        if (!registers.vflip && (registers.hflip ? xPos < X_START : xPos > X_END))
        {
            incrementAndNormalizeOverflow(yPos, 1, SCREEN_HEIGHT);
        }
        else if (registers.vflip && (registers.hflip ? xPos < X_START : xPos > X_END))
        {
            xPos = START_XPOS;
            decrementAndNormalizeUnderflow(yPos, 1, SCREEN_HEIGHT);
        }
    }

    return collision;
}

void GraphicsImpl::setHFlip(bool flip)
{
    LOG.debug("Setting horizontal flip to ", flip);
    registers.hflip = flip;
}

void GraphicsImpl::setVFlip(bool flip)
{
    LOG.debug("Setting vertical flip to ", flip);
    registers.vflip = flip;
}

void GraphicsImpl::setVBlank(bool value)
{
    vblank = value;
}

bool GraphicsImpl::isVBlank() const
{
    return vblank;
}

GraphicsImpl::Registers& GraphicsImpl::getRegisters()
{
    return registers;
}
