#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../src/core/GraphicsImpl.hpp"

namespace
{
    class GraphicsImplTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            testedGraphics = std::make_unique<GraphicsImpl>();
        }

        std::unique_ptr<GraphicsImpl> testedGraphics;

        const Palette DEFAULT_PALETTE = {
            0x00000000, 0x000000FF, 0x888888FF, 0xBF3932FF,
            0xDE7AAEFF, 0x4C3D21FF, 0x905F25FF, 0xE49452FF,
            0xEAD979FF, 0x537A3BFF, 0xABD54AFF, 0x252E38FF,
            0x00467FFF, 0x68ABCCFF, 0xBCDEE4FF, 0xFFFFFFFF
        };
    };
};

TEST_F(GraphicsImplTests, testInitPalette)
{
    testedGraphics->initPalette();
    auto resultPalette = testedGraphics->getPalette();
    
    for (auto i = 0u; i < resultPalette.size(); i++)
        EXPECT_EQ(DEFAULT_PALETTE[i], resultPalette[i]);
}

TEST_F(GraphicsImplTests, testLoadPalette)
{
    const Palette TEST_PALETTE = {
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
    };
    testedGraphics->loadPalette(TEST_PALETTE);
    auto resultPalette = testedGraphics->getPalette();

    for (auto i = 0u; i < resultPalette.size(); i++)
        EXPECT_EQ(TEST_PALETTE[i], resultPalette[i]);
}

TEST_F(GraphicsImplTests, testClearScreen)
{
    testedGraphics->clearScreen();

    for (const auto byte : testedGraphics->getScreenBuffer())
        EXPECT_EQ(0, byte);
    EXPECT_EQ(0, testedGraphics->getRegisters().bg);
}

TEST_F(GraphicsImplTests, testDrawSprite_noFlip)
{
    const std::vector<u8> TEST_SPRITE = {
        0x34, 0x56, 0x78, 0x9A
    };
    const auto SPRITE_WIDTH = 4;
    const auto SPRITE_HEIGHT = 1;
    testedGraphics->setSpriteDimensions(SPRITE_WIDTH, SPRITE_HEIGHT);
    testedGraphics->setHFlip(false);
    testedGraphics->setVFlip(false);

    const auto SPRITE1_POSX = 3;
    const auto SPRITE1_POSY = 0;
    auto result = testedGraphics->drawSprite(SPRITE1_POSX, SPRITE1_POSY, TEST_SPRITE.begin());
    EXPECT_EQ(0, result);
    auto screenBuffer = testedGraphics->getScreenBuffer();
    EXPECT_EQ(0x03, screenBuffer[1]);
    EXPECT_EQ(0x45, screenBuffer[2]);
    EXPECT_EQ(0x67, screenBuffer[3]);
    EXPECT_EQ(0x89, screenBuffer[4]);
    EXPECT_EQ(0xA0, screenBuffer[5]);

    const auto SPRITE2_POSX = 6;
    const auto SPRITE2_POSY = 0;
    result = testedGraphics->drawSprite(SPRITE2_POSX, SPRITE2_POSY, TEST_SPRITE.begin());
    EXPECT_EQ(1, result);
    screenBuffer = testedGraphics->getScreenBuffer();
    EXPECT_EQ(0x03, screenBuffer[1]);
    EXPECT_EQ(0x45, screenBuffer[2]);
    EXPECT_EQ(0x34, screenBuffer[3]);
    EXPECT_EQ(0x56, screenBuffer[4]);
    EXPECT_EQ(0x78, screenBuffer[5]);
    EXPECT_EQ(0x9A, screenBuffer[6]);
}

TEST_F(GraphicsImplTests, testDrawSprite_hFlip)
{
    const std::vector<u8> TEST_SPRITE = {
        0x34, 0x56, 0x78, 0x9A
    };
    const auto SPRITE_WIDTH = 4;
    const auto SPRITE_HEIGHT = 1;
    testedGraphics->setSpriteDimensions(SPRITE_WIDTH, SPRITE_HEIGHT);
    testedGraphics->setHFlip(true);
    testedGraphics->setVFlip(false);

    const auto SPRITE1_POSX = 3;
    const auto SPRITE1_POSY = 0;
    auto result = testedGraphics->drawSprite(SPRITE1_POSX, SPRITE1_POSY, TEST_SPRITE.begin());
    EXPECT_EQ(0, result);
    auto screenBuffer = testedGraphics->getScreenBuffer();
    EXPECT_EQ(0x0A, screenBuffer[1]);
    EXPECT_EQ(0x98, screenBuffer[2]);
    EXPECT_EQ(0x76, screenBuffer[3]);
    EXPECT_EQ(0x54, screenBuffer[4]);
    EXPECT_EQ(0x30, screenBuffer[5]);

    const auto SPRITE2_POSX = 6;
    const auto SPRITE2_POSY = 0;
    result = testedGraphics->drawSprite(SPRITE2_POSX, SPRITE2_POSY, TEST_SPRITE.begin());
    EXPECT_EQ(1, result);
    screenBuffer = testedGraphics->getScreenBuffer();
    EXPECT_EQ(0x0A, screenBuffer[1]);
    EXPECT_EQ(0x98, screenBuffer[2]);
    EXPECT_EQ(0xA9, screenBuffer[3]);
    EXPECT_EQ(0x87, screenBuffer[4]);
    EXPECT_EQ(0x65, screenBuffer[5]);
    EXPECT_EQ(0x43, screenBuffer[6]);
}

