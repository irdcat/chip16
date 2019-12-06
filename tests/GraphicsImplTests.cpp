#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/GraphicsImpl.hpp"

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