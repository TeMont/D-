#pragma once
#include "../parser.hpp"

class ExpressionParser
{
public:
	static std::optional<node::Expr> parseExpr(const std::string &expectedType, bool isRequired = true, uint8_t minPriority = 1);
	static std::optional<node::ValExpr> parseValExpr(const std::string &expectedType, bool isRequired = true);
	static std::optional<node::IncDec> parseIncDec();
};