#pragma once

#include <cstdint>

union RomHeader
{
    char rawData[16];
    struct
    {
        uint32_t magicNumber;
        uint8_t reserved;
        uint8_t specVersion;
        uint32_t romSize;
        uint16_t startAddr;
        uint32_t crc32Checksum;
    };
};