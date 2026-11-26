#include "cfg.hpp"

void CFGBuilder::addEdge(ControlFlowGraph& cfg,
                         BlockId from,
                         BlockId to) const
{
    cfg.nodes[from].successors.push_back(to);
    cfg.nodes[to].predecessors.push_back(from);
}

ControlFlowGraph CFGBuilder::build(const IRFunction& function) const
{
    ControlFlowGraph cfg;

    cfg.nodes.resize(function.blocks.size());

    for (const auto& block : function.blocks)
    {
        const auto& terminator = block.instructions.back();

        switch (terminator.opcode)
        {
            case IROpcode::Jump:
            {
                BlockId target =
                    static_cast<BlockId>(terminator.operands[0].value);

                addEdge(cfg, block.id, target);

                break;
            }

            case IROpcode::Branch:
            {
                BlockId trueBlock =
                    static_cast<BlockId>(terminator.operands[1].value);

                BlockId falseBlock =
                    static_cast<BlockId>(terminator.operands[2].value);

                addEdge(cfg, block.id, trueBlock);
                addEdge(cfg, block.id, falseBlock);

                break;
            }

            case IROpcode::Ret:
                break;

            default:
                break;
        }
    }

    return cfg;
}