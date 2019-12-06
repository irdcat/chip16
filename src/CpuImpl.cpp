#include "CpuImpl.hpp"

Logger CpuImpl::LOG(STRINGIFY(CpuImpl));

CpuImpl::CpuImpl(const std::shared_ptr<Memory>& memory, const std::shared_ptr<Bus>& bus)
    : memory(memory)
    , bus(bus)
    , registers()
{
}

u16 CpuImpl::fetchOpcode()
{
    LOG.debug("Fetching opcode.");
    u16 opcode = memory->readWord(registers.pc);
    registers.pc += 2;

    return opcode;
}

u16 CpuImpl::popFromStack()
{
    LOG.debug("Popping from stack.");
    registers.sp -= 2;
    return memory->readWord(registers.sp);
}

void CpuImpl::pushIntoStack(u16 value)
{
    LOG.debug("Pushing into stack: ", logHex(value));
    memory->writeWord(registers.sp, value);
    registers.sp += 2;
}

void CpuImpl::executeInstruction(u16 opcode)
{
    LOG.debug("Executing opcode: ", logHex(opcode));
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
    case 0x6:
        result = executeBitwiseAndInstruction(opcode);
        break;
    case 0x7:
        result = executeBitwiseOrInstruction(opcode);
        break;
    case 0x8:
        result = executeBitwiseXorInstruction(opcode);
        break;
    case 0xB:
        result = executeShiftInstruction(opcode);
        break;
    case 0xC:
        result = executeStackInstruction(opcode);
        break;
    case 0xD:
        result = executePaletteInstruction(opcode);
        break;
    case 0xE:
        result = executeNegationInstruction(opcode);
        break;
    }

    if (!result) 
        LOG.error("Unknown opcode: ", logHex(opcode));
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
        registers.pc = memory->readWord(registers.pc);
    }
    else if (innerInstructionIndex == 1)
    {
        registers.pc = registers.flags.c == 1 ? memory->readWord(registers.pc) : registers.pc + 2;
    }
    else if (innerInstructionIndex == 2)
    {
        if (decodeNibble(opcode, 0) == 0xF) 
        {
            registers.pc += 2;
            return false;
        }
        registers.pc = evaluateBranchCondition(decodeNibble(opcode, 0)) ?
            memory->readWord(registers.pc) : registers.pc + 2;
    }
    else if (innerInstructionIndex == 3)
    {
        registers.pc = registers.r[decodeNibble(opcode, 0)] == registers.r[decodeNibble(opcode, 1)] ?
            memory->readWord(registers.pc) : registers.pc + 2;
    }
    else if (innerInstructionIndex == 4)
    {
        auto addr = memory->readWord(registers.pc);
        pushIntoStack(registers.pc + 2);
        registers.pc = addr;
    }
    else if (innerInstructionIndex == 5)
    {
        registers.pc = popFromStack();
    }
    else if (innerInstructionIndex == 6)
    {
        registers.pc = registers.r[decodeNibble(opcode, 0)];
    }
    else if (innerInstructionIndex == 7)
    {
        if (decodeNibble(opcode, 0) == 0xF)
        {
            registers.pc += 2;
            return false;
        }
        auto addr = memory->readWord(registers.pc);
        bool condition = evaluateBranchCondition(decodeNibble(opcode, 0));
        if(condition)
            pushIntoStack(registers.pc + 2);
        registers.pc = condition ? addr : registers.pc + 2;
    }
    else if (innerInstructionIndex == 8)
    {
        auto addr = registers.r[decodeNibble(opcode, 0)];
        pushIntoStack(registers.pc + 2);
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
        registers.r[REG_INDEX] = word;
    }
    else if (innerInstructionIndex == 1)
    {
        const auto word = memory->readWord(registers.pc);
        registers.sp = word;
    }
    else if (innerInstructionIndex == 2)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto addr = memory->readWord(registers.pc);
        const auto word = memory->readWord(addr);
        registers.r[REG_INDEX] = word;
    }
    else if (innerInstructionIndex == 3)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto addr = registers.r[REG_INDEX_Y];
        const auto word = memory->readWord(addr);
        registers.r[REG_INDEX_X] = word;
    }
    else if (innerInstructionIndex == 4)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        registers.r[REG_INDEX_X] = registers.r[REG_INDEX_Y];
    }
    registers.pc += 2;
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
        memory->writeWord(addr, registers.r[REG_INDEX]);
    }
    else if (innerInstructionIndex == 1)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto addr = registers.r[REG_INDEX_Y];
        memory->writeWord(addr, registers.r[REG_INDEX_X]);
    }
    registers.pc += 2;
    return true;
}

