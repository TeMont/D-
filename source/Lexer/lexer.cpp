#include "lexer.hpp"

std::unordered_map<std::string, std::string> parser::m_vars;

[[nodiscard]] std::optional<Token> parser::peek(int offset) const
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
std::optional<uint8_t> parser::op_to_prior(Tokens op)
{
    switch (op)
    {
    case Tokens::OR:
        return 1;
        break;
    case Tokens::AND:
        return 2;
        break;
    case Tokens::EQEQ:
    case Tokens::NOTEQ:
        return 3;
        break;
    case Tokens::LESS:
    case Tokens::LESSEQ:
    case Tokens::GREATER:
    case Tokens::GREATEQ:
        return 4;
        break;
    case Tokens::PLUS:
    case Tokens::MINUS:
        return 5;
        break;
    case Tokens::MULT:
    case Tokens::DIV:
        return 6;
        break;

    default:
        return {};
    }
}

std::optional<node::ValExpr> parser::parseValExpr(const std::string& expectedType, bool isRequired)
{
    if (peek().has_value())
    {
        node::ValExpr valExpr;
        if (peek().value().type == Tokens::INT_LITERAL)
        {
			if (expectedType != INT_TYPE && expectedType != BOOL_TYPE && expectedType != ANY_TYPE)
			{
                return {};
			}
            valExpr = {node::ExprIntLit{consume()}};
        }
        else if (peek().value().type == Tokens::QOUTE)
        {
			if (expectedType != STR_TYPE && expectedType != ANY_TYPE)
			{
				return {};
			}
	        consume(); // consume '"'
            valExpr = {node::ExprStrLit{consume()}};
			consume(); // consume '"'
        }
        else if (peek().value().type == Tokens::APOST)
        {
	        if (expectedType != CHAR_TYPE && expectedType != BOOL_TYPE && expectedType != ANY_TYPE)
	        {
				return {};
			}
            consume(); // consume '
			valExpr = {node::ExprCharLit{consume()}};
			consume(); // consume '
        }
        else if (peek().value().type == Tokens::BOOL_LITERAL)
        {
            if (expectedType != BOOL_TYPE && expectedType != ANY_TYPE)
            {
                return {};
            }
			valExpr = {node::ExprBoolLit{consume()}};
        }
		else if (peek().value().type == Tokens::NOT)//FOR EXPRESSIONS LIKE !20, !x
        {
			consume();
			if (expectedType != BOOL_TYPE && expectedType != ANY_TYPE)
			{
				return {};
			}
			if (auto nodeExpr = parseValExpr(ANY_TYPE))
			{
				valExpr = {node::NotCondition({new node::ValExpr(nodeExpr.value())})};
			}
			else
			{
				return {};
			}
		}
        else if (peek().value().type == Tokens::INC)
        {
	        if (auto tempInc = parseIncDec())
	        {
		        valExpr = {node::PrefixInc{tempInc.value().ident}};
	        }
        }
        else if (peek().value().type == Tokens::DEC)
        {
	        if (auto tempInc = parseIncDec())
	        {
	            valExpr = {node::PrefixDec{tempInc.value().ident}};
			}
        }
        else if (peek().value().type == Tokens::IDENT)
        {
			if (peek(1).has_value() && peek(1).value().type == INC)
			{
				if (auto tempInc = parseIncDec())
				{
					valExpr = {node::PostfixInc{tempInc.value().ident}};
				}
			}
			else if (peek(1).has_value() && peek(1).value().type == DEC)
			{
				if (auto tempInc = parseIncDec())
				{
					valExpr = {node::PostfixDec{tempInc.value().ident}};
				}
			}
			else
			{
				auto ident = consume();
                valExpr = {node::ExprIdent{ident}};
			}
        }
        else
        {
			if (isRequired)
			{
		        std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected Expression";
		        exit(EXIT_FAILURE);
			}
			return {};
        }
        return valExpr;
    }
    else
    {
        return {};
    }
}

