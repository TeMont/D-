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
#define CHAR_TYPE "char"
#define FLOAT_TYPE "float"
#define ANY_TYPE "any"

enum Tokens
{
	INT_LITERAL,
	INT_LET,
	STRING_LITERAL,
	STRING_LET,
	BOOL_LITERAL,
	BOOL_LET,
	CHAR_LITERAL,
	CHAR_LET,
	FLOAT_LET,
	FLOAT_LITERAL,
	IDENT,
	EQ,
	PLUS,
	MINUS,
	MULT,
	DIV,
	PLUSEQ,
	MINUSEQ,
	MULTEQ,
	DIVEQ,
	LPAREN,
	RPAREN,
	SEMICOLON,
	QOUTE,
	APOST,
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
	FOR,
	WHILE,
	CONST,
	INC,
    DEC,
    CONTINUE,
    BREAK,
    SWITCH,
    CASE,
    DEFAULT,
    COLON,
};

extern std::map<std::string, Tokens> tokensMap;

struct Token
{
	Tokens type;
	std::optional<std::string> value;
};

class tokenizer
{
public:
	explicit tokenizer(std::string source) : m_src(std::move(source))
	{
	}

	std::vector<Token> tokenize();

private:

	[[nodiscard]] std::optional<char> peek(const int &offset = 0) const;

	char consume();

	const std::string m_src;
	size_t m_index = 0;
};

