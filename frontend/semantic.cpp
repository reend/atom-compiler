#include "semantic.hpp"
#include <stdexcept>
#include <string>
#include <utility>

void SemanticAnalyzer::error(const ASTNode& node,
                             std::string_view message) const
{
    throw std::runtime_error(
        "Semantic error at " +
        std::to_string(node.line) +
        ":" +
        std::to_string(node.column) +
        ": " +
        std::string(message)
    );
}

void SemanticAnalyzer::collectFunctions(const Program& program)
{
    for (const auto& function : program.functions)
    {
        if (functions_.find(function->name) != functions_.end())
        {
            error(*function, "Duplicate function");
        }

        if (function->parameters.size() > 2)
        {
            error(*function, "Too many parameters");
        }

        FunctionSymbol symbol;

        symbol.returnType = function->returnType;

        for (const auto& param : function->parameters)
        {
            symbol.parameterTypes.push_back(param.type);
        }

        functions_.emplace(function->name, std::move(symbol));
    }

    const auto it = functions_.find("main");

    if (it == functions_.end())
    {
        error(program, "Missing main function");
    }

    const FunctionSymbol& main = it->second;

    if (main.returnType != TypeKind::I64 ||
        !main.parameterTypes.empty())
    {
        error(program, "Expected main() -> i64");
    }
}

void SemanticAnalyzer::analyze(const Program& program)
{
    functions_.clear();
    variables_.clear();

    collectFunctions(program);

    for (const auto& function : program.functions)
    {
        analyzeFunction(*function);
    }
}

void SemanticAnalyzer::analyzeFunction(const FunctionDecl& function)
{
    variables_.clear();

    currentReturnType_ = function.returnType;

    for (const auto& param : function.parameters)
    {
        if (variables_.find(param.name) != variables_.end())
        {
            error(param, "Duplicate parameter");
        }

        variables_.emplace(
            param.name,
            VariableSymbol{param.type, false}
        );
    }

    analyzeBlock(*function.body, true);

    const auto& statements = function.body->statements;

    if (statements.empty() ||
        dynamic_cast<const ReturnStmt*>(statements.back().get()) == nullptr)
    {
        error(function, "Function must end with return");
    }
}

const VariableSymbol& SemanticAnalyzer::lookupVariable(
    std::string_view name,
    const ASTNode& node) const
{
    const auto it = variables_.find(name);

    if (it == variables_.end())
    {
        error(node, "Undefined variable: " + std::string(name));
    }

    return it->second;
}

const FunctionSymbol& SemanticAnalyzer::lookupFunction(
    std::string_view name,
    const ASTNode& node) const
{
    const auto it = functions_.find(name);

    if (it == functions_.end())
    {
        error(node, "Undefined function: " + std::string(name));
    }

    return it->second;
}

TypeKind SemanticAnalyzer::analyzeExpression(const Expr& expr)
{
    if (dynamic_cast<const IntegerExpr*>(&expr))
    {
        return TypeKind::I64;
    }

    if (dynamic_cast<const BoolExpr*>(&expr))
    {
        return TypeKind::Bool;
    }

    if (const auto* node = dynamic_cast<const IdentifierExpr*>(&expr))
    {
        return lookupVariable(node->name, *node).type;
    }

    if (const auto* node = dynamic_cast<const UnaryExpr*>(&expr))
    {
        TypeKind operand = analyzeExpression(*node->operand);

        if (operand != TypeKind::I64)
        {
            error(*node, "Unary minus requires i64");
        }

        return TypeKind::I64;
    }

    if (const auto* node = dynamic_cast<const BinaryExpr*>(&expr))
    {
        TypeKind left = analyzeExpression(*node->left);
        TypeKind right = analyzeExpression(*node->right);

        switch (node->op)
        {
            case TokenKind::Plus:
            case TokenKind::Minus:
            case TokenKind::Star:
                if (left != TypeKind::I64 ||
                    right != TypeKind::I64)
                {
                    error(*node, "Arithmetic operators require i64");
                }
                return TypeKind::I64;

            case TokenKind::Less:
            case TokenKind::Greater:
            case TokenKind::LessEqual:
            case TokenKind::GreaterEqual:
                if (left != TypeKind::I64 ||
                    right != TypeKind::I64)
                {
                    error(*node, "Comparison operators require i64");
                }
                return TypeKind::Bool;

            case TokenKind::Equal:
            case TokenKind::NotEqual:
                if (left != right)
                {
                    error(*node, "Equality operands must have the same type");
                }
                return TypeKind::Bool;

            default:
                error(*node, "Unknown binary operator");
        }
    }

    if (const auto* node = dynamic_cast<const CallExpr*>(&expr))
    {
        const FunctionSymbol& function =
            lookupFunction(node->callee, *node);

        if (node->arguments.size() != function.parameterTypes.size())
        {
            error(*node, "Incorrect number of arguments");
        }

        for (std::size_t i = 0; i < node->arguments.size(); ++i)
        {
            TypeKind argumentType =
                analyzeExpression(*node->arguments[i]);

            if (argumentType != function.parameterTypes[i])
            {
                error(*node->arguments[i], "Argument type mismatch");
            }
        }

        return function.returnType;
    }

    error(expr, "Unknown expression");
}

void SemanticAnalyzer::analyzeStatement(const Stmt& stmt,
                                        bool isFunctionBody)
{
    if (const auto* node = dynamic_cast<const VariableDecl*>(&stmt))
    {
        if (!isFunctionBody)
        {
            error(*node, "Variable declarations are only allowed in function body");
        }

        if (variables_.find(node->name) != variables_.end())
        {
            error(*node, "Duplicate variable");
        }

        TypeKind initializerType =
            analyzeExpression(*node->initializer);

        if (initializerType != node->type)
        {
            error(*node, "Initializer type mismatch");
        }

        variables_.emplace(
            node->name,
            VariableSymbol{node->type, node->isMutable}
        );

        return;
    }

    if (const auto* node = dynamic_cast<const AssignStmt*>(&stmt))
    {
        const VariableSymbol& variable =
            lookupVariable(node->name, *node);

        if (!variable.isMutable)
        {
            error(*node, "Cannot assign to immutable variable");
        }

        TypeKind valueType = analyzeExpression(*node->value);

        if (valueType != variable.type)
        {
            error(*node, "Assignment type mismatch");
        }

        return;
    }

    if (const auto* node = dynamic_cast<const ReturnStmt*>(&stmt))
    {
        TypeKind valueType = analyzeExpression(*node->value);

        if (valueType != currentReturnType_)
        {
            error(*node, "Return type mismatch");
        }

        return;
    }

    if (const auto* node = dynamic_cast<const IfStmt*>(&stmt))
    {
        TypeKind conditionType =
            analyzeExpression(*node->condition);

        if (conditionType != TypeKind::Bool)
        {
            error(*node, "If condition must be bool");
        }

        analyzeBlock(*node->thenBranch, false);

        if (node->elseBranch)
        {
            analyzeBlock(*node->elseBranch, false);
        }

        return;
    }

    if (const auto* node = dynamic_cast<const WhileStmt*>(&stmt))
    {
        TypeKind conditionType =
            analyzeExpression(*node->condition);

        if (conditionType != TypeKind::Bool)
        {
            error(*node, "While condition must be bool");
        }

        analyzeBlock(*node->body, false);

        return;
    }

    error(stmt, "Unknown statement");
}

void SemanticAnalyzer::analyzeBlock(const BlockStmt& block,
                                    bool isFunctionBody)
{
    for (const auto& stmt : block.statements)
    {
        analyzeStatement(*stmt, isFunctionBody);
    }
}