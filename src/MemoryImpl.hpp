#pragma once

#include <vector>
#include <utility>

#include "Memory.hpp"
#include "log/Logger.hpp"
#include "log/HexModificator.hpp"

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
    
    std::vector<u8>::const_iterator readByteReference(u16 addr) const override;

    template <typename T, typename ...Args>
    void writeData(u16 startPos, T data, Args ...args);

    void writeData(u16 startPos);

private:
    std::vector<u8> memory;

    static Logger LOG;
};

template<typename T, typename ...Args>
inline void MemoryImpl::writeData(u16 startPos, T data, Args ...args)
{
    memory[startPos] = data;
    this->writeData(++startPos, args...);
}

inline void MemoryImpl::writeData(u16 startPos)
{
}
