#include "token.hpp"

std::map<std::string, Tokens> TokensMap =
{
    {"return", RETURN},
    {"intLit", INT_LITERAL},
    {"stdOut", OUTPUT},
    {"stdInp", INPUT},
    {";", SEMICOLON},
    {"stringLit", STRING_LITERAL},
    {"\"", QOUTE},
    {"{", LBRACKET},
    {"}", RBRACKET},
    {"(", LPAREN},
    {")", RPAREN},
    {"identifier", IDENT},
    {"int", INT_LET},
    {"string", STRING_LET},
    {"=", EQUALS},
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
                tokens.push_back({TokensMap["identifier"], buffer});
                buffer.clear();
                continue;
            }
        }
        else if (std::isdigit(peek().value()))
        {
            buffer.push_back(consume());
            while (peek().has_value() && std::isdigit(peek().value()))
            {
                buffer.push_back(consume());  
            }
            tokens.push_back({TokensMap["intLit"], buffer});
            buffer.clear();
            continue;
        }
        else if (std::isspace(peek().value()))
        {
            consume();
            continue;
        }
        else if (peek().value() == '"')
        {
            buffer.push_back(consume());
            tokens.push_back({TokensMap[buffer]});
            buffer.clear();
            while (peek().has_value() && peek().value() != '"')
            {
                buffer.push_back(consume());
            }
            tokens.push_back({TokensMap["stringLit"], buffer});
            buffer.clear();
            buffer.push_back(consume());
            tokens.push_back({TokensMap[buffer]});
            buffer.clear();
            
            continue;
        }
        else if (peek().value() == ';' || peek().value() == '(' || peek().value() == ')' || peek().value() == '=')
        {
            buffer.push_back(consume());
            tokens.push_back({TokensMap[buffer]});
            buffer.clear();
            continue;
        }
        else 
        {
            std::cerr << "ERR001 Syntax Error";
            exit(EXIT_FAILURE);
        }
    }

    return tokens;
}

std::optional<char> tokenizer::peek(int offset) const 
{
    if (m_index + offset >= m_src.length())
    {
        return {};
    }
    else 
    {
        return m_src[m_index+offset];
    }
}

char tokenizer::consume()
{
    return m_src[m_index++];
}