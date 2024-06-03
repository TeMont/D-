#include "exprParser.hpp"

std::optional<node::ValExpr> expressionParser::parseValExpr(const std::string &expectedType, const bool &isRequired)
{
    if (parser::peek().has_value())
    {
        node::ValExpr valExpr;
        if (parser::peek().value().type == Tokens::INT_LITERAL)
        {
            if (expectedType != INT_TYPE && expectedType != BOOL_TYPE && expectedType != FLOAT_TYPE && expectedType != ANY_TYPE)
            {
                return {};
            }
            valExpr = {node::ExprIntLit{parser::consume()}};
        }
        else if (parser::peek().value().type == Tokens::QOUTE)
        {
            if (expectedType != STR_TYPE && expectedType != ANY_TYPE)
            {
                return {};
            }
            parser::consume(); // consume '"'
            valExpr = {node::ExprStrLit{parser::consume()}};
            parser::consume(); // consume '"'
        }
        else if (parser::peek().value().type == Tokens::APOST)
        {
            if (expectedType != CHAR_TYPE && expectedType != BOOL_TYPE && expectedType != ANY_TYPE)
            {
                return {};
            }
            parser::consume(); // consume '
            valExpr = {node::ExprCharLit{parser::consume()}};
            parser::consume(); // consume '
        }
        else if (parser::peek().value().type == Tokens::BOOL_LITERAL)
        {
            if (expectedType != BOOL_TYPE && expectedType != ANY_TYPE)
            {
                return {};
            }
            valExpr = {node::ExprBoolLit{parser::consume()}};
        }
        else if (parser::peek().value().type == Tokens::FLOAT_LITERAL)
        {
            if (expectedType != FLOAT_TYPE && expectedType != INT_TYPE && expectedType != BOOL_TYPE && expectedType != ANY_TYPE)
            {
                return {};
            }
            valExpr = {node::ExprFloatLit{parser::consume()}};
        }
        else if (parser::peek().value().type == Tokens::NOT) //FOR EXPRESSIONS LIKE !20, !x
        {
            parser::consume();
            if (expectedType != BOOL_TYPE && expectedType != ANY_TYPE)
            {
                return {};
            }
            if (auto nodeExpr = parseValExpr(ANY_TYPE))
            {
                valExpr = {node::NotCondition({new node::ValExpr(nodeExpr.value())})};
            }
            else
            {
                return {};
            }
        }
        else if (parser::peek().value().type == Tokens::INC || parser::peek().value().type == Tokens::DEC ||
            parser::peek().value().type == Tokens::IDENT && parser::peek(1).has_value() &&
            (parser::peek(1).value().type == INC || parser::peek(1).value().type == DEC))
        {
            if (auto const &incDecExpr = parseIncDec(); incDecExpr.has_value())
            {
                valExpr = {incDecExpr.value()};
            }
        }
        else if (parser::peek().value().type == Tokens::IDENT)
        {
            auto ident = parser::consume();
            valExpr = {node::ExprIdent{ident}};
        }
        else
        {
            if (isRequired)
            {
                std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected Expression";
                exit(EXIT_FAILURE);
            }
            return {};
        }
        return valExpr;
    }
    return {};
}

std::optional<node::Expr> expressionParser::parseExpr(const std::string &expectedType, const bool &isRequired, const uint8_t &minPriority)
{
    if (parser::peek().has_value() && parser::peek().value().type == Tokens::INPUT)
    {
        auto const &tmpInpStmt = parser::parseInputStmt().value();
        return node::Expr{new node::StmtInput(tmpInpStmt)};
    }
    const std::optional<node::ValExpr> &valFvl = parseValExpr(expectedType, isRequired);
    if (!valFvl.has_value())
    {
        if (isRequired)
        {
            std::cerr << "[Parse Error] ERR006 Value Doesn't Matches Type";
            exit(EXIT_FAILURE);
        }
        return {};
    }
    node::Expr exprFvl = {new node::ValExpr(valFvl.value())};
    while (true)
    {
        if (!parser::peek().has_value() || !op_to_prior(parser::peek().value().type).has_value() ||
            op_to_prior(parser::peek().value().type) < minPriority)
        {
            break;
        }

        std::optional<uint8_t> priority = op_to_prior(parser::peek().value().type);
        const Token &opr = parser::consume();
        const uint8_t &nextMinPriority = priority.value() + 1;
        std::optional<node::Expr> exprSvl = parseExpr(expectedType, isRequired, nextMinPriority);
        if (!exprSvl.has_value())
        {
            std::cerr << "[Parse Error] ERR006 Value Doesn't Matches Type";
            exit(EXIT_FAILURE);
        }
        exprFvl.var = new node::BinExpr({new node::Expr(exprFvl), new node::Expr(exprSvl.value()), opr.type});
    }
    return exprFvl;
}

std::optional<node::IncDec> expressionParser::parseIncDec()
{
    node::IncDec nodeIncDec = {};
    if (parser::peek().value().type == Tokens::INC || parser::peek().value().type == Tokens::DEC)
    {
        const Tokens &incDec = parser::consume().type;
        if (parser::peek().has_value() && parser::peek().value().type != Tokens::IDENT || !parser::peek().has_value())
        {
            std::cerr << "[Parse Error] ERR001 Syntax Error Expected Identifier";
            exit(EXIT_FAILURE);
        }
        nodeIncDec = {parser::consume(), (incDec == Tokens::INC), true};
    }
    else if (parser::peek().value().type == Tokens::IDENT)
    {
        auto const &ident = parser::consume();
        if (parser::peek().has_value() && parser::peek().value().type == INC ||
            parser::peek().has_value() && parser::peek().value().type == DEC)
        {
            nodeIncDec = {ident, (parser::peek().value().type == INC), false};
            parser::consume();
        }
    }
    return nodeIncDec;
}