std::optional<node::Expr> parser::parseExpr(const std::string& expectedType, bool isRequired, uint8_t minPriority)
{
    if (peek().has_value() && peek().value().type == Tokens::INPUT)
    {
        auto tmpInpStmt = parseInputStmt().value();
        return node::Expr{new node::StmtInput(tmpInpStmt)};
    }
    else
    {
        std::optional<node::ValExpr> valFvl = parseValExpr(expectedType, isRequired);
        if (!valFvl.has_value())
		{
			if (isRequired)
			{
				std::cerr << "[Parse Error] ERR006 Value Doesn't Matches Type";
				exit(EXIT_FAILURE);
			}
			return {};
        }
        node::Expr exprFvl = {new node::ValExpr(valFvl.value())};

        while (true)
        {
            std::optional<uint8_t> priority;
            if (!peek().has_value())
            {
                break;
            }
            priority = op_to_prior(peek().value().type);
            if (!priority.has_value() || priority < minPriority)
            {
                break;
            }
            Token opr = consume();
            uint8_t nextMinPriority = priority.value() + 1;
            if (auto exprSvl = parseExpr(expectedType, nextMinPriority))
            {
				node::BinExpr expr;
				node::Expr exprFvl2;
	            switch (opr.type)
	            {
		        case Tokens::PLUS:
		        {
		            node::BinExprAdd add;
		            exprFvl2 = exprFvl;
		            add.fvl = new node::Expr(exprFvl2);
		            add.svl = new node::Expr(exprSvl.value());
		            expr.var = new node::BinExprAdd(add);
		            break;
		        }
		        case Tokens::MINUS:
		        {
		            node::BinExprSub sub;
		            exprFvl2 = exprFvl;
		            sub.fvl = new node::Expr(exprFvl2);
		            sub.svl = new node::Expr(exprSvl.value());
		            expr.var = new node::BinExprSub(sub);
		            break;
		        }
		        case Tokens::MULT:
		        {
		            node::BinExprMul mul;
		            exprFvl2 = exprFvl;
		            mul.fvl = new node::Expr(exprFvl2);
		            mul.svl = new node::Expr(exprSvl.value());
		            expr.var = new node::BinExprMul(mul);
		            break;
		        }
		        case Tokens::DIV:
		        {
		            node::BinExprDiv div;
		            exprFvl2 = exprFvl;
		            div.fvl = new node::Expr(exprFvl2);
		            div.svl = new node::Expr(exprSvl.value());
		            expr.var = new node::BinExprDiv(div);
		            break;
		        }
		        case Tokens::EQEQ:
		        {
		            node::EQCondition eq;
		            exprFvl2 = exprFvl;
		            eq.fvl = new node::Expr(exprFvl2);
		            eq.svl = new node::Expr(exprSvl.value());
		            expr.var = new node::EQCondition(eq);
		            break;
		        }
		        case Tokens::NOTEQ:
		        {
		            node::NotEQCondition notEq;
		            exprFvl2 = exprFvl;
		            notEq.fvl = new node::Expr(exprFvl2);
		            notEq.svl = new node::Expr(exprSvl.value());
		            expr.var = new node::NotEQCondition(notEq);
		            break;
		        }
		        case Tokens::LESS:
		        {
		            node::LessCondition less;
		            exprFvl2 = exprFvl;
		            less.fvl = new node::Expr(exprFvl2);
		            less.svl = new node::Expr(exprSvl.value());
		            expr.var = new node::LessCondition(less);
		            break;
		        }
		        case Tokens::LESSEQ:
		        {
		            node::EQLessCondition lessEq;
		            exprFvl2 = exprFvl;
		            lessEq.fvl = new node::Expr(exprFvl2);
		            lessEq.svl = new node::Expr(exprSvl.value());
		            expr.var = new node::EQLessCondition(lessEq);
		            break;
		        }
		        case Tokens::GREATER:
		        {
		            node::GreaterCondition greater;
		            exprFvl2 = exprFvl;
		            greater.fvl = new node::Expr(exprFvl2);
		            greater.svl = new node::Expr(exprSvl.value());
		            expr.var = new node::GreaterCondition(greater);
		            break;
		        }
		        case Tokens::GREATEQ:
		        {
		            node::EQGreaterCondition greatEq;
		            exprFvl2 = exprFvl;
		            greatEq.fvl = new node::Expr(exprFvl2);
		            greatEq.svl = new node::Expr(exprSvl.value());
		            expr.var = new node::EQGreaterCondition(greatEq);
		            break;
		        }
		        case Tokens::AND:
		        {
		            node::AndCondition And;
		            exprFvl2 = exprFvl;
		            And.fvl = new node::Expr(exprFvl2);
		            And.svl = new node::Expr(exprSvl.value());
		            expr.var = new node::AndCondition(And);
		            break;
		        }
		        case Tokens::OR:
		        {
		            node::OrCondition Or;
		            exprFvl2 = exprFvl;
		            Or.fvl = new node::Expr(exprFvl2);
		            Or.svl = new node::Expr(exprSvl.value());
		            expr.var = new node::OrCondition(Or);
		            break;
		        }
	            }
	            exprFvl.var = new node::BinExpr(expr);
            }
            else
            {
                std::cerr << "[Parse Error] ERR006 Value Doesn't Matches Type";
                exit(EXIT_FAILURE);
            }
        }
        return exprFvl;
    }
}

