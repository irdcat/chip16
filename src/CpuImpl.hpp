#pragma once

#include <memory>

#include "Cpu.hpp"
#include "CpuFlags.hpp"
#include "Memory.hpp"
#include "ConditionalBranch.hpp"
#include "log/Logger.hpp"

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

    bool executeJumpInstruction(u16 opcode);      // 1x
    bool executeLoadInstruction(u16 opcode);      // 2x
    bool executeStoreInstruction(u16 opcode);     // 3x
    bool executeBitwiseOrInstruction(u16 opcode); // 7x
    bool executeStackInstruction(u16 opcode);     // Cx

    bool evaluateBranchCondition(unsigned index);
    unsigned decodeNibble(u16 word, unsigned nibblePos);

    bool isZero(unsigned data);
    bool isNegative(unsigned data);

    static Logger LOG;
};