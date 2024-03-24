#include "lexer.hpp"

std::optional<Token> parser::peek(int ahead) const 
{
    if ((m_index + ahead) > m_tokens.size())
    {
        return {};
    }
    else 
    {
        return m_tokens[m_index];
    }
}

Token parser::consume()
{
    return m_tokens[m_index++];
}

std::optional<node::Expr> parser::parseExpr()
{
    if (peek().has_value() && peek().value().type == Tokens::INT_LITERAL)
    {
        node::Expr expr_node;
        expr_node.int_lit = consume();
        return expr_node;
    }
    else 
    {
        return {};
    }
}

std::optional<node::RETURN> parser::parse()
{
    std::optional<node::RETURN> return_node;
    while (peek().has_value())
    {
        if (peek().value().type == Tokens::RETURN)
        {
            consume();
            if (auto node_expr = parseExpr())
            {
                return_node = node::RETURN{ node_expr.value() };

                if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                {
                    consume();
                    
                }
                else 
                {
                    std::cerr << "ERR001 Invalid Syntax Expected ';'\n";
                    exit(EXIT_FAILURE);
                }
            }
            else 
            {
                std::cerr << "ERR002 Invalid Expresion\n";
                exit(EXIT_FAILURE);
            }
        }
    }
    return return_node;
}

