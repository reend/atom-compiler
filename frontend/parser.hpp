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
};