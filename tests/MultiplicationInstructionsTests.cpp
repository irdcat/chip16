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

    class MultiplicationInstructionsTests : public ::testing::Test
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

        const u16 MULTIPLY_IMMEDATE_INSTRUCTION_OPCODE = 0x9000;
        const u16 MULTIPLY_REGISTER_INSTRUCTION_OPCODE = 0x9100;
        const u16 MULTIPLY_REGISTERS_INSTRUCTION_OPCODE = 0x9200;
    };
};

TEST_F(MultiplicationInstructionsTests, testMultiplyImmedate_isZeroSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x3FFF;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0000));
    testedCpu->executeInstruction(MULTIPLY_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0000, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyImmedate_isZeroNotSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0040;
    regs.flags.z = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0xFFE0));
    testedCpu->executeInstruction(MULTIPLY_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xF800, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyImmedate_isNegativeSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0040;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0xFFE0));
    testedCpu->executeInstruction(MULTIPLY_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xF800, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyImmedate_isNegativeNotSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0001;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0007));
    testedCpu->executeInstruction(MULTIPLY_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0007, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyImmedate_isCarrySet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xFFFF;
    regs.flags.c = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0xFFF9));
    testedCpu->executeInstruction(MULTIPLY_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0007, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyImmedate_isCarryNotSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0001;
    regs.flags.c = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0007));
    testedCpu->executeInstruction(MULTIPLY_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0007, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyRegister_isZeroSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x3FFF;
    regs.r[REG_INDEX_Y] = 0x0000;
    regs.flags.z = 0;
    testedCpu->executeInstruction(MULTIPLY_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0000, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyRegister_isZeroNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0040;
    regs.r[REG_INDEX_Y] = 0xFFE0;
    regs.flags.z = 1;
    testedCpu->executeInstruction(MULTIPLY_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xF800, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyRegister_isNegativeSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0040;
    regs.r[REG_INDEX_Y] = 0xFFE0;
    regs.flags.n = 0;
    testedCpu->executeInstruction(MULTIPLY_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xF800, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyRegister_isNegativeNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0001;
    regs.r[REG_INDEX_Y] = 0x0007;
    regs.flags.n = 1;
    testedCpu->executeInstruction(MULTIPLY_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0007, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyRegister_isCarrySet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFFF;
    regs.r[REG_INDEX_Y] = 0xFFF9;
    regs.flags.c = 0;
    testedCpu->executeInstruction(MULTIPLY_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0007, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyRegister_isCarryNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0001;
    regs.r[REG_INDEX_Y] = 0x0007;
    regs.flags.c = 1;
    testedCpu->executeInstruction(MULTIPLY_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0007, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyRegisters_isZeroSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x3FFF;
    regs.r[REG_INDEX_Y] = 0x0000;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(MULTIPLY_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0000, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyRegisters_isZeroNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0040;
    regs.r[REG_INDEX_Y] = 0xFFE0;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.z = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(MULTIPLY_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xF800, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyRegisters_isNegativeSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0040;
    regs.r[REG_INDEX_Y] = 0xFFE0;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(MULTIPLY_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xF800, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyRegisters_isNegativeNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0001;
    regs.r[REG_INDEX_Y] = 0x0007;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(MULTIPLY_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0007, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyRegisters_isCarrySet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFFF;
    regs.r[REG_INDEX_Y] = 0xFFF9;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.c = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(MULTIPLY_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0007, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(MultiplicationInstructionsTests, testMultiplyRegisters_isCarryNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0001;
    regs.r[REG_INDEX_Y] = 0x0007;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.c = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(MULTIPLY_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0007, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.c);
}