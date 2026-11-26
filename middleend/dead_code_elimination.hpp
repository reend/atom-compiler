#pragma once

#include "ir.hpp"
#include <unordered_map>
#include "cfg.hpp"
#include <unordered_set>

class DeadCodeElimination
{
public:
    bool run(IRModule& module);

private:
    bool runFunction(IRFunction& function);
    bool hasSideEffects(const IRInstruction& instruction) const;

    std::unordered_map<VReg, std::size_t> useCounts_;
    void countUses(const IRFunction& function);

    bool removeDeadInstructions(IRFunction& function);

    std::unordered_set<BlockId> findReachableBlocks(
    const IRFunction& function) const;

    bool removeUnreachableBlocks(IRFunction& function);
};