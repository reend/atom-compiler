#pragma once

#include "ir.hpp"
#include "cfg.hpp"

#include <unordered_map>

struct ConstantState
{
    std::unordered_map<SlotId, std::int64_t> slots;
};

class ConstantPropagation
{
public:
    bool run(IRModule& module);

private:
    std::unordered_map<VReg, std::int64_t> registerConstants_;
    std::unordered_map<SlotId, std::int64_t> slotConstants_;

    void runFunction(IRFunction& function);
    void runBlock(IRBasicBlock& block, bool rewrite);
    void processInstruction(IRInstruction& instruction, bool rewrite);

    std::optional<std::int64_t> getConstant(
        const IROperand& operand) const;

    void replaceWithConstant(
        IRInstruction& instruction,
        std::int64_t value);

    std::vector<ConstantState> inStates_;
    std::vector<ConstantState> outStates_;     
    
    ConstantState mergeStates(
    const ControlFlowGraph& cfg,
    BlockId blockId) const;

    bool changed_ = false;
};