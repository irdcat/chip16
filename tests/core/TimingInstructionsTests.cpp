#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../mocks/BusMock.hpp"
#include "../mocks/MemoryMock.hpp"

#include "../../src/core/CpuImpl.hpp"

namespace
{
    using ::testing::Return;

    class TimingInstructionsTests : public ::testing::Test
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

        const u16 NOP_INSTRUCTION_OPCODE = 0x0000;
        const u16 VBLNK_INSTRUCTION_OPCODE = 0x0200;
    };
};

TEST_F(TimingInstructionsTests, testNop)
{
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    testedCpu->executeInstruction(NOP_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x104, regs.pc);
}

TEST_F(TimingInstructionsTests, testVBlnk_vBlankNotSet)
{
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    EXPECT_CALL(*bus, isVBlank).Times(1).WillOnce(Return(false));
    testedCpu->executeInstruction(VBLNK_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x100, regs.pc);
}

TEST_F(TimingInstructionsTests, testVBlnk_vBlankSet)
{
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    EXPECT_CALL(*bus, isVBlank).Times(1).WillOnce(Return(true));
    testedCpu->executeInstruction(VBLNK_INSTRUCTION_OPCODE);
    EXPECT_EQ(0x104, regs.pc);
}