TEST_F(GraphicsImplTests, testDrawSprite_vFlip)
{
    const std::vector<u8> TEST_SPRITE = {
        0x34, 0x56, 0x78, 0x9A,
        0xA9, 0x87, 0x65, 0x43
    };
    const auto SPRITE_WIDTH = 4;
    const auto SPRITE_HEIGHT = 2;
    testedGraphics->setSpriteDimensions(SPRITE_WIDTH, SPRITE_HEIGHT);
    testedGraphics->setHFlip(false);
    testedGraphics->setVFlip(true);

    const auto SPRITE1_POSX = 3;
    const auto SPRITE1_POSY = 0;
    auto result = testedGraphics->drawSprite(SPRITE1_POSX, SPRITE1_POSY, TEST_SPRITE.begin());
    EXPECT_EQ(0, result);
    auto screenBuffer = testedGraphics->getScreenBuffer();
    EXPECT_EQ(0x0A, screenBuffer[1]);
    EXPECT_EQ(0x98, screenBuffer[2]);
    EXPECT_EQ(0x76, screenBuffer[3]);
    EXPECT_EQ(0x54, screenBuffer[4]);
    EXPECT_EQ(0x30, screenBuffer[5]);

    const auto SPRITE2_POSX = 6;
    const auto SPRITE2_POSY = 0;
    result = testedGraphics->drawSprite(SPRITE2_POSX, SPRITE2_POSY, TEST_SPRITE.begin());
    EXPECT_EQ(1, result);
    screenBuffer = testedGraphics->getScreenBuffer();
    EXPECT_EQ(0x0A, screenBuffer[1]);
    EXPECT_EQ(0x98, screenBuffer[2]);
    EXPECT_EQ(0xA9, screenBuffer[3]);
    EXPECT_EQ(0x87, screenBuffer[4]);
    EXPECT_EQ(0x65, screenBuffer[5]);
    EXPECT_EQ(0x43, screenBuffer[6]);
}

TEST_F(GraphicsImplTests, testDrawSprite_hvFlip)
{
    const std::vector<u8> TEST_SPRITE = {
        0x34, 0x56, 0x78, 0x9A,
        0xA9, 0x87, 0x65, 0x43
    };
    const auto SPRITE_WIDTH = 4;
    const auto SPRITE_HEIGHT = 2;
    testedGraphics->setSpriteDimensions(SPRITE_WIDTH, SPRITE_HEIGHT);
    testedGraphics->setHFlip(true);
    testedGraphics->setVFlip(true);

    const auto SPRITE1_POSX = 3;
    const auto SPRITE1_POSY = 0;
    auto result = testedGraphics->drawSprite(SPRITE1_POSX, SPRITE1_POSY, TEST_SPRITE.begin());
    EXPECT_EQ(0, result);
    auto screenBuffer = testedGraphics->getScreenBuffer();
    EXPECT_EQ(0x03, screenBuffer[1]);
    EXPECT_EQ(0x45, screenBuffer[2]);
    EXPECT_EQ(0x67, screenBuffer[3]);
    EXPECT_EQ(0x89, screenBuffer[4]);
    EXPECT_EQ(0xA0, screenBuffer[5]);

    const auto SPRITE2_POSX = 6;
    const auto SPRITE2_POSY = 0;
    result = testedGraphics->drawSprite(SPRITE2_POSX, SPRITE2_POSY, TEST_SPRITE.begin());
    EXPECT_EQ(1, result);
    screenBuffer = testedGraphics->getScreenBuffer();
    EXPECT_EQ(0x03, screenBuffer[1]);
    EXPECT_EQ(0x45, screenBuffer[2]);
    EXPECT_EQ(0x34, screenBuffer[3]);
    EXPECT_EQ(0x56, screenBuffer[4]);
    EXPECT_EQ(0x78, screenBuffer[5]);
    EXPECT_EQ(0x9A, screenBuffer[6]);
}