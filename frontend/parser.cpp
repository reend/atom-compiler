#include "parser.hpp"
#include "token.hpp"
#include <stdexcept>
#include <string>
#include <charconv>
#include <utility>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens) {}

const Token& Parser::peek() const
{
    return tokens_[pos_];
}

bool Parser::atEnd() const
{
    return peek().kind == TokenKind::Eof;
}

const Token& Parser::previous() const
{
    return tokens_[pos_ - 1];
}

const Token& Parser::advance()
{
    const Token& token = peek();

    if (!atEnd())
    {
        ++pos_;
    }

    return token;
}

bool Parser::check(TokenKind kind) const
{
    return peek().kind == kind;
}

bool Parser::match(TokenKind token)
{
    if (!check(token))
    {
        return false;
    }

    advance();

    return true;
}

const Token& Parser::expect(TokenKind kind)
{
    if (!check(kind))
    {
        throw std::runtime_error(
            "Expected " +
            std::string(tokenKindToString(kind)) +
            ", got " +
            std::string(tokenKindToString(peek().kind)) +
            " at " +
            std::to_string(peek().line) +
            ":" +
            std::to_string(peek().column)
        );
    }

    return advance();
}

std::unique_ptr<Program> Parser::parse()
{
    auto program = std::make_unique<Program>();

    program->line = peek().line;
    program->column = peek().column;

    while (!atEnd())
    {
        program->functions.push_back(parseFunction());
    }

    return program;
}

std::unique_ptr<FunctionDecl> Parser::parseFunction()
{
    const Token& fnToken = expect(TokenKind::Fn);

    auto function = std::make_unique<FunctionDecl>();

    function->line = fnToken.line;
    function->column = fnToken.column;

    const Token& name = expect(TokenKind::Identifier);

    function->name = name.lexeme;

    expect(TokenKind::LParen);

    if (!check(TokenKind::RParen))
    {
        do
        {
            function->parameters.push_back(parseParameter());
        }
        while (match(TokenKind::Comma));
    }

    expect(TokenKind::RParen);
    expect(TokenKind::Arrow);

    function->returnType = parseType();
    function->body = parseBlock();

    return function;
}

Parameter Parser::parseParameter()
{
    const Token& name = expect(TokenKind::Identifier);

    Parameter param;

    param.line = name.line;
    param.column = name.column;
    param.name = name.lexeme;

    expect(TokenKind::Colon);

    param.type = parseType();

    return param;
}

TypeKind Parser::parseType()
{
    if (match(TokenKind::I64))
    {
        return TypeKind::I64;
    }

    if (match(TokenKind::Bool))
    {
        return TypeKind::Bool;
    }

    throw std::runtime_error(
        "Expected type at " +
        std::to_string(peek().line) +
        ":" +
        std::to_string(peek().column)
    );
}

std::unique_ptr<BlockStmt> Parser::parseBlock()
{
    const Token& brace = expect(TokenKind::LBrace);

    auto block = std::make_unique<BlockStmt>();

    block->line = brace.line;
    block->column = brace.column;

    while (!check(TokenKind::RBrace) && !atEnd())
    {
        block->statements.push_back(parseStatement());
    }

    expect(TokenKind::RBrace);

    return block;
}

std::unique_ptr<Stmt> Parser::parseStatement()
{
    switch (peek().kind)
    {
        case TokenKind::Mut:
        case TokenKind::Const:
            return parseVariableDecl();

        case TokenKind::Identifier:
            return parseAssignment();

        case TokenKind::Return:
            return parseReturn();

        case TokenKind::If:
            return parseIf();

        case TokenKind::While:
            return parseWhile();

        default:
            throw std::runtime_error(
                "Unexpected token " +
                std::string(tokenKindToString(peek().kind)) +
                " at " +
                std::to_string(peek().line) +
                ":" +
                std::to_string(peek().column)
            );
    }
}

std::unique_ptr<VariableDecl> Parser::parseVariableDecl()
{
    const Token& token = advance();

    auto stmt = std::make_unique<VariableDecl>();

    stmt->line = token.line;
    stmt->column = token.column;
    stmt->isMutable = token.kind == TokenKind::Mut;

    const Token& name = expect(TokenKind::Identifier);

    stmt->name = name.lexeme;

    expect(TokenKind::Colon);

    stmt->type = parseType();

    expect(TokenKind::Assign);

    stmt->initializer = parseExpression();

    expect(TokenKind::Semicolon);

    return stmt;
}

std::unique_ptr<AssignStmt> Parser::parseAssignment()
{
    const Token& name = expect(TokenKind::Identifier);

    auto stmt = std::make_unique<AssignStmt>();

    stmt->line = name.line;
    stmt->column = name.column;
    stmt->name = name.lexeme;

    expect(TokenKind::Assign);

    stmt->value = parseExpression();

    expect(TokenKind::Semicolon);

    return stmt;
}

