#include "InstructionExecutionFacadeImpl.hpp"

InstructionExecutionFacadeImpl::InstructionExecutionFacadeImpl(const std::shared_ptr<Cpu> &cpu)
    : cpu(cpu)
{
}

void InstructionExecutionFacadeImpl::executeInstruction()
{
    const auto opcode = cpu->fetchOpcode();
    cpu->executeInstruction(opcode);
}