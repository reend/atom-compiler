#include "frontend/lexer.hpp"
#include "frontend/token.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: atomc <file.atom>\n";
        return 1;
    }

    std::ifstream file(argv[1]);

    if (!file)
    {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    const std::string source = buffer.str();

    try
    {
        Lexer lexer(source);
        const auto tokens = lexer.tokenize();

        for (const auto& token : tokens)
        {
            std::cout << token.line << ":"
                      << token.column << " "
                      << tokenKindToString(token.kind) << " "
                      << token.lexeme << "\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}