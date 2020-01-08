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

    class DivisionInstructionsTests : public ::testing::Test
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

        const u16 DIVIDE_IMMEDATE_INSTRUCTION_OPCODE = 0xA000;
        const u16 DIVIDE_REGISTER_INSTRUCTION_OPCODE = 0xA100;
        const u16 DIVIDE_REGISTERS_INSTRUCTION_OPCODE = 0xA200;
        const u16 MODULO_IMMEDATE_INSTRUCTION_OPCODE = 0xA300;
        const u16 MODULO_REGISTER_INSTRUCTION_OPCODE = 0xA400;
        const u16 MODULO_REGISTERS_INSTRUCTION_OPCODE = 0xA500;
        const u16 REMAINDER_IMMEDATE_INSTRUCTION_OPCODE = 0xA600;
        const u16 REMAINDER_REGISTER_INSTRUCTION_OPCODE = 0xA700;
        const u16 REMAINDER_REGISTERS_INSTRUCTION_OPCODE = 0xA800;
    };
};

TEST_F(DivisionInstructionsTests, testDivideImmedate_isZeroSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0000;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0020));
    testedCpu->executeInstruction(DIVIDE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0000, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testDivideImmedate_isZeroNotSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0020;
    regs.flags.z = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0010));
    testedCpu->executeInstruction(DIVIDE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0002, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testDivideImmedate_isCarrySet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0025;
    regs.flags.c = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0010));
    testedCpu->executeInstruction(DIVIDE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0002, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(DivisionInstructionsTests, testDivideImmedate_isCarryNotSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0020;
    regs.flags.c = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0010));
    testedCpu->executeInstruction(DIVIDE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0002, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(DivisionInstructionsTests, testDivideImmedate_isNegativeSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xFFE0;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0010));
    testedCpu->executeInstruction(DIVIDE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xFFFE, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testDivideImmedate_isNegativeNotSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0020;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0010));
    testedCpu->executeInstruction(DIVIDE_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0002, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testDivideRegister_isZeroSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0000;
    regs.r[REG_INDEX_Y] = 0x0020;
    regs.flags.z = 0;
    testedCpu->executeInstruction(DIVIDE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0000, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testDivideRegister_isZeroNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0020;
    regs.r[REG_INDEX_Y] = 0x0010;
    regs.flags.z = 1;
    testedCpu->executeInstruction(DIVIDE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0002, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testDivideRegister_isCarrySet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0025;
    regs.r[REG_INDEX_Y] = 0x0010;
    regs.flags.c = 0;
    testedCpu->executeInstruction(DIVIDE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0002, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(DivisionInstructionsTests, testDivideRegister_isCarryNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0020;
    regs.r[REG_INDEX_Y] = 0x0010;
    regs.flags.c = 1;
    testedCpu->executeInstruction(DIVIDE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0002, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(DivisionInstructionsTests, testDivideRegister_isNegativeSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFE0;
    regs.r[REG_INDEX_Y] = 0x0010;
    regs.flags.n = 0;
    testedCpu->executeInstruction(DIVIDE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFFFE, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testDivideRegister_isNegativeNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0020;
    regs.r[REG_INDEX_Y] = 0x0010;
    regs.flags.n = 1;
    testedCpu->executeInstruction(DIVIDE_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0002, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.n);
}
TEST_F(DivisionInstructionsTests, testDivideRegisters_isZeroSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0000;
    regs.r[REG_INDEX_Y] = 0x0020;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(DIVIDE_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0000, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testDivideRegisters_isZeroNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0020;
    regs.r[REG_INDEX_Y] = 0x0010;
    regs.flags.z = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(DIVIDE_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0002, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testDivideRegisters_isCarrySet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0025;
    regs.r[REG_INDEX_Y] = 0x0010;
    regs.flags.c = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(DIVIDE_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0002, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.c);
}

TEST_F(DivisionInstructionsTests, testDivideRegisters_isCarryNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0020;
    regs.r[REG_INDEX_Y] = 0x0010;
    regs.flags.c = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(DIVIDE_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0002, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.c);
}

TEST_F(DivisionInstructionsTests, testDivideRegisters_isNegativeSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFE0;
    regs.r[REG_INDEX_Y] = 0x0010;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(DIVIDE_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFFFE, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testDivideRegisters_isNegativeNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0020;
    regs.r[REG_INDEX_Y] = 0x0010;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(DIVIDE_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0002, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testModuloImmedate_isZeroSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0006;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0002));
    testedCpu->executeInstruction(MODULO_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0000, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testModuloImmedate_isZeroNotSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0007;
    regs.flags.z = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0002));
    testedCpu->executeInstruction(MODULO_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0001, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testModuloImmedate_isNegativeSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0007;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0xFFFE));
    testedCpu->executeInstruction(MODULO_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testModuloImmedate_isNegativeNotSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0007;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0002));
    testedCpu->executeInstruction(MODULO_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0001, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testModuloRegister_isZeroSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0006;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.z = 0;
    testedCpu->executeInstruction(MODULO_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0000, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testModuloRegister_isZeroNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0007;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.z = 1;
    testedCpu->executeInstruction(MODULO_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0001, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testModuloRegister_isNegativeSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0007;
    regs.r[REG_INDEX_Y] = 0xFFFE;
    regs.flags.n = 0;
    testedCpu->executeInstruction(MODULO_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testModuloRegister_isNegativeNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0007;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.n = 1;
    testedCpu->executeInstruction(MODULO_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0001, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testModuloRegisters_isZeroSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0006;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(MODULO_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0000, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testModuloRegisters_isZeroNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0007;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.z = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(MODULO_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0001, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testModuloRegisters_isNegativeSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0007;
    regs.r[REG_INDEX_Y] = 0xFFFE;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(MODULO_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testModuloRegisters_isNegativeNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0007;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(MODULO_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0001, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testRemainderImmedate_isZeroSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0006;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0002));
    testedCpu->executeInstruction(REMAINDER_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0000, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testRemainderImmedate_isZeroNotSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0007;
    regs.flags.z = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0002));
    testedCpu->executeInstruction(REMAINDER_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0001, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testRemainderImmedate_isNegativeSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0xFFF9;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0002));
    testedCpu->executeInstruction(REMAINDER_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testRemainderImmedate_isNegativeNotSet)
{
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x0007;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x0002));
    testedCpu->executeInstruction(REMAINDER_IMMEDATE_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x0001, regs.r[REG_INDEX]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testRemainderRegister_isZeroSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0006;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.z = 0;
    testedCpu->executeInstruction(REMAINDER_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0000, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testRemainderRegister_isZeroNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0007;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.z = 1;
    testedCpu->executeInstruction(REMAINDER_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0001, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testRemainderRegister_isNegativeSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFF9;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.n = 0;
    testedCpu->executeInstruction(REMAINDER_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX_X]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testRemainderRegister_isNegativeNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0007;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.n = 1;
    testedCpu->executeInstruction(REMAINDER_REGISTER_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0001, regs.r[REG_INDEX_X]);
    EXPECT_EQ(0, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testRemainderRegisters_isZeroSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0006;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.z = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(REMAINDER_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0000, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testRemainderRegisters_isZeroNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0007;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.z = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(REMAINDER_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0001, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.z);
}

TEST_F(DivisionInstructionsTests, testRemainderRegisters_isNegativeSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0xFFF9;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.n = 0;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(REMAINDER_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0xFFFF, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(1, regs.flags.n);
}

TEST_F(DivisionInstructionsTests, testRemainderRegisters_isNegativeNotSet)
{
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    const auto REG_INDEX_Z = 9;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x0007;
    regs.r[REG_INDEX_Y] = 0x0002;
    regs.flags.n = 1;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(REG_INDEX_Z << 8));
    testedCpu->executeInstruction(REMAINDER_REGISTERS_INSTRUCTION_OPCODE + REG_INDEX_X + (REG_INDEX_Y << 4));
    EXPECT_EQ(0x0001, regs.r[REG_INDEX_Z]);
    EXPECT_EQ(0, regs.flags.n);
}