bool CpuImpl::executeBitwiseAndInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);
    if (innerInstructionIndex > 4)
        return false;

    if (innerInstructionIndex == 0)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto word = memory->readWord(registers.pc);
        registers.r[REG_INDEX] &= word;
        registers.flags.z = isZero(registers.r[REG_INDEX]);
        registers.flags.n = isNegative(registers.r[REG_INDEX]);
    }
    else if (innerInstructionIndex == 1)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        registers.r[REG_INDEX_X] &= registers.r[REG_INDEX_Y];
        registers.flags.z = isZero(registers.r[REG_INDEX_X]);
        registers.flags.n = isNegative(registers.r[REG_INDEX_X]);
    }
    else if (innerInstructionIndex == 2)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto REG_INDEX_Z = decodeNibble(memory->readWord(registers.pc), 2);
        registers.r[REG_INDEX_Z] = registers.r[REG_INDEX_X] & registers.r[REG_INDEX_Y];
        registers.flags.z = isZero(registers.r[REG_INDEX_Z]);
        registers.flags.n = isNegative(registers.r[REG_INDEX_Z]);
    }
    else if (innerInstructionIndex == 3)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto word = memory->readWord(registers.pc);
        const u16 result = registers.r[REG_INDEX] & word;
        registers.flags.z = isZero(result);
        registers.flags.n = isNegative(result);
    }
    else if (innerInstructionIndex == 4)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const u16 result = registers.r[REG_INDEX_X] & registers.r[REG_INDEX_Y];
        registers.flags.z = isZero(result);
        registers.flags.n = isNegative(result);
    }
    registers.pc += 2;
    return true;
}

bool CpuImpl::executeBitwiseOrInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);
    if (innerInstructionIndex > 2)
        return false;

    if (innerInstructionIndex == 0)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto word = memory->readWord(registers.pc);
        registers.r[REG_INDEX] |= word;
        registers.flags.n = isNegative(registers.r[REG_INDEX]);
        registers.flags.z = isZero(registers.r[REG_INDEX]);
    }
    else if (innerInstructionIndex == 1)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        registers.r[REG_INDEX_X] |= registers.r[REG_INDEX_Y];
        registers.flags.n = isNegative(registers.r[REG_INDEX_X]);
        registers.flags.z = isZero(registers.r[REG_INDEX_X]);
    }
    else if (innerInstructionIndex == 2)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto REG_INDEX_Z = decodeNibble(memory->readWord(registers.pc), 2);
        registers.r[REG_INDEX_Z] = registers.r[REG_INDEX_X] | registers.r[REG_INDEX_Y];
        registers.flags.n = isNegative(registers.r[REG_INDEX_Z]);
        registers.flags.z = isZero(registers.r[REG_INDEX_Z]);
    }
    registers.pc += 2;
    return true;
}

bool CpuImpl::executeBitwiseXorInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);
    if (innerInstructionIndex > 2)
        return false;

    if (innerInstructionIndex == 0)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto word = memory->readWord(registers.pc);
        registers.r[REG_INDEX] ^= word;
        registers.flags.n = isNegative(registers.r[REG_INDEX]);
        registers.flags.z = isZero(registers.r[REG_INDEX]);
    }
    else if (innerInstructionIndex == 1)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        registers.r[REG_INDEX_X] ^= registers.r[REG_INDEX_Y];
        registers.flags.n = isNegative(registers.r[REG_INDEX_X]);
        registers.flags.z = isZero(registers.r[REG_INDEX_X]);
    }
    else if (innerInstructionIndex == 2)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto REG_INDEX_Z = decodeNibble(memory->readWord(registers.pc), 2);
        registers.r[REG_INDEX_Z] = registers.r[REG_INDEX_X] ^ registers.r[REG_INDEX_Y];
        registers.flags.n = isNegative(registers.r[REG_INDEX_Z]);
        registers.flags.z = isZero(registers.r[REG_INDEX_Z]);
    }
    registers.pc += 2;
    return true;
}

