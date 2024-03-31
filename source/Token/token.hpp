#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <map>
#include "../../headers/filesys.hpp"

#define INT_TYPE "integer"
#define STR_TYPE "string"
#define ANY_TYPE "any"

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
    RBRACKET,
    LPAREN,
    RPAREN,
    IDENT,
    INT_LET,
    STRING_LET,
    EQUALS,
    PLUS,
    MINUS,
    MULT,
    DIV,
};

extern std::map<std::string, Tokens> TokensMap;

struct Token
{
    Tokens type;
    std::optional<std::string> value;
};

class tokenizer
{
public:
    inline tokenizer(std::string source) : m_src(std::move(source)){}

    std::vector<Token> tokenize();

private:

    std::optional<char> peek(int offset = 0) const;

    char consume();

    const std::string m_src;   
    size_t m_index = 0;   
};

