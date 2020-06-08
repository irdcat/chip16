#pragma once

#include "AbstractSFMLView.hpp"
#include "AbstractViewManager.hpp"

class SFMLViewManager
    : public AbstractViewManager<AbstractSFMLView>
{
public:
    SFMLViewManager(sf::RenderWindow &window);

    ~SFMLViewManager() = default;

    void update(const double dt) override;

    void renderAll() override;

private:
    sf::RenderWindow& window;
};