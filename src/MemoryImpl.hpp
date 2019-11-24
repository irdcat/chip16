#pragma once

#include <vector>

#include "Memory.hpp"

class MemoryImpl : public Memory
{
public:
    MemoryImpl();

    ~MemoryImpl() = default;

    u8 readByte(u16 addr) const override;

    void writeByte(u16 addr, u8 byte) override;

    u16 readWord(u16 addr) const override;

    void writeWord(u16 addr, u16 word) override;

    ControllerState readControllerState(unsigned index) const override;

private:
    std::vector<u8> memory;
};