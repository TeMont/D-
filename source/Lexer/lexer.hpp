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
		ValExpr* val;
	};
	struct PostfixInc
	{
		Token ident;
	};
	struct PrefixInc
	{
		Token ident;
	};
	struct PostfixDec
	{
		Token ident;
	};
	struct PrefixDec
	{
		Token ident;
	};
    struct ValExpr
    {
        std::variant<ExprIntLit, ExprStrLit, ExprBoolLit, ExprCharLit, ExprIdent, NotCondition, PostfixInc, PrefixInc, PostfixDec, PrefixDec> var;
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
        Expr* cond;
        std::vector<Stmt> statements;
        std::optional<IfPred*> pred;
    };

    struct StmtElIf
    {
        Expr* cond;
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
		Expr* cond;
		std::vector<Stmt> statements;
	};

	struct StmtForLoop
	{
		Stmt* initStmt;
		Expr* cond;
		Stmt* iterationStmt;
		std::vector<Stmt> statements;
	};

    struct StmtReturn
    {
        Expr* Expr;
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
    struct StmtCharVar
    {
        Token ident;
        Expr* Expr;
    };
    struct StmtOutput
    {
        Expr* Expr;
    };
    struct StmtInput
    {
        Expr* msg;
    };

    struct Stmt
    {
        std::variant<StmtReturn, StmtIntLet, StmtStrLet, StmtBoolLet,
					 StmtCharLet, StmtStrVar, StmtIntVar, StmtBoolVar,
					 StmtCharVar, StmtIf, StmtOutput, StmtInput, StmtWhileLoop,
					 PrefixInc, PrefixDec, PostfixInc, PostfixDec> var;
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
	std::optional<node::PrefixInc> parseIncDec();
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

