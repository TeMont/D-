#include <iostream>
#include <optional>
#include <vector>
#include <map>
#include "../headers/filesys.hpp"
#include "Token/token.hpp"
#include "Assembly/compile.hpp"


int main(int argc, char *argv[]) 
{
    if (argc < 2)
    {
        std::cerr << "Incorrect Usage, Use XComp --help" << std::endl;
        return EXIT_FAILURE;
    }
    else if (argc == 2)
    {
        if (std::strcmp(argv[1], "--help") == 0)
        {
            std::cout << "Build:\n\tXComp <filename>.xy";
        }
        else
        {
            if (CheckFileExtension(argv[1], "xy"))
            {
                std::string entireFile = ReadEntireFile(argv[1]);     
                std::vector<Token> TokenVec = getTokens(entireFile);    
                for (int i = 0; i < TokenVec.size(); ++i)
                {
                    std::cout << "Index: " << TokenVec[i].type << "\tValue: " << TokenVec[i].value << std::endl;
                }
                std::stringstream AsmCode = TokensToAsm(TokenVec);
                std::ofstream asembly;
                asembly.open("res.asm");
                asembly << AsmCode.str();
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
    return EXIT_SUCCESS;
}