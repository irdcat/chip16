#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include "../../src/utils/Crc32.hpp"

namespace
{
    class Crc32Tests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
        }

        template <typename Container>
        struct Crc32TestData
        {
            Container data;
            std::uint_fast32_t expectedChecksum;
        };

        template <typename Container>
        Crc32TestData<Container> createTestData(Container data, std::uint_fast32_t expectedChecksum)
        {
            Crc32TestData<Container> crc32TestData;
            crc32TestData.data = data;
            crc32TestData.expectedChecksum = expectedChecksum;
            return crc32TestData;
        }
    };
};

TEST_F(Crc32Tests, testCrcChecksum1)
{
    auto crc32TestData = createTestData(std::string("The quick brown fox jumps over the lazy dog"), 0x414FA339);
    auto calculatedChecksum = Crc32::checksum(crc32TestData.data.begin(), crc32TestData.data.end());
    EXPECT_EQ(calculatedChecksum, crc32TestData.expectedChecksum);
}

TEST_F(Crc32Tests, testCrcChecksum2)
{
    std::vector<std::uint8_t> data(32, 0);
    auto crc32TestData = createTestData(data, 0x190A55AD);
    auto calculatedChecksum = Crc32::checksum(crc32TestData.data.begin(), crc32TestData.data.end());
    EXPECT_EQ(calculatedChecksum, crc32TestData.expectedChecksum);
}

TEST_F(Crc32Tests, testCrcChecksum3)
{
    std::vector<std::uint8_t> data(32, 0xFF);
    auto crc32TestData = createTestData(data, 0xFF6CAB0B);
    auto calculatedChecksum = Crc32::checksum(crc32TestData.data.begin(), crc32TestData.data.end());
    EXPECT_EQ(calculatedChecksum, crc32TestData.expectedChecksum);
}