bool CpuImpl::executeShiftInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);
    if (innerInstructionIndex > 5)
        return false;

    if (innerInstructionIndex == 0)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto operand = decodeNibble(memory->readWord(registers.pc), 2);
        registers.r[REG_INDEX] <<= operand;
        registers.flags.n = isNegative(registers.r[REG_INDEX]);
        registers.flags.z = isZero(registers.r[REG_INDEX]);
    }
    else if (innerInstructionIndex == 1)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto operand = decodeNibble(memory->readWord(registers.pc), 2);
        registers.r[REG_INDEX] >>= operand;
        registers.flags.n = isNegative(registers.r[REG_INDEX]);
        registers.flags.z = isZero(registers.r[REG_INDEX]);
    }
    else if (innerInstructionIndex == 2)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto operand = decodeNibble(memory->readWord(registers.pc), 2);
        bool msb = registers.r[REG_INDEX] & 0x8000;
        registers.r[REG_INDEX] = (registers.r[REG_INDEX] >> operand) | (msb << 15);
        registers.flags.n = isNegative(registers.r[REG_INDEX]);
        registers.flags.z = isZero(registers.r[REG_INDEX]);
    }
    else if (innerInstructionIndex == 3)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto operand = registers.r[REG_INDEX_Y];
        registers.r[REG_INDEX_X] <<= operand;
        registers.flags.n = isNegative(registers.r[REG_INDEX_X]);
        registers.flags.z = isZero(registers.r[REG_INDEX_X]);
    }
    else if (innerInstructionIndex == 4)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto operand = registers.r[REG_INDEX_Y];
        registers.r[REG_INDEX_X] >>= operand;
        registers.flags.n = isNegative(registers.r[REG_INDEX_X]);
        registers.flags.z = isZero(registers.r[REG_INDEX_X]);
    }
    else if (innerInstructionIndex == 5)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        bool msb = registers.r[REG_INDEX_X] & 0x8000;
        const auto operand = registers.r[REG_INDEX_Y];
        registers.r[REG_INDEX_X] = (registers.r[REG_INDEX_X] >> operand) | (msb << 15);
        registers.flags.n = isNegative(registers.r[REG_INDEX_X]);
        registers.flags.z = isZero(registers.r[REG_INDEX_X]);
    }
    registers.pc += 2;
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

bool CpuImpl::executePaletteInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);
    if (innerInstructionIndex > 1)
        return false;

    u16 addr = innerInstructionIndex == 0 ? 
        memory->readWord(registers.pc) : registers.r[decodeNibble(opcode, 0)];
    Palette palette;
    for (auto i = 0; i < 16; i++)
    {
        std::uint32_t color = 0xFF;
        for (auto j = 0; j < 3; j++)
            color |= memory->readByte(addr + (i * 3) + j) << ((3 - j) * 8);
        palette[i] = color;
    }
    bus->loadPalette(palette);
    registers.pc += 2;
    return true;
}

bool CpuImpl::executeNegationInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);
    if (innerInstructionIndex > 5)
        return false;

    if (innerInstructionIndex == 0)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        auto word = memory->readWord(registers.pc);
        registers.r[REG_INDEX] = ~(word & 0xFFFF);
        registers.flags.z = isZero(registers.r[REG_INDEX]);
        registers.flags.n = isNegative(registers.r[REG_INDEX]);
    }
    else if (innerInstructionIndex == 1)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        registers.r[REG_INDEX] = ~registers.r[REG_INDEX];
        registers.flags.z = isZero(registers.r[REG_INDEX]);
        registers.flags.n = isNegative(registers.r[REG_INDEX]);
    }
    else if (innerInstructionIndex == 2)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        registers.r[REG_INDEX_X] = ~registers.r[REG_INDEX_Y];
        registers.flags.z = isZero(registers.r[REG_INDEX_X]);
        registers.flags.n = isNegative(registers.r[REG_INDEX_X]);
    }
    else if (innerInstructionIndex == 3)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        auto word = memory->readWord(registers.pc);
        registers.r[REG_INDEX] = negate(word);
        registers.flags.z = isZero(registers.r[REG_INDEX]);
        registers.flags.n = isNegative(registers.r[REG_INDEX]);
    }
    else if (innerInstructionIndex == 4)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        auto word = registers.r[REG_INDEX];
        registers.r[REG_INDEX] = negate(word);
        registers.flags.z = isZero(registers.r[REG_INDEX]);
        registers.flags.n = isNegative(registers.r[REG_INDEX]);
    }
    else if (innerInstructionIndex == 5)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        auto word = registers.r[REG_INDEX_Y];
        registers.r[REG_INDEX_X] = negate(word);
        registers.flags.z = isZero(registers.r[REG_INDEX_X]);
        registers.flags.n = isNegative(registers.r[REG_INDEX_X]);
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

bool CpuImpl::isZero(unsigned data)
{
    return (data & 0xFFFF) == 0;
}

bool CpuImpl::isNegative(unsigned data)
{
    return (data >> 15) == 1;
}

u16 CpuImpl::negate(u16 word)
{
    return word == 0 ? word : word ^ (1 << 15);
}
