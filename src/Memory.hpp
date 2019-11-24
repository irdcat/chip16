#pragma once

#include "Types.hpp"
#include "ControllerState.hpp"

class Memory
{
public:
    virtual ~Memory() = default;

    virtual u8 readByte(u16 addr) const = 0;

    virtual void writeByte(u16 addr, u8 byte) = 0;

    virtual u16 readWord(u16 addr) const = 0;

    virtual void writeWord(u16 addr, u16 word) = 0;

    virtual ControllerState readControllerState(unsigned index) const = 0;
};