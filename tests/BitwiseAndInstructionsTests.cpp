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

    class BitwiseAndInstructionsTests : public ::testing::Test
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

        const u16 BITWISE_AND_IMMEDATE_INSTRUCTION_OPCODE = 0x6000;
        const u16 BITWISE_AND_REGISTER_INSTRUCTION_OPCODE = 0x6100;
        const u16 BITWISE_AND_REGISTERS_INSTRUCTION_OPCODE = 0x6200;
        const u16 BITWISE_TEST_IMMEDATE_INSTRUCTION_OPCODE = 0x6300;
        const u16 BITWISE_TEST_REGISTER_INSTRUCTION_OPCODE = 0x6400;
    };
};

TEST_F(BitwiseAndInstructionsTests, testAndImmedate_isNegativeUnset)
{
    // ANDI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xFFFF;
    regs.flags.n = 0;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x1234));
    testedCpu->executeInstruction(BITWISE_AND_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x1234, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(BitwiseAndInstructionsTests, testAndImmedate_isNegativeSet)
{
    // ANDI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xFEFE;
    regs.flags.n = 0;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x8000));
    testedCpu->executeInstruction(BITWISE_AND_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x8000, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(BitwiseAndInstructionsTests, testAndImmedate_isZeroUnset)
{
    // ANDI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x3494;
    regs.flags.n = 0;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x3400));
    testedCpu->executeInstruction(BITWISE_AND_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x3400, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(BitwiseAndInstructionsTests, testAndImmedate_isZeroSet)
{
    // ANDI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xF300;
    regs.flags.n = 0;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0800));
    testedCpu->executeInstruction(BITWISE_AND_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(BitwiseAndInstructionsTests, testAndRegister_isNegativeUnset)
{
    // AND Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFFF;
    regs.r[REG_INDEX_Y] = 0x1234;
    regs.flags.n = 0;
    regs.flags.z = 0;
    testedCpu->executeInstruction(BITWISE_AND_REGISTER_INSTRUCTION_OPCODE 
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x1234, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(BitwiseAndInstructionsTests, testAndRegister_isNegativeSet)
{
    // AND Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFEFE;
    regs.r[REG_INDEX_Y] = 0x8000;
    regs.flags.n = 0;
    regs.flags.z = 0;
    testedCpu->executeInstruction(BITWISE_AND_REGISTER_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x8000, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(BitwiseAndInstructionsTests, testAndRegister_isZeroUnset)
{
    // AND Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFFF;
    regs.r[REG_INDEX_Y] = 0x1234;
    regs.flags.n = 0;
    regs.flags.z = 0;
    testedCpu->executeInstruction(BITWISE_AND_REGISTER_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x1234, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(BitwiseAndInstructionsTests, testAndRegister_isZeroSet)
{
    // AND Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 0;
    regs.flags.n = 0;
    regs.flags.z = 0;
    testedCpu->executeInstruction(BITWISE_AND_REGISTER_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(BitwiseAndInstructionsTests, testAndRegisters_isNegativeUnset)
{
    // AND Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFFF;
    regs.r[REG_INDEX_Y] = 0x1234;
    regs.r[REG_INDEX_Z] = 0;
    regs.flags.z = 0;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(BITWISE_AND_REGISTERS_INSTRUCTION_OPCODE 
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x1234, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(BitwiseAndInstructionsTests, testAndRegisters_isNegativeSet)
{
    // AND Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFEFE;
    regs.r[REG_INDEX_Y] = 0x8000;
    regs.r[REG_INDEX_Z] = 0;
    regs.flags.z = 0;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(BITWISE_AND_REGISTERS_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x8000, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(BitwiseAndInstructionsTests, testAndRegisters_isZeroUnset)
{
    // AND Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFFF;
    regs.r[REG_INDEX_Y] = 0x1234;
    regs.r[REG_INDEX_Z] = 0;
    regs.flags.z = 0;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(BITWISE_AND_REGISTERS_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x1234, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(BitwiseAndInstructionsTests, testAndRegisters_isZeroSet)
{
    // AND Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 0;
    regs.r[REG_INDEX_Z] = 0xFFFF;
    regs.flags.z = 0;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(BITWISE_AND_REGISTERS_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(BitwiseAndInstructionsTests, testBitwiseTestImmedate_isNegativeUnset)
{
    // TSTI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xFFFF;
    regs.flags.n = 0;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x1234));
    testedCpu->executeInstruction(BITWISE_TEST_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(BitwiseAndInstructionsTests, testBitwiseTestImmedate_isNegativeSet)
{
    // TSTI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xFEFE;
    regs.flags.n = 0;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x8000));
    testedCpu->executeInstruction(BITWISE_TEST_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(BitwiseAndInstructionsTests, testBitwiseTestImmedate_isZeroUnset)
{
    // TSTI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xFFFF;
    regs.flags.n = 0;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x1234));
    testedCpu->executeInstruction(BITWISE_TEST_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(BitwiseAndInstructionsTests, testBitwiseTestImmedate_isZeroSet)
{
    // TSTI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0;
    regs.flags.n = 0;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0));
    testedCpu->executeInstruction(BITWISE_TEST_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(BitwiseAndInstructionsTests, testBitwiseTestRegister_isNegativeUnset)
{
    // TST Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFFF;
    regs.r[REG_INDEX_Y] = 0x1234;
    regs.flags.n = 0;
    regs.flags.z = 0;
    testedCpu->executeInstruction(BITWISE_TEST_REGISTER_INSTRUCTION_OPCODE 
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(BitwiseAndInstructionsTests, testBitwiseTestRegister_isNegativeSet)
{
    // TST Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFEFE;
    regs.r[REG_INDEX_Y] = 0x8000;
    regs.flags.n = 0;
    regs.flags.z = 0;
    testedCpu->executeInstruction(BITWISE_TEST_REGISTER_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(BitwiseAndInstructionsTests, testBitwiseTestRegister_isZeroUnset)
{
    // TST Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFFF;
    regs.r[REG_INDEX_Y] = 0x1234;
    regs.flags.n = 0;
    regs.flags.z = 0;
    testedCpu->executeInstruction(BITWISE_TEST_REGISTER_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(BitwiseAndInstructionsTests, testBitwiseTestRegister_isZeroSet)
{
    // TST Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 0;
    regs.flags.n = 0;
    regs.flags.z = 0;
    testedCpu->executeInstruction(BITWISE_TEST_REGISTER_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(1, regs.flags.z);
}