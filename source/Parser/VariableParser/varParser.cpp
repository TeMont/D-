#include "varParser.hpp"

std::unordered_map<std::string, std::string> varParser::m_vars;

std::optional<node::StmtLet> varParser::parseLet()
{
    bool isConst = false;
    Tokens letType;
    if (parser::peek().value().type == CONST)
    {
        isConst = true;
        parser::consume();
        letType = parser::consume().type;
    }
    else if (parser::peek(1).has_value() && parser::peek(1).value().type == CONST)
    {
        isConst = true;
        letType = parser::consume().type;
        parser::consume();
    }
    else
    {
        letType = parser::consume().type;
    }
    if (parser::peek().has_value() && parser::peek().value().type != IDENT || !parser::peek().has_value())
    {
        std::cerr << "[Parse Error] ERR002 Expected An Identifier";
        exit(EXIT_FAILURE);
    }
    auto const &varIdent = parser::consume();
    if (parser::peek().has_value() && parser::peek().value().type == EQ)
    {
        parser::consume();
        if (auto const &nodeExpr = expressionParser::parseExpr(letToType[letType]))
        {
            return node::StmtLet{varIdent, new node::Expr(nodeExpr.value()), isConst, letType};
        }
        std::cerr << "[Parse Error] ERR006 Value Doesn't Matches Type";
        exit(EXIT_FAILURE);
    }
    return node::StmtLet{varIdent, nullptr, isConst, letType};
}
