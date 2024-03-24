#include "token.hpp"

std::map<std::string, Tokens> TokensMap =
{
    {"return", RETURN},
    {"integer", INT_LITERAL},
    {"stdOut", OUTPUT},
    {"stdInp", INPUT},
    {";", SEMICOLON},
    {"string", STRING_LITERAL},
    {"\"", QOUTE},
    {"(", LBRACKET},
    {")", RBRACKET}
};

std::vector<Token> tokenizer::tokenize()
{
    std::vector<Token> tokens;
    std::string buffer;
    
    while (peek().has_value())
    {
        if (std::isalpha(peek().value()))
        {
            buffer.push_back(consume());
            while (peek().has_value() && std::isalnum(peek().value()))
            {
                buffer.push_back(consume());
            }

            auto it = TokensMap.find(buffer);
            if (it != TokensMap.end())
            {
                tokens.push_back({TokensMap[buffer]});
                buffer.clear();
                continue;
            }
            else
            {
                std::cerr << "ERR001 Syntax Error character: " << m_index;
                exit(EXIT_FAILURE);
            }
        }
        else if (std::isdigit(peek().value()))
        {
            buffer.push_back(consume());
            while (peek().has_value() && std::isdigit(peek().value()))
            {
                buffer.push_back(consume());  
            }
            tokens.push_back({TokensMap["integer"], buffer});
            buffer.clear();
            continue;
        }
        else if (std::isspace(peek().value()))
        {
            consume();
            continue;
        }
        else if (peek().value() == ';' || peek().value() == '(' || peek().value() == ')' || peek().value() == '\"')
        {
            buffer.push_back(consume());
            tokens.push_back({TokensMap[buffer]});
            buffer.clear();
            continue;
        }
    }

    return tokens;
}

std::optional<char> tokenizer::peek(int ahead) const 
{
    if (m_index + ahead> m_src.length())
    {
        return {};
    }
    else 
    {
        return m_src[m_index];
    }
}

char tokenizer::consume()
{
    return m_src[m_index++];
}