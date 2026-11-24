#pragma once

#include <cstddef>
#include <string_view>

enum class TokenKind 
{
    // Keywords
    Fn,
    Mut,
    Const,
    If,
    Else,
    While,
    Return,

    // Types
    I64,
    Bool,

    // Boolean literals
    True,
    False,

    // Literals and identifiers
    Identifier,
    Integer,

    // Arithmetic
    Plus,
    Minus,
    Star,

    // Comparison
    Equal,
    NotEqual,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,

    // Assignment
    Assign,

    // Delimiters
    LParen,
    RParen,
    LBrace,
    RBrace,
    Comma,
    Semicolon,
    Colon,
    Arrow,

    // End of file
    Eof
};

struct Token 
{
    TokenKind kind;
    std::string_view lexeme;

    std::size_t line;
    std::size_t column;
};