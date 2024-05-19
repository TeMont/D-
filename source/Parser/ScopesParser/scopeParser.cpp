#include "scopeParser.hpp"

node::Scope scopeParser::parseScope()
{
	parser::tryConsume('{');
	node::Scope scope;
	while (const auto &stmt = parser::parseStmt())
	{
		scope.statements.push_back(stmt.value());
	}
	parser::tryConsume('}');
	return scope;
}