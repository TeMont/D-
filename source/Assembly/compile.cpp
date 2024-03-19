#include "compile.hpp"

std::stringstream TokensToAsm(std::vector<Token> TokensVec)
{
    std::stringstream AsmCode;
    AsmCode << "global _start\nstart:\n";

    for (int i = 0; i < TokensVec.size(); ++i)
    {
        if (TokensVec[i].type == _return)
        {
            if (i + 1 < TokensVec.size() && !TokensVec[i+1].value.empty())
            {
                if (i + 2 < TokensVec.size() && TokensVec[i+2].type == semi)
                {
                    AsmCode << "\tmov rax, 60\n";
                    AsmCode << "\tmov rdi, " << TokensVec[i+1].value << '\n';
                    AsmCode << "\tsyscall\n";
                }
                else 
                {
                    std::cerr << "ERR001 syntax error: Expected ';'";
                    exit(EXIT_FAILURE);
                }
            }
            else 
            {
                std::cerr << "ERR001 syntax error: Expected value to return";
                exit(EXIT_FAILURE);
            }
        }
    }


    return AsmCode;
}