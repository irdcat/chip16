#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/BusImpl.hpp"
#include "../src/Graphics.hpp"

namespace
{
    using ::testing::_;

    class GraphicsMock : public Graphics
    {
    public:
        MOCK_METHOD0(initPalette, void());
        MOCK_METHOD1(loadPalette, void(const Palette&));
        MOCK_CONST_METHOD0(getPalette, const Palette&());
        MOCK_CONST_METHOD1(getColorFromPalette, u32(unsigned));
    };

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