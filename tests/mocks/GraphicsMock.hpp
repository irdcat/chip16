#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../src/core/Graphics.hpp"

class GraphicsMock : public Graphics
{
public:
    MOCK_METHOD0(initPalette, void());
    MOCK_METHOD1(loadPalette, void(const Palette&));
    MOCK_CONST_METHOD0(getPalette, const Palette& ());
    MOCK_CONST_METHOD1(getColorFromPalette, u32(unsigned));
    MOCK_METHOD0(clearScreen, void());
    MOCK_CONST_METHOD0(getScreenBuffer, const std::vector<u8>& ());
    MOCK_METHOD1(setBackgroundColorIndex, void(u8));
    MOCK_METHOD2(setSpriteDimensions, void(u8, u8));
    MOCK_METHOD3(drawSprite, bool(u16, u16, std::vector<u8>::const_iterator));
    MOCK_METHOD1(setHFlip, void(bool));
    MOCK_METHOD1(setVFlip, void(bool));
    MOCK_CONST_METHOD0(isVBlank, bool());
};