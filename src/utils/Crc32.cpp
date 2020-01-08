#include "Crc32.hpp"

std::array<std::uint_fast32_t, Crc32::LOOKUP_TABLE_SIZE> Crc32::generateLookupTable()
{
    const auto REVERSED_POLYNOMIAL = std::uint_fast32_t{ 0xEDB88320uL };

    unsigned n = 0;
    auto byteChecksumGenerator = [&n]() {
        auto checksum = static_cast<std::uint_fast32_t>(n++);
        for (auto i = 0; i < 8; i++)
            checksum = (checksum >> 1) ^ ((checksum & 1) ? REVERSED_POLYNOMIAL : 0);

        return checksum;
    };

    auto table = std::array<std::uint_fast32_t, LOOKUP_TABLE_SIZE>{};
    std::generate(table.begin(), table.end(), byteChecksumGenerator);

    return table;
}
