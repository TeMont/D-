#include "parser.hpp"

size_t parser::m_index = 0;
std::vector<Token> parser::m_tokens = {};
node::Prog parser::prog = {};

[[nodiscard]] std::optional<Token> parser::peek(int offset)
{
	if (m_index + offset >= m_tokens.size())
	{
		return {};
	}
	else
	{
		return m_tokens[m_index + offset];
	}
}

Token parser::consume()
{
	return m_tokens[m_index++];
}

void parser::tryConsume(char charToConsume)
{
	if (peek().has_value() && peek().value().type != tokensMap[std::string(1, charToConsume)] || !peek().has_value())
	{
		std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected '" + std::string(1, charToConsume) + "'";
		exit(EXIT_FAILURE);
	}
	consume();
}

std::optional<node::StmtIf> parser::parseIfStmt()
{
	consume();
	tryConsume('(');
	if (auto cond = ExpressionParser::parseExpr(ANY_TYPE))
	{
		tryConsume(')');
		tryConsume('{');
		std::vector<node::Stmt> stmts;
		while (peek().has_value() && peek().value().type != Tokens::RBRACKET)
		{
			if (auto const &stmt = parseStmt())
			{
				stmts.push_back(stmt.value());
			}
		}
		tryConsume('}');
		auto pred = parseIfPred();
		if (pred.has_value())
		{
			return {node::StmtIf({new node::Expr(cond.value()), stmts, {new node::IfPred(pred.value())}})};
		}
		return {node::StmtIf({new node::Expr(cond.value()), stmts})};
	}
	else
	{
		std::cerr << "[Parse Error] ERR007 Expected Condition";
		exit(EXIT_FAILURE);
	}
}

std::optional<node::IfPred> parser::parseIfPred()
{
	if (!peek().has_value())
	{
		return {};
	}
	std::optional<node::IfPred> stmtPred;
	if (peek().value().type == Tokens::ELIF)
	{
		auto tempIfStmt = parseIfStmt().value();
		if (tempIfStmt.pred.has_value())
		{
			stmtPred = {new node::StmtElIf({tempIfStmt.cond, tempIfStmt.statements, tempIfStmt.pred})};
		}
		else
		{
			stmtPred = {new node::StmtElIf({tempIfStmt.cond, tempIfStmt.statements})};
		}
	}
	else if (peek().value().type == Tokens::ELSE)
	{
		consume();
		tryConsume('{');
		std::vector<node::Stmt> stmts;
		while (peek().has_value() && peek().value().type != Tokens::RBRACKET)
		{
			if (auto const &stmt = parseStmt())
			{
				stmts.push_back(stmt.value());
			}
		}
		tryConsume('}');
		stmtPred = {new node::StmtElse({stmts})};
	}
	else
	{
		return {};
	}
	return stmtPred;
}

std::optional<node::StmtInput> parser::parseInputStmt()
{
	consume();
	tryConsume('(');
	if (auto nodeExpr = ExpressionParser::parseExpr(ANY_TYPE))
	{
		tryConsume(')');
		return node::StmtInput{new node::Expr(nodeExpr.value())};
	}
	else
	{
		std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected Expression";
		exit(EXIT_FAILURE);
	}
}

