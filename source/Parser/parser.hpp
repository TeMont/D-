#pragma once

#include "parseNode/node.hpp"
#include "../Token/TokenUtils/tokenUtils.hpp"
#include "ExpressionParser/exprParser.hpp"
#include "VariableParser/varParser.hpp"
#include "ScopesParser/scopeParser.hpp"

class parser
{
public:
	inline explicit parser(const std::vector<Token> &tokens)
	{
		m_tokens = tokens;
	}

	static std::optional<node::StmtInput> parseInputStmt();
	static std::optional<node::Stmt> parseStmt(bool expectSemi = true);
	static std::optional<node::Prog> parseProg();

#ifdef TEST
	void setTokens(std::vector<Token> tokens)
	{
		m_tokens = std::move(tokens);
	}
	void pushToken(const Token& token)
	{
		m_tokens.push_back(token);
	}
#endif

	[[nodiscard]] static std::optional<Token> peek(int offset = 0);
	static Token consume();
	static void tryConsume(char charToConsume);
	static std::vector<Token> m_tokens;
	static size_t m_index;
	static node::Prog prog;
};

