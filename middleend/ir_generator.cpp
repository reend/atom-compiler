#include "ir_generator.hpp"
#include <stdexcept>
#include <utility>

VReg IRGenerator::createVReg()
{
    return currentFunction_->vregCount++;
}

SlotId IRGenerator::createSlot()
{
    return currentFunction_->slotCount++;
}

BlockId IRGenerator::createBlock()
{
    BlockId id = currentFunction_->blocks.size();

    currentFunction_->blocks.push_back(
        IRBasicBlock{id, {}}
    );

    return id;
}

void IRGenerator::setCurrentBlock(BlockId id)
{
    currentBlock_ = id;
}

void IRGenerator::emit(IRInstruction instruction)
{
    currentFunction_->blocks[currentBlock_]
        .instructions.push_back(std::move(instruction));
}

IRModule IRGenerator::generate(const Program& program)
{
    IRModule module;

    for (const auto& function : program.functions)
    {
        module.functions.emplace_back();

        currentFunction_ = &module.functions.back();

        generateFunction(*function);
    }

    currentFunction_ = nullptr;

    return module;
}

void IRGenerator::generateFunction(const FunctionDecl& function)
{
    variables_.clear();

    currentFunction_->name = function.name;

    for (const auto& param : function.parameters)
    {
        SlotId slot = createSlot();

        variables_.emplace(param.name, slot);

        currentFunction_->parameters.push_back(slot);
    }

    BlockId entry = createBlock();

    setCurrentBlock(entry);

    generateBlock(*function.body);
}

void IRGenerator::generateBlock(const BlockStmt& block)
{
    for (const auto& stmt : block.statements)
    {
        if (isBlockTerminated())
        {
            break;
        }

        generateStatement(*stmt);
    }
}

VReg IRGenerator::generateExpression(const Expr& expr)
{
    if (const auto* node = dynamic_cast<const IntegerExpr*>(&expr))
    {
        VReg dest = createVReg();

        emit(IRInstruction{
            IROpcode::Const,
            dest,
            {{IROperandKind::Immediate, node->value}},
            {}
        });

        return dest;
    }

    if (const auto* node = dynamic_cast<const BoolExpr*>(&expr))
    {
        VReg dest = createVReg();

        emit(IRInstruction{
            IROpcode::Const,
            dest,
            {{IROperandKind::Immediate, node->value ? 1 : 0}},
            {}
        });

        return dest;
    }

    if (const auto* node = dynamic_cast<const IdentifierExpr*>(&expr))
    {
        SlotId slot = variables_.at(node->name);

        VReg dest = createVReg();

        emit(IRInstruction{
            IROpcode::Load,
            dest,
            {{IROperandKind::Slot, static_cast<std::int64_t>(slot)}},
            {}
        });

        return dest;
    }

    if (const auto* node = dynamic_cast<const UnaryExpr*>(&expr))
    {
        VReg operand = generateExpression(*node->operand);

        VReg dest = createVReg();

        emit(IRInstruction{
            IROpcode::Neg,
            dest,
            {{IROperandKind::Register, static_cast<std::int64_t>(operand)}},
            {}
        });

        return dest;
    }

    if (const auto* node = dynamic_cast<const BinaryExpr*>(&expr))
    {
        VReg left = generateExpression(*node->left);
        VReg right = generateExpression(*node->right);

        IROpcode opcode;

        switch (node->op)
        {
            case TokenKind::Plus:
                opcode = IROpcode::Add;
                break;

            case TokenKind::Minus:
                opcode = IROpcode::Sub;
                break;

            case TokenKind::Star:
                opcode = IROpcode::Mul;
                break;

            case TokenKind::Equal:
                opcode = IROpcode::Eq;
                break;

            case TokenKind::NotEqual:
                opcode = IROpcode::Ne;
                break;

            case TokenKind::Less:
                opcode = IROpcode::Lt;
                break;

            case TokenKind::Greater:
                opcode = IROpcode::Gt;
                break;

            case TokenKind::LessEqual:
                opcode = IROpcode::Le;
                break;

            case TokenKind::GreaterEqual:
                opcode = IROpcode::Ge;
                break;

            default:
                throw std::runtime_error("Unknown binary operator");
        }

        VReg dest = createVReg();

        emit(IRInstruction{
            opcode,
            dest,
            {
                {IROperandKind::Register, static_cast<std::int64_t>(left)},
                {IROperandKind::Register, static_cast<std::int64_t>(right)}
            },
            {}
        });

        return dest;
    }

    if (const auto* node = dynamic_cast<const CallExpr*>(&expr))
    {
        std::vector<IROperand> operands;

        for (const auto& argument : node->arguments)
        {
            VReg reg = generateExpression(*argument);

            operands.push_back({
                IROperandKind::Register,
                static_cast<std::int64_t>(reg)
            });
        }

        VReg dest = createVReg();

        emit(IRInstruction{
            IROpcode::Call,
            dest,
            std::move(operands),
            node->callee
        });

        return dest;
    }

    throw std::runtime_error("Unknown expression in IR generator");
}

