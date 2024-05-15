#include "token.hpp"

std::map<std::string, Tokens> tokensMap = //CONVERT STRING TO TOKEN
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
        {"stdInput", INPUT},
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
        {"++", INC},
        {"--", DEC},
        {"if", IF},
        {"elif", ELIF},
        {"else", ELSE},
        {"for", FOR},
        {"while", WHILE},
        {"const", CONST},
};

std::vector<Token> tokenizer::tokenize()
{
    std::vector<Token> tokens;
    std::string buffer;

    while (peek().has_value())
    {
		char ch = peek().value();
        if (std::isalpha(ch))
        {
            buffer.push_back(consume());
            while (peek().has_value() && std::isalnum(peek().value()))
            {
                buffer.push_back(consume());
            }

            auto it = tokensMap.find(buffer);
            if (it != tokensMap.end())
            {
                tokens.push_back({tokensMap[buffer]});
                buffer.clear();
            }
            else
            {
                if (buffer == "true" || buffer == "false")
                {
                    tokens.push_back({tokensMap["boolLit"], buffer});
                }
                else
                {
                    tokens.push_back({tokensMap["identifier"], buffer});
                }
                buffer.clear();
            }
        }
        else if (std::isdigit(ch))
        {
            buffer.push_back(consume());
            while (peek().has_value() && std::isdigit(peek().value()))
            {
                buffer.push_back(consume());
            }
            tokens.push_back({tokensMap["intLit"], buffer});
            buffer.clear();
        }
        else if (std::isspace(ch))
        {
            consume();
        }
        else if (ch == '"')//STRING LITERAL
        {
            buffer.push_back(consume());
            tokens.push_back({tokensMap[buffer]});
            buffer.clear();
            while (peek().has_value() && peek().value() != '"')
            {
                if (peek().value() == '\\')
                {
                    consume();
                    char symbol = consume();
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
                        buffer.push_back('\'');
                        buffer.push_back('\'');
                        break;
                    }

                    buffer.push_back(',');
                    buffer.push_back('\'');
                    continue;
                }
                buffer.push_back(consume());
            }
            if (peek().has_value() && peek().value() != '"' || !peek().has_value())
            {
	            std::cerr << "[Tokenize Error] ERR001 Syntax Error Expected '\"'";
	            exit(EXIT_FAILURE);
            }
            tokens.push_back({tokensMap["stringLit"], buffer});
            buffer.clear();
            buffer.push_back(consume()); // consume '"'
            tokens.push_back({tokensMap[buffer]});
            buffer.clear();
        }
        else if (ch == '\'')//CHAR LITERAL
        {
            buffer.push_back(consume());
            tokens.push_back({tokensMap[buffer]});
            buffer.clear();
            while (peek().has_value() && peek().value() != '\'')
            {
                buffer.push_back(consume());
            }
            if (peek().has_value() && peek().value() != '\'' || !peek().has_value())
            {
	            std::cerr << "[Tokenize Error] ERR001 Syntax Error Expected '\''";
	            exit(EXIT_FAILURE);
            }
            if (buffer.size() > 1)
            {
                std::cerr << "[Tokenize Error] ERR010 Char Literal Cannot Be More Than One Symbol";
                exit(EXIT_FAILURE);
            }
            tokens.push_back({tokensMap["charLit"], buffer});
            buffer.clear();
            buffer.push_back(consume()); // consume '''
            tokens.push_back({tokensMap[buffer]});
            buffer.clear();
        }
        else if (ch == '/' && peek(1).has_value() && peek(1).value() == '/') // ONE-LINE COMMENT
        {
            consume(); // consume '/'
            consume(); // consume '/'
            while (peek().has_value() && peek().value() != '\n')
            {
                consume();
            }
        }
        else if (ch == '/' && peek(1).has_value() && peek(1).value() == '*') // MULTI-LINE COMMENT
        {
            consume(); // consume '/'
            consume(); // consume '*'
            while (!(peek().has_value() && peek().value() == '*' && peek(1).has_value() && peek(1).value() == '/'))
            {
                consume();
            }
            if (!peek().has_value() || !peek(1).has_value())
            {
	            std::cerr << "[Tokenize Error] Expected end of multi-line comment";
	            exit(EXIT_FAILURE);
            }
			consume(); // consume '*'
			consume(); // consume '/'
        }
        else
        {
            buffer.push_back(consume());
            auto it = tokensMap.find(buffer);
			if (buffer == "+" || buffer == "-")
			{
				if (std::isdigit(peek().value()))
				{
					buffer.push_back(consume());
					while (peek().has_value() && std::isdigit(peek().value()))
					{
						buffer.push_back(consume());
					}
					tokens.push_back({tokensMap["intLit"], buffer});
					buffer.clear();
					continue;
				}
				else if (peek() == buffer[0])
				{
					buffer.push_back(consume());
				}
			}
            if (it != tokensMap.end())
            {
                if (peek().has_value())
                {
                    if (peek().value() == '=')// USED FOR TOKENS LIKE != <= >=
                    {
                        buffer.push_back(consume());
                    }
                }
                auto it2 = tokensMap.find(buffer);
                if (it2 == tokensMap.end())
                {
                    buffer.pop_back();//IF IS NOT IN TOKENS MAP POP '=' SYMBOL
                }
                tokens.push_back({tokensMap[buffer]});
                buffer.clear();
            }
            else if (!peek().has_value())
            {
                std::cerr << "[Tokenize Error] ERR001 Syntax Error Unexpected '" << buffer << "'";
                exit(EXIT_FAILURE);
            }
        }
    }
    return tokens;
}

[[nodiscard]] std::optional<char> tokenizer::peek(int offset) const
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