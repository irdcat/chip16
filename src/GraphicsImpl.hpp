#pragma once

#include "Graphics.hpp"
#include "log/Logger.hpp"

class GraphicsImpl : public Graphics
{
public:
    GraphicsImpl() = default;

    ~GraphicsImpl() = default;

    void initPalette() override;

    void loadPalette(const Palette& pal) override;

    const Palette& getPalette() const override;

    u32 getColorFromPalette(unsigned index) const override;

private:
    Palette palette;

    static Logger LOG;
};