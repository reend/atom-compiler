#pragma once 
#include "token.hpp"
#include <string_view>
#include <vector>

class Lexer 
{
    public:
        explicit Lexer(std::string_view source);
        std::vector<Token> tokenize;

    private:
        std::string_view source_;

        std::size_t pos_ = 0;
        std::size_t line_ = 1;
        std::size_t column_ = 1;

        bool atEnd() const;
        char peek() const;
        char advance();
        bool match(char expected);

        void skipWhitespace();

        Token nextToken();
        Token scanNumber();
        Token scanIdentifier();
};