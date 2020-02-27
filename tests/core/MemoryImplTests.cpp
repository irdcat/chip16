#include <memory>
#include <sstream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../src/core/MemoryImpl.hpp"

namespace
{
    class MemoryImplTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            testedMemory = std::make_unique<MemoryImpl>();
        }

        std::unique_ptr<MemoryImpl> testedMemory;
    };
}

TEST_F(MemoryImplTests, testReadByte)
{
    testedMemory->writeData(0, 0x77, 0x87);
    EXPECT_EQ(0x77, testedMemory->readByte(0x0));
    EXPECT_EQ(0x87, testedMemory->readByte(0x1));
}

TEST_F(MemoryImplTests, testWriteByte)
{
    testedMemory->writeData(0, 0x77, 0x87);
    testedMemory->writeByte(0x0, 0x25);
    EXPECT_EQ(0x25, testedMemory->readByte(0x0));
    EXPECT_EQ(0x87, testedMemory->readByte(0x1));
}

TEST_F(MemoryImplTests, testReadWord)
{
    testedMemory->writeData(0, 0x77, 0x87);
    EXPECT_EQ(0x8777, testedMemory->readWord(0x0));
}

TEST_F(MemoryImplTests, testWriteWord)
{
    testedMemory->writeData(0, 0x77, 0x87);
    testedMemory->writeWord(0x0, 0x2587);
    EXPECT_EQ(0x87, testedMemory->readByte(0x0));
    EXPECT_EQ(0x25, testedMemory->readByte(0x1));
}

TEST_F(MemoryImplTests, testReadControllerState)
{
    testedMemory->writeData(0xFFF0, 0x00, 0x89);
    EXPECT_EQ(0x8900, testedMemory->readControllerState(0).raw);
}

TEST_F(MemoryImplTests, testLoadRomFromStream)
{
    const char* ROM = "\x31\x11\x02\x24\x55\x65\x42\x21\x20\x20\x20\x00\x00\x00\x00\x00"
                      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    const std::string ROM_STR(ROM, 32);
    std::stringstream romStream;
    romStream.write(&ROM_STR[0], ROM_STR.size());

    testedMemory->loadRomFromStream(romStream);

    auto word1 = testedMemory->readWord(0x0000); // Word at addr 0
    auto word2 = testedMemory->readWord(0x0002); // Word at addr 2
    auto word3 = testedMemory->readWord(0x0004); // Word at addr 4

    auto byte1 = testedMemory->readByte(0x0006); // Word at addr 6
    auto byte2 = testedMemory->readByte(0x0007); // Word at addr 7

    EXPECT_EQ(0x1131, word1);
    EXPECT_EQ(0x2402, word2);
    EXPECT_EQ(0x6555, word3);

    EXPECT_EQ(0x42, byte1);
    EXPECT_EQ(0x21, byte2);
}