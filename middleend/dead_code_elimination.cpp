#include "dead_code_elimination.hpp"
#include <algorithm>
#include <utility>

bool DeadCodeElimination::run(IRModule& module)
{
    bool changed = false;

    for (auto& function : module.functions)
    {
        if (runFunction(function))
        {
            changed = true;
        }
    }

    return changed;
}

bool DeadCodeElimination::hasSideEffects(
    const IRInstruction& instruction) const
{
    switch (instruction.opcode)
    {
        case IROpcode::Store:
        case IROpcode::Call:
        case IROpcode::Jump:
        case IROpcode::Branch:
        case IROpcode::Ret:
            return true;

        default:
            return false;
    }
}

void DeadCodeElimination::countUses(const IRFunction& function)
{
    useCounts_.clear();

    for (const auto& block : function.blocks)
    {
        for (const auto& instruction : block.instructions)
        {
            for (const auto& operand : instruction.operands)
            {
                if (operand.kind == IROperandKind::Register)
                {
                    VReg reg = static_cast<VReg>(operand.value);
                    ++useCounts_[reg];
                }
            }
        }
    }
}

bool DeadCodeElimination::removeDeadInstructions(IRFunction& function)
{
    bool changed = false;

    for (auto& block : function.blocks)
    {
        auto& instructions = block.instructions;
        auto oldSize = instructions.size();

        instructions.erase(
            std::remove_if(
                instructions.begin(),
                instructions.end(),
                [this](const IRInstruction& instruction)
                {
                    if (hasSideEffects(instruction))
                    {
                        return false;
                    }

                    if (!instruction.destination.has_value())
                    {
                        return false;
                    }

                    VReg dest = *instruction.destination;

                    return useCounts_.find(dest) == useCounts_.end();
                }),
            instructions.end());

        if (instructions.size() != oldSize)
        {
            changed = true;
        }
    }

    return changed;
}

bool DeadCodeElimination::runFunction(IRFunction& function)
{
    bool changed = removeUnreachableBlocks(function);

    while (true)
    {
        countUses(function);

        if (!removeDeadInstructions(function))
        {
            break;
        }

        changed = true;
    }

    return changed;
}

std::unordered_set<BlockId> DeadCodeElimination::findReachableBlocks(
    const IRFunction& function) const
{
    CFGBuilder builder;
    ControlFlowGraph cfg = builder.build(function);

    std::unordered_set<BlockId> reachable;
    std::vector<BlockId> worklist = {0};

    while (!worklist.empty())
    {
        BlockId id = worklist.back();
        worklist.pop_back();

        if (reachable.find(id) != reachable.end())
        {
            continue;
        }

        reachable.insert(id);

        for (BlockId successor : cfg.nodes[id].successors)
        {
            worklist.push_back(successor);
        }
    }

    return reachable;
}

bool DeadCodeElimination::removeUnreachableBlocks(IRFunction& function)
{
    auto reachable = findReachableBlocks(function);

    if (reachable.size() == function.blocks.size())
    {
        return false;
    }

    std::unordered_map<BlockId, BlockId> newIds;
    std::vector<IRBasicBlock> newBlocks;

    for (auto& block : function.blocks)
    {
        if (reachable.find(block.id) == reachable.end())
        {
            continue;
        }

        BlockId newId = newBlocks.size();
        newIds[block.id] = newId;

        block.id = newId;
        newBlocks.push_back(std::move(block));
    }

    for (auto& block : newBlocks)
    {
        for (auto& instruction : block.instructions)
        {
            for (auto& operand : instruction.operands)
            {
                if (operand.kind == IROperandKind::Block)
                {
                    BlockId oldId = static_cast<BlockId>(operand.value);
                    operand.value = static_cast<std::int64_t>(
                        newIds.at(oldId));
                }
            }
        }
    }

    function.blocks = std::move(newBlocks);

    return true;
}