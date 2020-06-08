#pragma once

#include <memory>

#include "GraphicsFacade.hpp"
#include "../graphics/GraphicsService.hpp"

template <typename T>
class AbstractGraphicsFacade
    : public GraphicsFacade<T>
{
public:
    AbstractGraphicsFacade(const std::shared_ptr<GraphicsService<T>> &graphicsService);

    virtual ~AbstractGraphicsFacade() = default;

protected:
    std::shared_ptr<GraphicsService<T>> graphicsService;
};

template <typename T>
AbstractGraphicsFacade<T>::AbstractGraphicsFacade(const std::shared_ptr<GraphicsService<T>> &graphicsService)
    : graphicsService(graphicsService)
{
}
