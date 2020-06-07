#pragma once

#include <memory>

#include "AbstractSFMLView.hpp"
#include "../facades/GraphicsFacade.hpp"
#include "../facades/InstructionExecutionFacade.hpp"

class EmulationSFMLView
    : public AbstractSFMLView
{
public:
    EmulationSFMLView(const std::shared_ptr<GraphicsFacade<sf::RenderTexture>> &graphicsFacade,
        const std::shared_ptr<InstructionExecutionFacade> &instructionExecutionFacade);

    ~EmulationSFMLView() = default;

    void update(const double dt) override;

private:
    double updateTimeCounter;

    std::shared_ptr<GraphicsFacade<sf::RenderTexture>> graphicsFacade;

    std::shared_ptr<InstructionExecutionFacade> instructionExecutionFacade;
};