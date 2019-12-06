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
    };

    class BusMock : public Bus
    {
    public:
        MOCK_METHOD1(loadPalette, void(const Palette&));
    };

    class CpuImplTests : public ::testing::Test
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
    };
};

TEST_F(CpuImplTests, fetchOpcodeTest)
{
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x120;
    EXPECT_CALL(*memory, readWord(0x120)).Times(1).WillOnce(Return(0x5432));
    auto result = testedCpu->fetchOpcode();
    EXPECT_EQ(0x5432, result);
}

TEST_F(CpuImplTests, popFromStackTest)
{
    auto& regs = testedCpu->getRegisters();
    regs.sp = 0xFFE0;
    EXPECT_CALL(*memory, readWord(0xFFDE)).Times(1).WillOnce(Return(0x5555));
    auto result = testedCpu->popFromStack();
    EXPECT_EQ(0x5555, result);
    EXPECT_EQ(0xFFDE, regs.sp);
}

TEST_F(CpuImplTests, pushIntoStackTest)
{
    auto& regs = testedCpu->getRegisters();
    regs.sp = 0xFFE0;
    EXPECT_CALL(*memory, writeWord(0xFFE0, 0x7777)).Times(1);
    testedCpu->pushIntoStack(0x7777);
    EXPECT_EQ(0xFFE2, regs.sp);
}