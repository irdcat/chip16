#pragma once

template <class GraphicsBuffer>
class GraphicsFacade
{
public:
    virtual ~GraphicsFacade() = default;

    virtual void renderCurrentChip16State(GraphicsBuffer &graphicsBuffer) = 0;
};