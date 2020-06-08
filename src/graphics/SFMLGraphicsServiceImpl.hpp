#pragma once

#include <SFML/Graphics.hpp>
#include "GraphicsService.hpp"
#include "../log/Logger.hpp"

class SFMLGraphicsServiceImpl 
    : public GraphicsService<sf::RenderTexture>
{
public:
    using SFMLColorPalette = std::array<sf::Color, 16>;

    SFMLGraphicsServiceImpl() = default;

    ~SFMLGraphicsServiceImpl() = default;

    void convertFromChip16Buffer(const std::vector<u8> &chip16Buffer, sf::RenderTexture &graphicsBuffer, 
        const Palette &palette, const unsigned bgColorIndex) override;

private:
    SFMLColorPalette convertToSFMLColorPalette(const Palette &palette);

    sf::Color convertToSFMLColor(const u32 color);

    bool validateGraphicsBuffer(sf::RenderTexture &graphicsBuffer);

    static Logger LOG;
};