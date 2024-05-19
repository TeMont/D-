#include "scopeCompiler.hpp"

void scopeCompiler::compScope(const node::Scope &scope)
{
	auto beginVars = varCompiler::m_vars;
	size_t beginStackSize = varCompiler::m_stackSize;
	for (auto const &stmt : scope.statements)
	{
		compiler::compStmt(stmt);
	}
	const size_t popCount = varCompiler::m_stackSize - beginStackSize;
	if (popCount != 0)
	{
		compiler::m_output << "\tadd rsp, " << popCount * 8 << "\n";
	}
	varCompiler::m_stackSize -= popCount;
	varCompiler::m_vars = beginVars;
}