#pragma once

#include <memory>

#include "Cpu.hpp"
#include "CpuFlags.hpp"
#include "Memory.hpp"

class CpuImpl : public Cpu
{
public:
	CpuImpl() = default;

    CpuImpl(const std::shared_ptr<Memory>& memory);

    ~CpuImpl() = default;

    u16 fetchOpcode() override;

    u16 popFromStack() override;

    void pushIntoStack(u16 value) override;

	void executeInstruction(u16 opcode) override;

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
    std::shared_ptr<Memory> memory;
};