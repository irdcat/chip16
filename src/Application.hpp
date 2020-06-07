#pragma once

#include <memory>
#include <SFML/Graphics.hpp>

#include <boost/di.hpp>

#include "facades/RomFacade.hpp"
#include "view/AbstractSFMLView.hpp"
#include "view/AbstractViewManager.hpp"

class Application
{
public:
    Application();

    ~Application() = default;

    void run(int argc, char ** argv);

private:
    sf::RenderWindow window;

    std::unique_ptr<AbstractViewManager<AbstractSFMLView>> viewManager;

    std::shared_ptr<RomFacade> romFacade;
};
