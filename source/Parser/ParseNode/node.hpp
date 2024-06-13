#pragma once
#include "../../Token/token.hpp"
#include <variant>

namespace node
{
    struct Expr;
    struct ValExpr;
    struct Stmt;
    struct StmtInput;

    struct ExprIntLit
    {
        Token intLit;
    };

    struct ExprStrLit
    {
        Token strLit;
    };

    struct ExprBoolLit
    {
        Token boolLit;
    };

    struct ExprCharLit
    {
        Token charLit;
    };

    struct ExprFloatLit
    {
        Token floatLit;
    };

    struct ExprIdent
    {
        Token ident;
    };

    struct NotCondition
    {
        ValExpr *val;
    };

    struct IncDec
    {
        Token ident;
        bool isInc;
        bool isPref;
    };

    struct ValExpr
    {
        std::variant<ExprIntLit, ExprStrLit, ExprBoolLit, ExprCharLit, ExprFloatLit, ExprIdent, NotCondition, IncDec> var;
    };

    struct BinExpr
    {
        Expr *fvl;
        Expr *svl;
        Tokens oper;
    };

    struct Expr
    {
        std::variant<ValExpr *, BinExpr *, StmtInput *> var;
    };

    struct IfPred;

    struct Scope
    {
        std::vector<Stmt> statements;
    };

    struct StmtIf
    {
        Expr *cond;
        Scope scope;
        std::optional<IfPred *> pred;
    };

    struct StmtElIf
    {
        Expr *cond;
        Scope scope;
        std::optional<IfPred *> pred;
    };

    struct StmtElse
    {
        Scope scope;
    };

    struct IfPred
    {
        std::variant<StmtElIf *, StmtElse *> var;
    };

    struct StmtWhileLoop
    {
        Expr *cond;
        Scope scope;
    };

    struct StmtForLoop
    {
        std::optional<Stmt *> initStmt;
        std::optional<Expr *> cond;
        std::optional<Stmt *> iterationStmt;
        Scope scope;
    };

    struct StmtReturn
    {
        Expr *Expr;
    };

    struct StmtLet
    {
        Token ident;
        Expr *Expr = nullptr;
        bool isConst = false;
        Tokens letType;
    };

    struct StmtVar
    {
        Token ident;
        Expr *Expr;
        std::string type;
    };

    struct StmtOutput
    {
        Expr *Expr;
    };

    struct StmtInput
    {
        Expr *msg;
    };

    struct Case
    {
        Expr *cond;
        std::optional<Scope> scope;
        std::vector<Stmt> statements;
    };

    struct Default
    {
        std::optional<Scope> scope;
        std::vector<Stmt> statements;
    };

    struct StmtSwitch
    {
        Expr *constant;
        std::vector<Case> *cases;
        std::optional<Default> Default;
    };

    struct StmtCont {};
    struct StmtBreak {};

    struct Stmt
    {
        std::variant<StmtReturn, StmtLet, StmtVar, StmtIf, StmtOutput, StmtInput, StmtWhileLoop, StmtForLoop, IncDec, StmtCont, StmtBreak, StmtSwitch>
        var;
    };

    struct Prog
    {
        std::vector<Stmt> statements;
    };
}
