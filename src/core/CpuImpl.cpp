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

    if (validateInstructionIndex(opcode))
    {
        switch (group)
        {
        case 0x0:
            result = executeMiscInstruction(opcode);
            break;
        case 0x1:
            result = executeJumpInstruction(opcode);
            break;
        case 0x2:
            result = executeLoadInstruction(opcode);
            break;
        case 0x3:
            result = executeStoreInstruction(opcode);
            break;
        case 0x4:
            result = executeAdditionInstruction(opcode);
            break;
        case 0x5:
            result = executeSubtractionInstruction(opcode);
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
        case 0x9:
            result = executeMultiplicationInstruction(opcode);
            break;
        case 0xA:
            result = executeDivisionInstruction(opcode);
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
    }

    if (!result) 
        LOG.error("Unknown opcode: ", logHex(opcode));
}

CpuRegisters& CpuImpl::getRegisters()
{
    return registers;
}

bool CpuImpl::validateInstructionIndex(u16 opcode)
{
    static const unsigned MAX_INDEXES[] = {
        0xE, 0x8, 0x4, 0x1,
        0x2, 0x4, 0x4, 0x2,
        0x2, 0x2, 0x8, 0x5, 
        0x5, 0x1, 0x5
    };
    return decodeNibble(opcode, 2) <= MAX_INDEXES[decodeNibble(opcode, 3)];
}

bool CpuImpl::executeMiscInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);
    
    if (innerInstructionIndex == 0)
    {
        // NOP
    }
    else if (innerInstructionIndex == 1)
    {
        bus->clearScreen();
    }
    else if (innerInstructionIndex == 2)
    {
        if (!bus->isVBlank())
        {
            registers.pc -= 4;
        }
        else
        {
            bus->setVBlank(false);
        }
        
    }
    else if (innerInstructionIndex == 3)
    {
        const auto COLOR_INDEX = decodeNibble(memory->readWord(registers.pc), 2);
        bus->setBackgroundColorIndex(COLOR_INDEX);
    }
    else if (innerInstructionIndex == 4)
    {
        const auto word = memory->readWord(registers.pc);
        const auto WIDTH = (word >> 8) & 0xFF;
        const auto HEIGHT = word & 0xFF;
        bus->setSpriteDimensions(WIDTH, HEIGHT);
    }
    else if (innerInstructionIndex == 5)
    {
        const auto POS_X = registers.r[decodeNibble(opcode, 0)];
        const auto POS_Y = registers.r[decodeNibble(opcode, 1)];
        const auto addr = memory->readWord(registers.pc);
        registers.flags.c = bus->drawSprite(POS_X, POS_Y, memory->readByteReference(addr));
    }
    else if (innerInstructionIndex == 6)
    {
        const auto POS_X = registers.r[decodeNibble(opcode, 0)];
        const auto POS_Y = registers.r[decodeNibble(opcode, 1)];
        const auto REG_INDEX_Z = decodeNibble(memory->readWord(registers.pc), 2);
        const auto addr = registers.r[REG_INDEX_Z];
        registers.flags.c = bus->drawSprite(POS_X, POS_Y, memory->readByteReference(addr));
    }
    else if (innerInstructionIndex == 7)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto max = memory->readWord(registers.pc);
        registers.r[REG_INDEX] = Random::get(u16(0), max);
    }
    else if (innerInstructionIndex == 8)
    {
        const auto flipFlags = memory->readWord(registers.pc) & 0x3;
        bus->setHFlip(flipFlags & 0x2);
        bus->setVFlip(flipFlags & 0x1);
    }
    else if (innerInstructionIndex == 9)
    {
        // TODO: Implement instruction
    }
    else if (innerInstructionIndex == 9)
    {
        // TODO: Implement instruction
    }
    else if (innerInstructionIndex == 0xA)
    {
        // TODO: Implement instruction
    }
    else if (innerInstructionIndex == 0xB)
    {
        // TODO: Implement instruction
    }
    else if (innerInstructionIndex == 0xC)
    {
        // TODO: Implement instruction
    }
    else if (innerInstructionIndex == 0xD)
    {
        // TODO: Implement instruction
    }
    else if (innerInstructionIndex == 0xE)
    {
        // TODO: Implement instruction
    }
    registers.pc += 2;
    return true;
}

