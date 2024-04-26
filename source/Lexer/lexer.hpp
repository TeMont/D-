#pragma once
#include "../Token/token.hpp"
#include <variant>


namespace node 
{
    struct Expr;
    struct Stmt;
    struct StmtInput;
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
    struct ExprCharLit
    {
        Token char_lit;
    };
    struct ExprIdent
    {
        Token ident;
    };
    struct ValExpr
    {
        std::variant<ExprIntLit, ExprStrLit, ExprBoolLit, ExprCharLit, ExprIdent> var;
    };

    struct BinExprAdd
    {
        Expr* fvl = nullptr;
        Expr* svl = nullptr;
    };
    struct BinExprSub
    {
        Expr* fvl = nullptr;
        Expr* svl = nullptr;
    };
    struct BinExprMul
    {
        Expr* fvl = nullptr;
        Expr* svl = nullptr;
    };
    struct BinExprDiv
    {
        Expr* fvl = nullptr;
        Expr* svl = nullptr;
    };
    struct EQCondition
    {
        Expr* fvl = nullptr;
        Expr* svl = nullptr;
    };
    struct NotEQCondition
    {
        Expr* fvl = nullptr;
        Expr* svl = nullptr;
    };
    struct LessCondition
    {
        Expr* fvl = nullptr;
        Expr* svl = nullptr;
    };
    struct GreaterCondition
    {
        Expr* fvl = nullptr;
        Expr* svl = nullptr;
    };
    struct EQLessCondition
    {
        Expr* fvl = nullptr;
        Expr* svl = nullptr;
    };
    struct EQGreaterCondition
    {
        Expr* fvl = nullptr;
        Expr* svl = nullptr;
    };
    struct AndCondition
    {
        Expr* fvl = nullptr;
        Expr* svl = nullptr;
    };
    struct OrCondition
    {
        Expr* fvl = nullptr;
        Expr* svl = nullptr;
    };    
    struct BinExpr
    {
        std::variant<BinExprAdd*, BinExprSub*, BinExprMul*, BinExprDiv*, EQCondition*, NotEQCondition*, LessCondition*, GreaterCondition*, EQLessCondition*, EQGreaterCondition*, AndCondition*, OrCondition*> var;
    };

    struct Expr
    {
        std::variant<ValExpr*, BinExpr*, StmtInput*> var;
    };

    struct IfPred;

    struct StmtIf
    {
        Expr* Cond = nullptr;
        std::vector<Stmt> statements;
        std::optional<IfPred*> pred;
    };

    struct StmtElIf
    {
        Expr* Cond = nullptr;
        std::vector<Stmt> statements;
        std::optional<IfPred*> pred;
    };

    struct StmtElse
    {
        std::vector<Stmt> statements;
    };    

    struct IfPred
    {
        std::variant<StmtElIf*, StmtElse*> var;
    };

    struct StmtReturn
    {
        Expr* Expr = nullptr;
    };
    struct StmtIntLet
    {
        Token ident;
        Expr* Expr = nullptr;
    };
    struct StmtStrLet
    {
        Token ident;
        Expr* Expr = nullptr;
    };
    struct StmtBoolLet
    {
        Token ident;
        Expr* Expr = nullptr;
    };
    struct StmtCharLet
    {
        Token ident;
        Expr* Expr = nullptr;
    };
    struct StmtIntVar
    {
        Token ident;
        Expr* Expr = nullptr;
    };
    struct StmtStrVar
    {
        Token ident;
        Expr* Expr = nullptr;
    };
    struct StmtBoolVar
    {
        Token ident;
        Expr* Expr = nullptr;
    };
    struct StmtCharVar
    {
        Token ident;
        Expr* Expr = nullptr;
    };
    struct StmtOutput
    {
        Expr* Expr = nullptr;
    };
    struct StmtInput
    {
        Expr* msg = nullptr;
    };

    struct Stmt
    {
        std::variant<StmtReturn, StmtIntLet, StmtStrLet, StmtBoolLet, StmtCharLet, StmtStrVar, StmtIntVar, StmtBoolVar, StmtCharVar, StmtIf, StmtOutput, StmtInput> var; 
    };
    
    struct Prog
    {
        std::vector<Stmt> statements;
    };
}

class parser
{
public:
    inline parser(std::vector<Token> tokens) 
    : m_tokens(std::move(tokens)) {}

    std::optional<node::Expr> parseExpr(std::string ExpectedType = ANY_TYPE, uint8_t min_priority = 1);
    std::optional<node::BinExpr> parseBinExpr(std::string ExpectedType = ANY_TYPE);
    std::optional<node::ValExpr> parseValExpr(std::string ExpectedType = ANY_TYPE);
    std::optional<node::StmtIf> parseIfStmt();
    std::optional<node::IfPred> parseIfPred();
    std::optional<node::StmtIntLet> parseLet(std::string ExpectedType = ANY_TYPE);
    std::optional<node::StmtInput> parseInputStmt();
    std::optional<node::Stmt> parseStmt();
    std::optional<node::Prog> parseProg();

private:

    std::optional<Token> peek(int offset = 0) const;
    Token consume();
    std::optional<uint8_t> op_to_prior(Tokens op);

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
    node::Prog prog;
};

