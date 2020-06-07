#include "SFMLGraphicsServiceImpl.hpp"

Logger SFMLGraphicsServiceImpl::LOG(STRINGIFY(SFMLGraphicsServiceImpl));

void SFMLGraphicsServiceImpl::convertFromChip16Buffer(const std::vector<u8> &chip16Buffer, sf::RenderTexture &graphicsBuffer, 
    const Palette &palette, const unsigned bgColorIndex) 
{
    LOG.info("Rendering Chip16 graphics buffer on SFML graphics buffer");
    if(!validateGraphicsBuffer(graphicsBuffer))
        return;
        
    const auto BUFFER_WIDTH = 320;
    const auto BUFFER_HEIGHT = 240;
    SFMLColorPalette sfmlColorPalette = convertToSFMLColorPalette(palette);
    sf::Color backgroundColor = sfmlColorPalette[bgColorIndex];
    sf::Image image;
    image.create(BUFFER_WIDTH, BUFFER_HEIGHT);

    auto counter = 0;
    for (auto & data : chip16Buffer)
    {
        sf::Color firstPixelColor = sfmlColorPalette[(data & 0xF0) >> 4];
        sf::Color secondPixelColor = sfmlColorPalette[data & 0x0F];
        sf::Vector2u firstPixelPosition((counter*2) % BUFFER_WIDTH, counter / BUFFER_HEIGHT);
        sf::Vector2u secondPixelPosition((((counter*2)+1) % BUFFER_WIDTH), counter / BUFFER_HEIGHT);

        image.setPixel(firstPixelPosition.x, firstPixelPosition.y, firstPixelColor);
        image.setPixel(secondPixelPosition.x, secondPixelPosition.y, secondPixelColor);

        counter++;
    }

    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite;
    sprite.setPosition(0,0);
    sprite.setTexture(texture);
    graphicsBuffer.clear(backgroundColor);
    graphicsBuffer.draw(sprite);
    graphicsBuffer.display();
}

SFMLGraphicsServiceImpl::SFMLColorPalette SFMLGraphicsServiceImpl::convertToSFMLColorPalette(const Palette &palette)
{
    SFMLColorPalette sfmlColorPalette;
    for(auto i = 0u; i < palette.size(); i++) 
    {
        sfmlColorPalette[i] = convertToSFMLColor(palette[i]);
    }
    return sfmlColorPalette;
}

sf::Color SFMLGraphicsServiceImpl::convertToSFMLColor(const u32 color)
{
    return sf::Color(color);
}

bool SFMLGraphicsServiceImpl::validateGraphicsBuffer(sf::RenderTexture &graphicsBuffer)
{
    sf::Vector2u bufferSize = graphicsBuffer.getTexture().getSize();
    if(bufferSize.x != 320u || bufferSize.y != 240u)
    {
        LOG.warn("Buffer size other than 320x240 is unsupported!");
        return false;
    }
    return true;
}