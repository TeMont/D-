#pragma once
#include "../Token/token.hpp"

namespace node 
{
    struct Expr
    {
        Token int_lit;
    };

    struct RETURN
    {
        node::Expr Expr;
    };

}

class parser
{
public:
    explicit parser(std::vector<Token> tokens) 
    : m_tokens(std::move(tokens)) {}

    std::optional<node::Expr> parseExpr();

    std::optional<node::RETURN> parse();

private:

    std::optional<Token> peek(int pos = 1) const;

    Token consume();

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
};

