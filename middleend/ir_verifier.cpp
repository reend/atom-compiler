#include "ir_verifier.hpp"

#include <stdexcept>

bool IRVerifier::isTerminator(IROpcode opcode) const
{
    return opcode == IROpcode::Jump ||
           opcode == IROpcode::Branch ||
           opcode == IROpcode::Ret;
}

void IRVerifier::verify(const IRModule& module) const
{
    for (const auto& function : module.functions)
    {
        verifyFunction(function);
    }
}

void IRVerifier::verifyFunction(const IRFunction& function) const
{
    if (function.blocks.empty())
    {
        throw std::runtime_error("Function has no basic blocks");
    }

    for (std::size_t i = 0; i < function.blocks.size(); ++i)
    {
        const auto& block = function.blocks[i];

        if (block.id != i)
        {
            throw std::runtime_error("Invalid basic block ID");
        }

        verifyBlock(function, block);
    }
}

void IRVerifier::verifyBlock(const IRFunction& function,
                             const IRBasicBlock& block) const
{
    if (block.instructions.empty())
    {
        throw std::runtime_error("Empty basic block");
    }

    if (!isTerminator(block.instructions.back().opcode))
    {
        throw std::runtime_error("Basic block has no terminator");
    }

    for (std::size_t i = 0; i < block.instructions.size(); ++i)
    {
        const auto& instruction = block.instructions[i];

        if (isTerminator(instruction.opcode) &&
            i != block.instructions.size() - 1)
        {
            throw std::runtime_error("Instruction after terminator");
        }

        for (const auto& operand : instruction.operands)
        {
            if (operand.kind == IROperandKind::Block)
            {
                if (operand.value < 0 ||
                    static_cast<BlockId>(operand.value) >= function.blocks.size())
                {
                    throw std::runtime_error("Invalid jump target");
                }
            }
        }
    }
}