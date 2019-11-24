#pragma once

#include "Types.hpp"

class Cpu
{
public:
	virtual ~Cpu() = default;

    virtual u16 fetchOpcode() = 0;

    virtual u16 popFromStack() = 0;

    virtual void pushIntoStack(u16 value) = 0;

	virtual void executeInstruction(u16 opcode) = 0;
};