void IRGenerator::generateStatement(const Stmt& stmt)
{
    if (const auto* node = dynamic_cast<const VariableDecl*>(&stmt))
    {
        VReg value = generateExpression(*node->initializer);

        SlotId slot = createSlot();

        variables_.emplace(node->name, slot);

        emit(IRInstruction{
            IROpcode::Store,
            std::nullopt,
            {
                {IROperandKind::Slot, static_cast<std::int64_t>(slot)},
                {IROperandKind::Register, static_cast<std::int64_t>(value)}
            },
            {}
        });

        return;
    }

    if (const auto* node = dynamic_cast<const AssignStmt*>(&stmt))
    {
        SlotId slot = variables_.at(node->name);

        VReg value = generateExpression(*node->value);

        emit(IRInstruction{
            IROpcode::Store,
            std::nullopt,
            {
                {IROperandKind::Slot, static_cast<std::int64_t>(slot)},
                {IROperandKind::Register, static_cast<std::int64_t>(value)}
            },
            {}
        });

        return;
    }

    if (const auto* node = dynamic_cast<const ReturnStmt*>(&stmt))
    {
        VReg value = generateExpression(*node->value);

        emit(IRInstruction{
            IROpcode::Ret,
            std::nullopt,
            {{IROperandKind::Register, static_cast<std::int64_t>(value)}},
            {}
        });

        return;
    }

    if (const auto* node = dynamic_cast<const IfStmt*>(&stmt))
    {
        VReg condition = generateExpression(*node->condition);

        BlockId thenBlock = createBlock();
        BlockId endBlock = createBlock();

        BlockId elseBlock = node->elseBranch
            ? createBlock()
            : endBlock;

        emit(IRInstruction{
            IROpcode::Branch,
            std::nullopt,
            {
                {IROperandKind::Register, static_cast<std::int64_t>(condition)},
                {IROperandKind::Block, static_cast<std::int64_t>(thenBlock)},
                {IROperandKind::Block, static_cast<std::int64_t>(elseBlock)}
            },
            {}
        });

        setCurrentBlock(thenBlock);

        generateBlock(*node->thenBranch);

        if (!isBlockTerminated())
        {
            emit(IRInstruction{
                IROpcode::Jump,
                std::nullopt,
                {{IROperandKind::Block, static_cast<std::int64_t>(endBlock)}},
                {}
            });
        }

        if (node->elseBranch)
        {
            setCurrentBlock(elseBlock);

            generateBlock(*node->elseBranch);

            if (!isBlockTerminated())
            {
                emit(IRInstruction{
                    IROpcode::Jump,
                    std::nullopt,
                    {{IROperandKind::Block, static_cast<std::int64_t>(endBlock)}},
                    {}
                });
            }
        }

        setCurrentBlock(endBlock);

        return;
    }

    if (const auto* node = dynamic_cast<const WhileStmt*>(&stmt))
    {
        BlockId conditionBlock = createBlock();
        BlockId bodyBlock = createBlock();
        BlockId endBlock = createBlock();

        emit(IRInstruction{
            IROpcode::Jump,
            std::nullopt,
            {{IROperandKind::Block, static_cast<std::int64_t>(conditionBlock)}},
            {}
        });

        setCurrentBlock(conditionBlock);

        VReg condition = generateExpression(*node->condition);

        emit(IRInstruction{
            IROpcode::Branch,
            std::nullopt,
            {
                {IROperandKind::Register, static_cast<std::int64_t>(condition)},
                {IROperandKind::Block, static_cast<std::int64_t>(bodyBlock)},
                {IROperandKind::Block, static_cast<std::int64_t>(endBlock)}
            },
            {}
        });

        setCurrentBlock(bodyBlock);

        generateBlock(*node->body);

        if (!isBlockTerminated())
        {
            emit(IRInstruction{
                IROpcode::Jump,
                std::nullopt,
                {{IROperandKind::Block, static_cast<std::int64_t>(conditionBlock)}},
                {}
            });
        }

        setCurrentBlock(endBlock);

        return;
    }

    throw std::runtime_error("Unknown statement in IR generator");
}

bool IRGenerator::isBlockTerminated() const
{
    const auto& instructions =
        currentFunction_->blocks[currentBlock_].instructions;

    if (instructions.empty())
    {
        return false;
    }

    IROpcode opcode = instructions.back().opcode;

    return opcode == IROpcode::Ret ||
           opcode == IROpcode::Jump ||
           opcode == IROpcode::Branch;
}