std::unique_ptr<ReturnStmt> Parser::parseReturn()
{
    const Token& token = expect(TokenKind::Return);

    auto stmt = std::make_unique<ReturnStmt>();

    stmt->line = token.line;
    stmt->column = token.column;

    stmt->value = parseExpression();

    expect(TokenKind::Semicolon);

    return stmt;
}

std::unique_ptr<IfStmt> Parser::parseIf()
{
    const Token& token = expect(TokenKind::If);

    auto stmt = std::make_unique<IfStmt>();

    stmt->line = token.line;
    stmt->column = token.column;

    expect(TokenKind::LParen);

    stmt->condition = parseExpression();

    expect(TokenKind::RParen);

    stmt->thenBranch = parseBlock();

    if (match(TokenKind::Else))
    {
        stmt->elseBranch = parseBlock();
    }

    return stmt;
}

std::unique_ptr<WhileStmt> Parser::parseWhile()
{
    const Token& token = expect(TokenKind::While);

    auto stmt = std::make_unique<WhileStmt>();

    stmt->line = token.line;
    stmt->column = token.column;

    expect(TokenKind::LParen);

    stmt->condition = parseExpression();

    expect(TokenKind::RParen);

    stmt->body = parseBlock();

    return stmt;
}

std::unique_ptr<Expr> Parser::parsePrimary()
{
    if (match(TokenKind::Integer))
    {
        const Token& token = previous();

        auto expr = std::make_unique<IntegerExpr>();

        expr->line = token.line;
        expr->column = token.column;

        const char* begin = token.lexeme.data();
        const char* end = begin + token.lexeme.size();

        const auto result =
            std::from_chars(begin, end, expr->value);

        if (result.ec != std::errc{} || result.ptr != end)
        {
            throw std::runtime_error(
                "Invalid integer at " +
                std::to_string(token.line) +
                ":" +
                std::to_string(token.column)
            );
        }

        return expr;
    }

    if (match(TokenKind::True) || match(TokenKind::False))
    {
        const Token& token = previous();

        auto expr = std::make_unique<BoolExpr>();

        expr->line = token.line;
        expr->column = token.column;

        expr->value = token.kind == TokenKind::True;

        return expr;
    }

    if (match(TokenKind::Identifier))
    {
        const Token& name = previous();

        if (match(TokenKind::LParen))
        {
            auto expr = std::make_unique<CallExpr>();

            expr->line = name.line;
            expr->column = name.column;
            expr->callee = name.lexeme;

            if (!check(TokenKind::RParen))
            {
                do
                {
                    expr->arguments.push_back(parseExpression());
                }
                while (match(TokenKind::Comma));
            }

            expect(TokenKind::RParen);

            return expr;
        }

        auto expr = std::make_unique<IdentifierExpr>();

        expr->line = name.line;
        expr->column = name.column;
        expr->name = name.lexeme;

        return expr;
    }

    if (match(TokenKind::LParen))
    {
        auto expr = parseExpression();

        expect(TokenKind::RParen);

        return expr;
    }

    throw std::runtime_error(
        "Expected expression, got " +
        std::string(tokenKindToString(peek().kind)) +
        " at " +
        std::to_string(peek().line) +
        ":" +
        std::to_string(peek().column)
    );
}

std::unique_ptr<Expr> Parser::parseExpression(int minPrecedence)
{
    auto left = parseUnary();

    while (true)
    {
        const int precedence = getPrecedence(peek().kind);

        if (precedence < minPrecedence)
        {
            break;
        }

        const Token& op = advance();

        auto right = parseExpression(precedence + 1);

        auto expr = std::make_unique<BinaryExpr>();

        expr->line = op.line;
        expr->column = op.column;
        expr->op = op.kind;

        expr->left = std::move(left);
        expr->right = std::move(right);

        left = std::move(expr);
    }

    return left;
}

std::unique_ptr<Expr> Parser::parseUnary()
{
    if (match(TokenKind::Minus))
    {
        const Token& token = previous();

        auto expr = std::make_unique<UnaryExpr>();

        expr->line = token.line;
        expr->column = token.column;
        expr->op = token.kind;

        expr->operand = parseUnary();

        return expr;
    }

    return parsePrimary();
}

int Parser::getPrecedence(TokenKind kind) const
{
    switch (kind)
    {
        case TokenKind::Equal:
        case TokenKind::NotEqual:
            return 1;

        case TokenKind::Less:
        case TokenKind::Greater:
        case TokenKind::LessEqual:
        case TokenKind::GreaterEqual:
            return 2;

        case TokenKind::Plus:
        case TokenKind::Minus:
            return 3;

        case TokenKind::Star:
            return 4;

        default:
            return 0;
    }
}