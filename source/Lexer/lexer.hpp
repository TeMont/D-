#pragma once
#include "../Token/token.hpp"
#include <variant>


namespace node 
{
    struct ExprIntLit
    {
        Token int_lit;
    };

    struct ExprStrLit
    {
        Token str_lit;
    };
    
    struct ExprIdent
    {
        Token ident;
    };
    
    struct Expr
    {
        std::variant<ExprIntLit, ExprStrLit, ExprIdent> var;
    };

    struct StmtReturn
    {
        node::Expr Expr;
    };

    struct StmtIntLet
    {
        Token ident;
        Expr expr;
    };
    
    struct StmtStrLet
    {
        Token ident;
        Expr expr;
    };

    struct StmtIntVar
    {
        Token ident;
        Expr expr;
    };
    
    struct StmtStrVar
    {
        Token ident;
        Expr expr;
    };

    struct Stmt
    {
        std::variant<StmtReturn, StmtIntLet, StmtStrLet, StmtStrVar, StmtIntVar> var; 
    };
    
    struct Prog
    {
        std::vector<Stmt> statements;
    };
}

class parser
{
public:
    explicit parser(std::vector<Token> tokens) 
    : m_tokens(std::move(tokens)) {}

    std::optional<node::Expr> parseExpr(std::string ExpectedType);
    std::optional<node::Stmt> parseStmt();

    std::optional<node::Prog> parseProg();

private:

    std::optional<Token> peek(int offset = 0) const;

    Token consume();

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
};

