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

    class AdditionInstructionsTests : public ::testing::Test
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

        const u16 ADD_IMMEDATE_INSTRUCTION_OPCODE = 0x4000;
        const u16 ADD_REGISTER_INSTRUCTION_OPCODE = 0x4100;
        const u16 ADD_REGISTERS_INSTRUCTION_OPCODE = 0x4200;
    };
};

TEST_F(AdditionInstructionsTests, testAddImmedate_isZeroSet)
{
    // ADDI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.z = 0;
    regs.r[REG_INDEX] = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0));
    testedCpu->executeInstruction(ADD_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(AdditionInstructionsTests, testAddImmedate_isZeroNotSet)
{
    // ADDI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.z = 1;
    regs.r[REG_INDEX] = 0x74;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x1023));
    testedCpu->executeInstruction(ADD_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x1097, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(AdditionInstructionsTests, testAddImmedate_isCarrySet)
{
    // ADDI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 0;
    regs.r[REG_INDEX] = 0x0A00;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0xFE00));
    testedCpu->executeInstruction(ADD_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0800, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(AdditionInstructionsTests, testAddImmedate_isCarryNotSet)
{
    // ADDI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 1;
    regs.r[REG_INDEX] = 0x2C00;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x2D00));
    testedCpu->executeInstruction(ADD_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x5900, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(AdditionInstructionsTests, testAddImmedate_isNegativeSet)
{
    // ADDI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.n = 0;
    regs.r[REG_INDEX] = 0xF900;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0xED00));
    testedCpu->executeInstruction(ADD_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xE600, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(AdditionInstructionsTests, testAddImmedate_isNegativeNotSet)
{
    // ADDI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.n = 1;
    regs.r[REG_INDEX] = 0x2C00;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x2D00));
    testedCpu->executeInstruction(ADD_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x5900, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(AdditionInstructionsTests, testAddImmedate_isOverflowSet)
{
    // ADDI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 0;
    regs.r[REG_INDEX] = 0x6800;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x2D00));
    testedCpu->executeInstruction(ADD_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x9500, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.o);
}

TEST_F(AdditionInstructionsTests, testAddImmedate_isOverflowNotSet)
{
    // ADDI Rx, $value
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 1;
    regs.r[REG_INDEX] = 0x5C00;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0xD900));
    testedCpu->executeInstruction(ADD_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x3500, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.o);
}

TEST_F(AdditionInstructionsTests, testAddRegister_isZeroSet)
{
    // ADD Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.z = 0;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 0;
    testedCpu->executeInstruction(ADD_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(AdditionInstructionsTests, testAddRegister_isZeroNotSet)
{
    // ADD Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.z = 1;
    regs.r[REG_INDEX_X] = 0x1023;
    regs.r[REG_INDEX_Y] = 0x74;
    testedCpu->executeInstruction(ADD_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x1097, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(AdditionInstructionsTests, testAddRegister_isCarrySet)
{
    // ADD Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 0;
    regs.r[REG_INDEX_X] = 0x0A00;
    regs.r[REG_INDEX_Y] = 0xFE00;
    testedCpu->executeInstruction(ADD_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0800, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(AdditionInstructionsTests, testAddRegister_isCarryNotSet)
{
    // ADD Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 1;
    regs.r[REG_INDEX_X] = 0x2D00;
    regs.r[REG_INDEX_Y] = 0x2C00;
    testedCpu->executeInstruction(ADD_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x5900, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(AdditionInstructionsTests, testAddRegister_isNegativeSet)
{
    // ADD Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.n = 0;
    regs.r[REG_INDEX_X] = 0xED00;
    regs.r[REG_INDEX_Y] = 0xF900;
    testedCpu->executeInstruction(ADD_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xE600, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(AdditionInstructionsTests, testAddRegister_isNegativeNotSet)
{
    // ADD Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.n = 1;
    regs.r[REG_INDEX_X] = 0x2D00;
    regs.r[REG_INDEX_Y] = 0x2C00;
    testedCpu->executeInstruction(ADD_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x5900, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(AdditionInstructionsTests, testAddRegister_isOverflowSet)
{
    // ADD Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 0;
    regs.r[REG_INDEX_X] = 0x6800;
    regs.r[REG_INDEX_Y] = 0x2D00;
    testedCpu->executeInstruction(ADD_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x9500, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.o);
}

TEST_F(AdditionInstructionsTests, testAddRegister_isOverflowNotSet)
{
    // ADD Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 1;
    regs.r[REG_INDEX_X] = 0x5C00;
    regs.r[REG_INDEX_Y] = 0xD900;
    testedCpu->executeInstruction(ADD_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x3500, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.o);
}

TEST_F(AdditionInstructionsTests, testAddRegisters_isZeroSet)
{
    // ADD Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.z = 0;
    regs.r[REG_INDEX_X] = 0;
    regs.r[REG_INDEX_Y] = 0;
    regs.r[REG_INDEX_Z] = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(ADD_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(AdditionInstructionsTests, testAddRegisters_isZeroNotSet)
{
    // ADD Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.z = 1;
    regs.r[REG_INDEX_X] = 0x1023;
    regs.r[REG_INDEX_Y] = 0x74;
    regs.r[REG_INDEX_Z] = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(ADD_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x1097, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(AdditionInstructionsTests, testAddRegisters_isCarrySet)
{
    // ADD Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 0;
    regs.r[REG_INDEX_X] = 0x0A00;
    regs.r[REG_INDEX_Y] = 0xFE00;
    regs.r[REG_INDEX_Z] = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(ADD_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0800, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(AdditionInstructionsTests, testAddRegisters_isCarryNotSet)
{
    // ADD Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 1;
    regs.r[REG_INDEX_X] = 0x2C00;
    regs.r[REG_INDEX_Y] = 0x2D00;
    regs.r[REG_INDEX_Z] = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(ADD_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x5900, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(AdditionInstructionsTests, testAddRegisters_isNegativeSet)
{
    // ADD Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.n = 0;
    regs.r[REG_INDEX_X] = 0xED00;
    regs.r[REG_INDEX_Y] = 0xF900;
    regs.r[REG_INDEX_Z] = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(ADD_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xE600, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(AdditionInstructionsTests, testAddRegisters_isNegativeNotSet)
{
    // ADD Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.n = 1;
    regs.r[REG_INDEX_X] = 0x2C00;
    regs.r[REG_INDEX_Y] = 0x2D00;
    regs.r[REG_INDEX_Z] = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(ADD_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x5900, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(AdditionInstructionsTests, testAddRegisters_isOverflowSet)
{
    // ADD Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 0;
    regs.r[REG_INDEX_X] = 0x6800;
    regs.r[REG_INDEX_Y] = 0x2D00;
    regs.r[REG_INDEX_Z] = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(ADD_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x9500, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.o);
}

TEST_F(AdditionInstructionsTests, testAddRegisters_isOverflowNotSet)
{
    // ADD Rx, Ry, Rz
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 1;
    regs.r[REG_INDEX_X] = 0x5C00;
    regs.r[REG_INDEX_Y] = 0xD900;
    regs.r[REG_INDEX_Z] = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(ADD_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x3500, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.o);
}