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
    };

    class BusMock : public Bus
    {
    public:
        MOCK_METHOD1(loadPalette, void(const Palette&));
    };

    class LoadInstructionsTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            memory = std::make_shared<MemoryMock>();
            testedCpu = std::make_unique<CpuImpl>(memory, bus);
        }

        std::unique_ptr<CpuImpl> testedCpu;
        std::shared_ptr<MemoryMock> memory;
        std::shared_ptr<BusMock> bus;

        const u16 LOAD_REGISTER_IMMEDATE_INSTRUCTION_OPCODE = 0x2000;
        const u16 LOAD_SP_IMMEDATE_INSTRUCTION_OPCODE = 0x2100;
        const u16 LOAD_REGISTER_INDIRECT_INSTRUCTION_OPCODE = 0x2200;
        const u16 LOAD_REGISTER_INDEXED_INSTRUCTION_OPCODE = 0x2300;
        const u16 MOVE_REGISTER_INSTRUCTION_OPCODE = 0x2400;
    };
};

TEST_F(LoadInstructionsTests, testLoadRegisterImmedate)
{
    // LDI Rx, value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x9876;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x5555));
    testedCpu->executeInstruction(LOAD_REGISTER_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x5555, regs.r[REG_INDEX]);
}

TEST_F(LoadInstructionsTests, testLoadStackPointerImmedate)
{
    // LDI SP, value
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0x9876;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x5555));
    testedCpu->executeInstruction(LOAD_SP_IMMEDATE_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x5555, regs.sp);
}

TEST_F(LoadInstructionsTests, testLoadRegisterIndirect)
{
    // LDM Rx, addr
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x9876;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x2222));
    EXPECT_CALL(*memory, readWord(0x2222)).Times(1).WillOnce(Return(0x5555));
    testedCpu->executeInstruction(LOAD_REGISTER_INDIRECT_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x5555, regs.r[REG_INDEX]);
}

TEST_F(LoadInstructionsTests, testLoadRegisterIndexed)
{
    // LDM Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.r[REG_INDEX_X] = 0x9876;
    regs.r[REG_INDEX_Y] = 0x2222;
    EXPECT_CALL(*memory, readWord(0x2222)).Times(1).WillOnce(Return(0x5555));
    testedCpu->executeInstruction(LOAD_REGISTER_INDEXED_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x5555, regs.r[REG_INDEX_X]);
}

TEST_F(LoadInstructionsTests, testMoveRegister)
{
    // MOV Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.r[REG_INDEX_X] = 0x9876;
    regs.r[REG_INDEX_Y] = 0x5555;
    testedCpu->executeInstruction(MOVE_REGISTER_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x5555, regs.r[REG_INDEX_X]);
}