std::optional<node::StmtIf> parser::parseIfStmt()
{
    consume();
	tryConsume('(');
    if (auto cond = parseExpr(ANY_TYPE))
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

std::optional<node::StmtIntLet> parser::parseLet(const std::string& expectedType)
{
	bool isConst = false;
	if (peek().value().type == CONST || peek(1).has_value() && peek(1).value().type == CONST)
	{
		isConst = true;
		consume();
	}
    consume();
    if (peek().has_value() && peek().value().type != Tokens::IDENT || !peek().has_value())
    {
	    std::cerr << "[Parse Error] ERR002 Expected An Identifier";
	    exit(EXIT_FAILURE);
	}
    auto varIdent = consume();
    if (peek().has_value() && peek().value().type == Tokens::EQ)
    {
        consume();
        if (auto nodeExpr = parseExpr(expectedType))
        {
            return node::StmtIntLet{varIdent, new node::Expr(nodeExpr.value()), isConst};
        }
        else
        {
            std::cerr << "[Parse Error] ERR006 Value Doesn't Matches Type";
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        return node::StmtIntLet{varIdent, nullptr, isConst};
    }
}

std::optional<node::StmtInput> parser::parseInputStmt()
{
    consume();
	tryConsume('(');
    if (auto nodeExpr = parseExpr(ANY_TYPE))
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

std::optional<node::PrefixInc> parser::parseIncDec()
{
	node::PrefixInc nodeInc;
	if (peek().value().type == Tokens::INC || peek().value().type == Tokens::DEC)
	{
		consume();
		if (peek().has_value() && peek().value().type != Tokens::IDENT || !peek().has_value())
		{
			std::cerr << "[Parse Error] ERR001 Syntax Error Expected Identifier";
			exit(EXIT_FAILURE);
		}
		nodeInc = {consume()};
	}
	else if (peek().value().type == Tokens::IDENT)
	{
		auto ident = consume();
		if (peek().has_value() && peek().value().type == INC || peek().has_value() && peek().value().type == DEC)
		{
			nodeInc = {ident};
			consume();
		}
	}
	else
	{
		return {};
	}
	return nodeInc;
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
        if (auto nodeExpr = parseExpr(ANY_TYPE))
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
        if (auto nodeExpr = parseExpr(ANY_TYPE))
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
	    if (auto cond = parseExpr(ANY_TYPE))
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
		cond = parseExpr(ANY_TYPE, false);
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
    else if (peek().value().type == Tokens::INT_LET || peek().value().type == Tokens::CONST && peek(1).value().type == Tokens::INT_LET)
    {
        auto tempLetStmt = parseLet(INT_TYPE).value();
        stmtNode = {node::StmtIntLet{tempLetStmt.ident, tempLetStmt.Expr, tempLetStmt.isConst}};
        m_vars.insert({tempLetStmt.ident.value.value(), INT_TYPE});
    }
    else if (peek().value().type == Tokens::STRING_LET || peek().value().type == Tokens::CONST && peek(1).value().type == Tokens::STRING_LET)
    {
        auto tempLetStmt = parseLet(STR_TYPE).value();
        stmtNode = {node::StmtStrLet{tempLetStmt.ident, tempLetStmt.Expr, tempLetStmt.isConst}};
        m_vars.insert({tempLetStmt.ident.value.value(), STR_TYPE});
    }
    else if (peek().value().type == Tokens::BOOL_LET || peek().value().type == Tokens::CONST && peek(1).value().type == Tokens::BOOL_LET)
    {
        auto tempLetStmt = parseLet(ANY_TYPE).value();
        stmtNode = {node::StmtBoolLet{tempLetStmt.ident, tempLetStmt.Expr, tempLetStmt.isConst}};
        m_vars.insert({tempLetStmt.ident.value.value(), BOOL_TYPE});
    }
    else if (peek().value().type == Tokens::CHAR_LET || peek().value().type == Tokens::CONST && peek(1).value().type == Tokens::CHAR_LET)
    {
        auto tempLetStmt = parseLet(CHAR_TYPE).value();
        stmtNode = {node::StmtCharLet{tempLetStmt.ident, tempLetStmt.Expr, tempLetStmt.isConst}};
        m_vars.insert({tempLetStmt.ident.value.value(), CHAR_TYPE});
    }
	else if (peek().value().type == Tokens::INC || peek().value().type == Tokens::DEC || peek(1).value().type == Tokens::INC || peek(1).value().type == Tokens::DEC)
    {
		if (peek().value().type == Tokens::INC)
		{
			if (auto tempInc = parseIncDec())
			{
				stmtNode = {node::PrefixInc{tempInc.value().ident}};
			}
		}
		else if (peek().value().type == Tokens::DEC)
		{
			if (auto tempInc = parseIncDec())
			{
				stmtNode = {node::PrefixDec{tempInc.value().ident}};
			}
		}
		else if (peek(1).value().type == Tokens::INC)
		{
			if (auto tempInc = parseIncDec())
			{
				stmtNode = {node::PostfixInc{tempInc.value().ident}};
			}
		}
		else if (peek(1).value().type == Tokens::DEC)
		{
			if (auto tempInc = parseIncDec())
			{
				stmtNode = {node::PostfixDec{tempInc.value().ident}};
			}
		}
	}
    else if (peek().value().type == Tokens::IDENT)
    {
        auto varIdent = consume();
        if (!m_vars.count(varIdent.value.value()))
        {
	        std::cerr << "[Parse Error] ERR005 Undeclared Identifier '" << varIdent.value.value() << "'";
	        exit(EXIT_FAILURE);
		}
	    tryConsume('=');
        if (auto nodeExpr = parseExpr(m_vars[varIdent.value.value()]))
        {
            if (m_vars[varIdent.value.value()] == STR_TYPE)
            {
                stmtNode = {{node::StmtStrVar{varIdent, new node::Expr(nodeExpr.value())}}};
            }
            else if (m_vars[varIdent.value.value()] == INT_TYPE)
            {
                stmtNode = {{node::StmtIntVar{varIdent, new node::Expr(nodeExpr.value())}}};
            }
            else if (m_vars[varIdent.value.value()] == CHAR_TYPE)
            {
                stmtNode = {{node::StmtCharVar{varIdent, new node::Expr(nodeExpr.value())}}};
            }
            else if (m_vars[varIdent.value.value()] == BOOL_TYPE)
            {
                stmtNode = {{node::StmtBoolVar{varIdent, new node::Expr(nodeExpr.value())}}};
            }
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