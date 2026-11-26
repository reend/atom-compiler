#include "cfg_printer.hpp"

#include <iostream>

void CFGPrinter::printBlocks(const std::vector<BlockId>& blocks) const
{
    std::cout << "[";

    for (std::size_t i = 0; i < blocks.size(); ++i)
    {
        if (i != 0)
        {
            std::cout << ", ";
        }

        std::cout << blocks[i];
    }

    std::cout << "]";
}

void CFGPrinter::print(const IRFunction& function,
                       const ControlFlowGraph& cfg) const
{
    std::cout << "CFG: " << function.name << "\n";

    for (std::size_t i = 0; i < cfg.nodes.size(); ++i)
    {
        const auto& node = cfg.nodes[i];

        std::cout << "block" << i << ":\n";

        std::cout << "    successors:   ";
        printBlocks(node.successors);
        std::cout << "\n";

        std::cout << "    predecessors: ";
        printBlocks(node.predecessors);
        std::cout << "\n";
    }

    std::cout << "\n";
}