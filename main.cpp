#include "frontend/lexer.hpp"
#include "frontend/token.hpp"
#include "frontend/parser.hpp"
#include "frontend/ast_printer.hpp"
#include "frontend/semantic.hpp"

#include "middleend/ir_generator.hpp"
#include "middleend/ir_printer.hpp"
#include "middleend/ir_verifier.hpp"
#include "middleend/cfg.hpp"
#include "middleend/cfg_printer.hpp"
#include "middleend/constant_folding.hpp"
#include "middleend/constant_propagation.hpp"
#include "middleend/dead_code_elimination.hpp"

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

        ConstantFolding constantFolding;
        ConstantPropagation constantPropagation;
        DeadCodeElimination deadCodeElimination;

        bool changed = true;

        while (changed)
        {
            bool folded = constantFolding.run(module);
            bool propagated = constantPropagation.run(module);
            bool eliminated = deadCodeElimination.run(module);

            changed = folded || propagated || eliminated;
        }

        verifier.verify(module);

        IRPrinter printer;
        printer.print(module);

        CFGBuilder cfgBuilder;
        CFGPrinter cfgPrinter;

        for (const auto& function : module.functions)
        {
            ControlFlowGraph cfg = cfgBuilder.build(function);
            cfgPrinter.print(function, cfg);
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}