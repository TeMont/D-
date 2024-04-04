#include "lexer.hpp"

std::optional<Token> parser::peek(int offset) const
{
    if (m_index + offset >= m_tokens.size())
    {
        return {};
    }
    else
    {
        return m_tokens[m_index + offset];
    }
}
Token parser::consume()
{
    return m_tokens[m_index++];
}

std::optional<node::ValExpr> parser::parseValExpr(std::string ExpectedType)
{
    if (peek().has_value())
    {
        node::ValExpr valExpr;
        if (peek().value().type == Tokens::INT_LITERAL)
        {
            if (ExpectedType == INT_TYPE || ExpectedType == ANY_TYPE)
            {
                valExpr = {node::ExprIntLit{consume()}};
            }
            else
            {
                std::cerr << "ERR006 Value Doesn't Matches Type";
                exit(EXIT_FAILURE);
            }
        }
        else if (peek().value().type == Tokens::STRING_LITERAL)
        {
            if (ExpectedType == STR_TYPE || ExpectedType == ANY_TYPE)
            {
                valExpr = {node::ExprStrLit{consume()}};
            }
            else
            {
                std::cerr << "ERR006 Value Doesn't Matches Type";
                exit(EXIT_FAILURE);
            } 
        }
        else if (peek().value().type == Tokens::IDENT)
        {
            valExpr = {node::ExprIdent{consume()}};
        }
        else
        {
            return {};
        }
        return valExpr;
    }
    else
    {
        return {};
    }
}

std::optional<node::Expr> parser::parseExpr(std::string ExpectedType, uint8_t min_priority)
{
    std::optional<node::ValExpr> val_fvl = parseValExpr(ExpectedType);
    if (val_fvl.has_value())
    {
        node::Expr expr_fvl = {new node::ValExpr(val_fvl.value())};

        while (1)
        {
            std::optional<uint8_t> priority;
            if (peek().has_value())
            {
                priority = op_to_prior(peek().value().type);
                if (!priority.has_value() || priority < min_priority)
                {
                    break;
                }
            }
            else 
            {
                break;
            }
            Token opr = consume();
            uint8_t next_min_priority = priority.value() + 1;
            auto expr_svl = parseExpr(ExpectedType, next_min_priority);

            if (expr_svl.has_value())
            {
                node::BinExpr expr;
                node::Expr expr_fvl2;
                if (opr.type == Tokens::PLUS)
                {
                    node::BinExprAdd add;
                    expr_fvl2 = expr_fvl;
                    add.fvl = new node::Expr(expr_fvl2);
                    add.svl = new node::Expr(expr_svl.value());
                    expr.var = new node::BinExprAdd(add);
                }
                else if (opr.type == Tokens::MINUS)
                {
                    node::BinExprSub sub;
                    expr_fvl2 = expr_fvl;
                    sub.fvl = new node::Expr(expr_fvl2);
                    sub.svl = new node::Expr(expr_svl.value());
                    expr.var = new node::BinExprSub(sub);
                }
                else if (opr.type == Tokens::MULT)
                {
                    node::BinExprMul mul;
                    expr_fvl2 = expr_fvl;
                    mul.fvl = new node::Expr(expr_fvl2);
                    mul.svl = new node::Expr(expr_svl.value());
                    expr.var = new node::BinExprMul(mul);
                }
                else if (opr.type == Tokens::DIV)
                {
                    node::BinExprDiv div;
                    expr_fvl2 = expr_fvl;
                    div.fvl = new node::Expr(expr_fvl2);
                    div.svl = new node::Expr(expr_svl.value());
                    expr.var = new node::BinExprDiv(div);
                }
                expr_fvl.var = new node::BinExpr(expr);
            }   
            else 
            {
                std::cerr << "Expected Value After Operator";
                exit(EXIT_FAILURE);
            }
        }
        return expr_fvl;
    }
    else 
    {
        return {};
    }
}