std::optional<node::Stmt> parser::parseStmt(bool expectSemi)
{
	std::optional<node::Stmt> stmtNode;
	if (!peek().has_value())
	{
		return {};
	}
	if (peek().value().type == Tokens::RETURN)
	{
		consume();
		if (auto nodeExpr = ExpressionParser::parseExpr(ANY_TYPE))
		{
			stmtNode = {{node::StmtReturn{new node::Expr(nodeExpr.value())}}};
		}
		else
		{
			std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected Expression";
			exit(EXIT_FAILURE);
		}
	}
	else if (peek().value().type == Tokens::IF)
	{
		auto tempIfStmt = parseIfStmt().value();
		if (tempIfStmt.pred.has_value())
		{
			stmtNode = {node::StmtIf({tempIfStmt.cond, tempIfStmt.statements, tempIfStmt.pred})};
		}
		else
		{
			stmtNode = {node::StmtIf({tempIfStmt.cond, tempIfStmt.statements})};
		}
		expectSemi = false;
	}
	else if (peek().value().type == Tokens::ELIF)
	{
		std::cerr << "[Parse Error] ERR008 Illegal 'elif' without matching if";
		exit(EXIT_FAILURE);
	}
	else if (peek().value().type == Tokens::ELSE)
	{
		std::cerr << "[Parse Error] ERR008 Illegal 'else' without matching if";
		exit(EXIT_FAILURE);
	}
	else if (peek().value().type == Tokens::OUTPUT)
	{
		consume();
		tryConsume('(');
		if (auto nodeExpr = ExpressionParser::parseExpr(ANY_TYPE))
		{
			tryConsume(')');
			stmtNode = {node::StmtOutput{new node::Expr(nodeExpr.value())}};
		}
		else
		{
			std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected Expression";
			exit(EXIT_FAILURE);
		}
	}
	else if (peek().value().type == Tokens::INPUT)
	{
		auto tmpInpStmt = parseInputStmt().value();
		stmtNode = node::Stmt{tmpInpStmt};
	}
	else if (peek().value().type == Tokens::WHILE)
	{
		consume();
		tryConsume('(');
		if (auto cond = ExpressionParser::parseExpr(ANY_TYPE))
		{
			tryConsume(')');
			tryConsume('{');
			std::vector<node::Stmt> stmts;
			while (peek().has_value() && peek().value().type != Tokens::RBRACKET)
			{
				if (auto const &stmt = parseStmt())
				{
					stmts.push_back(stmt.value());
				}
			}
			tryConsume('}');
			stmtNode = {node::StmtWhileLoop{new node::Expr({cond.value()}), stmts}};
			expectSemi = false;
		}
		else
		{
			std::cerr << "[Parse Error] ERR007 Expected Condition";
			exit(EXIT_FAILURE);
		}
	}
	else if (peek().value().type == Tokens::FOR)
	{
		node::StmtForLoop stmtForLoop;
		consume();
		tryConsume('(');
		std::optional<node::Stmt> initStmt = parseStmt();
		if (!initStmt.has_value())
		{
			tryConsume(';');
		}
		std::optional<node::Expr> cond;
		cond = ExpressionParser::parseExpr(ANY_TYPE, false);
		tryConsume(';');
		std::optional<node::Stmt> itStmt = parseStmt(false);
		tryConsume(')');
		tryConsume('{');
		std::vector<node::Stmt> stmts;
		while (peek().has_value() && peek().value().type != Tokens::RBRACKET)
		{
			if (auto const &stmt = parseStmt())
			{
				stmts.push_back(stmt.value());
			}
		}
		tryConsume('}');
		if (initStmt.has_value())
		{
			stmtForLoop.initStmt = {new node::Stmt{initStmt.value()}};
		}
		if (cond.has_value())
		{
			stmtForLoop.cond = {new node::Expr{cond.value()}};
		}
		if (itStmt.has_value())
		{
			stmtForLoop.iterationStmt = {new node::Stmt{itStmt.value()}};
		}
		stmtForLoop.statements = stmts;
		stmtNode = {stmtForLoop};
		expectSemi = false;
	}
	else if ((peek().value().type == Tokens::INT_LET || peek().value().type == Tokens::STRING_LET ||
	          peek().value().type == Tokens::BOOL_LET || peek().value().type == Tokens::CHAR_LET) ||
	         peek().value().type == Tokens::CONST &&
	         (peek(1).value().type == Tokens::INT_LET || peek(1).value().type == Tokens::STRING_LET ||
	          peek(1).value().type == Tokens::BOOL_LET || peek(1).value().type == Tokens::CHAR_LET))
	{
		auto letStmt = varParser::parseLet().value();
		stmtNode = {letStmt};
		varParser::m_vars.insert({letStmt.ident.value.value(), letToType[letStmt.letType]});
	}
	else if (peek().value().type == Tokens::INC || peek().value().type == Tokens::DEC ||
	         peek(1).value().type == Tokens::INC || peek(1).value().type == Tokens::DEC)
	{
		std::optional<node::IncDec> incDecStmt = ExpressionParser::parseIncDec();
		if (incDecStmt.has_value())
		{
			stmtNode = {incDecStmt.value()};
		}
	}
	else if (peek().value().type == Tokens::IDENT)
	{
		auto varIdent = consume();
		if (!varParser::m_vars.count(varIdent.value.value()))
		{
			std::cerr << "[Parse Error] ERR005 Undeclared Identifier '" << varIdent.value.value() << "'";
			exit(EXIT_FAILURE);
		}
		tryConsume('=');
		if (auto nodeExpr = ExpressionParser::parseExpr(varParser::m_vars[varIdent.value.value()]))
		{
			stmtNode = {{node::StmtVar{varIdent, new node::Expr(nodeExpr.value()), varParser::m_vars[varIdent.value.value()]}}};
		}
		else
		{
			std::cerr << "[Parse Error] ERR006 Value Doesn't Matches Type";
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		return {};
	}
	if (expectSemi)
	{
		tryConsume(';');
	}
	return stmtNode;
}

std::optional<node::Prog> parser::parseProg()
{
	while (peek().has_value())
	{
		if (auto stmt = parseStmt())
		{
			prog.statements.push_back(stmt.value());
		}
		else
		{
			std::cerr << "[Parse Error] ERR003 Invalid Statement";
			exit(EXIT_FAILURE);
		}
	}
	return prog;
}