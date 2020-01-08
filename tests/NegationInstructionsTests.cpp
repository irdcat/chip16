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

    class NegationInstructionsTests : public ::testing::Test
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

        const u16 NOT_IMMEDATE_INSTRUCTION_OPCODE = 0xE000;
        const u16 NOT_REGISTER_INSTRUCTION_OPCODE = 0xE100;
        const u16 NOT_REGISTER_INDIRECT_INSTRUCTION_OPCODE = 0xE200;
        const u16 NEG_IMMEDATE_INSTRUCTION_OPCODE = 0xE300;
        const u16 NEG_REGISTER_INSTRUCTION_OPCODE = 0xE400;
        const u16 NEG_REGISTER_INDIRECT_INSTRUCTION_OPCODE = 0xE500;
    };
};

TEST_F(NegationInstructionsTests, testNotImmedate_isNegativeUnset)
{
    // NOTI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0;
    regs.flags.z = 0;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x80FF));
    testedCpu->executeInstruction(NOT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x7F00, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(NegationInstructionsTests, testNotImmedate_isNegativeSet)
{
    // NOTI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0;
    regs.flags.z = 0;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x7F00));
    testedCpu->executeInstruction(NOT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x80FF, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(NegationInstructionsTests, testNotImmedate_isZeroUnset)
{
    // NOTI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0;
    regs.flags.z = 1;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x80FF));
    testedCpu->executeInstruction(NOT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x7F00, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(NegationInstructionsTests, testNotImmedate_isZeroSet)
{
    // NOTI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0;
    regs.flags.z = 0;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0xFFFF));
    testedCpu->executeInstruction(NOT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(NegationInstructionsTests, testNotRegister_isNegativeUnset)
{
    // NOT Rx
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x80FF;
    regs.flags.z = 0;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NOT_REGISTER_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x7F00, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(NegationInstructionsTests, testNotRegister_isNegativeSet)
{
    // NOT Rx
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x7F00;
    regs.flags.z = 0;
    regs.flags.n = 0;
    testedCpu->executeInstruction(NOT_REGISTER_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x80FF, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(NegationInstructionsTests, testNotRegister_isZeroUnset)
{
    // NOT Rx
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x80FF;
    regs.flags.z = 1;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NOT_REGISTER_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x7F00, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(NegationInstructionsTests, testNotRegister_isZeroSet)
{
    // NOT Rx
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xFFFF;
    regs.flags.z = 0;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NOT_REGISTER_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(NegationInstructionsTests, testNotRegisterIndirect_isNegativeUnset)
{
    // NOT Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 0x80FF;
    regs.flags.z = 0;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NOT_REGISTER_INDIRECT_INSTRUCTION_OPCODE 
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x7F00, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(NegationInstructionsTests, testNotRegisterIndirect_isNegativeSet)
{
    // NOT Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 0x7F00;
    regs.flags.z = 0;
    regs.flags.n = 0;
    testedCpu->executeInstruction(NOT_REGISTER_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x80FF, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(NegationInstructionsTests, testNotRegisterIndirect_isZeroUnset)
{
    // NOT Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 0x80FF;
    regs.flags.z = 1;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NOT_REGISTER_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x7F00, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(NegationInstructionsTests, testNotRegisterIndirect_isZeroSet)
{
    // NOT Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 0xFFFF;
    regs.flags.z = 0;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NOT_REGISTER_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(NegationInstructionsTests, testNegateImmedate_isNegativeUnset)
{
    // NEGI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0;
    regs.flags.z = 0;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x80FF));
    testedCpu->executeInstruction(NEG_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x7F01, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(NegationInstructionsTests, testNegateImmedate_isNegativeSet)
{
    // NEGI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0;
    regs.flags.z = 0;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0xFF));
    testedCpu->executeInstruction(NEG_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xFF01, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(NegationInstructionsTests, testNegateImmedate_isZeroUnset)
{
    // NEGI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0;
    regs.flags.z = 0;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x80FF));
    testedCpu->executeInstruction(NEG_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x7F01, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(NegationInstructionsTests, testNegateImmedate_isZeroSet)
{
    // NEGI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0;
    regs.flags.z = 0;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0));
    testedCpu->executeInstruction(NEG_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(NegationInstructionsTests, testNegateRegister_isNegativeUnset)
{
    // NEG Rx
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x80FF;
    regs.flags.z = 0;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NEG_REGISTER_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x7F01, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(NegationInstructionsTests, testNegateRegister_isNegativeSet)
{
    // NEG Rx
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xFF;
    regs.flags.z = 0;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NEG_REGISTER_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xFF01, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(NegationInstructionsTests, testNegateRegister_isZeroUnset)
{
    // NEG Rx
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x80FF;
    regs.flags.z = 1;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NEG_REGISTER_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x7F01, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(NegationInstructionsTests, testNegateRegister_isZeroSet)
{
    // NEG Rx
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0;
    regs.flags.z = 0;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NEG_REGISTER_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(NegationInstructionsTests, testNegateRegisterIndirect_isNegativeUnset)
{
    // NEG Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 0x80FF;
    regs.flags.z = 0;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NEG_REGISTER_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x7F01, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(NegationInstructionsTests, testNegateRegisterIndirect_isNegativeSet)
{
    // NEG Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 0xFF;
    regs.flags.z = 0;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NEG_REGISTER_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFF01, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(NegationInstructionsTests, testNegateRegisterIndirect_isZeroUnset)
{
    // NEG Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 0xFF;
    regs.flags.z = 1;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NEG_REGISTER_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFF01, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(NegationInstructionsTests, testNegateRegisterIndirect_isZeroSet)
{
    // NEG Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 0;
    regs.flags.z = 1;
    regs.flags.n = 1;
    testedCpu->executeInstruction(NEG_REGISTER_INDIRECT_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.z);
}