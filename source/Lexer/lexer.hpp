#pragma once
#include "../Token/token.hpp"
#include <variant>


namespace node 
{
    struct Expr;
    struct ExprIntLit
    {
        Token int_lit;
    };
    struct ExprStrLit
    {
        Token str_lit;
    };
    struct ExprBoolLit
    {
        Token bool_lit;
    };
    struct ExprIdent
    {
        Token ident;
    };
    struct ValExpr
    {
        std::variant<ExprIntLit, ExprStrLit, ExprBoolLit, ExprIdent> var;
    };

    struct BinExprAdd
    {
        Expr* fvl;
        Expr* svl;
    };
    struct BinExprSub
    {
        Expr* fvl;
        Expr* svl;
    };
    struct BinExprMul
    {
        Expr* fvl;
        Expr* svl;
    };
    struct BinExprDiv
    {
        Expr* fvl;
        Expr* svl;
    };
    struct BinExpr
    {
        std::variant<BinExprAdd*, BinExprSub*, BinExprMul*, BinExprDiv*> var;
    };
    struct Expr
    {
        std::variant<ValExpr*, BinExpr*> var;
    };
    
    struct StmtReturn
    {
        Expr* Expr;
    };

    struct StmtIntLet
    {
        Token ident;
        Expr* Expr;
    };
    
    struct StmtStrLet
    {
        Token ident;
        Expr* Expr;
    };

    struct StmtBoolLet
    {
        Token ident;
        Expr* Expr;
    };

    struct StmtIntVar
    {
        Token ident;
        Expr* Expr;
    };
    
    struct StmtStrVar
    {
        Token ident;
        Expr* Expr;
    };

    struct StmtBoolVar
    {
        Token ident;
        Expr* Expr;
    };

    struct Stmt
    {
        std::variant<StmtReturn, StmtIntLet, StmtStrLet, StmtBoolLet, StmtStrVar, StmtIntVar, StmtBoolVar> var; 
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

    std::optional<node::Expr> parseExpr(std::string ExpectedType = ANY_TYPE, uint8_t min_priority = 1);

    std::optional<node::BinExpr> parseBinExpr(std::string ExpectedType);

    std::optional<node::ValExpr> parseValExpr(std::string ExpectedType);

    std::optional<node::Stmt> parseStmt();

    std::optional<node::Prog> parseProg();


private:

    std::optional<Token> peek(int offset = 0) const;

    Token consume();

    std::optional<uint8_t> op_to_prior(Tokens op)
    {
        switch (op)
        {
        case Tokens::PLUS:
        case Tokens::MINUS:
            return 1;
            break;

        case Tokens::MULT:
        case Tokens::DIV:
            return 2;
            break;
        
        default:
            return {};
        }
    }

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
};

