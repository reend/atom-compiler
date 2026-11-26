#pragma once

#include "cfg.hpp"

class CFGPrinter
{
public:
    void print(const IRFunction& function,
               const ControlFlowGraph& cfg) const;

private:
    void printBlocks(const std::vector<BlockId>& blocks) const;
};