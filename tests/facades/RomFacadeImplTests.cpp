#include <memory>
#include <sstream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../mocks//CpuMock.hpp"
#include "../mocks/MemoryMock.hpp"
#include "../mocks/RomInputStreamMock.hpp"

#include "../../src/facades/RomFacadeImpl.hpp"

namespace
{
    using ::testing::Return;
    using ::testing::ReturnRef;
    using ::testing::Eq;
    using ::testing::_;

    class RomFacadeImplTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            cpu = std::make_shared<CpuMock>();
            memory = std::make_shared<MemoryMock>();
            testedFacade = std::make_unique<RomFacadeImpl>(cpu, memory);

            ON_CALL(*cpu, getRegisters).WillByDefault(ReturnRef(testCpuRegisters));
        }

        CpuRegisters testCpuRegisters;
        std::shared_ptr<CpuMock> cpu;
        std::shared_ptr<MemoryMock> memory;
        std::unique_ptr<RomFacadeImpl> testedFacade;
    };
};

TEST_F(RomFacadeImplTests, testLoadRomWithoutHeader)
{
    const char * ROM_WITHOUT_HEADER =
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" // 8 NOP instructions
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";// 8 NOP instructions
    const std::string ROM_WITHOUT_HEADER_STR(ROM_WITHOUT_HEADER, 32);
    std::stringstream romWithoutHeaderStream;
    romWithoutHeaderStream.write(&ROM_WITHOUT_HEADER_STR[0], ROM_WITHOUT_HEADER_STR.size());

    auto romInputStream = std::make_shared<RomInputStreamMock>();
    EXPECT_CALL(*romInputStream, getStream()).Times(1).WillOnce(ReturnRef(romWithoutHeaderStream));
    testCpuRegisters.pc = 0xFFFF;

    EXPECT_CALL(*memory, loadRomFromStream(_)).Times(1);
    auto result = testedFacade->loadRomIntoMemory(romInputStream);

    EXPECT_TRUE(result);
    EXPECT_EQ(0, testCpuRegisters.pc);
}

TEST_F(RomFacadeImplTests, testLoadRomWithCorrectHeader)
{
    const char * ROM_WITH_CORRECT_HEADER =
        "\x43\x48\x31\x36\x00\x11\x20\x00\x00\x00\x00\x00\xAD\x55\x0A\x19" // Magic number, Version 1.1, Rom size 32B, Start addr 0x00, checksum 0x190A55AD
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" // 8 NOP instructions
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";// 8 NOP instructions
    const std::string ROM_WITH_CORRECT_HEADER_STR(ROM_WITH_CORRECT_HEADER, 48);
    std::stringstream romWithCorrectHeader;
    romWithCorrectHeader.write(&ROM_WITH_CORRECT_HEADER_STR[0], ROM_WITH_CORRECT_HEADER_STR.size());

    auto romInputStream = std::make_shared<RomInputStreamMock>();
    EXPECT_CALL(*romInputStream, getStream()).Times(1).WillOnce(ReturnRef(romWithCorrectHeader));
    testCpuRegisters.pc = 0xFFFF;

    EXPECT_CALL(*memory, loadRomFromStream(_)).Times(1);
    auto result = testedFacade->loadRomIntoMemory(romInputStream);

    EXPECT_TRUE(result);
    EXPECT_EQ(0, testCpuRegisters.pc);
}

TEST_F(RomFacadeImplTests, testLoadRomWithIncorrectChecksum)
{
    const char * INCORRECT_CHECKSUM_ROM =
        "\x43\x48\x31\x36\x00\x11\x20\x00\x00\x00\x00\x00\xAD\x55\x0A\x38" // Magic number, Version 1.1, Rom size 32B, Start addr 0x00, checksum 0x380A55AD
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" // 8 NOP instructions
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";// 8 NOP instructions
    const std::string INCORRECT_CHECKSUM_ROM_STR(INCORRECT_CHECKSUM_ROM, 48);
    std::stringstream romWithIncorrectChecksum;
    romWithIncorrectChecksum.write(&INCORRECT_CHECKSUM_ROM_STR[0], INCORRECT_CHECKSUM_ROM_STR.size());

    auto romInputStream = std::make_shared<RomInputStreamMock>();
    EXPECT_CALL(*romInputStream, getStream()).Times(1).WillOnce(ReturnRef(romWithIncorrectChecksum));
    testCpuRegisters.pc = 0xFFFF;

    auto result = testedFacade->loadRomIntoMemory(romInputStream);

    EXPECT_FALSE(result);
    EXPECT_EQ(0xFFFF, testCpuRegisters.pc);
}

TEST_F(RomFacadeImplTests, testLoadRomWithNonZeroStartAddr)
{
    const char * NON_ZERO_START_ADDR_ROM =
        "\x43\x48\x31\x36\x00\x11\x20\x00\x00\x00\x16\x00\xAD\x55\x0A\x19" // Magic number, Version 1.1, Rom size 32B, Start addr 0x20, checksum 0x190A55AD
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" // 8 NOP instructions
        "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";// 8 NOP instructions
    const std::string NON_ZERO_START_ADDR_ROM_STR(NON_ZERO_START_ADDR_ROM, 48);
    std::stringstream romWithNonZeroStartAddr;
    romWithNonZeroStartAddr.write(&NON_ZERO_START_ADDR_ROM_STR[0], NON_ZERO_START_ADDR_ROM_STR.size());

    auto romInputStream = std::make_shared<RomInputStreamMock>();
    EXPECT_CALL(*romInputStream, getStream()).Times(1).WillOnce(ReturnRef(romWithNonZeroStartAddr));
    testCpuRegisters.pc = 0xFFFF;

    EXPECT_CALL(*memory, loadRomFromStream(_)).Times(1);
    auto result = testedFacade->loadRomIntoMemory(romInputStream);

    EXPECT_TRUE(result);
    EXPECT_EQ(0x16, testCpuRegisters.pc);
}