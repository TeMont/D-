#pragma once

#include "../Parser/parser.hpp"
#include "ExpressionCompiler/exprCompiler.hpp"
#include "VariableCompiler/varCompiler.hpp"
#include "ScopeCompiler/scopeCompiler.hpp"
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
	static void compInput(const node::StmtInput &stmtInput);
	static void compStmt(const node::Stmt &stmt);
	static std::string createLabel();
	static std::string createSCLabel();

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

	const node::Prog m_prog;
	static std::stringstream m_output;
	static std::stringstream m_SC;
	static std::stringstream m_bssSC;
	static uint64_t m_labelCount;
	static uint64_t m_SCCount;
};
