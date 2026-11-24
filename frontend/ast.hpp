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