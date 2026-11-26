#include "frontend/lexer.hpp"
#include "frontend/parser.hpp"
#include "frontend/semantic.hpp"

#include "middleend/ir_generator.hpp"
#include "middleend/ir_verifier.hpp"
#include "middleend/constant_folding.hpp"
#include "middleend/constant_propagation.hpp"
#include "middleend/dead_code_elimination.hpp"

#include "backend/code_generator.hpp"

#include "driver.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>
#include <stdexcept>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: atomc <source.atom>\n";
        return 1;
    }

    std::ifstream file(argv[1]);

    if (!file)
    {
        std::cerr << "Cannot open file: " << argv[1] << '\n';
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string source = buffer.str();

    try
    {
        // Frontend

        Lexer lexer(source);
        const auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto program = parser.parse();

        SemanticAnalyzer semantic;
        semantic.analyze(*program);

        // Middleend

        IRGenerator generator;
        IRModule module = generator.generate(*program);

        IRVerifier verifier;
        verifier.verify(module);

        // Optimization

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

        // Backend

CompilerDriver driver;

OutputFiles files = driver.createOutputFiles(argv[1]);

std::ofstream assembly(files.assembly);

if (!assembly)
{
    throw std::runtime_error(
        "Cannot create assembly file");
}

    CodeGenerator codeGenerator;
    codeGenerator.generate(module, assembly);

    assembly.close();

    if (!assembly)
    {
        throw std::runtime_error(
            "Cannot write assembly file");
    }

    driver.assemble(files.assembly, files.object);
    driver.link(files.object, files.executable);
    driver.cleanup(files);

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}