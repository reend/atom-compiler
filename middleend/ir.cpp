#include "ir.hpp"

std::string_view irOpcodeToString(IROpcode opcode)
{
    switch (opcode)
    {
        case IROpcode::Const:  return "const";
        case IROpcode::Load:   return "load";
        case IROpcode::Store:  return "store";

        case IROpcode::Add:    return "add";
        case IROpcode::Sub:    return "sub";
        case IROpcode::Mul:    return "mul";
        case IROpcode::Neg:    return "neg";

        case IROpcode::Eq:     return "eq";
        case IROpcode::Ne:     return "ne";
        case IROpcode::Lt:     return "lt";
        case IROpcode::Gt:     return "gt";
        case IROpcode::Le:     return "le";
        case IROpcode::Ge:     return "ge";

        case IROpcode::Jump:   return "jump";
        case IROpcode::Branch: return "branch";

        case IROpcode::Call:   return "call";
        case IROpcode::Ret:    return "ret";
    }

    return "unknown";
}