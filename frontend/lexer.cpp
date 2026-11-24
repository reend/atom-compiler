#include "lexer.hpp"
#include "token.hpp"
#include <stdexcept>
#include <unordered_map>
#include <iostream>

namespace
{
    const std::unordered_map<std::string_view, TokenKind> keywords =
    {
        {"fn",     TokenKind::Fn},
        {"mut",    TokenKind::Mut},
        {"const",  TokenKind::Const},
        {"if",     TokenKind::If},
        {"else",   TokenKind::Else},
        {"while",  TokenKind::While},
        {"return", TokenKind::Return},

        {"i64",    TokenKind::I64},
        {"bool",   TokenKind::Bool},

        {"true",   TokenKind::True},
        {"false", TokenKind::False}
    };

    constexpr bool isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    constexpr bool isAlpha(char c)
    {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
               c == '_';
    }

    constexpr bool isAlphaNumeric(char c)
    {
        return isAlpha(c) || isDigit(c);
    }
}

Lexer::Lexer(std::string_view source)
    : source_(source) {}

bool Lexer::atEnd() const 
{
    return pos_ >= source_.size();
}

char Lexer::peek() const 
{
    return atEnd() ? '\0' : source_[pos_];
}

char Lexer::advance() 
{
    if (atEnd()) 
    {
        return '\0';
    }

    char c = source_[pos_++];

    if (c == '\n') 
    {
        ++line_;
        column_ = 1;
    } 
    else 
    {
        ++column_;
    }

    return c;
}

bool Lexer::match(char expected) 
{
    if (atEnd() || peek() != expected) 
    {
        return false;
    }

    advance();
    return true;
}

void Lexer::skipWhitespace() 
{
    while (!atEnd()) 
    {
        switch (peek()) 
        {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                advance();
                break;
            case '/':
                if (pos_ + 1 >= source_.size() ||
                    source_[pos_ + 1] != '/') 
                {
                        return;
                }

                while (!atEnd() && peek() != '\n') 
                {
                    advance();
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::scanNumber()
{
    const std::size_t start = pos_;
    const std::size_t line = line_;
    const std::size_t column = column_;

    while (!atEnd() && peek() >= '0' && isDigit(peek()))
    {
        advance();
    }

    return Token
    {
        TokenKind::Integer,
        source_.substr(start, pos_ - start),
        line,
        column
    };
}

Token Lexer::scanIdentifier()
{
    const std::size_t start = pos_;
    const std::size_t line = line_;
    const std::size_t column = column_;

    while(!atEnd() && isAlphaNumeric(peek()))
    {
        advance();
    }

    const std::string_view lexeme = 
        source_.substr(start, pos_ - start);

    const auto it = keywords.find(lexeme);

    const TokenKind kind = it != keywords.end()
        ? it->second
        : TokenKind::Identifier;

    return Token 
    {
        kind, 
        lexeme,
        line,
        column
    };
}

Token Lexer::nextToken()
{
    std::cerr << "Before skipWhitespace\n";
    skipWhitespace();
    std::cerr << "After skipWhitespace\n";

    const std::size_t start = pos_;
    const std::size_t line = line_;
    const std::size_t column = column_;

    if (atEnd())
    {
        return Token 
        {
            TokenKind::Eof,
            {},
            line,
            column
        };
    }

    if (isDigit(peek()))
    {
        return scanNumber();
    }

    if (isAlpha(peek()))
    {
        return scanIdentifier();
    }

    TokenKind kind;

    switch (advance())
    {
        case '+':
            kind = TokenKind::Plus;
            break;

        case '-':
            kind = match('>')
                ? TokenKind::Arrow
                : TokenKind::Minus;
            break;

        case '*':
            kind = TokenKind::Star;
            break;

        case '=':
            kind = match('=')
                ? TokenKind::Equal
                : TokenKind::Assign;
            break;

        case '!':
            if (!match('='))
            {
                throw std::runtime_error("Exptected '=' after '!'");
            }  
            kind = TokenKind::NotEqual;
            break;

        case '<':
            kind = match('=')
                ? TokenKind::LessEqual
                : TokenKind::Less;
            break;

        case '>':
            kind = match('=')
                ? TokenKind::GreaterEqual
                : TokenKind::Greater;
            break;

        case '(':
            kind = TokenKind::LParen;
            break;

        case ')':
            kind = TokenKind::RParen;
            break;

        case '{':
            kind = TokenKind::LBrace;
            break;

        case '}':
            kind = TokenKind::RBrace;
            break;

        case ',':
            kind = TokenKind::Comma;
            break;

        case ';':
            kind = TokenKind::Semicolon;
            break;

        case ':':
            kind = TokenKind::Colon;
            break;

        default:
            throw std::runtime_error("Unexpected character");
    }

    return Token
    {
        kind,
        source_.substr(start, pos_ - start),
        line,
        column
    };
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    while (true)
    {
        std::cerr << "Before: pos_ = " << pos_ << "\n";
        Token token = nextToken();
        std::cerr << "After: pos_ = " << pos_
                  << ", lexeme = " << token.lexeme << "\n";

        tokens.push_back(token);

        if (token.kind == TokenKind::Eof)
        {
            break;
        }
    }

    return tokens;
}