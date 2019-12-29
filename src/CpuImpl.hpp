#pragma once

#include <memory>

#include "Cpu.hpp"
#include "CpuFlags.hpp"
#include "Memory.hpp"
#include "Bus.hpp"
#include "ConditionalBranch.hpp"
#include "log/Logger.hpp"
#include "log/HexModificator.hpp"
#include "utils/Random.hpp"

class CpuImpl : public Cpu
{
public:
	CpuImpl() = default;

    CpuImpl(const std::shared_ptr<Memory>& memory, const std::shared_ptr<Bus>& bus);

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
    bool executeMiscInstruction(u16 opcode);           // 0x
    bool executeJumpInstruction(u16 opcode);           // 1x
    bool executeLoadInstruction(u16 opcode);           // 2x
    bool executeStoreInstruction(u16 opcode);          // 3x
    bool executeAdditionInstruction(u16 opcode);       // 4x
    bool executeSubtractionInstruction(u16 opcode);    // 5x
    bool executeBitwiseAndInstruction(u16 opcode);     // 6x
    bool executeBitwiseOrInstruction(u16 opcode);      // 7x
    bool executeBitwiseXorInstruction(u16 opcode);     // 8x
    bool executeMultiplicationInstruction(u16 opcode); // 9x
    bool executeShiftInstruction(u16 opcode);          // Bx
    bool executeStackInstruction(u16 opcode);          // Cx
    bool executePaletteInstruction(u16 opcode);        // Dx
    bool executeNegationInstruction(u16 opcode);       // Ex

    bool evaluateBranchCondition(unsigned index);
    unsigned decodeNibble(u16 word, unsigned nibblePos);

    bool isZero(unsigned data) const;
    bool isNegative(unsigned data) const;

    bool isAdditionCarry(unsigned data) const;
    bool isAdditionOverflow(unsigned operand1, unsigned operand2, unsigned result) const;

    bool isSubtractionBorrow(unsigned result) const;
    bool isSubtractionOverflow(unsigned operand1, unsigned operand2, unsigned result) const;
    
    bool isMultiplicationCarry(unsigned result) const;

    u16 negate(u16 word);

    Registers registers;
    std::shared_ptr<Memory> memory;
    std::shared_ptr<Bus> bus;

    static Logger LOG;
};