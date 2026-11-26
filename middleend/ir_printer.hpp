#pragma once

#include "ir.hpp"

class IRPrinter
{
public:
    void print(const IRModule& module);

private:
    void printFunction(const IRFunction& function);
    void printBlock(const IRBasicBlock& block);
    void printInstruction(const IRInstruction& instruction);
    void printOperand(const IROperand& operand);
};