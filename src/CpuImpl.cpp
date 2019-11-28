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
    const auto group = decodeNibble(opcode, 3);
    bool result = false;
    switch (group)
    {
    case 0x1: 
        result = executeJumpInstruction(opcode); 
        break;
    case 0x2:
        result = executeLoadInstruction(opcode);
        break;
    case 0x3:
        result = executeStoreInstruction(opcode);
        break;
    case 0xC:
        result = executeStackInstruction(opcode);
        break;
    }

    if (!result) 
    {
        // TODO: Log error
    }
}

CpuImpl::Registers& CpuImpl::getRegisters()
{
    return registers;
}

bool CpuImpl::executeJumpInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);
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
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
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
        const auto REG_INDEX = decodeNibble(opcode, 0);
        auto addr = registers.r[REG_INDEX];
        registers.pc = addr;
    }
    else if (innerInstructionIndex == 7)
    {
        bool condition = evaluateBranchCondition(decodeNibble(opcode, 0));
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
        const auto REG_INDEX = decodeNibble(opcode, 0);
        auto addr = registers.r[REG_INDEX];
        registers.pc += 2;
        pushIntoStack(registers.pc);
        registers.pc = addr;
    }
    return true;
}

bool CpuImpl::executeLoadInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);
    if (innerInstructionIndex > 4)
        return false;

    if (innerInstructionIndex == 0)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto word = memory->readWord(registers.pc);
        registers.pc += 2;
        registers.r[REG_INDEX] = word;
    }
    else if (innerInstructionIndex == 1)
    {
        const auto word = memory->readWord(registers.pc);
        registers.pc += 2;
        registers.sp = word;
    }
    else if (innerInstructionIndex == 2)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto addr = memory->readWord(registers.pc);
        registers.pc += 2;
        const auto word = memory->readWord(addr);
        registers.r[REG_INDEX] = word;
    }
    else if (innerInstructionIndex == 3)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto addr = registers.r[REG_INDEX_Y];
        const auto word = memory->readWord(addr);
        registers.pc += 2;
        registers.r[REG_INDEX_X] = word;
    }
    else if (innerInstructionIndex == 4)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        registers.pc += 2;
        registers.r[REG_INDEX_X] = registers.r[REG_INDEX_Y];
    }

    return true;
}

bool CpuImpl::executeStoreInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);
    if (innerInstructionIndex > 1)
        return false;

    if (innerInstructionIndex == 0)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto addr = memory->readWord(registers.pc);
        registers.pc += 2;
        memory->writeWord(addr, registers.r[REG_INDEX]);
    }
    else if (innerInstructionIndex == 1)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto addr = registers.r[REG_INDEX_Y];
        registers.pc += 2;
        memory->writeWord(addr, registers.r[REG_INDEX_X]);
    }

    return true;
}

bool CpuImpl::executeStackInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);
    if (innerInstructionIndex > 5)
        return false;

    if (innerInstructionIndex == 0)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        pushIntoStack(registers.r[REG_INDEX]);
    }
    else if (innerInstructionIndex == 1)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        registers.r[REG_INDEX] = popFromStack();
    }
    else if (innerInstructionIndex == 2)
    {
        for (auto i = 0; i < 16; i++)
            pushIntoStack(registers.r[i]);
    }
    else if (innerInstructionIndex == 3)
    {
        for (auto i = 0; i < 16; i++)
            registers.r[i] = popFromStack();
    }
    else if (innerInstructionIndex == 4)
    {
        pushIntoStack(registers.flags.raw);
    }
    else if (innerInstructionIndex == 5)
    {
        registers.flags.raw = popFromStack() & 0xFF;
    }

    registers.pc += 2;
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

unsigned CpuImpl::decodeNibble(u16 word, unsigned nibblePos)
{
    if (nibblePos < 4)
    {
        // Hex digit is 4 bit long
        return (word >> (nibblePos * 4)) & 0xF;
    }
    else
    {
        return 0;
    }
}
