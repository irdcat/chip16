#pragma once

#include "View.hpp"

template <typename GraphicsBuffer>
class AbstractView
    : public View
{
public:
    virtual ~AbstractView() = default;

    const GraphicsBuffer& getGraphicsBuffer() const;

protected:
    GraphicsBuffer graphicsBuffer;
};

template <typename GraphicsBuffer>
const GraphicsBuffer& AbstractView<GraphicsBuffer>::getGraphicsBuffer() const
{
    return graphicsBuffer;
}