bool CpuImpl::executeJumpInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);

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

bool CpuImpl::executeAdditionInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);

    if (innerInstructionIndex == 0)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const unsigned operand1 = memory->readWord(registers.pc);
        const unsigned operand2 = registers.r[REG_INDEX];
        const unsigned result = operand1 + operand2;
        registers.flags.c = isAdditionCarry(result);
        registers.flags.n = isNegative(result);
        registers.flags.z = isZero(result);
        registers.flags.o = isAdditionOverflow(operand1, operand2, result);
        registers.r[REG_INDEX] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 1)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const unsigned operand1 = registers.r[REG_INDEX_X];
        const unsigned operand2 = registers.r[REG_INDEX_Y];
        const unsigned result = operand1 + operand2;
        registers.flags.c = isAdditionCarry(result);
        registers.flags.n = isNegative(result);
        registers.flags.z = isZero(result);
        registers.flags.o = isAdditionOverflow(operand1, operand2, result);
        registers.r[REG_INDEX_X] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 2)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto REG_INDEX_Z = decodeNibble(memory->readWord(registers.pc), 2);
        const unsigned operand1 = registers.r[REG_INDEX_X];
        const unsigned operand2 = registers.r[REG_INDEX_Y];
        const unsigned result = operand1 + operand2;
        registers.flags.c = isAdditionCarry(result);
        registers.flags.n = isNegative(result);
        registers.flags.z = isZero(result);
        registers.flags.o = isAdditionOverflow(operand1, operand2, result);
        registers.r[REG_INDEX_Z] = result & 0xFFFF;
    }
    registers.pc += 2;
    return true;
}

bool CpuImpl::executeSubtractionInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);

    if (innerInstructionIndex == 0)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const unsigned operand1 = registers.r[REG_INDEX];
        const unsigned operand2 = memory->readWord(registers.pc);
        const unsigned result = operand1 + negate(operand2);
        registers.flags.c = isSubtractionBorrow(result);
        registers.flags.n = isNegative(result);
        registers.flags.z = isZero(result);
        registers.flags.o = isSubtractionOverflow(operand1, operand2, result);
        registers.r[REG_INDEX] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 1)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const unsigned operand1 = registers.r[REG_INDEX_X];
        const unsigned operand2 = registers.r[REG_INDEX_Y];
        const unsigned result = operand1 + negate(operand2);
        registers.flags.c = isSubtractionBorrow(result);
        registers.flags.n = isNegative(result);
        registers.flags.z = isZero(result);
        registers.flags.o = isSubtractionOverflow(operand1, operand2, result);
        registers.r[REG_INDEX_X] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 2)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto REG_INDEX_Z = decodeNibble(memory->readWord(registers.pc), 2);
        const unsigned operand1 = registers.r[REG_INDEX_X];
        const unsigned operand2 = registers.r[REG_INDEX_Y];
        const unsigned result = operand1 + negate(operand2);
        registers.flags.c = isSubtractionBorrow(result);
        registers.flags.n = isNegative(result);
        registers.flags.z = isZero(result);
        registers.flags.o = isSubtractionOverflow(operand1, operand2, result);
        registers.r[REG_INDEX_Z] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 3)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const unsigned operand1 = registers.r[REG_INDEX];
        const unsigned operand2 = memory->readWord(registers.pc);
        const unsigned result = operand1 + negate(operand2);
        registers.flags.c = isSubtractionBorrow(result);
        registers.flags.n = isNegative(result);
        registers.flags.z = isZero(result);
        registers.flags.o = isSubtractionOverflow(operand1, operand2, result);
    }
    else if (innerInstructionIndex == 4)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const unsigned operand1 = registers.r[REG_INDEX_X];
        const unsigned operand2 = registers.r[REG_INDEX_Y];
        const unsigned result = operand1 + negate(operand2);
        registers.flags.c = isSubtractionBorrow(result);
        registers.flags.n = isNegative(result);
        registers.flags.z = isZero(result);
        registers.flags.o = isSubtractionOverflow(operand1, operand2, result);
    }
    registers.pc += 2;
    return true;
}

