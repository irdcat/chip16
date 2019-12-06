#pragma once

#include "Bus.hpp"
#include "Graphics.hpp"
#include "log/Logger.hpp"

class BusImpl : public Bus
{
public:
    BusImpl() = default;

    BusImpl(const std::shared_ptr<Graphics>& graphics);

    ~BusImpl() = default;

    void loadPalette(const Palette& palette) override;

private:
    std::shared_ptr<Graphics> graphics;

    static Logger LOG;
};