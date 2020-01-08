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

    class SubtractionInstructionsTests : public ::testing::Test
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

        const u16 SUBTRACT_IMMEDATE_INSTRUCTION_OPCODE = 0x5000;
        const u16 SUBTRACT_REGISTER_INSTRUCTION_OPCODE = 0x5100;
        const u16 SUBTRACT_REGISTERS_INSTRUCTION_OPCODE = 0x5200;
        const u16 COMPARE_IMMEDATE_INSTRUCTION_OPCODE = 0x5300;
        const u16 COMPARE_REGISTER_INSTRUCTION_OPCODE = 0x5400;
    };
};

TEST_F(SubtractionInstructionsTests, testSubtractImmedate_isZeroSet)
{
    // SUBI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x1023;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x1023));
    testedCpu->executeInstruction(SUBTRACT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(SubtractionInstructionsTests, testSubtractImmedate_isZeroNotSet)
{
    // SUBI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x7400;
    regs.flags.z = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x2300));
    testedCpu->executeInstruction(SUBTRACT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x5100, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(SubtractionInstructionsTests, testSubtractImmedate_isNegativeSet)
{
    // SUBI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xED00;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0700));
    testedCpu->executeInstruction(SUBTRACT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xE600, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(SubtractionInstructionsTests, testSubtractImmedate_isNegativeNotSet)
{
    // SUBI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x5C00;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x3700));
    testedCpu->executeInstruction(SUBTRACT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x2500, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(SubtractionInstructionsTests, testSubtractImmedate_isOverflowSet)
{
    // SUBI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x6800;
    regs.flags.o = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0xD300));
    testedCpu->executeInstruction(SUBTRACT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x9500, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.o);
}

TEST_F(SubtractionInstructionsTests, testSubtractImmedate_isOverflowNotSet)
{
    // SUBI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x3B00;
    regs.flags.o = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x4B00));
    testedCpu->executeInstruction(SUBTRACT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xF000, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.o);
}

TEST_F(SubtractionInstructionsTests, testSubtractImmedate_isCarrySet)
{
    // SUBI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x3B00;
    regs.flags.c = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x4B00));
    testedCpu->executeInstruction(SUBTRACT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xF000, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(SubtractionInstructionsTests, testSubtractImmedate_isCarryNotSet)
{
    // SUBI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x4B00;
    regs.flags.c = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x3B00));
    testedCpu->executeInstruction(SUBTRACT_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x1000, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegister_isZeroSet)
{
    // SUB Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x1023;
    regs.r[REG_INDEX_Y] = 0x1023;
    regs.flags.z = 0;
    testedCpu->executeInstruction(SUBTRACT_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegister_isZeroNotSet)
{
    // SUB Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7400;
    regs.r[REG_INDEX_Y] = 0x2300;
    regs.flags.z = 1;
    testedCpu->executeInstruction(SUBTRACT_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x5100, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegister_isNegativeSet)
{
    // SUB Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xED00;
    regs.r[REG_INDEX_Y] = 0x0700;
    regs.flags.n = 0;
    testedCpu->executeInstruction(SUBTRACT_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xE600, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegister_isNegativeNotSet)
{
    // SUB Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x5C00;
    regs.r[REG_INDEX_Y] = 0x3700;
    regs.flags.n = 1;
    testedCpu->executeInstruction(SUBTRACT_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x2500, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegister_isOverflowSet)
{
    // SUB Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x6800;
    regs.r[REG_INDEX_Y] = 0xD300;
    regs.flags.o = 0;
    testedCpu->executeInstruction(SUBTRACT_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x9500, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.o);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegister_isOverflowNotSet)
{
    // SUB Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x3B00;
    regs.r[REG_INDEX_Y] = 0x4B00;
    regs.flags.o = 1;
    testedCpu->executeInstruction(SUBTRACT_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xF000, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.o);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegister_isCarrySet)
{
    // SUB Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x3B00;
    regs.r[REG_INDEX_Y] = 0x4B00;
    regs.flags.c = 0;
    testedCpu->executeInstruction(SUBTRACT_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xF000, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegister_isCarryNotSet)
{
    // SUB Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x4B00;
    regs.r[REG_INDEX_Y] = 0x3B00;
    regs.flags.c = 1;
    testedCpu->executeInstruction(SUBTRACT_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x1000, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegisters_isZeroSet)
{
    // SUB Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x1023;
    regs.r[REG_INDEX_Y] = 0x1023;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(SUBTRACT_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegisters_isZeroNotSet)
{
    // SUB Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7400;
    regs.r[REG_INDEX_Y] = 0x2300;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.z = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(SUBTRACT_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x5100, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegisters_isNegativeSet)
{
    // SUB Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xED00;
    regs.r[REG_INDEX_Y] = 0x0700;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(SUBTRACT_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xE600, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegisters_isNegativeNotSet)
{
    // SUB Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x5C00;
    regs.r[REG_INDEX_Y] = 0x3700;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(SUBTRACT_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x2500, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegisters_isOverflowSet)
{
    // SUB Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x6800;
    regs.r[REG_INDEX_Y] = 0xD300;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.o = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(SUBTRACT_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x9500, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.o);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegisters_isOverflowNotSet)
{
    // SUB Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x3B00;
    regs.r[REG_INDEX_Y] = 0x4B00;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.o = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(SUBTRACT_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xF000, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.o);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegisters_isCarrySet)
{
    // SUB Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x3B00;
    regs.r[REG_INDEX_Y] = 0x4B00;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.c = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(SUBTRACT_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xF000, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(SubtractionInstructionsTests, testSubtractRegisters_isCarryNotSet)
{
    // SUB Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x4B00;
    regs.r[REG_INDEX_Y] = 0x3B00;
    regs.r[REG_INDEX_Z] = 0x0000;
    regs.flags.c = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(SUBTRACT_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x1000, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(SubtractionInstructionsTests, testCompareImmedate_isZeroSet)
{
    // CMPI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x1023;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x1023));
    testedCpu->executeInstruction(COMPARE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(SubtractionInstructionsTests, testCompareImmedate_isZeroNotSet)
{
    // CMPI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x7400;
    regs.flags.z = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x2300));
    testedCpu->executeInstruction(COMPARE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(SubtractionInstructionsTests, testCompareImmedate_isNegativeSet)
{
    // CMPI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xED00;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0700));
    testedCpu->executeInstruction(COMPARE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(SubtractionInstructionsTests, testCompareImmedate_isNegativeNotSet)
{
    // CMPI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x5C00;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x3700));
    testedCpu->executeInstruction(COMPARE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(SubtractionInstructionsTests, testCompareImmedate_isOverflowSet)
{
    // CMPI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x6800;
    regs.flags.o = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0xD300));
    testedCpu->executeInstruction(COMPARE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(1, regs.flags.o);
}

TEST_F(SubtractionInstructionsTests, testCompareImmedate_isOverflowNotSet)
{
    // CMPI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x3B00;
    regs.flags.o = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x4B00));
    testedCpu->executeInstruction(COMPARE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.flags.o);
}

TEST_F(SubtractionInstructionsTests, testCompareImmedate_isCarrySet)
{
    // CMPI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x3B00;
    regs.flags.c = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x4B00));
    testedCpu->executeInstruction(COMPARE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(SubtractionInstructionsTests, testCompareImmedate_isCarryNotSet)
{
    // CMPI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x4B00;
    regs.flags.c = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x3B00));
    testedCpu->executeInstruction(COMPARE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(SubtractionInstructionsTests, testCompareRegister_isZeroSet)
{
    // CMP Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x1023;
    regs.r[REG_INDEX_Y] = 0x1023;
    regs.flags.z = 0;
    testedCpu->executeInstruction(COMPARE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(SubtractionInstructionsTests, testCompareRegister_isZeroNotSet)
{
    // CMP Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x7400;
    regs.r[REG_INDEX_Y] = 0x2300;
    regs.flags.z = 1;
    testedCpu->executeInstruction(COMPARE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(SubtractionInstructionsTests, testCompareRegister_isNegativeSet)
{
    // CMP Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xED00;
    regs.r[REG_INDEX_Y] = 0x0700;
    regs.flags.n = 0;
    testedCpu->executeInstruction(COMPARE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(SubtractionInstructionsTests, testCompareRegister_isNegativeNotSet)
{
    // CMP Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x5C00;
    regs.r[REG_INDEX_Y] = 0x3700;
    regs.flags.n = 1;
    testedCpu->executeInstruction(COMPARE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(SubtractionInstructionsTests, testCompareRegister_isOverflowSet)
{
    // CMP Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x6800;
    regs.r[REG_INDEX_Y] = 0xD300;
    regs.flags.o = 0;
    testedCpu->executeInstruction(COMPARE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(1, regs.flags.o);
}

TEST_F(SubtractionInstructionsTests, testCompareRegister_isOverflowNotSet)
{
    // CMP Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x3B00;
    regs.r[REG_INDEX_Y] = 0x4B00;
    regs.flags.o = 1;
    testedCpu->executeInstruction(COMPARE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.flags.o);
}

TEST_F(SubtractionInstructionsTests, testCompareRegister_isCarrySet)
{
    // CMP Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x3B00;
    regs.r[REG_INDEX_Y] = 0x4B00;
    regs.flags.c = 0;
    testedCpu->executeInstruction(COMPARE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(SubtractionInstructionsTests, testCompareRegister_isCarryNotSet)
{
    // CMP Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x4B00;
    regs.r[REG_INDEX_Y] = 0x3B00;
    regs.flags.c = 1;
    testedCpu->executeInstruction(COMPARE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.flags.c);
}