#include "MemoryImpl.hpp"

Logger MemoryImpl::LOG(STRINGIFY(MemoryImpl));

MemoryImpl::MemoryImpl()
    : memory(0x10000)
{
}

u8 MemoryImpl::readByte(u16 addr) const
{
    LOG.debug("Reading byte from memory at address ", logHex(addr));
    return memory[addr];
}

void MemoryImpl::writeByte(u16 addr, u8 byte)
{
    LOG.debug("Writing byte ", logHex(byte), " into memory at address ", logHex(addr));
    memory[addr] = byte;
}

u16 MemoryImpl::readWord(u16 addr) const
{
    return readByte(addr) + readByte(addr + 1) * 0x100;
}

void MemoryImpl::writeWord(u16 addr, u16 word)
{
    writeByte(addr, word & 0xFF);
    writeByte(addr + 1, (word >> 8) & 0xFF);
}

ControllerState MemoryImpl::readControllerState(unsigned index) const
{
    if (index > 2) 
        index = 2;
    
    u16 rawStateValue = readWord(0xFFF0 + index * 2);
    ControllerState state;
    state.raw = rawStateValue;
    return state;
}
