#pragma once
#include "../Token/token.hpp"
#include <variant>
#include <unordered_map>


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
    struct ExprIdent
    {
        Token ident;
    };
	struct NotCondition
	{
		ValExpr* val = nullptr;
	};
    struct ValExpr
    {
        std::variant<ExprIntLit, ExprStrLit, ExprBoolLit, ExprCharLit, ExprIdent, NotCondition> var;
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
        std::variant<
                    BinExprAdd*, BinExprSub*, BinExprMul*, BinExprDiv*,
					EQCondition*, NotEQCondition*, LessCondition*, GreaterCondition*,
					EQLessCondition*, EQGreaterCondition*, AndCondition*, OrCondition*> var;
    };

    struct Expr
    {
        std::variant<ValExpr*, BinExpr*, StmtInput*> var;
    };

    struct IfPred;

    struct StmtIf
    {
        Expr* cond = nullptr;
        std::vector<Stmt> statements;
        std::optional<IfPred*> pred;
    };

    struct StmtElIf
    {
        Expr* cond = nullptr;
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

	struct StmtWhileLoop
	{
		Expr* cond = nullptr;
		std::vector<Stmt> statements;
	};

    struct StmtReturn
    {
        Expr* Expr = nullptr;
    };
    struct StmtIntLet
    {
        Token ident;
        Expr* Expr = nullptr;
		bool isConst = false;
    };
    struct StmtStrLet
    {
        Token ident;
        Expr* Expr = nullptr;
		bool isConst = false;
    };
    struct StmtBoolLet
    {
        Token ident;
        Expr* Expr = nullptr;
		bool isConst = false;
    };
    struct StmtCharLet
    {
        Token ident;
        Expr* Expr = nullptr;
		bool isConst = false;
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
        std::variant<StmtReturn, StmtIntLet, StmtStrLet, StmtBoolLet, StmtCharLet, StmtStrVar, StmtIntVar, StmtBoolVar, StmtCharVar, StmtIf, StmtOutput, StmtInput, StmtWhileLoop> var;
    };
    
    struct Prog
    {
        std::vector<Stmt> statements;
    };
}

class parser
{
public:
    inline explicit parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens)) {}

    std::optional<node::Expr> parseExpr(const std::string& expectedType, uint8_t minPriority = 1);
    std::optional<node::ValExpr> parseValExpr(const std::string& expectedType);
    std::optional<node::StmtIf> parseIfStmt();
    std::optional<node::IfPred> parseIfPred();
    std::optional<node::StmtIntLet> parseLet(const std::string& expectedType);
    std::optional<node::StmtInput> parseInputStmt();
    std::optional<node::Stmt> parseStmt();
    std::optional<node::Prog> parseProg();

#ifdef TEST
    void setTokens(std::vector<Token> tokens)
    {
        m_tokens = std::move(tokens);
    }
    void pushToken(const Token& token)
    {
        m_tokens.push_back(token);
    }
#endif

private:

    [[nodiscard]] std::optional<Token> peek(int offset = 0) const;
    Token consume();
    static std::optional<uint8_t> op_to_prior(Tokens op);

    std::vector<Token> m_tokens;
    static std::unordered_map<std::string, std::string> m_vars;
    size_t m_index = 0;
    node::Prog prog;
};

