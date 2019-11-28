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

    class StackInstructionsTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            memory = std::make_shared<MemoryMock>();
            testedCpu = std::make_unique<CpuImpl>(memory);
        }

        std::unique_ptr<CpuImpl> testedCpu;
        std::shared_ptr<MemoryMock> memory;

        const u16 PUSH_INSTRUCTION_OPCODE = 0xC000;
        const u16 POP_INSTRUCTION_OPCODE = 0xC100;
        const u16 PUSH_ALL_INSTRUCTION_OPCODE = 0xC200;
        const u16 POP_ALL_INSTRUCTION_OPCODE = 0xC300;
        const u16 PUSH_FLAGS_INSTRUCTION_OPCODE = 0xC400;
        const u16 POP_FLAGS_INSTRUCTION_OPCODE = 0xC500;
    };
};

TEST_F(StackInstructionsTests, testPush)
{
    // PUSH Rx
    const auto REG_INDEX = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.r[REG_INDEX] = 0x5555;
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x5555)).Times(1);
    testedCpu->executeInstruction(PUSH_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xFFEA, regs.sp);
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(StackInstructionsTests, testPop)
{
    // POP Rx
    const auto REG_INDEX = 6;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.r[REG_INDEX] = 0;
    EXPECT_CALL(*memory, readWord(0xFFE6)).Times(1).WillOnce(Return(0x5555));
    testedCpu->executeInstruction(POP_INSTRUCTION_OPCODE + REG_INDEX);
    EXPECT_EQ(0xFFE6, regs.sp);
    EXPECT_EQ(0x5555, regs.r[REG_INDEX]);
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(StackInstructionsTests, testPushAll)
{
    // PUSHALL
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0x10;

    for (auto i = 0; i < 16; i++)
        regs.r[i] = i * 0x10;

    EXPECT_CALL(*memory, writeWord(0x10, 0x00)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x12, 0x10)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x14, 0x20)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x16, 0x30)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x18, 0x40)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x1A, 0x50)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x1C, 0x60)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x1E, 0x70)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x20, 0x80)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x22, 0x90)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x24, 0xA0)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x26, 0xB0)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x28, 0xC0)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x2A, 0xD0)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x2C, 0xE0)).Times(1);
    EXPECT_CALL(*memory, writeWord(0x2E, 0xF0)).Times(1);

    testedCpu->executeInstruction(PUSH_ALL_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x30, regs.sp);
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(StackInstructionsTests, testPopAll)
{
    // POPALL
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0x30;

    for (auto i = 0; i < 16; i++)
        regs.r[i] = 0;

    EXPECT_CALL(*memory, readWord(0x2E)).Times(1).WillOnce(Return(0x2000));
    EXPECT_CALL(*memory, readWord(0x2C)).Times(1).WillOnce(Return(0x2001));
    EXPECT_CALL(*memory, readWord(0x2A)).Times(1).WillOnce(Return(0x2002));
    EXPECT_CALL(*memory, readWord(0x28)).Times(1).WillOnce(Return(0x2003));
    EXPECT_CALL(*memory, readWord(0x26)).Times(1).WillOnce(Return(0x2004));
    EXPECT_CALL(*memory, readWord(0x24)).Times(1).WillOnce(Return(0x2005));
    EXPECT_CALL(*memory, readWord(0x22)).Times(1).WillOnce(Return(0x2006));
    EXPECT_CALL(*memory, readWord(0x20)).Times(1).WillOnce(Return(0x2007));
    EXPECT_CALL(*memory, readWord(0x1E)).Times(1).WillOnce(Return(0x2008));
    EXPECT_CALL(*memory, readWord(0x1C)).Times(1).WillOnce(Return(0x2009));
    EXPECT_CALL(*memory, readWord(0x1A)).Times(1).WillOnce(Return(0x200A));
    EXPECT_CALL(*memory, readWord(0x18)).Times(1).WillOnce(Return(0x200B));
    EXPECT_CALL(*memory, readWord(0x16)).Times(1).WillOnce(Return(0x200C));
    EXPECT_CALL(*memory, readWord(0x14)).Times(1).WillOnce(Return(0x200D));
    EXPECT_CALL(*memory, readWord(0x12)).Times(1).WillOnce(Return(0x200E));
    EXPECT_CALL(*memory, readWord(0x10)).Times(1).WillOnce(Return(0x200F));

    testedCpu->executeInstruction(POP_ALL_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x10, regs.sp);
    EXPECT_EQ(0x2000, regs.r[0x0]);
    EXPECT_EQ(0x2001, regs.r[0x1]);
    EXPECT_EQ(0x2002, regs.r[0x2]);
    EXPECT_EQ(0x2003, regs.r[0x3]);
    EXPECT_EQ(0x2004, regs.r[0x4]);
    EXPECT_EQ(0x2005, regs.r[0x5]);
    EXPECT_EQ(0x2006, regs.r[0x6]);
    EXPECT_EQ(0x2007, regs.r[0x7]);
    EXPECT_EQ(0x2008, regs.r[0x8]);
    EXPECT_EQ(0x2009, regs.r[0x9]);
    EXPECT_EQ(0x200A, regs.r[0xA]);
    EXPECT_EQ(0x200B, regs.r[0xB]);
    EXPECT_EQ(0x200C, regs.r[0xC]);
    EXPECT_EQ(0x200D, regs.r[0xD]);
    EXPECT_EQ(0x200E, regs.r[0xE]);
    EXPECT_EQ(0x200F, regs.r[0xF]);
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(StackInstructionsTests, testPushFlags)
{
    // PUSHF
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.raw = 0x55;
    EXPECT_CALL(*memory, writeWord(0xFFE8, 0x55)).Times(1);
    testedCpu->executeInstruction(PUSH_FLAGS_INSTRUCTION_OPCODE);
    EXPECT_EQ(0xFFEA, regs.sp);
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(StackInstructionsTests, testPopFlags)
{
    // POPF
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.sp = 0xFFE8;
    regs.flags.raw = 0;
    EXPECT_CALL(*memory, readWord(0xFFE6)).Times(1).WillOnce(Return(0x55));
    testedCpu->executeInstruction(POP_FLAGS_INSTRUCTION_OPCODE);
    EXPECT_EQ(0xFFE6, regs.sp);
    EXPECT_EQ(0x55, regs.flags.raw);
    EXPECT_EQ(0x104, regs.pc);
}