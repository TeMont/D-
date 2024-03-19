#pragma once
#include <iostream>
#include <optional>
#include <vector>
#include <map>
#include "../../headers/filesys.hpp"

enum Tokens
{
    _return,
    int_lit,
    output,
    input,
    semi
};

extern std::map<std::string, Tokens> TokensMap;

struct Token
{
    Tokens type;
    std::string value;
};

std::vector<Token> getTokens(std::string str);
