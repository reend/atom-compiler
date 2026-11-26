#pragma once
#include <cstdint>
#include <cstddef>
#include <string_view>
#include <vector>
#include <optional>

enum class IROpcode
{
    // Constants
    Const,

    // Memory
    Load,
    Store,

    // Arithmetic
    Add,
    Sub,
    Mul,
    Neg,

    // Comparison
    Eq,
    Ne,
    Lt,
    Gt,
    Le,
    Ge,

    // Control flow
    Jump,
    Branch,

    // Functions
    Call,
    Ret
};

using VReg = std::size_t;
using SlotId = std::size_t;
using BlockId = std::size_t;

enum class IROperandKind
{
    Register,
    Immediate,
    Slot,
    Block
};

struct IROperand
{
    IROperandKind kind;
    std::int64_t value;
};

struct IRInstruction
{
    IROpcode opcode;
    std::optional<VReg> destination;
    std::vector<IROperand> operands;
    std::string_view callee;
};

struct IRBasicBlock
{
    BlockId id;
    std::vector<IRInstruction> instructions;
};

struct IRFunction
{
    std::string_view name;

    std::vector<SlotId> parameters;
    std::vector<IRBasicBlock> blocks;

    std::size_t slotCount = 0;
    std::size_t vregCount = 0;
};  

struct IRModule
{
    std::vector<IRFunction> functions;
};

std::string_view irOpcodeToString(IROpcode opcode);