#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../src/core/Bus.hpp"

class BusMock : public Bus
{
public:
    MOCK_METHOD1(loadPalette, void(const Palette&));
    MOCK_METHOD0(clearScreen, void());
    MOCK_METHOD1(setBackgroundColorIndex, void(u8));
    MOCK_METHOD2(setSpriteDimensions, void(u8, u8));
    MOCK_METHOD3(drawSprite, bool(u16, u16, std::vector<u8>::const_iterator));
    MOCK_METHOD1(setHFlip, void(bool));
    MOCK_METHOD1(setVFlip, void(bool));
    MOCK_CONST_METHOD0(isVBlank, bool());
};