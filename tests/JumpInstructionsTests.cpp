#include <memory>
#include <cstdarg>
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

    class JumpInstructionsTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            memory = std::make_shared<MemoryMock>();
            bus = std::make_shared<BusMock>();
            testedCpu = std::make_unique<CpuImpl>(memory, bus);
        }

        void setMemoryMocks(u16 startAddr, ...)
        {
            std::va_list args;
            va_start(args, startAddr);
            ON_CALL(*memory, readWord(startAddr)).WillByDefault(Return(va_arg(args, u16)));
            startAddr += 2;
            va_end(args);
        }

        std::unique_ptr<CpuImpl> testedCpu;
        std::shared_ptr<MemoryMock> memory;
        std::shared_ptr<BusMock> bus;

        const u16 JUMP_INSTRUCTION_OPCODE = 0x1000;
        const u16 JUMP_CARRY_INSTRUCTION_OPCODE = 0x1100;
        const u16 JUMP_CONDITIONALLY_INSTRUCTION_OPCODE = 0x1200;
        const u16 JUMP_REGS_EQUAL_INSTRUCTION_OPCODE = 0x1300;
        const u16 CALL_INSTRUCTION_OPCODE = 0x1400;
        const u16 RETURN_INSTRUCTION_OPCODE = 0x1500;
        const u16 JUMP_INDIRECT_INSTRUCTION_OPCODE = 0x1600;
        const u16 CALL_CONDITIONALLY_INSTRUCTION_OPCODE = 0x1700;
        const u16 CALL_INDIRECT_INSTRUCTION_OPCODE = 0x1800;
    };
};

