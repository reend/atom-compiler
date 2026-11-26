#include "constant_folding.hpp"
#include <limits>

void ConstantFolding::run(IRModule& module)
{
    for (auto& function : module.functions)
    {
        runFunction(function);
    }
}

void ConstantFolding::runFunction(IRFunction& function)
{
    for (auto& block : function.blocks)
    {
        runBlock(block);
    }
}

void ConstantFolding::runBlock(IRBasicBlock& block)
{
    constants_.clear();

    for (auto& instruction : block.instructions)
    {
        foldInstruction(instruction);
    }
}

std::optional<std::int64_t> ConstantFolding::getConstant(
    const IROperand& operand) const
{
    if (operand.kind == IROperandKind::Immediate)
    {
        return operand.value;
    }

    if (operand.kind == IROperandKind::Register)
    {
        VReg reg = static_cast<VReg>(operand.value);

        auto it = constants_.find(reg);

        if (it != constants_.end())
        {
            return it->second;
        }
    }

    return std::nullopt;
}

void ConstantFolding::replaceWithConstant(
    IRInstruction& instruction,
    std::int64_t value)
{
    instruction.opcode = IROpcode::Const;

    instruction.operands = {
        {IROperandKind::Immediate, value}
    };

    instruction.callee = {};

    constants_[*instruction.destination] = value;
}

void ConstantFolding::foldInstruction(IRInstruction& instruction)
{
    if (!instruction.destination.has_value())
    {
        return;
    }

    VReg dest = *instruction.destination;

    if (instruction.opcode == IROpcode::Const)
    {
        constants_[dest] = instruction.operands[0].value;

        return;
    }

    if (instruction.opcode == IROpcode::Neg)
    {
        auto value = getConstant(instruction.operands[0]);

        if (value.has_value() &&
            *value != std::numeric_limits<std::int64_t>::min())
        {
            replaceWithConstant(instruction, -*value);
        }

        return;
    }

    if (instruction.operands.size() != 2)
    {
        return;
    }

    auto left = getConstant(instruction.operands[0]);
    auto right = getConstant(instruction.operands[1]);

    if (!left.has_value() || !right.has_value())
    {
        return;
    }

    std::int64_t result;

    switch (instruction.opcode)
    {
        case IROpcode::Add:
        {
            if (__builtin_add_overflow(*left, *right, &result))
            {
                return;
            }

            break;
        }

        case IROpcode::Sub:
        {
            if (__builtin_sub_overflow(*left, *right, &result))
            {
                return;
            }

            break;
        }

        case IROpcode::Mul:
        {
            if (__builtin_mul_overflow(*left, *right, &result))
            {
                return;
            }

            break;
        }

        case IROpcode::Eq:
            result = *left == *right;
            break;

        case IROpcode::Ne:
            result = *left != *right;
            break;

        case IROpcode::Lt:
            result = *left < *right;
            break;

        case IROpcode::Gt:
            result = *left > *right;
            break;

        case IROpcode::Le:
            result = *left <= *right;
            break;

        case IROpcode::Ge:
            result = *left >= *right;
            break;

        default:
            return;
    }

    replaceWithConstant(instruction, result);
}