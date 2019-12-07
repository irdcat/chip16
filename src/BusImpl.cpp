#include "BusImpl.hpp"

Logger BusImpl::LOG(STRINGIFY(BusImpl));

BusImpl::BusImpl(const std::shared_ptr<Graphics>& graphics)
    : graphics(graphics)
{
}

void BusImpl::loadPalette(const Palette& palette)
{
    LOG.debug("Loading palette.");
    graphics->loadPalette(palette);
}

void BusImpl::clearScreen()
{
    graphics->clearScreen();
}

void BusImpl::setBackgroundColorIndex(u8 index)
{
    graphics->setBackgroundColorIndex(index);
}

void BusImpl::setSpriteDimensions(u8 width, u8 height)
{
    graphics->setSpriteDimensions(width, height);
}

bool BusImpl::drawSprite(u16 x, u16 y, std::vector<u8>::const_iterator start)
{
    return graphics->drawSprite(x, y, start);
}

void BusImpl::setHFlip(bool flip)
{
    graphics->setHFlip(flip);
}

void BusImpl::setVFlip(bool flip)
{
    graphics->setVFlip(flip);
}
