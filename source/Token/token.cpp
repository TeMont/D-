#include "token.hpp"

std::map<std::string, Tokens> TokensMap =
    {
        {"return", RETURN},
        {"int", INT_LET},
        {"intLit", INT_LITERAL},
        {"string", STRING_LET},
        {"stringLit", STRING_LITERAL},
        {"bool", BOOL_LET},
        {"boolLit", BOOL_LITERAL},
        {"stdOut", OUTPUT},
        {"stdInp", INPUT},
        {";", SEMICOLON},
        {"\"", QOUTE},
        {"{", LBRACKET},
        {"}", RBRACKET},
        {"(", LPAREN},
        {")", RPAREN},
        {"identifier", IDENT},
        {"=", EQ},
        {"+", PLUS},
        {"-", MINUS},
        {"*", MULT},
        {"/", DIV},
        {"==", EQEQ},
        {"<", LESS},
        {">", GREATER},
        {"<=", LESSEQ},
        {">=", GREATEQ},
        {"!", NOT},
        {"!=", NOTEQ},
        {"||", OR},
        {"&&", AND},
        {"if", IF},
        {"elif", ELIF},
        {"else", ELSE},
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
                if (buffer == "true" || buffer == "false")
                {
                    tokens.push_back({TokensMap["boolLit"], buffer});
                }
                else
                {
                    tokens.push_back({TokensMap["identifier"], buffer});
                }
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
        else
        {
            buffer.push_back(consume());
            if ((buffer == "&" && peek().has_value() && peek().value() == '&') || (buffer == "|" && peek().has_value() && peek().value() == '|'))
            {
                buffer.push_back(consume());
            }
            auto it = TokensMap.find(buffer);
            if (it != TokensMap.end())
            {
                if (TokensMap[buffer] == Tokens::EQ || TokensMap[buffer] == Tokens::LESS || TokensMap[buffer] == Tokens::GREATER || TokensMap[buffer] == Tokens::NOT)
                {
                    if (peek().has_value())
                    {
                        std::string tempBuffer;
                        tempBuffer.push_back(peek().value());
                        if (tempBuffer == "=")
                        {
                            buffer.push_back(consume());
                        }
                    }
                }
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
        return m_src[m_index + offset];
    }
}

char tokenizer::consume()
{
    return m_src[m_index++];
}