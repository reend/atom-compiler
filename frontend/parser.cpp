#include "parser.hpp"
#include <stdexcept>
#include <string>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens)
    {

    }

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