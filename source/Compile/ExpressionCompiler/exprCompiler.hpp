#pragma once
#include "../compiler.hpp"

class expressionCompiler
{
public:
	static bool compExpr(const node::Expr &expr, const std::string &expectedType);
	static bool compBinExpr(const node::BinExpr &expr, const std::string &expectedType);
	static void compBoolExpr(const std::optional<std::string> &literal, bool isReversed = false);
	static bool compValExpr(const node::ValExpr &expr, const std::string &expectedType);
	static void compIncDec(const Token &ident, bool isInc, const std::string &expectedType);
};
