#pragma once

#include "ir.hpp"

struct CFGNode
{
    std::vector<BlockId> successors;
    std::vector<BlockId> predecessors;
};

struct ControlFlowGraph
{
    std::vector<CFGNode> nodes;
};

class CFGBuilder
{
public:
    ControlFlowGraph build(const IRFunction& function) const;

private:
    void addEdge(ControlFlowGraph& cfg,
                 BlockId from,
                 BlockId to) const;
};