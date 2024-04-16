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

            if (peek().has_value() && peek().value() == '\n')
            {
                tokens.push_back({TokensMap["stringLit"], buffer});
                buffer.clear();

                buffer.push_back(consume()); // consume '"'
                tokens.push_back({TokensMap[buffer]});
                buffer.clear();
            }
            else
            {
                std::cerr << "ERR001 Syntax Error Expected '\"'";
                exit(EXIT_FAILURE);
            }

            continue;
        }
        else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '/') // ONE-LINE COMMENT
        {
            consume(); // consume '/'
            consume(); // consume '/'
            while (peek().has_value() && peek().value() != '\n')
            {
                consume();
            }
            continue;
        }
        else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '*') // MULTI-LINE COMMENT
        {
            consume(); // consume '/'
            consume(); // consume '*'
            while (peek().has_value() && peek().value() != '*' && peek(1).value() != '/')
            {
                consume();
            }

            if (peek().has_value() && peek(1).has_value())
            {
                consume(); // consume '*'
                consume(); // consume '/'
            }
            else
            {
                std::cerr << "Expected end of multi-line comment";
                exit(EXIT_FAILURE);
            }
            continue;
        }
        else
        {
            buffer.push_back(consume());
            auto it = TokensMap.find(buffer);
            if (it != TokensMap.end())
            {
                if (peek().has_value())
                {
                    if (peek().value() == '=')
                    {
                        buffer.push_back(consume());
                    }
                }
                auto it2 = TokensMap.find(buffer);
                if (it2 == TokensMap.end())
                {
                    buffer.pop_back();
                }
                tokens.push_back({TokensMap[buffer]});
                buffer.clear();
                continue;
            }
            else
            {
                std::cerr << "ERR001 Syntax Error Unexpected '" << buffer << "'";
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