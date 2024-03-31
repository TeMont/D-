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
    struct ValExpr
    {
        std::variant<ExprIntLit, ExprStrLit, ExprIdent> var;
    };

    struct BinExprAdd
    {
        ValExpr fvl;
        ValExpr svl;
    };
    struct BinExprSub
    {
        ValExpr fvl;
        ValExpr svl;
    };
    struct BinExprMul
    {
        ValExpr fvl;
        ValExpr svl;
    };
    struct BinExprDiv
    {
        ValExpr fvl;
        ValExpr svl;
    };
    struct BinExpr
    {
        std::variant<BinExprAdd, BinExprSub, BinExprMul, BinExprDiv> var;
    };
    struct Expr
    {
        std::variant<ValExpr, BinExpr> var;
    };
    
    struct StmtReturn
    {
        Expr Expr;
    };

    struct StmtIntLet
    {
        Token ident;
        Expr Expr;
    };
    
    struct StmtStrLet
    {
        Token ident;
        Expr Expr;
    };

    struct StmtIntVar
    {
        Token ident;
        Expr Expr;
    };
    
    struct StmtStrVar
    {
        Token ident;
        Expr Expr;
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

    std::optional<node::BinExpr> parseBinExpr(std::string ExpectedType);

    std::optional<node::ValExpr> parseValExpr(std::string ExpectedType);

    std::optional<node::Stmt> parseStmt();

    std::optional<node::Prog> parseProg();

private:

    std::optional<Token> peek(int offset = 0) const;

    Token consume();

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
};

