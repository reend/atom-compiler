#pragma once

#include "ir.hpp"

#include <unordered_map>

class ConstantFolding
{
public:
    void run(IRModule& module);

private:
    std::unordered_map<VReg, std::int64_t> constants_;

    void runFunction(IRFunction& function);
    void runBlock(IRBasicBlock& block);
    void foldInstruction(IRInstruction& instruction);

    std::optional<std::int64_t> getConstant(
        const IROperand& operand) const;

    void replaceWithConstant(IRInstruction& instruction,
                             std::int64_t value);
};