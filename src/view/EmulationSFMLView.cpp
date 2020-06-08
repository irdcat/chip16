#include "EmulationSFMLView.hpp"

EmulationSFMLView::EmulationSFMLView(const std::shared_ptr<GraphicsFacade<sf::RenderTexture>> &graphicsFacade,
        const std::shared_ptr<InstructionExecutionFacade> &instructionExecutionFacade)
        : graphicsFacade(graphicsFacade)
        , instructionExecutionFacade(instructionExecutionFacade)
{
    graphicsBuffer.create(320, 240);
    updateTimeCounter = 0;
}

void EmulationSFMLView::update(const double dt)
{
    updateTimeCounter+=dt;
    if(updateTimeCounter > 1/60)
    {
        graphicsFacade->renderCurrentChip16State(graphicsBuffer);
        updateTimeCounter = 0;
    }
    instructionExecutionFacade->executeInstruction();
}