#pragma once
#include <cstddef>
#include <string_view>

enum class TokenKind {
    // keywords
    Fn,
    Let,
    If,
    Else,
    While,
    Return,

    // literals and identifiers
    Identifier,
    Integer,

    // arithmetic
    Plus,
    Minus,
    Star,

    // comparison
    Equal,
    NotEqual,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,

    // assignment
    Assign,

    // delimiters
    LParen,
    RParen,
    LBrace,
    RBrace, 
    Comma,
    Semicolon,

    // eof
    Eof
};

struct Token {
    TokenKind kind;
    std::string_view lexeme;
    
    std::size_t line;
    std::size_t column;
};