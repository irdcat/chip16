#pragma once

#include <array>
#include <cstdint>
#include <numeric>
#include <algorithm>

class Crc32
{
public:
    template <typename Iterator>
    static std::uint_fast32_t checksum(Iterator first, Iterator last);

private:
    static constexpr std::size_t LOOKUP_TABLE_SIZE = 256;

    static std::array<std::uint_fast32_t, LOOKUP_TABLE_SIZE> generateLookupTable();
};

template<typename Iterator>
inline std::uint_fast32_t Crc32::checksum(Iterator first, Iterator last)
{
    static const auto LOOKUP_TABLE = generateLookupTable();

    return std::uint_fast32_t{ 0xFFFFFFFFuL }
        & ~std::accumulate(first, last, std::uint_fast32_t{ 0xFFFFFFFFuL },
            [](std::uint_fast32_t checksum, std::uint_fast8_t value) {
                return LOOKUP_TABLE[(checksum ^ value) & 0xFFu] ^ (checksum >> 8);
            });
}
