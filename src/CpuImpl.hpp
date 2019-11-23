#pragma once

#include "Cpu.hpp"
#include "CpuFlags.hpp"

class CpuImpl : public Cpu
{
public:
	CpuImpl() = default;

    ~CpuImpl() = default;

    u16 fetchOpcode() override;

	void executeInstruction() override;

    struct Registers 
    {
        u16 pc;
        u16 sp;
        u16 r[16];
        CpuFlags flags;
    };

    Registers& getRegisters();

private:
    Registers registers;
};