#pragma once

#include <memory>

#include "InstructionExecutionFacade.hpp"
#include "../core/Cpu.hpp"

class InstructionExecutionFacadeImpl
    : public InstructionExecutionFacade
{
public:
    InstructionExecutionFacadeImpl(const std::shared_ptr<Cpu> &cpu);

    ~InstructionExecutionFacadeImpl() = default;

    void executeInstruction() override;

private:
    std::shared_ptr<Cpu> cpu;
};