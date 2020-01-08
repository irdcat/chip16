#pragma once

#include "Types.hpp"
#include "CpuFlags.hpp"

struct CpuRegisters
{
    u16 pc;
    u16 sp;
    u16 r[16];
    CpuFlags flags;
};