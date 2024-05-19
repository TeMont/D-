#pragma once

#include "../compiler.hpp"

class varCompiler
{
public:
	struct Var
	{
		size_t stackLoc;
		std::string Type;
		bool isConst;
	};

	static void compVar(const node::StmtVar &stmtVar);
	static void compLet(const node::StmtLet &stmtLet);
	static std::unordered_map<std::string, Var> m_vars;
	static size_t m_stackSize;
	static void push(const std::string &reg);
	static void pop(const std::string &reg);
};
