#pragma once

#include "Types.hpp"
#include "ControllerState.hpp"

class Memory
{
public:
    virtual ~Memory() = default;

    /**
     * Reads byte from memory at given address.
     *
     * @param addr Address of the memory to read value from.
     * @return Byte from memory.
     */
    virtual u8 readByte(u16 addr) const = 0;

    /**
     * Writes byte into memory at given address.
     * 
     * @param addr Address of the memory to read value from.
     * @param byte Byte to be written in memory.
     */
    virtual void writeByte(u16 addr, u8 byte) = 0;

    /**
     * Reads word from memory at given address.
     *
     * @param addr Address of the memory to read value from.
     * @return Word from memory.
     */
    virtual u16 readWord(u16 addr) const = 0;

    /**
     * Writes word into memory at given address.
     *
     * @param addr Address of the memory to read value from.
     * @param word Word to be written in memory.
     */
    virtual void writeWord(u16 addr, u16 word) = 0;

    /**
     * Reads controlles state from memory.
     *
     * @param index Index of the controller.
     * @return Controller state.
     */
    virtual ControllerState readControllerState(unsigned index) const = 0;
};