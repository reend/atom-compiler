#include "constant_propagation.hpp"

bool ConstantPropagation::run(IRModule& module)
{
    changed_ = false;

    for (auto& function : module.functions)
    {
        runFunction(function);
    }

    return changed_;
}

void ConstantPropagation::runFunction(IRFunction& function)
{
    CFGBuilder builder;
    ControlFlowGraph cfg = builder.build(function);

    inStates_.clear();
    outStates_.clear();

    inStates_.resize(function.blocks.size());
    outStates_.resize(function.blocks.size());

    bool changed = true;

    while (changed)
    {
        changed = false;

        for (auto& block : function.blocks)
        {
            ConstantState newIn = mergeStates(cfg, block.id);

            if (newIn.slots != inStates_[block.id].slots)
            {
                inStates_[block.id] = newIn;
                changed = true;
            }

            ConstantState oldOut = outStates_[block.id];

            runBlock(block, false);

            if (oldOut.slots != outStates_[block.id].slots)
            {
                changed = true;
            }
        }
    }

    for (auto& block : function.blocks)
    {
        runBlock(block, true);
    }
}

void ConstantPropagation::runBlock(
    IRBasicBlock& block,
    bool rewrite)
{
    registerConstants_.clear();

    slotConstants_ = inStates_[block.id].slots;

    for (auto& instruction : block.instructions)
    {
        processInstruction(instruction, rewrite);
    }

    outStates_[block.id].slots = slotConstants_;
}

std::optional<std::int64_t> ConstantPropagation::getConstant(
    const IROperand& operand) const
{
    if (operand.kind == IROperandKind::Immediate)
    {
        return operand.value;
    }

    if (operand.kind == IROperandKind::Register)
    {
        VReg reg = static_cast<VReg>(operand.value);

        auto it = registerConstants_.find(reg);

        if (it != registerConstants_.end())
        {
            return it->second;
        }
    }

    if (operand.kind == IROperandKind::Slot)
    {
        SlotId slot = static_cast<SlotId>(operand.value);

        auto it = slotConstants_.find(slot);

        if (it != slotConstants_.end())
        {
            return it->second;
        }
    }

    return std::nullopt;
}

void ConstantPropagation::replaceWithConstant(
    IRInstruction& instruction,
    std::int64_t value)
{
    instruction.opcode = IROpcode::Const;

    instruction.operands = {
        {IROperandKind::Immediate, value}
    };

    instruction.callee = {};

    registerConstants_[*instruction.destination] = value;

    changed_ = true;
}

void ConstantPropagation::processInstruction(IRInstruction& instruction, bool rewrite)
{
    if (instruction.opcode == IROpcode::Const)
    {
        VReg dest = *instruction.destination;

        registerConstants_[dest] = instruction.operands[0].value;

        return;
    }

    if (instruction.opcode == IROpcode::Store)
    {
        SlotId slot = static_cast<SlotId>(
            instruction.operands[0].value);

        auto value = getConstant(instruction.operands[1]);

        if (value.has_value())
        {
            slotConstants_[slot] = *value;
        }
        else
        {
            slotConstants_.erase(slot);
        }

        return;
    }

    if (instruction.opcode == IROpcode::Load)
    {
        auto value = getConstant(instruction.operands[0]);

        if (value.has_value())
        {
            registerConstants_[*instruction.destination] = *value;

            if (rewrite)
            {
                replaceWithConstant(instruction, *value);
            }
        }

        return;
    }
}

ConstantState ConstantPropagation::mergeStates(
    const ControlFlowGraph& cfg,
    BlockId blockId) const
{
    const auto& predecessors = cfg.nodes[blockId].predecessors;

    if (predecessors.empty())
    {
        return {};
    }

    ConstantState result = outStates_[predecessors[0]];

    for (std::size_t i = 1; i < predecessors.size(); ++i)
    {
        const auto& state = outStates_[predecessors[i]];

        for (auto it = result.slots.begin();
             it != result.slots.end();)
        {
            auto other = state.slots.find(it->first);

            if (other == state.slots.end() ||
                other->second != it->second)
            {
                it = result.slots.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    return result;
}