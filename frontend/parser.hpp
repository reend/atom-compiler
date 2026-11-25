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
};