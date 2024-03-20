#include "compile.hpp"

std::stringstream TokensToAsm(std::vector<Token> TokensVec)
{
    std::stringstream AsmCode;
    AsmCode << "global _start\n_start:\n";

    for (int i = 0; i < TokensVec.size(); ++i)
    {
        if (TokensVec[i].type == RETURN)
        {
            if (i + 1 < TokensVec.size() && !TokensVec[i+1].value.empty())
            {
                if (i + 2 < TokensVec.size() && TokensVec[i+2].type == SEMICOLON)
                {
                    AsmCode << "\tmov rax, " << TokensVec[i+1].value << '\n';
                    AsmCode << "\tret\n";
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
        else if (TokensVec[i].type == OUTPUT)
        {
            if (i + 1 < TokensVec.size() && TokensVec[i+1].type == LBRACKET)
            {
                if (i + 2 < TokensVec.size() && TokensVec[i+2].type == QOUTE)
                {
                    if (i + 3 < TokensVec.size() && TokensVec[i+3].type == STRING_LITERAL)
                    {
                        if (i + 4 < TokensVec.size() && TokensVec[i+4].type == QOUTE)
                        {
                            if (i + 5 < TokensVec.size() && TokensVec[i+5].type == RBRACKET)
                            {
                                AsmCode << "section .data\n";
                                AsmCode << "msg db '" << TokensVec[i+3].value << "', 0\n";
                                AsmCode << "len equ $ - msg\n";
                            }
                            else 
                            {
                                std::cerr << "ERR001 syntax error: Expected ')'";
                                exit(EXIT_FAILURE);
                            }
                        }
                        else 
                        {
                            std::cerr << "ERR001 syntax error: Expected '\"'";
                            exit(EXIT_FAILURE);
                        }
                    }
                    else 
                    {
                        std::cerr << "ERR001 syntax error: Expected String";
                        exit(EXIT_FAILURE);
                    }
                }
                else 
                {
                    std::cerr << "ERR001 syntax error: Expected '\"'";
                    exit(EXIT_FAILURE);
                }
            }
            else 
            {
                std::cerr << "ERR001 syntax error: Expected '('";
                exit(EXIT_FAILURE);
            }
        }
    }

    return AsmCode;
}

bool CreateObjectFile(std::string Path)
{

    int res = system(("nasm -f win64 " + Path).c_str());
    if (res)
    {
        std::cerr << "Error Creating Object File";
        exit(EXIT_FAILURE);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 1;
}

bool LinkObjectFiles(std::string Path)
{

    int res = system(("golink /entry:_start /console kernel32.dll user32.dll " + Path).c_str());
    if (res)
    {
        std::cerr << "Error Linking File";
        exit(EXIT_FAILURE);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 1;
}