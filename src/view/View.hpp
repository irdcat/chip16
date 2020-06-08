#pragma once

class View
{
public:
    virtual ~View() = default;

    virtual void update(const double dt) = 0;
};