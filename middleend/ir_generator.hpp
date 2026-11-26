#pragma once

#include "ir.hpp"
#include "../frontend/ast.hpp"

#include <unordered_map>

class IRGenerator
{
public:
    IRModule generate(const Program& program);

private:
    IRFunction* currentFunction_ = nullptr;
    BlockId currentBlock_ = 0;

    std::unordered_map<std::string_view, SlotId> variables_;

    void generateFunction(const FunctionDecl& function);
    void generateBlock(const BlockStmt& block);
    void generateStatement(const Stmt& stmt);
    VReg generateExpression(const Expr& expr);

    VReg createVReg();
    SlotId createSlot();
    BlockId createBlock();

    void setCurrentBlock(BlockId id);
    void emit(IRInstruction instruction);

    bool isBlockTerminated() const;
};