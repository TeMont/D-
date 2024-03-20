#include "lexer.hpp"


std::vector<Token> tokenize(std::string str) 
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
            buffer.clear();
        }
        else
        {
            if (!buffer.empty())
            {
                resVec.push_back({TokensMap[buffer]});
                buffer.clear();
            }
            if (isspace(ch))
            {
                if (!buffer.empty())
                {
                    auto it = TokensMap.find(buffer);
                    if (it != TokensMap.end())
                    {
                        resVec.push_back({TokensMap[buffer]});
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
                buffer.push_back(';');
                resVec.push_back({TokensMap[buffer]});
                buffer.clear();
            }
            else if (ch == '(')
            {
                buffer.push_back('(');
                resVec.push_back({TokensMap[buffer]});
                buffer.clear();
            }
            else if (ch == ')')
            {
                buffer.push_back(')');
                resVec.push_back({TokensMap[buffer]});
                buffer.clear();
            }
            else if (ch == '\"')
            {
                buffer.push_back('\"');
                resVec.push_back({TokensMap[buffer]});
                buffer.clear();
                ++i;
                ch = str[i];
                while (ch != '\"')
                {
                    buffer.push_back(ch);
                    ++i;
                    ch = str[i];
                }
                resVec.push_back({TokensMap["string"], buffer});
                buffer.clear();
            }
            else
            {
                std::cerr << "ERR001 Syntax Error character: " << i;
                exit(EXIT_FAILURE);
            }
        }
    }

    if (isalpha(str[str.length()-1]))
    {
        auto it = TokensMap.find(buffer);
        if (it != TokensMap.end())
        {
            resVec.push_back({TokensMap[buffer]});
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
