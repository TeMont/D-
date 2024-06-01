#pragma once
#include "../compiler.hpp"

class expressionCompiler
{
public:
    static bool compExpr(const node::Expr &expr, const std::string &expectedType, bool isConvertable = true);
    static bool compBinExpr(const node::BinExpr &expr, const std::string &expectedType, bool isConvertable);
    static void compBoolExpr(const std::optional<std::string> &literal, bool isReversed = false);
    static void compBoolExprFloat32(const std::optional<std::string> &literal, bool isReversed = false, bool isRegister = false);
    static bool compValExpr(const node::ValExpr &expr, const std::string &expectedType, bool isConvertable);
    static void compIncDec(const Token &ident, bool isInc, const std::string &expectedType);

private:
    static void intToFloat(const std::string &reg);
    static void floatToInt(const std::string &reg);
};
