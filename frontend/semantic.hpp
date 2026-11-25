#pragma once

#include "ast.hpp"

#include <string_view>
#include <unordered_map>
#include <vector>

struct VariableSymbol
{
    TypeKind type;
    bool isMutable;
};

struct FunctionSymbol
{
    TypeKind returnType;
    std::vector<TypeKind> parameterTypes;
};

class SemanticAnalyzer
{
public:
    void analyze(const Program& program);

private:
    std::unordered_map<std::string_view, VariableSymbol> variables_;
    std::unordered_map<std::string_view, FunctionSymbol> functions_;

    TypeKind currentReturnType_;

    void collectFunctions(const Program& program);
    void analyzeFunction(const FunctionDecl& function);

    void analyzeBlock(const BlockStmt& block, bool isFunctionBody);
    void analyzeStatement(const Stmt& stmt, bool isFunctionBody);

    TypeKind analyzeExpression(const Expr& expr);

    const VariableSymbol& lookupVariable(std::string_view name,
                                         const ASTNode& node) const;

    const FunctionSymbol& lookupFunction(std::string_view name,
                                         const ASTNode& node) const;

    [[noreturn]] void error(const ASTNode& node, std::string_view message) const;
};