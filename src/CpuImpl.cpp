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
    registers.sp += 2;
    return memory->readWord(registers.sp);
}

void CpuImpl::pushIntoStack(u16 value)
{
    memory->writeWord(registers.sp, value);
    registers.sp -= 2;
}

void CpuImpl::executeInstruction(u16 opcode)
{
    // Implement opcode executing
}

CpuImpl::Registers& CpuImpl::getRegisters()
{
    return registers;
}
