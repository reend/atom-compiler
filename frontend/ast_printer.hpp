#pragma once

#include "ast.hpp"

class ASTPrinter
{
public:
    void print(const Program& program);

private:
    void printFunction(const FunctionDecl& function, int depth);
    void printBlock(const BlockStmt& block, int depth);
    void printStatement(const Stmt& stmt, int depth);
    void printExpression(const Expr& expr, int depth);

    void indent(int depth);
};

