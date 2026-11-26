#include "ir_printer.hpp"

#include <iostream>

void IRPrinter::printOperand(const IROperand& operand)
{
    switch (operand.kind)
    {
        case IROperandKind::Register:
            std::cout << "v" << operand.value;
            break;

        case IROperandKind::Immediate:
            std::cout << operand.value;
            break;

        case IROperandKind::Slot:
            std::cout << "slot" << operand.value;
            break;

        case IROperandKind::Block:
            std::cout << "block" << operand.value;
            break;
    }
}

void IRPrinter::printInstruction(const IRInstruction& instruction)
{
    std::cout << "    ";

    if (instruction.destination.has_value())
    {
        std::cout << "v" << *instruction.destination << " = ";
    }

    std::cout << irOpcodeToString(instruction.opcode);

    if (instruction.opcode == IROpcode::Call)
    {
        std::cout << " " << instruction.callee;
    }

    for (std::size_t i = 0; i < instruction.operands.size(); ++i)
    {
        if (i == 0 && instruction.opcode != IROpcode::Call)
        {
            std::cout << " ";
        }
        else
        {
            std::cout << ", ";
        }

        printOperand(instruction.operands[i]);
    }

    std::cout << "\n";
}

void IRPrinter::printBlock(const IRBasicBlock& block)
{
    std::cout << "block" << block.id << ":\n";

    for (const auto& instruction : block.instructions)
    {
        printInstruction(instruction);
    }
}

void IRPrinter::printFunction(const IRFunction& function)
{
    std::cout << "function " << function.name << "(";

    for (std::size_t i = 0; i < function.parameters.size(); ++i)
    {
        if (i != 0)
        {
            std::cout << ", ";
        }

        std::cout << "slot" << function.parameters[i];
    }

    std::cout << "):\n";

    for (const auto& block : function.blocks)
    {
        printBlock(block);
    }

    std::cout << "\n";
}

void IRPrinter::print(const IRModule& module)
{
    for (const auto& function : module.functions)
    {
        printFunction(function);
    }
}