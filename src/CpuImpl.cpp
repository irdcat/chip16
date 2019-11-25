#include "CpuImpl.hpp"

CpuImpl::CpuImpl(const std::shared_ptr<Memory>& memory)
    : memory(memory)
    , registers()
{
}

u16 CpuImpl::fetchOpcode()
{
    u16 opcode = memory->readWord(registers.pc);
    registers.pc += 2;

    return opcode;
}

u16 CpuImpl::popFromStack()
{
    registers.sp -= 2;
    return memory->readWord(registers.sp);
}

void CpuImpl::pushIntoStack(u16 value)
{
    memory->writeWord(registers.sp, value);
    registers.sp += 2;
}

void CpuImpl::executeInstruction(u16 opcode)
{
    const auto group = (opcode & 0xF000) >> 12;
    switch (group)
    {
    case 0x1: 
        executeJumpInstruction(opcode); 
        break;
    }
}

CpuImpl::Registers& CpuImpl::getRegisters()
{
    return registers;
}

bool CpuImpl::executeJumpInstruction(u16 opcode)
{
    const auto innerInstructionIndex = (opcode & 0xF00) >> 8;
    if (innerInstructionIndex > 8)
        return false;

    if (innerInstructionIndex == 0)
    {
        auto addr = memory->readWord(registers.pc);
        registers.pc = addr;
    }
    else if (innerInstructionIndex == 1)
    {
        if (registers.flags.c == 1)
        {
            auto addr = memory->readWord(registers.pc);
            registers.pc = addr;
        }
        else
        {
            registers.pc += 2;
        }
    }
    else if (innerInstructionIndex == 2)
    {
        bool condition = evaluateBranchCondition(opcode & 0xF);
        if (condition)
        {
            auto addr = memory->readWord(registers.pc);
            registers.pc = addr;
        }
        else
        {
            registers.pc += 2;
        }
    }
    else if (innerInstructionIndex == 3)
    {
        const auto REG_INDEX_X = opcode & 0xF;
        const auto REG_INDEX_Y = (opcode & 0xF0) >> 4;
        if (registers.r[REG_INDEX_X] == registers.r[REG_INDEX_Y])
        {
            auto addr = memory->readWord(registers.pc);
            registers.pc = addr;
        }
        else
        {
            registers.pc += 2;
        }
    }
    else if (innerInstructionIndex == 4)
    {
        auto addr = memory->readWord(registers.pc);
        registers.pc += 2;
        pushIntoStack(registers.pc);
        registers.pc = addr;
    }
    else if (innerInstructionIndex == 5)
    {
        auto addr = popFromStack();
        registers.pc = addr;
    }
    else if (innerInstructionIndex == 6)
    {
        const auto REG_INDEX = opcode & 0xF;
        auto addr = registers.r[REG_INDEX];
        registers.pc = addr;
    }
    else if (innerInstructionIndex == 7)
    {
        bool condition = evaluateBranchCondition(opcode & 0xF);
        if (condition)
        {
            auto addr = memory->readWord(registers.pc);
            registers.pc += 2;
            pushIntoStack(registers.pc);
            registers.pc = addr;
        }
        else
        {
            registers.pc += 2;
        }
    }
    else if (innerInstructionIndex == 8)
    {
        const auto REG_INDEX = opcode & 0xF;
        auto addr = registers.r[REG_INDEX];
        registers.pc += 2;
        pushIntoStack(registers.pc);
        registers.pc = addr;
    }
    return true;
}

bool CpuImpl::evaluateBranchCondition(unsigned index)
{
    ConditionalBranch conditionalBranch = static_cast<ConditionalBranch>(index);
    const auto flags = registers.flags;

    switch (conditionalBranch)
    {
    case ConditionalBranch::ZERO: 
        return flags.z == 1;
    case ConditionalBranch::NOT_ZERO: 
        return flags.z == 0;
    case ConditionalBranch::NEGATIVE:
        return flags.n == 1;
    case ConditionalBranch::NOT_NEGATIVE:
        return flags.n == 0;
    case ConditionalBranch::POSITIVE:
        return flags.n == 0 && flags.z == 0;
    case ConditionalBranch::OVERFLOWED:
        return flags.o == 1;
    case ConditionalBranch::NOT_OVERFLOW:
        return flags.o == 0;
    case ConditionalBranch::ABOVE:
        return flags.c == 0 && flags.z == 0;
    case ConditionalBranch::ABOVE_EQUAL:
        return flags.c == 0;
    case ConditionalBranch::BELOW:
        return flags.c == 1;
    case ConditionalBranch::BELOW_EQUAL:
        return flags.c == 1 || flags.z == 1;
    case ConditionalBranch::GREATER:
        return flags.o == flags.n && flags.z == 0;
    case ConditionalBranch::GREATER_EQUAL:
        return flags.o == flags.n;
    case ConditionalBranch::LESS:
        return flags.o != flags.n;
    case ConditionalBranch::LESS_EQUAL:
        return flags.o != flags.n || flags.z == 1;
    }

    return false;
}
