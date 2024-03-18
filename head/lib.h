#pragma once
#include <iostream>
#include <optional>
#include <vector>
#include <map>
#include "../head/filesys.h"

enum Tokens
{
    _return,
    int_lit,
    output,
    input,
    semi
};

std::map<std::string, Tokens> TokensMap =
{
    {"return", _return},
    {"integer", int_lit},
    {"stdOut", output},
    {"stdInp", input},
    {";", semi}
};

struct Token
{
    Tokens type;
    std::string value;
};

std::vector<Token> getTokens(std::string str)
{
    std::vector<Token> resVec;
    std::string buffer;
    char ch;
    for (int i = 0; i < str.length(); ++i)
    {
        ch = str[i];
        if (std::isalpha(ch))
        {
            buffer.push_back(ch);
        }
        else if (std::isdigit(ch) && buffer.empty())
        {
            buffer.push_back(ch);
            ++i;
            while (std::isdigit(str[i]))
            {
                ch = str[i];
                buffer.push_back(ch);
                ++i;
            }
            --i;
            resVec.push_back({TokensMap["integer"], buffer});
            //std::cout << buffer << std::endl;
            buffer.clear();
        }
        else
        {
            if (isspace(ch))
            {
                if (!buffer.empty())
                {
                    auto it = TokensMap.find(buffer);
                    if (it != TokensMap.end())
                    {
                        resVec.push_back({TokensMap[buffer]});
                        //std::cout << buffer << std::endl;
                        buffer.clear();
                    }
                    else
                    {
                        std::cerr << "ERR001 Syntax Error character: " << i;
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else if (ch == ';')
            {
                if (!buffer.empty())
                {
                    resVec.push_back({TokensMap[buffer]});
                    //std::cout << buffer << std::endl;
                    buffer.clear();
                }
                buffer.push_back(';');
                resVec.push_back({TokensMap[buffer]});
                //std::cout << buffer << std::endl;
                buffer.clear();
            }
            else
            {
                std::cerr << "ERR001 Syntax Error character: " << i;
                exit(EXIT_FAILURE);
            }
        }
        //std::cout << "BUF: " << buffer << std::endl;
    }

    if (isalpha(str[str.length()-1]))
    {
        auto it = TokensMap.find(buffer);
        if (it != TokensMap.end())
        {
            resVec.push_back({TokensMap[buffer]});
            //std::cout << buffer << std::endl;
            buffer.clear();
        }
        else
        {
            std::cerr << "ERR001 Syntax Error character: " << str.length();
            exit(EXIT_FAILURE);
        }
    }
    return resVec;
}