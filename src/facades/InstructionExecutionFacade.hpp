#pragma once

class InstructionExecutionFacade
{
public:
    ~InstructionExecutionFacade() = default;

    virtual void executeInstruction() = 0;
};