#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <map>
#include <thread>
#include <chrono>
#include <filesystem>
#include "../headers/filesys.hpp"
#include "Token/token.hpp"
#include "Lexer/lexer.hpp"
#include "Compile/compile.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Incorrect Usage, Use XComp --help" << std::endl;
        return (EXIT_FAILURE);
    }
    else if (argc == 2)
    {
        if (std::strcmp(argv[1], "--help") == 0)
        {
            std::cout << "Build:\n\tXComp <filename>.xy <executableName>";
        }
        else
        {
            if (CheckFileExtension(argv[1], "xy"))
            {

                std::string source = ReadSource(argv[1]);
                tokenizer Tokenize(std::move(source));
                std::vector<Token> TokenVec = Tokenize.tokenize();

                parser parse(std::move(TokenVec));
                std::optional<node::Prog> prog = parse.parseProg();

                if (!prog.has_value())
                {
                    std::cerr << "Invalid Program";
                }

                std::stringstream AsmCode;

                compiler compile(prog.value());
                AsmCode = compile.compile();


                std::string projName = argv[1];
                size_t dotPos = projName.find_last_of('.');
                if (dotPos != std::string::npos)
                {
                    projName = projName.substr(0, dotPos);
                }


                std::ofstream asembly;
                asembly.open(projName + ".asm");
                asembly << AsmCode.str();
                asembly.close();

                if (CreateObjectFile(projName))
                {
                    if (LinkObjectFiles(projName))
                    {
                        system(("del " + projName + ".obj").c_str());
                        std::cout << argv[1] << " Was succesfully compiled!\n";
                        std::cout << "Code ended with: " << system((projName + ".exe").c_str());
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
    }
    else
    {
    }
    // std::cout << system("result.exe");

    return EXIT_SUCCESS;
}