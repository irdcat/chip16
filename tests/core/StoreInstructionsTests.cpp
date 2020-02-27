#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../mocks/BusMock.hpp"
#include "../mocks/MemoryMock.hpp"

#include "../../src/core/CpuImpl.hpp"

namespace
{
    using ::testing::Return;

    class StoreInstructionsTests : public ::testing::Test
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

        const u16 STORE_INDIRECT_INSTRUCTION_OPCODE = 0x3000;
        const u16 STORE_INDEXED_INSTRUCTION_OPCODE = 0x3100;
    };
};

TEST_F(StoreInstructionsTests, storeIndirectTest)
{
    // STM Rx, addr
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX] = 0x9876;
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x5555));
    EXPECT_CALL(*memory, writeWord(0x5555, 0x9876)).Times(1);
    testedCpu->executeInstruction(STORE_INDIRECT_INSTRUCTION_OPCODE + REG_INDEX);
}

TEST_F(StoreInstructionsTests, storeIndexedTest)
{
    // STM Rx, Ry
    const auto REG_INDEX_X = 5;
    const auto REG_INDEX_Y = 7;
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    regs.r[REG_INDEX_X] = 0x9876;
    regs.r[REG_INDEX_Y] = 0x5555;
    EXPECT_CALL(*memory, writeWord(0x5555, 0x9876)).Times(1);
    testedCpu->executeInstruction(STORE_INDEXED_INSTRUCTION_OPCODE
        + REG_INDEX_X + (REG_INDEX_Y << 4));
}