#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <thread>
#include <filesystem>
#include "../headers/filesys.hpp"
#include "Token/token.hpp"
#include "Parser/parser.hpp"
#include "Compile/compiler.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Incorrect Usage, Use XComp --help" << std::endl;
        return EXIT_FAILURE;
    }

    if (std::strcmp(argv[1], "--help") == 0)
    {
        std::cout << "Build:\n\tXComp <filename>.xy";
    }
    else
    {
        if (CheckFileExtension(argv[1], "xy"))
        {
            std::string source = ReadSource(argv[1]);
            tokenizer tokenize(std::move(source));
            std::vector<Token> tokenVec = tokenize.tokenize();

            parser parse(tokenVec);
            std::optional<node::Prog> prog = parser::parseProg();

            if (!prog.has_value())
            {
                std::cerr << "Invalid Program";
            }

            std::stringstream asmCode;

            compiler compile(prog.value());
            asmCode = compile.compile();


            std::string projName = argv[1];
            size_t dotPos = projName.find_last_of('.');
            if (dotPos != std::string::npos)
            {
                projName = projName.substr(0, dotPos);
            }

            std::ofstream assembly;
            assembly.open(projName + ".asm");
            assembly << asmCode.str();
            assembly.close();

            if (createObjectFile(projName))
            {
                if (linkObjectFiles(projName))
                {
                    system(("del " + projName + ".obj").c_str());
                    std::cout << argv[1] << " Was successfully compiled!\n";
                }
                else
                {
                    std::cerr << "Error Linking File";
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                std::cerr << "Error Creating Object File";
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            std::cerr << "Incorrect File Extension, Use XComp --help" << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
