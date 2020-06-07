#include "SFMLViewManager.hpp"

SFMLViewManager::SFMLViewManager(sf::RenderWindow &window)
    : window(window)
{
}

void SFMLViewManager::update(const double dt)
{
    for(auto & view : views)
    {
        view->update(dt);
    }
}

void SFMLViewManager::renderAll()
{
    window.clear();
    for(auto & view : views)
    {
        sf::Sprite sprite;
        sprite.setPosition(0, 0);
        sprite.setTexture(view->getGraphicsBuffer().getTexture());
        window.draw(sprite);
    }
    window.display();
}