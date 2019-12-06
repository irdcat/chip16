#include "GraphicsImpl.hpp"

Logger GraphicsImpl::LOG(STRINGIFY(GraphicsImpl));

void GraphicsImpl::initPalette()
{
    LOG.debug("Initializing default palette");
    // Default palette
    const Palette defaultPalette = {
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
