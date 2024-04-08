#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <map>
#include "../../headers/filesys.hpp"

#define INT_TYPE "integer"
#define STR_TYPE "string"
#define BOOL_TYPE "bool"
#define ANY_TYPE "any"

enum Tokens
{
    INT_LITERAL,
    INT_LET,
    STRING_LITERAL,
    STRING_LET,
    BOOL_LITERAL,
    BOOL_LET,
    IDENT,
    EQUALS,
    PLUS,
    MINUS,
    MULT,
    DIV,
    LPAREN,
    RPAREN,
    RETURN,
    OUTPUT,
    INPUT,
    SEMICOLON,
    QOUTE,
    LBRACKET,
    RBRACKET,
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

