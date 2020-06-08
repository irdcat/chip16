#pragma once

#include <SFML/Graphics.hpp>

#include "AbstractGraphicsFacade.hpp"
#include "../core/Graphics.hpp"

class SFMLGraphicsFacadeImpl
    : public AbstractGraphicsFacade<sf::RenderTexture>
{
public:
    SFMLGraphicsFacadeImpl(const std::shared_ptr<GraphicsService<sf::RenderTexture>> &graphicsService, 
        const std::shared_ptr<Graphics> &chip16Graphics);

    ~SFMLGraphicsFacadeImpl() = default;

    void renderCurrentChip16State(sf::RenderTexture &graphicsBuffer) override;

private:
    std::shared_ptr<Graphics> chip16Graphics;
};