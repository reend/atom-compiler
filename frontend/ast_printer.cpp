#include "ast_printer.hpp"
#include <iostream>

namespace
{
    std::string_view typeToString(TypeKind type)
    {
        switch (type)
        {
            case TypeKind::I64:
                return "i64";

            case TypeKind::Bool:
                return "bool";
        }

        return "Unknown";
    }
}

void ASTPrinter::indent(int depth)
{
    for (int i = 0; i < depth; ++i)
    {
        std::cout << "    ";
    }
}

void ASTPrinter::printExpression(const Expr& expr, int depth)
{
    indent(depth);

    if (const auto* node = dynamic_cast<const IntegerExpr*>(&expr))
    {
        std::cout << "IntegerExpr: " << node->value << "\n";
    }
    else if (const auto* node = dynamic_cast<const BoolExpr*>(&expr))
    {
        std::cout << "BoolExpr: "
                  << (node->value ? "true" : "false") << "\n";
    }
    else if (const auto* node = dynamic_cast<const IdentifierExpr*>(&expr))
    {
        std::cout << "IdentifierExpr: " << node->name << "\n";
    }
    else if (const auto* node = dynamic_cast<const UnaryExpr*>(&expr))
    {
        std::cout << "UnaryExpr: "
                  << tokenKindToString(node->op) << "\n";

        printExpression(*node->operand, depth + 1);
    }
    else if (const auto* node = dynamic_cast<const BinaryExpr*>(&expr))
    {
        std::cout << "BinaryExpr: "
                  << tokenKindToString(node->op) << "\n";

        printExpression(*node->left, depth + 1);
        printExpression(*node->right, depth + 1);
    }
    else if (const auto* node = dynamic_cast<const CallExpr*>(&expr))
    {
        std::cout << "CallExpr: " << node->callee << "\n";

        for (const auto& arg : node->arguments)
        {
            printExpression(*arg, depth + 1);
        }
    }
    else
    {
        std::cout << "Unknown expression\n";
    }
}

void ASTPrinter::printStatement(const Stmt& stmt, int depth)
{
    indent(depth);

    if (const auto* node = dynamic_cast<const VariableDecl*>(&stmt))
    {
        std::cout << "VariableDecl: "
                  << (node->isMutable ? "mut " : "const ")
                  << node->name << ": "
                  << typeToString(node->type) << "\n";

        printExpression(*node->initializer, depth + 1);
    }
    else if (const auto* node = dynamic_cast<const AssignStmt*>(&stmt))
    {
        std::cout << "AssignStmt: " << node->name << "\n";

        printExpression(*node->value, depth + 1);
    }
    else if (const auto* node = dynamic_cast<const ReturnStmt*>(&stmt))
    {
        std::cout << "ReturnStmt\n";

        printExpression(*node->value, depth + 1);
    }
    else if (const auto* node = dynamic_cast<const IfStmt*>(&stmt))
    {
        std::cout << "IfStmt\n";

        indent(depth + 1);
        std::cout << "Condition:\n";
        printExpression(*node->condition, depth + 2);

        indent(depth + 1);
        std::cout << "Then:\n";
        printBlock(*node->thenBranch, depth + 2);

        if (node->elseBranch)
        {
            indent(depth + 1);
            std::cout << "Else:\n";
            printBlock(*node->elseBranch, depth + 2);
        }
    }
    else if (const auto* node = dynamic_cast<const WhileStmt*>(&stmt))
    {
        std::cout << "WhileStmt\n";

        indent(depth + 1);
        std::cout << "Condition:\n";
        printExpression(*node->condition, depth + 2);

        indent(depth + 1);
        std::cout << "Body:\n";
        printBlock(*node->body, depth + 2);
    }
    else
    {
        std::cout << "Unknown statement\n";
    }
}

void ASTPrinter::printBlock(const BlockStmt& block, int depth)
{
    indent(depth);

    std::cout << "BlockStmt\n";

    for (const auto& stmt : block.statements)
    {
        printStatement(*stmt, depth + 1);
    }
}

void ASTPrinter::printFunction(const FunctionDecl& function, int depth)
{
    indent(depth);

    std::cout << "FunctionDecl: "
              << function.name << " -> "
              << typeToString(function.returnType) << "\n";

    for (const auto& param : function.parameters)
    {
        indent(depth + 1);

        std::cout << "Parameter: "
                  << param.name << ": "
                  << typeToString(param.type) << "\n";
    }

    printBlock(*function.body, depth + 1);
}

void ASTPrinter::print(const Program& program)
{
    std::cout << "Program\n";

    for (const auto& function : program.functions)
    {
        printFunction(*function, 1);
    }
}