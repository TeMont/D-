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
        {"char", CHAR_LET},
        {"charLit", CHAR_LITERAL},
        {"stdOut", OUTPUT},
        {"stdInp", INPUT},
        {";", SEMICOLON},
        {"\"", QOUTE},
        {"\'", APOST},
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
                if (peek().value() == '\\')
                {
                    consume();
                    auto symbol = consume();
                    buffer.push_back('\'');
                    buffer.push_back(',');
                    switch (symbol)
                    {
                    case 'n':
                        buffer.push_back('1');
                        buffer.push_back('0');
                        break;
                    case 't':
                        buffer.push_back('9');
                        break;
                    case 'r':
                        buffer.push_back('1');
                        buffer.push_back('3');
                        break;
                    case 'b':
                        buffer.push_back('8');
                        break;
                    case 'f':
                        buffer.push_back('1');
                        buffer.push_back('2');
                        break;
                    case '\'':
                        buffer.push_back('3');
                        buffer.push_back('9');
                        break;
                    case '\"':
                        buffer.push_back('3');
                        buffer.push_back('4');
                        break;
                    case '\\':
                        buffer.push_back('9');
                        buffer.push_back('2');
                        break;
                    case '0':
                        buffer.push_back('0');
                        break;
                    case 'v':
                        buffer.push_back('1');
                        buffer.push_back('1');
                        break;
                    case 'a':
                        buffer.push_back('7');
                        break;
                    case 'e':
                        buffer.push_back('2');
                        buffer.push_back('7');
                        break;
                    default:
                        break;
                    }
                    buffer.push_back(',');
                    buffer.push_back('\'');
                    continue;
                }
                buffer.push_back(consume());
            }

            if (peek().has_value() && peek().value() == '"')
            {
                tokens.push_back({TokensMap["stringLit"], buffer});
                buffer.clear();

                buffer.push_back(consume()); // consume '"'
                tokens.push_back({TokensMap[buffer]});
                buffer.clear();
            }
            else
            {
                std::cerr << "[Tokenize Error] ERR001 Syntax Error Expected '\"'";
                exit(EXIT_FAILURE);
            }

            continue;
        }
        else if (peek().value() == '\'')
        {
            buffer.push_back(consume());
            tokens.push_back({TokensMap[buffer]});
            buffer.clear();

            while (peek().has_value() && peek().value() != '\'')
            {
                buffer.push_back(consume());
            }

            if (peek().has_value() && peek().value() == '\'')
            {
                if (buffer.size() > 1)
                {
                    std::cerr << "[Tokenize Error] ERR010 Char Literal Cannot Be More Than One Symbol";
                    exit(EXIT_FAILURE);
                }
                tokens.push_back({TokensMap["charLit"], buffer});
                buffer.clear();

                buffer.push_back(consume()); // consume '''
                tokens.push_back({TokensMap[buffer]});
                buffer.clear();
            }
            else
            {
                std::cerr << "[Tokenize Error] ERR001 Syntax Error Expected '\''";
                exit(EXIT_FAILURE);
            }
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
                std::cerr << "[Tokenize Error] Expected end of multi-line comment";
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
            else if (!peek().has_value())
            {
                std::cerr << "ERR001 Syntax Error Unexpected '" << buffer << "'";
                exit(EXIT_FAILURE);
            }
            else
            {
                continue;
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