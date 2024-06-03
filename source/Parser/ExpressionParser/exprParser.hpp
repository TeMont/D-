#pragma once
#include "../parser.hpp"

class expressionParser
{
public:
    static std::optional<node::Expr> parseExpr(const std::string &expectedType, const bool &isRequired = true,
                                               const uint8_t &minPriority = 1);
    static std::optional<node::ValExpr> parseValExpr(const std::string &expectedType, const bool &isRequired = true);
    static std::optional<node::IncDec> parseIncDec();
};
