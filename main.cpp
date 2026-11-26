#include "frontend/lexer.hpp"
#include "frontend/token.hpp"
#include "frontend/parser.hpp"
#include "frontend/ast_printer.hpp"
#include "frontend/semantic.hpp"
#include "middleend/ir_generator.hpp"
#include "middleend/ir_printer.hpp"
#include "middleend/ir_verifier.hpp"

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

        Parser parser(tokens);
        auto program = parser.parse();

        SemanticAnalyzer semantic;
        semantic.analyze(*program);

        IRGenerator generator;
        IRModule module = generator.generate(*program);

        IRVerifier verifier;
        verifier.verify(module);

        IRPrinter printer;
        printer.print(module);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }

    return 0;
}