#pragma once
#include <iostream>
#include <optional>
#pragma once
#include <vector>
#include <map>
#include "../../headers/filesys.hpp"

enum Tokens
{
    RETURN,
    INT_LITERAL,
    OUTPUT,
    INPUT,
    SEMICOLON,
    STRING_LITERAL,
    QOUTE,
    LBRACKET,
    RBRACKET
};

extern std::map<std::string, Tokens> TokensMap;

struct Token
{
    Tokens type;
    std::string value;
    
};
