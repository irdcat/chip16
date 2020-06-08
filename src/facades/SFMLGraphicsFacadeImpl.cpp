#include "SFMLGraphicsFacadeImpl.hpp"

SFMLGraphicsFacadeImpl::SFMLGraphicsFacadeImpl(const std::shared_ptr<GraphicsService<sf::RenderTexture>> &graphicsService,
    const std::shared_ptr<Graphics> &chip16Graphics)
    : chip16Graphics(chip16Graphics)
    , AbstractGraphicsFacade(graphicsService)
{
    this->chip16Graphics->setVBlank(true);
}

void SFMLGraphicsFacadeImpl::renderCurrentChip16State(sf::RenderTexture &graphicsBuffer)
{
    const std::vector<u8> chip16Buffer = chip16Graphics->getScreenBuffer();
    const Palette chip16Palette = chip16Graphics->getPalette();
    const unsigned bgColorIndex = chip16Graphics->getBackgroundColorIndex();

    graphicsService->convertFromChip16Buffer(chip16Buffer, graphicsBuffer, chip16Palette, bgColorIndex);
    chip16Graphics->setVBlank(true);
}