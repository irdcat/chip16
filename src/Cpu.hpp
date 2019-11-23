#pragma once

#include "Types.hpp"

class Cpu
{
public:
	virtual ~Cpu() = default;

    virtual u16 fetchOpcode() = 0;

	virtual void executeInstruction() = 0;
};