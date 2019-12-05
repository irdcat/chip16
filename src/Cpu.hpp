#pragma once

#include "Types.hpp"

class Cpu
{
public:
	virtual ~Cpu() = default;

    /**
     * Fetches opcode from memory.
     *
     * @return Fetched opcode.
     */
    virtual u16 fetchOpcode() = 0;

    /**
     * Pops value from stack.
     *
     * @return Popped value.
     */
    virtual u16 popFromStack() = 0;

    /**
     * Pushes value into stack.
     *
     * @param value Value to be pushed
     */
    virtual void pushIntoStack(u16 value) = 0;

    /**
     * Executes instruction with given opcode.
     *
     * @param opcode Opcode of the instruction to be executed.
     */
	virtual void executeInstruction(u16 opcode) = 0;
};