#pragma once
#include "../compiler.hpp"

class expressionCompiler
{
public:
    static bool compExpr(const node::Expr &expr, const std::string &expectedType, const bool &isConvertable = true);
    static bool compBinExpr(const node::BinExpr &expr, const std::string &expectedType, const bool &isConvertable);
    static void compBoolExpr(const std::optional<std::string> &literal, const bool &isReversed = false);
    static void compBoolExprFloat32(const std::optional<std::string> &literal, const bool &isReversed = false, const bool &isRegister = false);
    static bool compValExpr(const node::ValExpr &expr, const std::string &expectedType, const bool &isConvertable);
    static void compIncDec(const Token &ident, const bool &isInc, const std::string &expectedType);

private:
    static void intToFloat(const std::string &reg);
    static void floatToInt(const std::string &reg);
};
