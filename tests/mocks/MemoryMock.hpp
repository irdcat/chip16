#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../src/core/Memory.hpp"

class MemoryMock : public Memory
{
public:
    MOCK_CONST_METHOD1(readByte, u8(u16));
    MOCK_METHOD2(writeByte, void(u16, u8));
    MOCK_CONST_METHOD1(readWord, u16(u16));
    MOCK_METHOD2(writeWord, void(u16, u16));
    MOCK_CONST_METHOD1(readControllerState, ControllerState(unsigned));
    MOCK_CONST_METHOD1(readByteReference, std::vector<u8>::const_iterator(u16));
    MOCK_METHOD1(loadRomFromStream, void(std::istream&));
};