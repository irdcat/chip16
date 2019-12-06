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