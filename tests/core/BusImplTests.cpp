#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../mocks/GraphicsMock.hpp"

#include "../../src/core/BusImpl.hpp"

namespace
{
    using ::testing::_;
    using ::testing::Return;

    class BusImplTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            graphics = std::make_shared<GraphicsMock>();
            testedBus = std::make_unique<BusImpl>(graphics);
        }

        std::unique_ptr<BusImpl> testedBus;
        std::shared_ptr<GraphicsMock> graphics;
    };
};

TEST_F(BusImplTests, testLoadPalette)
{
    const Palette TEST_PALETTE = {
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
    };

    EXPECT_CALL(*graphics, loadPalette(_)).Times(1);
    testedBus->loadPalette(TEST_PALETTE);
}

TEST_F(BusImplTests, testClearScreen)
{
    EXPECT_CALL(*graphics, clearScreen).Times(1);
    testedBus->clearScreen();
}

TEST_F(BusImplTests, testSetBackgroundColorIndex)
{
    EXPECT_CALL(*graphics, setBackgroundColorIndex(0xF)).Times(1);
    testedBus->setBackgroundColorIndex(0xF);
}

TEST_F(BusImplTests, testSetSpriteDimensions)
{
    EXPECT_CALL(*graphics, setSpriteDimensions(24, 36)).Times(1);
    testedBus->setSpriteDimensions(24, 36);
}

TEST_F(BusImplTests, testDrawSprite_notCollided)
{
    EXPECT_CALL(*graphics, drawSprite(98, 21, _)).Times(1).WillOnce(Return(false));
    auto result = testedBus->drawSprite(98, 21, std::vector<u8>::iterator());
    EXPECT_FALSE(result);
}

TEST_F(BusImplTests, testDrawSprite_collided)
{
    EXPECT_CALL(*graphics, drawSprite(98, 21, _)).Times(1).WillOnce(Return(true));
    auto result = testedBus->drawSprite(98, 21, std::vector<u8>::iterator());
    EXPECT_TRUE(result);
}

TEST_F(BusImplTests, testSetVerticalFlip)
{
    EXPECT_CALL(*graphics, setVFlip(true)).Times(1);
    testedBus->setVFlip(true);
}

TEST_F(BusImplTests, testSetHorizontalFlip)
{
    EXPECT_CALL(*graphics, setHFlip(true)).Times(1);
    testedBus->setHFlip(true);
}

TEST_F(BusImplTests, testIsVblank)
{
    EXPECT_CALL(*graphics, isVBlank).Times(1);
    testedBus->isVBlank();
}