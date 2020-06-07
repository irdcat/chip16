#pragma once

class ViewManager
{
public:
    virtual ~ViewManager() = default;

    virtual void update(const double dt) = 0;

    virtual void renderAll() = 0;
};