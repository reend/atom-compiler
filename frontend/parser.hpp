#pragma once

#include "ast.hpp"
#include "token.hpp"

#include <cstddef>
#include <memory>
#include <vector>

class Parser 
{
    public:
        explicit Parser(const std::vector<Token>& tokens);

        std::unique_ptr<Program> parse();

    private:
        const std::vector<Token>& tokens_;

        std::size_t pos_ = 0;

        bool atEnd() const;

        const Token& peek() const;
        const Token& previous() const;
        const Token& advance();

        bool check(TokenKind kind) const;
        bool match(TokenKind kind);

        const Token& expect(TokenKind kind);

        std::unique_ptr<FunctionDecl> parseFunction();
        Parameter parseParameter();
        TypeKind parseType();
        std::unique_ptr<BlockStmt> parseBlock();

        std::unique_ptr<Stmt> parseStatement();

        std::unique_ptr<VariableDecl> parseVariableDecl();
        std::unique_ptr<AssignStmt> parseAssignment();
        std::unique_ptr<ReturnStmt> parseReturn();
        std::unique_ptr<IfStmt> parseIf();
        std::unique_ptr<WhileStmt> parseWhile();

        std::unique_ptr<Expr> parsePrimary();
        std::unique_ptr<Expr> parseUnary();
        std::unique_ptr<Expr> parseExpression(int minPrecedence = 1);
};