TEST_F(JumpInstructionsTests, testDirectUncontidionalJump)
{
    // JMP addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectCarryConditionalJump_shouldJump)
{
    // JMC addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CARRY_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectCarryConditionalJump_shouldNotJump)
{
    // JMC addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CARRY_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_zero_shouldJump)
{
    // JZ addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.z = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE 
        + static_cast<u16>(ConditionalBranch::ZERO));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_zero_shouldNotJump)
{
    // JZ addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::ZERO));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_notZero_shouldJump)
{
    // JNZ addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NOT_ZERO));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_notZero_shouldNotJump)
{
    // JNZ addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.z = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NOT_ZERO));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_negative_shouldJump)
{
    // JN addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.n = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NEGATIVE));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_negative_shouldNotJump)
{
    // JN addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.n = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NEGATIVE));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_notNegative_shouldJump)
{
    // JNN addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.n = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NOT_NEGATIVE));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_notNegative_shouldNotJump)
{
    // JNN addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.n = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NOT_NEGATIVE));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_positive_shouldJump)
{
    // JP addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.n = 0;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::POSITIVE));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_positive_shouldNotJump)
{
    // JP addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.n = 1;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::POSITIVE));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_overflow_shouldJump)
{
    // JO addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::OVERFLOWED));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_overflow_shouldNotJump)
{
    // JO addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::OVERFLOWED));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_notOverflow_shouldJump)
{
    // JNO addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NOT_OVERFLOW));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_notOverflow_shouldNotJump)
{
    // JNO addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NOT_OVERFLOW));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_above_shouldJump)
{
    // JA addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 0;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::ABOVE));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_above_shouldNotJump)
{
    // JA addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 0;
    regs.flags.z = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::ABOVE));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_aboveEqual_shouldJump)
{
    // JAE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::ABOVE_EQUAL));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_aboveEqual_shouldNotJump)
{
    // JAE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::ABOVE_EQUAL));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_below_shouldJump)
{
    // JB addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::BELOW));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_below_shouldNotJump)
{
    // JB addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::BELOW));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_belowEqual_shouldJump)
{
    // JBE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 1;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::BELOW_EQUAL));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_belowEqual_shouldNotJump)
{
    // JBE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.c = 0;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::BELOW_EQUAL));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_greater_shouldJump)
{
    // JG addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::GREATER));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_greater_shouldNotJump)
{
    // JG addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 1;
    regs.flags.n = 1;
    regs.flags.z = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::GREATER));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_greaterEqual_shouldJump)
{
    // JGE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 1;
    regs.flags.n = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::GREATER_EQUAL));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_greaterEqual_shouldNotJump)
{
    // JGE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 1;
    regs.flags.n = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::GREATER_EQUAL));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_less_shouldJump)
{
    // JL addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 1;
    regs.flags.n = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::LESS));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_less_shouldNotJump)
{
    // JL addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 1;
    regs.flags.n = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::LESS));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_lessEqual_shouldJump)
{
    // JLE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 1;
    regs.flags.n = 0;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::LESS_EQUAL));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalJump_lessEqual_shouldNotJump)
{
    // JLE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.flags.o = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::LESS_EQUAL));
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectEqualConditionalJump_shouldJump)
{
    // JME Rx, Ry, addr
    const std::size_t REG_INDEX_1 = 5;
    const std::size_t REG_INDEX_2 = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_1] = 0x7777;
    regs.r[REG_INDEX_2] = 0x7777;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_REGS_EQUAL_INSTRUCTION_OPCODE
        + (REG_INDEX_1 << 4 | REG_INDEX_2));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectEqualConditionalJump_shouldNotJump)
{
    // JME Rx, Ry, addr
    const std::size_t REG_INDEX_1 = 5;
    const std::size_t REG_INDEX_2 = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_1] = 0x7777;
    regs.r[REG_INDEX_2] = 0x7777;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(JUMP_REGS_EQUAL_INSTRUCTION_OPCODE
        + (REG_INDEX_1 << 4 | REG_INDEX_2));
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectCall)
{
    // CALL addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testReturn)
{
    // RET
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFEA;
    setMemoryMocks(0xFFE8, 0x5555);
    testedCpu->executeInstruction(RETURN_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testJumpIndirect)
{
    // JMP Rx
    const std::size_t REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x5555;
    testedCpu->executeInstruction(JUMP_INDIRECT_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x5555, regs.pc);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_zero_shouldCall)
{
    // CZ addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.z = 1;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::ZERO));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_zero_shouldNotCall)
{
    // CZ addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::ZERO));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_notZero_shouldCall)
{
    // CNZ addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NOT_ZERO));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_notZero_shouldNotCall)
{
    // CNZ addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.z = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NOT_ZERO));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_negative_shouldCall)
{
    // CN addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.n = 1;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NEGATIVE));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_negative_shouldNotCall)
{
    // CN addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.n = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NEGATIVE));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_notNegative_shouldCall)
{
    // CNN addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.n = 0;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NOT_NEGATIVE));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_notNegative_shouldNotCall)
{
    // CNN addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.n = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NOT_NEGATIVE));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_positive_shouldCall)
{
    // CP addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.n = 0;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::POSITIVE));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_positive_shouldNotCall)
{
    // CP addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.n = 1;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::POSITIVE));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_overflow_shouldCall)
{
    // CO addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.o = 1;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::OVERFLOWED));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_overflow_shouldNotCall)
{
    // CO addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.o = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::OVERFLOWED));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_notOverflow_shouldCall)
{
    // CNO addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.o = 0;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NOT_OVERFLOW));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_notOverflow_shouldNotCall)
{
    // CNO addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.o = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::NOT_OVERFLOW));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_above_shouldCall)
{
    // CA addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.c = 0;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::ABOVE));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_above_shouldNotCall)
{
    // CA addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.c = 1;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::ABOVE));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_aboveEqual_shouldCall)
{
    // CAE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.c = 0;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::ABOVE_EQUAL));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_aboveEqual_shouldNotCall)
{
    // CAE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.c = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::ABOVE_EQUAL));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_below_shouldCall)
{
    // CB addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.c = 1;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::BELOW));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_below_shouldNotCall)
{
    // CB addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.c = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::BELOW));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_belowEqual_shouldCall)
{
    // CBE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.c = 1;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::BELOW_EQUAL));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_belowEqual_shouldNotCall)
{
    // CBE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.c = 0;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::BELOW_EQUAL));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_greater_shouldCall)
{
    // CG addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.o = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::GREATER));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_greater_shouldNotCall)
{
    // CG addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.o = 1;
    regs.flags.n = 0;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::GREATER));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_greaterEqual_shouldCall)
{
    // CGE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.o = 1;
    regs.flags.n = 1;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::GREATER_EQUAL));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_greaterEqual_shouldNotCall)
{
    // CGE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.o = 1;
    regs.flags.n = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::GREATER_EQUAL));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_less_shouldCall)
{
    // CL addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.o = 1;
    regs.flags.n = 0;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::LESS));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_less_shouldNotCall)
{
    // CL addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.o = 1;
    regs.flags.n = 1;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::LESS));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_lessEqual_shouldCall)
{
    // CLE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.o = 1;
    regs.flags.n = 0;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::LESS_EQUAL));
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}

TEST_F(JumpInstructionsTests, testDirectConditionalCall_lessEqual_shouldNotCall)
{
    // CLE addr
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.o = 1;
    regs.flags.n = 1;
    regs.flags.z = 0;
    setMemoryMocks(0x102, 0x5555);
    testedCpu->executeInstruction(CALL_CONDITIONALLY_INSTRUCTION_OPCODE
        + static_cast<u16>(ConditionalBranch::LESS_EQUAL));
    EXPECT_EQ(0x104, regs.pc);
    EXPECT_EQ(0xFFE8, regs.sp);
}

TEST_F(JumpInstructionsTests, testIndirectCall)
{
    // CALL Rx
    const std::size_t REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.r[REG_INDEX] = 0x5555;
    setMemoryMocks(0x102, 0x5555);
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x104)).Times(1);
    testedCpu->executeInstruction(CALL_INDIRECT_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0x5555, regs.pc);
    EXPECT_EQ(0xFFEA, regs.sp);
}