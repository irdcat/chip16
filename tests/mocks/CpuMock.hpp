#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../src/core/Cpu.hpp"

class CpuMock : public Cpu
{
public:
    MOCK_METHOD0(fetchOpcode, u16());
    MOCK_METHOD0(popFromStack, u16());
    MOCK_METHOD1(pushIntoStack, void(u16));
    MOCK_METHOD1(executeInstruction, void(u16));
    MOCK_METHOD0(getRegisters, CpuRegisters& ());
};