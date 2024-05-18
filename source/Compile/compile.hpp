#pragma once

#include "../Lexer/lexer.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <unordered_map>

bool createObjectFile(const std::string &path);
bool linkObjectFiles(const std::string &path);

class compiler
{
public:
	explicit compiler(node::Prog prog) : m_prog(std::move(prog))
	{
	}

	std::stringstream compile();
	static bool compExpr(const node::Expr &expr, const std::string &expectedType);
	static bool compBinExpr(const node::BinExpr &expr, const std::string &expectedType);
	static void compBoolExpr(const std::optional<std::string> &literal, bool isReversed = false);
	static bool compValExpr(const node::ValExpr &expr, const std::string &expectedType);
	static void compIfPred(const node::IfPred &pred, const std::string &endLabel);
	static void compVar(const node::StmtVar &stmtVar);
	static void compIncDec(const Token &ident, bool isInc, const std::string &expectedType);
	static void compLet(const node::StmtLet &stmtLet);
	static void compInput(const node::StmtInput &stmtInput);
	static void compStmt(const node::Stmt &stmt);
	static std::string createLabel();
	static std::string createSCLabel();
	static void push(const std::string &reg);
	static void pop(const std::string &reg);

#ifdef TEST
	static std::string getM_output()
	{
		std::string s = m_output.str();
		return s;
	}

	static void pushVar(const std::string& name, const std::string& type)
	{
		m_vars.insert({name, Var{m_stackSize, type}});
	}
	static void clearM_vars()
	{
		m_vars.clear();
	}
	static void clearM_output()
	{
		m_output.str("");
	}
	static void resetM_stackSize()
	{
		m_stackSize = 0;
	}
	static void resetM_labelCount()
	{
		m_labelCount = 0;
		m_SCCount = 0;
	}
#endif

private:

	struct Var
	{
		size_t stackLoc;
		std::string Type;
		bool isConst;
	};
	const node::Prog m_prog;
	static size_t m_stackSize;
	static std::unordered_map<std::string, Var> m_vars;
	static std::stringstream m_output;
	static std::stringstream m_SC;
	static std::stringstream m_bssSC;
	static uint64_t m_labelCount;
	static uint64_t m_SCCount;
};
