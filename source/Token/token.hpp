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
/*dasdsadasddas*/
enum Tokens
{
    INT_LITERAL,
    INT_LET,
    STRING_LITERAL,
    STRING_LET,
    BOOL_LITERAL,
    BOOL_LET,
    IDENT,
    EQ,
    PLUS,
    MINUS,
    MULT,
    DIV,
    LPAREN,
    RPAREN,
    SEMICOLON,
    QOUTE,
    LBRACKET,
    RBRACKET,
    RETURN,
    OUTPUT,
    INPUT,
    EQEQ,
    LESS,
    GREATER,
    LESSEQ,
    GREATEQ,
    NOT,
    NOTEQ,
    OR,
    AND,
    IF,
    ELIF,
    ELSE,
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
    inline tokenizer(const std::string source) : m_src(std::move(source)){}
    
    std::vector<Token> tokenize();

private:

    std::optional<char> peek(int offset = 0) const;

    char consume();

    const std::string m_src;   
    size_t m_index = 0;   
};