std::optional<node::Stmt> parser::parseStmt()
{
    std::optional<node::Stmt> stmt_node;
    if (peek().has_value())
    {
        if (peek().value().type == Tokens::RETURN)
        {
            consume();
            if (auto node_expr = parseExpr(INT_TYPE))
            {
                stmt_node = {{node::StmtReturn{new node::Expr(node_expr.value())}}};

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
        }
        else if (peek().value().type == Tokens::INT_LET)
        {
            consume();
            if (peek().has_value() && peek().value().type == Tokens::IDENT)
            {
                auto var_ident = consume();
                if (peek().has_value() && peek().value().type == Tokens::EQUALS)
                {
                    consume();
                    if (auto node_expr = parseExpr(INT_TYPE))
                    {
                        stmt_node = {{node::StmtIntLet{var_ident, new node::Expr(node_expr.value())}}};
                        if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                        {
                            consume();
                        }
                        else
                        {
                            std::cerr << "ERR001 Invalid Syntax Expected ';'";
                            exit(EXIT_FAILURE);
                        }
                    }
                    else
                    {
                        std::cerr << "ERR001 Invalid Syntax Expected Expression";
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    stmt_node = {{node::StmtIntLet{var_ident}}};
                    if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                    {
                        consume();
                    }
                    else
                    {
                        std::cerr << "ERR001 Invalid Syntax Expected ';'";
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else
            {
                std::cerr << "ERR002 Expected An Identifier";
                exit(EXIT_FAILURE);
            }
        }
        else if (peek().value().type == Tokens::STRING_LET)
        {
            consume();
            if (peek().has_value() && peek().value().type == Tokens::IDENT)
            {
                auto var_ident = consume();
                if (peek().has_value() && peek().value().type == Tokens::EQUALS)
                {
                    consume();
                    if (peek().has_value() && peek().value().type == Tokens::QOUTE)
                    {
                        consume();
                        if (auto node_expr = parseExpr(STR_TYPE))
                        {
                            stmt_node = {{node::StmtStrLet{var_ident, new node::Expr(node_expr.value())}}};
                            if (peek().has_value() && peek().value().type == Tokens::QOUTE)
                            {
                                consume();
                                if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                                {
                                    consume();
                                }
                                else
                                {
                                    std::cerr << "ERR001 Invalid Syntax Expected ';'";
                                    exit(EXIT_FAILURE);
                                }
                            }
                            else
                            {
                                std::cerr << "ERR001 Invalid Syntax Expected '\"'";
                                exit(EXIT_FAILURE);
                            }
                        }
                        else
                        {
                            std::cerr << "ERR001 Invalid Syntax Expected Expression";
                            exit(EXIT_FAILURE);
                        }
                    }
                    else if (peek().has_value() && peek().value().type == Tokens::IDENT)
                    {
                        if (auto node_expr = parseExpr(STR_TYPE))
                        {
                            stmt_node = {{node::StmtStrLet{var_ident, new node::Expr(node_expr.value())}}};
                            if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                            {
                                consume();
                            }
                            else
                            {
                                std::cerr << "ERR001 Invalid Syntax Expected ';'";
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                    else
                    {
                        std::cerr << "ERR001 Invalid Syntax Expected '\"'";
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    stmt_node = {{node::StmtStrLet{var_ident}}};
                    if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                    {
                        consume();
                    }
                    else
                    {
                        std::cerr << "ERR001 Invalid Syntax Expected ';'";
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else
            {
                std::cerr << "ERR002 Expected An Identifier";
                exit(EXIT_FAILURE);
            }
        }

        else if (peek().value().type == Tokens::IDENT)
        {
            auto var_ident = consume();
            if (peek().value().type == Tokens::EQUALS)
            {
                consume();
                if (peek().has_value() && peek().value().type == Tokens::QOUTE)
                {
                    consume();
                    if (auto node_expr = parseExpr(STR_TYPE))
                    {
                        stmt_node = {{node::StmtStrVar{var_ident, new node::Expr(node_expr.value())}}};
                        if (peek().has_value() && peek().value().type == Tokens::QOUTE)
                        {
                            consume();
                            if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                            {
                                consume();
                            }
                            else
                            {
                                std::cerr << "ERR001 Invalid Syntax Expected ';'";
                                exit(EXIT_FAILURE);
                            }
                        }
                        else
                        {
                            std::cerr << "ERR001 Invalid Syntax Expected '\"'";
                            exit(EXIT_FAILURE);
                        }
                    }
                    else
                    {
                        std::cerr << "ERR001 Invalid Syntax Expected String Literal";
                        exit(EXIT_FAILURE);
                    }
                }
                else if (auto node_expr = parseExpr())
                {
                    stmt_node = {{node::StmtIntVar{var_ident, new node::Expr(node_expr.value())}}};
                    if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                    {
                        consume();
                    }
                    else
                    {
                        std::cerr << "ERR001 Invalid Syntax Expected ';'";
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else
            {
                std::cerr << "ERR001 Invalid Syntax Expected '='";
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            std::cerr << "ERR001 Syntax Error";
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        return {};
    }
    return stmt_node;
}

std::optional<node::Prog> parser::parseProg()
{
    node::Prog prog;
    while (peek().has_value())
    {
        if (auto stmt = parseStmt())
        {
            prog.statements.push_back(stmt.value());
        }
        else
        {
            std::cerr << "ERR003 Invalid Statement";
            exit(EXIT_FAILURE);
        }
    }
    return prog;
}