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