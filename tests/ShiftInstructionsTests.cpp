#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/CpuImpl.hpp"
#include "../src/Memory.hpp"

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

    class ShiftInstructionsTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            memory = std::make_shared<MemoryMock>();
            testedCpu = std::make_unique<CpuImpl>(memory);
        }

        std::unique_ptr<CpuImpl> testedCpu;
        std::shared_ptr<MemoryMock> memory;

        const u16 LOGICAL_SHIFT_LEFT_IMMEDATE_INSTRUCTION_OPCODE = 0xB000;
        const u16 LOGICAL_SHIFT_RIGHT_IMMEDATE_INSTRUCTION_OPCODE = 0xB100;
        const u16 ARITHMETIC_SHIFT_RIGHT_IMMEDATE_INSTRUCTION_OPCODE = 0xB200;
        const u16 LOGICAL_SHIFT_LEFT_INDIRECT_INSTRUCTION_OPCODE = 0xB300;
        const u16 LOGICAL_SHIFT_RIGHT_INDIRECT_INSTRUCTION_OPCODE = 0xB400;
        const u16 ARITHMETIC_SHIFT_RIGHT_INDIRECT_INSTRUCTION_OPCODE = 0xB500;
    };
};

TEST_F(ShiftInstructionsTests, testLogicalShiftLeftImmedate_isNegativeUnset)
{
    // SHL Rx, N
    const auto REG_INDEX = 5;
    const auto SHIFT = 1;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x3FFF;
    regs.flags.n = 1;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(SHIFT << 8));
    testedCpu->executeInstruction(LOGICAL_SHIFT_LEFT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x7FFE, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftLeftImmedate_isNegativeSet)
{
    // SHL Rx, N
    const auto REG_INDEX = 5;
    const auto SHIFT = 1;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x7FFF;
    regs.flags.n = 1;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(SHIFT << 8));
    testedCpu->executeInstruction(LOGICAL_SHIFT_LEFT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xFFFE, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftLeftImmedate_isZeroUnset)
{
    // SHL Rx, N
    const auto REG_INDEX = 5;
    const auto SHIFT = 1;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x3FFF;
    regs.flags.n = 1;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(SHIFT << 8));
    testedCpu->executeInstruction(LOGICAL_SHIFT_LEFT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x7FFE, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftLeftImmedate_isZeroSet)
{
    // SHL Rx, N
    const auto REG_INDEX = 5;
    const auto SHIFT = 1;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0;
    regs.flags.n = 1;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(SHIFT << 8));
    testedCpu->executeInstruction(LOGICAL_SHIFT_LEFT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftRightImmedate_isNegativeUnset)
{
    // SHR Rx, N
    const auto REG_INDEX = 5;
    const auto SHIFT = 1;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x7FFE;
    regs.flags.n = 1;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(SHIFT << 8));
    testedCpu->executeInstruction(LOGICAL_SHIFT_RIGHT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x3FFF, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftRightImmedate_isNegativeSet)
{
    // SHR Rx, N
    const auto REG_INDEX = 5;
    const auto SHIFT = 0;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xFFFE;
    regs.flags.n = 1;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(SHIFT << 8));
    testedCpu->executeInstruction(LOGICAL_SHIFT_RIGHT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xFFFE, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftRightImmedate_isZeroUnset)
{
    // SHR Rx, N
    const auto REG_INDEX = 5;
    const auto SHIFT = 1;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x7FFE;
    regs.flags.n = 1;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(SHIFT << 8));
    testedCpu->executeInstruction(LOGICAL_SHIFT_RIGHT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x3FFF, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftRightImmedate_isZeroSet)
{
    // SHR Rx, N
    const auto REG_INDEX = 5;
    const auto SHIFT = 1;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0;
    regs.flags.n = 1;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(SHIFT << 8));
    testedCpu->executeInstruction(LOGICAL_SHIFT_RIGHT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(ShiftInstructionsTests, testArithmeticShiftRightImmedate_isNegativeUnset)
{
    // SAR Rx, N
    const auto REG_INDEX = 5;
    const auto SHIFT = 1;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x7FFE;
    regs.flags.n = 1;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(SHIFT << 8));
    testedCpu->executeInstruction(ARITHMETIC_SHIFT_RIGHT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x3FFF, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(ShiftInstructionsTests, testArithmeticShiftRightImmedate_isNegativeSet)
{
    // SAR Rx, N
    const auto REG_INDEX = 5;
    const auto SHIFT = 1;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xFFFE;
    regs.flags.n = 1;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(SHIFT << 8));
    testedCpu->executeInstruction(ARITHMETIC_SHIFT_RIGHT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(ShiftInstructionsTests, testArithmeticShiftRightImmedate_isZeroUnset)
{
    // SAR Rx, N
    const auto REG_INDEX = 5;
    const auto SHIFT = 1;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x7FFE;
    regs.flags.n = 1;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(SHIFT << 8));
    testedCpu->executeInstruction(ARITHMETIC_SHIFT_RIGHT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x3FFF, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(ShiftInstructionsTests, testArithmeticShiftRightImmedate_isZeroSet)
{
    // SAR Rx, N
    const auto REG_INDEX = 5;
    const auto SHIFT = 1;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0;
    regs.flags.n = 1;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(SHIFT << 8));
    testedCpu->executeInstruction(ARITHMETIC_SHIFT_RIGHT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftLeftIndirect_isNegativeUnset)
{
    // SHL Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x3FFF;
    regs.r[REG_INDEX_Y] = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    testedCpu->executeInstruction(LOGICAL_SHIFT_LEFT_INDIRECT_INSTRUCTION_OPCODE 
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x7FFE, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftLeftIndirect_isNegativeSet)
{
    // SHL Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7FFF;
    regs.r[REG_INDEX_Y] = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    testedCpu->executeInstruction(LOGICAL_SHIFT_LEFT_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFFFE, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftLeftIndirect_isZeroUnset)
{
    // SHL Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x3FFF;
    regs.r[REG_INDEX_Y] = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    testedCpu->executeInstruction(LOGICAL_SHIFT_LEFT_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x7FFE, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftLeftIndirect_isZeroSet)
{
    // SHL Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    testedCpu->executeInstruction(LOGICAL_SHIFT_LEFT_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftRightIndirect_isNegativeUnset)
{
    // SHR Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7FFE;
    regs.r[REG_INDEX_Y] = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    testedCpu->executeInstruction(LOGICAL_SHIFT_RIGHT_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x3FFF, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftRightIndirect_isNegativeSet)
{
    // SHR Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFFE;
    regs.r[REG_INDEX_Y] = 0;
    regs.flags.n = 1;
    regs.flags.z = 0;
    testedCpu->executeInstruction(LOGICAL_SHIFT_RIGHT_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFFFE, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftRightIndirect_isZeroUnset)
{
    // SHR Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7FFE;
    regs.r[REG_INDEX_Y] = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    testedCpu->executeInstruction(LOGICAL_SHIFT_RIGHT_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x3FFF, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(ShiftInstructionsTests, testLogicalShiftRightIndirect_isZeroSet)
{
    // SHR Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    testedCpu->executeInstruction(LOGICAL_SHIFT_RIGHT_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(ShiftInstructionsTests, testArithmeticShiftRightIndirect_isNegativeUnset)
{
    // SAR Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7FFE;
    regs.r[REG_INDEX_Y] = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    testedCpu->executeInstruction(ARITHMETIC_SHIFT_RIGHT_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x3FFF, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(ShiftInstructionsTests, testArithmeticShiftRightIndirect_isNegativeSet)
{
    // SAR Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFFE;
    regs.r[REG_INDEX_Y] = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    testedCpu->executeInstruction(ARITHMETIC_SHIFT_RIGHT_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(ShiftInstructionsTests, testArithmeticShiftRightIndirect_isZeroUnset)
{
    // SAR Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7FFE;
    regs.r[REG_INDEX_Y] = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    testedCpu->executeInstruction(ARITHMETIC_SHIFT_RIGHT_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x3FFF, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(ShiftInstructionsTests, testArithmeticShiftRightIndirect_isZeroSet)
{
    // SAR Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    testedCpu->executeInstruction(ARITHMETIC_SHIFT_RIGHT_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.z);
}