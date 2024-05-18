#include "varCompiler.hpp"



std::unordered_map<std::string, varCompiler::Var> varCompiler::m_vars;

void varCompiler::compVar(const node::StmtVar &stmtVar)
{
	if (!m_vars.count(stmtVar.ident.value.value()))
	{
		std::cerr << "[Compile Error] ERR004 Identifier '" << stmtVar.ident.value.value() << "' Was Not Declared";
		exit(EXIT_FAILURE);
	}
	const auto &var = m_vars[stmtVar.ident.value.value()];
	if (var.isConst)
	{
		std::cerr << "[Compile Error] ERR012 Cannot Change Value Of Const Variables";
		exit(EXIT_FAILURE);
	}
	if (var.Type == BOOL_TYPE)
	{
		if (!expressionCompiler::compExpr(*stmtVar.Expr, INT_TYPE) &&
		    !expressionCompiler::compExpr(*stmtVar.Expr, CHAR_TYPE) &&
		    !expressionCompiler::compExpr(*stmtVar.Expr, BOOL_TYPE))
		{
			std::cerr << "[Compile Error] ERR010 Expression Must Have Bool Type (Or Convertable To It)";
			exit(EXIT_FAILURE);
		}
		compiler::pop("rdx");
		expressionCompiler::compBoolExpr("rdx");
		compiler::pop("rdx");
		compiler::m_output << "\tmov [rsp + " + std::to_string((compiler::m_stackSize - var.stackLoc - 1) * 8) + "], rdx\n";
		compiler::m_output << "\txor rdx, rdx\n";
	}
	else if (var.Type == stmtVar.type)
	{
		if (!expressionCompiler::compExpr(*stmtVar.Expr, stmtVar.type))
		{
			std::cerr << "[Compile Error] ERR006 Value Doesnt Matches Type";
			exit(EXIT_FAILURE);
		}
		compiler::pop("rdx");
		compiler::m_output << "\tmov [rsp + " + std::to_string((compiler::m_stackSize - var.stackLoc - 1) * 8) + "], rdx\n";
		compiler::m_output << "\txor rdx, rdx\n";
	}
	else
	{
		std::cerr << "[Compile Error] ERR006 Value Doesnt Matches Type";
		exit(EXIT_FAILURE);
	}
}

void varCompiler::compLet(const node::StmtLet &stmtLet)
{
	if (m_vars.count(stmtLet.ident.value.value()))
	{
		std::cerr << "[Compile Error] ERR004 Identifier '" << stmtLet.ident.value.value() << "' Is Already Declared";
		exit(EXIT_FAILURE);
	}
	std::string varType = letToType[stmtLet.letType];
	m_vars.insert({stmtLet.ident.value.value(), Var{compiler::m_stackSize, varType, stmtLet.isConst}});
	if (varType == BOOL_TYPE)
	{
		if (stmtLet.Expr != nullptr)
		{
			if (!expressionCompiler::compExpr(*stmtLet.Expr, INT_TYPE) &&
			    !expressionCompiler::compExpr(*stmtLet.Expr, CHAR_TYPE) &&
			    !expressionCompiler::compExpr(*stmtLet.Expr, BOOL_TYPE))
			{
				std::cerr << "[Compile Error] ERR010 Expression Must Have Bool Type (Or Convertable To It)";
				exit(EXIT_FAILURE);
			}
			compiler::pop("rdx");
			expressionCompiler::compBoolExpr("rdx");
		}
		else
		{
			compiler::push("rdx");
		}
	}
	else
	{
		if (stmtLet.Expr != nullptr)
		{
			if (!expressionCompiler::compExpr(*stmtLet.Expr, varType))
			{
				std::cerr << "[Compile Error] ERR006 Value Doesnt Matches Type";
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			if (stmtLet.isConst)
			{
				std::cerr << "[Compile Error] ERR011 Const Variables Cannot Be Declared Without Value";
				exit(EXIT_FAILURE);
			}
			compiler::push("rdx");
		}
	}
}