#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/CpuImpl.hpp"
#include "../src/Memory.hpp"
#include "../src/Bus.hpp"

namespace
{
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
        MOCK_METHOD1(loadRomFromStream, void(std::istream&));
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

    class TimingInstructionsTests : public ::testing::Test
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

        const u16 NOP_INSTRUCTION_OPCODE = 0x0000;
        const u16 VBLNK_INSTRUCTION_OPCODE = 0x0200;
    };
};

TEST_F(TimingInstructionsTests, testNop)
{
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    testedCpu->executeInstruction(NOP_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(TimingInstructionsTests, testVBlnk_vBlankNotSet)
{
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    EXPECT_CALL(*bus, isVBlank).Times(1).WillOnce(Return(false));
    testedCpu->executeInstruction(VBLNK_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x100, regs.pc);
}

TEST_F(TimingInstructionsTests, testVBlnk_vBlankSet)
{
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    EXPECT_CALL(*bus, isVBlank).Times(1).WillOnce(Return(true));
    testedCpu->executeInstruction(VBLNK_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x104, regs.pc);
}