bool CpuImpl::executeBitwiseAndInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);

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

bool CpuImpl::executeMultiplicationInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);

    if (innerInstructionIndex == 0)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto operand1 = registers.r[REG_INDEX];
        const auto operand2 = memory->readWord(registers.pc);
        const unsigned result = operand1 * operand2;
        registers.flags.c = isMultiplicationCarry(result);
        registers.flags.n = isNegative(result);
        registers.flags.z = isZero(result);
        registers.r[REG_INDEX] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 1)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto operand1 = registers.r[REG_INDEX_X];
        const auto operand2 = registers.r[REG_INDEX_Y];
        const unsigned result = operand1 * operand2;
        registers.flags.c = isMultiplicationCarry(result);
        registers.flags.n = isNegative(result);
        registers.flags.z = isZero(result);
        registers.r[REG_INDEX_X] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 2)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto REG_INDEX_Z = decodeNibble(memory->readWord(registers.pc), 2);
        const auto operand1 = registers.r[REG_INDEX_X];
        const auto operand2 = registers.r[REG_INDEX_Y];
        const unsigned result = operand1 * operand2;
        registers.flags.c = isMultiplicationCarry(result);
        registers.flags.n = isNegative(result);
        registers.flags.z = isZero(result);
        registers.r[REG_INDEX_Z] = result & 0xFFFF;
    }
    registers.pc += 2;
    return true;
}

