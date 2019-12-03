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

    class BitwiseXorInstructionTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            memory = std::make_shared<MemoryMock>();
            testedCpu = std::make_unique<CpuImpl>(memory);
        }

        std::unique_ptr<CpuImpl> testedCpu;
        std::shared_ptr<MemoryMock> memory;

        const u16 BITWISE_XOR_IMMEDATE_INSTRUCTION_OPCODE = 0x8000;
        const u16 BITWISE_XOR_REGISTER_INSTRUCTION_OPCODE = 0x8100;
        const u16 BITWISE_XOR_REGISTERS_INSTRUCTION_OPCODE = 0x8200;
    };
};

TEST_F(BitwiseXorInstructionTests, testXorImmedate_isNegativeUnset)
{
    // ORI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x7F00;
    regs.flags.n = 1;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0xFF));
    testedCpu->executeInstruction(BITWISE_XOR_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x7FFF, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(BitwiseXorInstructionTests, testXorImmedate_isNegativeSet)
{
    // ORI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x7F00;
    regs.flags.n = 0;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x80FF));
    testedCpu->executeInstruction(BITWISE_XOR_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(BitwiseXorInstructionTests, testXorImmedate_isZeroUnset)
{
    // ORI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x7F00;
    regs.flags.n = 0;
    regs.flags.z = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x80FF));
    testedCpu->executeInstruction(BITWISE_XOR_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(BitwiseXorInstructionTests, testXorImmedate_isZeroSet)
{
    // ORI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0;
    regs.flags.n = 0;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0));
    testedCpu->executeInstruction(BITWISE_XOR_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(BitwiseXorInstructionTests, testXorRegister_isNegativeUnset)
{
    // OR Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7F00;
    regs.r[REG_INDEX_Y] = 0xFF;
    regs.flags.z = 0;
    regs.flags.n = 1;
    testedCpu->executeInstruction(BITWISE_XOR_REGISTER_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x7FFF, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(BitwiseXorInstructionTests, testXorRegister_isNegativeSet)
{
    // OR Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7F00;
    regs.r[REG_INDEX_Y] = 0x80FF;
    regs.flags.z = 0;
    regs.flags.n = 0;
    testedCpu->executeInstruction(BITWISE_XOR_REGISTER_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(BitwiseXorInstructionTests, testXorRegister_isZeroUnset)
{
    // OR Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7F00;
    regs.r[REG_INDEX_Y] = 0x80FF;
    regs.flags.z = 1;
    regs.flags.n = 0;
    testedCpu->executeInstruction(BITWISE_XOR_REGISTER_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(BitwiseXorInstructionTests, testXorRegister_isZeroSet)
{
    // OR Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0;
    regs.r[REG_INDEX_Y] = 0x0;
    regs.flags.z = 0;
    regs.flags.n = 0;
    testedCpu->executeInstruction(BITWISE_XOR_REGISTER_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(BitwiseXorInstructionTests, testXorRegisters_isNegativeUnset)
{
    // OR Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7F00;
    regs.r[REG_INDEX_Y] = 0xFF;
    regs.flags.z = 0;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(BITWISE_XOR_REGISTERS_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x7FFF, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(BitwiseXorInstructionTests, testXorRegisters_isNegativeSet)
{
    // OR Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7F00;
    regs.r[REG_INDEX_Y] = 0x80FF;
    regs.flags.z = 0;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(BITWISE_XOR_REGISTERS_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(BitwiseXorInstructionTests, testXorRegisters_isZeroUnset)
{
    // OR Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7F00;
    regs.r[REG_INDEX_Y] = 0x80FF;
    regs.flags.z = 0;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(BITWISE_XOR_REGISTERS_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(BitwiseXorInstructionTests, testXorRegisters_isZeroSet)
{
    // OR Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0;
    regs.r[REG_INDEX_Y] = 0x0;
    regs.flags.z = 0;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(BITWISE_XOR_REGISTERS_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.z);
}