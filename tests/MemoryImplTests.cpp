#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/MemoryImpl.hpp"

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