#pragma once

#include "token.hpp"

#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

enum class TypeKind
{
    I64,
    Bool
};

struct ASTNode
{
    std::size_t line;
    std::size_t column;

    virtual ~ASTNode() = default;
};

struct Expr : ASTNode
{

};

struct Stmt : ASTNode
{

};

struct IntegerExpr : Expr
{
    std::int64_t value;
};

struct BoolExpr : Expr
{
    bool value;
};

struct IdentifierExpr : Expr
{
    std::string_view name;
};

struct BinaryExpr : Expr 
{
    TokenKind op;

    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct UnaryExpr : Expr 
{
    TokenKind op;
    std::unique_ptr<Expr> operand;
};

struct CallExpr : Expr 
{
    std::string_view callee;
    std::vector<std::unique_ptr<Expr>> arguments;
};

struct VariableDecl : Stmt 
{
    bool isMutable;

    std::string_view name;
    TypeKind type;

    std::unique_ptr<Expr> initializer;
};

struct AssignStmt : Stmt 
{
    std::string_view name;
    std::unique_ptr<Expr> value;
};

struct ReturnStmt : Stmt 
{
    std::unique_ptr<Expr> value;
};

struct BlockStmt : Stmt 
{
    std::vector<std::unique_ptr<Stmt>> statements;
};

struct IfStmt : Stmt 
{
    std::unique_ptr<Expr> condition;

    std::unique_ptr<BlockStmt> thenBranch;
    std::unique_ptr<BlockStmt> elseBranch;
};

struct WhileStmt : Stmt
{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;
};

struct Parameter : ASTNode
{
    std::string_view name;
    TypeKind kind;
};

struct FunctionDecl : ASTNode 
{
    std::string_view name;
    std::vector<Parameter> parameters;
    TypeKind returnType;
    std::unique_ptr<BlockStmt> body;
};

struct Program : ASTNode 
{
    std::vector<std::unique_ptr<FunctionDecl>> functions;
};