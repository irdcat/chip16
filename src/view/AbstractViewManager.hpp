#pragma once

#include <list>
#include <memory>
#include "ViewManager.hpp"

template <class ViewType>
class AbstractViewManager
    : public ViewManager
{
public:
    ~AbstractViewManager() = default;

    void addView(const std::shared_ptr<ViewType> & view);

protected:
    std::list<std::shared_ptr<ViewType>> views;
};

template <class ViewType>
void AbstractViewManager<ViewType>::addView(const std::shared_ptr<ViewType> & view)
{
    views.push_back(view);
}
