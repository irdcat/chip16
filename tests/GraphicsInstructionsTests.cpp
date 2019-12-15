#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/CpuImpl.hpp"
#include "../src/Memory.hpp"
#include "../src/Bus.hpp"

namespace
{
    using ::testing::Eq;
    using ::testing::Return;

    class MemoryMock : public Memory
    {
    public:
        MOCK_CONST_METHOD1(readByte, u8(u16));
        MOCK_METHOD2(writeByte, void(u16, u8));
        MOCK_CONST_METHOD1(readWord, u16(u16));
        MOCK_METHOD2(writeWord, void(u16, u16));
        MOCK_CONST_METHOD1(readControllerState, ControllerState(unsigned));
        MOCK_CONST_METHOD1(readByteReference, std::vector<u8>::const_iterator(u16));
    };

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

    class GraphicsInstructionsTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            memory = std::make_shared<MemoryMock>();
            bus = std::make_shared<BusMock>();
            testedCpu = std::make_unique<CpuImpl>(memory, bus);
        }

        std::unique_ptr<CpuImpl> testedCpu;
        std::shared_ptr<MemoryMock> memory;
        std::shared_ptr<BusMock> bus;

        const u16 CLEAR_SCREEN_INSTRUCTION_OPCODE = 0x0100;
        const u16 BACKGROUND_COLOR_INSTRUCTION_OPCODE = 0x0300;
        const u16 SPRITE_DIMENSIONS_INSTRUCTION_OPCODE = 0x0400;
        const u16 DRAW_SPRITE_IMMEDATE_INSTRUCTION_OPCODE = 0x0500;
        const u16 DRAW_SPRITE_INDIRECT_INSTRUCTION_OPCODE = 0x0600;
        const u16 FLIP_INSTRUCTION_OPCODE = 0x0800;
    };
};

TEST_F(GraphicsInstructionsTests, testClearScreen)
{
    EXPECT_CALL(*bus, clearScreen).Times(1);
    testedCpu->executeInstruction(CLEAR_SCREEN_INSTRUCTION_OPCODE);
}

TEST_F(GraphicsInstructionsTests, testBackgroundColor)
{
    const auto COLOR_INDEX = 0xF;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    EXPECT_CALL(*bus, setBackgroundColorIndex(0xF)).Times(1);
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(COLOR_INDEX << 8));
    testedCpu->executeInstruction(BACKGROUND_COLOR_INSTRUCTION_OPCODE);
}

TEST_F(GraphicsInstructionsTests, testSpriteDimensions)
{
    const auto SPRITE_WIDTH = 24;
    const auto SPRITE_HEIGHT = 16;
    const auto WORD = (SPRITE_WIDTH << 8) + SPRITE_HEIGHT;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    EXPECT_CALL(*bus, setSpriteDimensions(24, 16)).Times(1);
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(WORD));
    testedCpu->executeInstruction(SPRITE_DIMENSIONS_INSTRUCTION_OPCODE);
}

TEST_F(GraphicsInstructionsTests, testDrawSpriteImmedate_notCollided)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    const std::vector<u8> TEST_SPRITE = {
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };

    regs.r[REG_INDEX_X] = 1;
    regs.r[REG_INDEX_Y] = 5;
    regs.pc = 0x102;
    regs.flags.c = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x2000));
    EXPECT_CALL(*memory, readByteReference(0x2000)).Times(1).WillOnce(Return(TEST_SPRITE.begin()));
    EXPECT_CALL(*bus, drawSprite(1, 5, Eq(TEST_SPRITE.begin()))).Times(1).WillOnce(Return(false));
    testedCpu->executeInstruction(DRAW_SPRITE_IMMEDATE_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(GraphicsInstructionsTests, testDrawSpriteImmedate_collided)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    const std::vector<u8> TEST_SPRITE = {
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };

    regs.r[REG_INDEX_X] = 1;
    regs.r[REG_INDEX_Y] = 5;
    regs.pc = 0x102;
    regs.flags.c = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x2000));
    EXPECT_CALL(*memory, readByteReference(0x2000)).Times(1).WillOnce(Return(TEST_SPRITE.begin()));
    EXPECT_CALL(*bus, drawSprite(1, 5, Eq(TEST_SPRITE.begin()))).Times(1).WillOnce(Return(true));
    testedCpu->executeInstruction(DRAW_SPRITE_IMMEDATE_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(GraphicsInstructionsTests, testDrawSpriteIndirect_notCollided)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    const std::vector<u8> TEST_SPRITE = {
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };

    regs.r[REG_INDEX_X] = 1;
    regs.r[REG_INDEX_Y] = 5;
    regs.r[REG_INDEX_Z] = 0x2000;
    regs.pc = 0x102;
    regs.flags.c = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    EXPECT_CALL(*memory, readByteReference(0x2000)).Times(1).WillOnce(Return(TEST_SPRITE.begin()));
    EXPECT_CALL(*bus, drawSprite(1, 5, Eq(TEST_SPRITE.begin()))).Times(1).WillOnce(Return(false));
    testedCpu->executeInstruction(DRAW_SPRITE_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(GraphicsInstructionsTests, testDrawSpriteIndirect_collided)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    const std::vector<u8> TEST_SPRITE = {
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };

    regs.r[REG_INDEX_X] = 1;
    regs.r[REG_INDEX_Y] = 5;
    regs.r[REG_INDEX_Z] = 0x2000;
    regs.pc = 0x102;
    regs.flags.c = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    EXPECT_CALL(*memory, readByteReference(0x2000)).Times(1).WillOnce(Return(TEST_SPRITE.begin()));
    EXPECT_CALL(*bus, drawSprite(1, 5, Eq(TEST_SPRITE.begin()))).Times(1).WillOnce(Return(true));
    testedCpu->executeInstruction(DRAW_SPRITE_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(GraphicsInstructionsTests, testFlip_verticalFalse_horizontalFalse)
{
    bool hFlip = false;
    bool vFlip = false;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return((hFlip << 1) + vFlip));
    EXPECT_CALL(*bus, setVFlip(false)).Times(1);
    EXPECT_CALL(*bus, setHFlip(false)).Times(1);
    testedCpu->executeInstruction(FLIP_INSTRUCTION_OPCODE);
}

TEST_F(GraphicsInstructionsTests, testFlip_verticalTrue_horizontalFalse)
{
    bool hFlip = false;
    bool vFlip = true;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return((hFlip << 1) + vFlip));
    EXPECT_CALL(*bus, setVFlip(true)).Times(1);
    EXPECT_CALL(*bus, setHFlip(false)).Times(1);
    testedCpu->executeInstruction(FLIP_INSTRUCTION_OPCODE);
}

TEST_F(GraphicsInstructionsTests, testFlip_verticalFalse_horizontalTrue)
{
    bool hFlip = true;
    bool vFlip = false;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return((hFlip << 1) + vFlip));
    EXPECT_CALL(*bus, setVFlip(false)).Times(1);
    EXPECT_CALL(*bus, setHFlip(true)).Times(1);
    testedCpu->executeInstruction(FLIP_INSTRUCTION_OPCODE);
}

TEST_F(GraphicsInstructionsTests, testFlip_verticalTrue_horizontalTrue)
{
    bool hFlip = true;
    bool vFlip = true;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return((hFlip << 1) + vFlip));
    EXPECT_CALL(*bus, setVFlip(true)).Times(1);
    EXPECT_CALL(*bus, setHFlip(true)).Times(1);
    testedCpu->executeInstruction(FLIP_INSTRUCTION_OPCODE);
}