bool CpuImpl::executeDivisionInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);

    auto rem = [](const auto operand1, const auto operand2) {
        return operand1 % operand2;
    };
    auto mod = [](const auto operand1, const auto operand2) {
        return ((operand1 % operand2) + operand2) % operand2;
    };

    if (innerInstructionIndex == 0)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto operand1 = static_cast<s16>(registers.r[REG_INDEX]);
        const auto operand2 = static_cast<s16>(memory->readWord(registers.pc));
        const unsigned result = static_cast<u32>(operand1 / operand2);
        registers.flags.c = isDivisionCarry(operand1, operand2);
        registers.flags.z = isZero(result);
        registers.flags.n = isNegative(result);
        registers.r[REG_INDEX] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 1)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto operand1 = static_cast<s16>(registers.r[REG_INDEX_X]);
        const auto operand2 = static_cast<s16>(registers.r[REG_INDEX_Y]);
        const unsigned result = static_cast<u32>(operand1 / operand2);
        registers.flags.c = isDivisionCarry(operand1, operand2);
        registers.flags.z = isZero(result);
        registers.flags.n = isNegative(result);
        registers.r[REG_INDEX_X] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 2)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto REG_INDEX_Z = decodeNibble(memory->readWord(registers.pc), 2);
        const auto operand1 = static_cast<s16>(registers.r[REG_INDEX_X]);
        const auto operand2 = static_cast<s16>(registers.r[REG_INDEX_Y]);
        const unsigned result = static_cast<u32>(operand1 / operand2);
        registers.flags.c = isDivisionCarry(operand1, operand2);
        registers.flags.z = isZero(result);
        registers.flags.n = isNegative(result);
        registers.r[REG_INDEX_Z] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 3)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto operand1 = static_cast<s16>(registers.r[REG_INDEX]);
        const auto operand2 = static_cast<s16>(memory->readWord(registers.pc));
        const unsigned result = static_cast<u32>(mod(operand1, operand2));
        registers.flags.z = isZero(result);
        registers.flags.n = isNegative(result);
        registers.r[REG_INDEX] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 4)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto operand1 = static_cast<s16>(registers.r[REG_INDEX_X]);
        const auto operand2 = static_cast<s16>(registers.r[REG_INDEX_Y]);
        const unsigned result = static_cast<u32>(mod(operand1, operand2));
        registers.flags.z = isZero(result);
        registers.flags.n = isNegative(result);
        registers.r[REG_INDEX_X] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 5)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto REG_INDEX_Z = decodeNibble(memory->readWord(registers.pc), 2);
        const auto operand1 = static_cast<s16>(registers.r[REG_INDEX_X]);
        const auto operand2 = static_cast<s16>(registers.r[REG_INDEX_Y]);
        const unsigned result = static_cast<u32>(mod(operand1, operand2));
        registers.flags.z = isZero(result);
        registers.flags.n = isNegative(result);
        registers.r[REG_INDEX_Z] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 6)
    {
        const auto REG_INDEX = decodeNibble(opcode, 0);
        const auto operand1 = static_cast<s16>(registers.r[REG_INDEX]);
        const auto operand2 = static_cast<s16>(memory->readWord(registers.pc));
        const unsigned result = static_cast<u32>(rem(operand1, operand2));
        registers.flags.z = isZero(result);
        registers.flags.n = isNegative(result);
        registers.r[REG_INDEX] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 7)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto operand1 = static_cast<s16>(registers.r[REG_INDEX_X]);
        const auto operand2 = static_cast<s16>(registers.r[REG_INDEX_Y]);
        const unsigned result = static_cast<u32>(rem(operand1, operand2));
        registers.flags.z = isZero(result);
        registers.flags.n = isNegative(result);
        registers.r[REG_INDEX_X] = result & 0xFFFF;
    }
    else if (innerInstructionIndex == 8)
    {
        const auto REG_INDEX_X = decodeNibble(opcode, 0);
        const auto REG_INDEX_Y = decodeNibble(opcode, 1);
        const auto REG_INDEX_Z = decodeNibble(memory->readWord(registers.pc), 2);
        const auto operand1 = static_cast<s16>(registers.r[REG_INDEX_X]);
        const auto operand2 = static_cast<s16>(registers.r[REG_INDEX_Y]);
        const unsigned result = static_cast<u32>(rem(operand1, operand2));
        registers.flags.z = isZero(result);
        registers.flags.n = isNegative(result);
        registers.r[REG_INDEX_Z] = result & 0xFFFF;
    }
    registers.pc += 2;
    return true;
}

bool CpuImpl::executeShiftInstruction(u16 opcode)
{
    const auto innerInstructionIndex = decodeNibble(opcode, 2);

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

bool CpuImpl::isZero(unsigned data) const
{
    return (data & 0xFFFF) == 0;
}

bool CpuImpl::isNegative(unsigned data) const
{
    return (data >> 15) & 1;
}

bool CpuImpl::isAdditionCarry(unsigned data) const
{
    return (data >> 16) & 1;
}

bool CpuImpl::isAdditionOverflow(unsigned operand1, unsigned operand2, unsigned result) const
{
    return (isNegative(operand1) && isNegative(operand2) && !isNegative(result))
        || (!isNegative(operand1) && !isNegative(operand2) && isNegative(result));
}

bool CpuImpl::isSubtractionBorrow(unsigned result) const
{
    return !((result >> 16) & 1);
}

bool CpuImpl::isSubtractionOverflow(unsigned operand1, unsigned operand2, unsigned result) const
{
    return (!isNegative(result) && isNegative(operand1) && !isNegative(operand2))
        || (isNegative(result) && !isNegative(operand1) && isNegative(operand2));
}

bool CpuImpl::isMultiplicationCarry(unsigned result) const
{
    return result > UINT16_MAX;
}

bool CpuImpl::isDivisionCarry(unsigned operand1, unsigned operand2) const
{
    return !!(operand1 % operand2);
}

u16 CpuImpl::negate(u16 word)
{
    return static_cast<u16>(-static_cast<s16>(word));
}
