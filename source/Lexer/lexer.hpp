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
		std::variant<ExprIntLit, ExprStrLit, ExprBoolLit, ExprCharLit, ExprIdent, NotCondition, IncDec> var;
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
	struct StmtIf
	{
		Expr *cond;
		std::vector<Stmt> statements;
		std::optional<IfPred *> pred;
	};
	struct StmtElIf
	{
		Expr *cond;
		std::vector<Stmt> statements;
		std::optional<IfPred *> pred;
	};
	struct StmtElse
	{
		std::vector<Stmt> statements;
	};
	struct IfPred
	{
		std::variant<StmtElIf *, StmtElse *> var;
	};
	struct StmtWhileLoop
	{
		Expr *cond;
		std::vector<Stmt> statements;
	};
	struct StmtForLoop
	{
		std::optional<Stmt *> initStmt;
		std::optional<Expr *> cond;
		std::optional<Stmt *> iterationStmt;
		std::vector<Stmt> statements;
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
	struct StmtIntVar
	{
		Token ident;
		Expr *Expr;
	};
	struct StmtStrVar
	{
		Token ident;
		Expr *Expr;
	};
	struct StmtBoolVar
	{
		Token ident;
		Expr *Expr;
	};
	struct StmtCharVar
	{
		Token ident;
		Expr *Expr;
	};
	struct StmtOutput
	{
		Expr *Expr;
	};
	struct StmtInput
	{
		Expr *msg;
	};
	struct Stmt
	{
		std::variant<StmtReturn, StmtLet, StmtStrVar, StmtIntVar, StmtBoolVar,
				StmtCharVar, StmtIf, StmtOutput, StmtInput, StmtWhileLoop, StmtForLoop, IncDec> var;
	};
	struct Prog
	{
		std::vector<Stmt> statements;
	};
}
class parser
{
public:
	inline explicit parser(std::vector<Token> tokens) : m_tokens(std::move(tokens))
	{
	}

	std::optional<node::Expr>
	parseExpr(const std::string &expectedType, bool isRequired = true, uint8_t minPriority = 1);
	std::optional<node::ValExpr> parseValExpr(const std::string &expectedType, bool isRequired = true);
	std::optional<node::StmtIf> parseIfStmt();
	std::optional<node::IfPred> parseIfPred();
	std::optional<node::StmtLet> parseLet();
	std::optional<node::StmtInput> parseInputStmt();
	std::optional<node::IncDec> parseIncDec();
	std::optional<node::Stmt> parseStmt(bool expectSemi = true);
	std::optional<node::Prog> parseProg();
	static std::unordered_map<Tokens, std::string> letToType;

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
	void tryConsume(char charToConsume);
	static std::optional<uint8_t> op_to_prior(Tokens op);
	std::vector<Token> m_tokens;
	static std::unordered_map<std::string, std::string> m_vars;
	size_t m_index = 0;
	node::Prog prog;
};

