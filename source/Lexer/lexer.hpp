#pragma once
#include "../Token/token.hpp"
#include <variant>


namespace node 
{
    struct Expr;
    struct Stmt;
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
    struct EQCondition
    {
        Expr* fvl;
        Expr* svl;
    };
    struct NotEQCondition
    {
        Expr* fvl;
        Expr* svl;
    };
    struct LessCondition
    {
        Expr* fvl;
        Expr* svl;
    };
    struct GreaterCondition
    {
        Expr* fvl;
        Expr* svl;
    };
    struct EQLessCondition
    {
        Expr* fvl;
        Expr* svl;
    };
    struct EQGreaterCondition
    {
        Expr* fvl;
        Expr* svl;
    };
    struct AndCondition
    {
        Expr* fvl;
        Expr* svl;
    };
    struct OrCondition
    {
        Expr* fvl;
        Expr* svl;
    };    
    struct BinExpr
    {
        std::variant<BinExprAdd*, BinExprSub*, BinExprMul*, BinExprDiv*, EQCondition*, NotEQCondition*, LessCondition*, GreaterCondition*, EQLessCondition*, EQGreaterCondition*, AndCondition*, OrCondition*> var;
    };

    struct Expr
    {
        std::variant<ValExpr*, BinExpr*> var;
    };

    struct IfPred;

    struct StmtIf
    {
        Expr* Cond;
        std::vector<Stmt> statements;
        std::optional<IfPred*> pred;
    };

    struct StmtElIf
    {
        Expr* Cond;
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
        std::variant<StmtReturn, StmtIntLet, StmtStrLet, StmtBoolLet, StmtStrVar, StmtIntVar, StmtBoolVar, StmtIf> var; 
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

    std::optional<node::IfPred> parse_if_pred();

    std::optional<node::Stmt> parseStmt();

    std::optional<node::Prog> parseProg();


private:

    std::optional<Token> peek(int offset = 0) const;

    Token consume();

    std::optional<uint8_t> op_to_prior(Tokens op)
    {
        switch (op)
        {
        case Tokens::OR:
            return 1;
            break;
        case Tokens::AND:
            return 2;
            break;
        case Tokens::EQEQ:
        case Tokens::NOTEQ:
            return 3;
            break;
        case Tokens::LESS:
        case Tokens::LESSEQ:
        case Tokens::GREATER:
        case Tokens::GREATEQ:
            return 4;
            break;
        case Tokens::PLUS:
        case Tokens::MINUS:
            return 5;
            break;
        case Tokens::MULT:
        case Tokens::DIV:
            return 6;
            break;
        
        default:
            return {};
        }
    }

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
    node::Prog prog;
};

