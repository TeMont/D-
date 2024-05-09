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

std::optional<node::ValExpr> parser::parseValExpr(const std::string& expectedType)
{
    if (peek().has_value())
    {
        node::ValExpr valExpr;
        if (peek().value().type == Tokens::INT_LITERAL)
        {
            if (expectedType == INT_TYPE || expectedType == ANY_TYPE)
            {
                valExpr = {node::ExprIntLit{consume()}};
            }
            else
            {
                return {};
            }
        }
        else if (peek().value().type == Tokens::QOUTE)
        {
            if (expectedType == STR_TYPE || expectedType == ANY_TYPE)
            {
                consume(); // consume '"'
                if (peek().has_value() && peek().value().type == Tokens::STRING_LITERAL)
                {
                    valExpr = {node::ExprStrLit{consume()}};
                    if (peek().value().type == Tokens::QOUTE)
                    {
                        consume(); // consume '"'
                    }
                    else
                    {
                        std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected '\"'";
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    std::cerr << "[Parse Error] ERR001 Syntax Error Expected String Literal";
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                return {};
            }
        }
        else if (peek().value().type == Tokens::APOST)
        {
            if (expectedType == CHAR_TYPE || expectedType == ANY_TYPE)
            {
                consume(); // consume '
                if (peek().has_value() && peek().value().type == Tokens::CHAR_LITERAL)
                {
                    valExpr = {node::ExprCharLit{consume()}};
                    if (peek().value().type == Tokens::APOST)
                    {
                        consume(); // consume '
                    }
                    else
                    {
                        std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected '\"'";
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    std::cerr << "[Parse Error] ERR001 Syntax Error Expected Char Literal";
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                return {};
            }
        }
        else if (peek().value().type == Tokens::BOOL_LITERAL)
        {
            if (expectedType == BOOL_TYPE || expectedType == ANY_TYPE)
            {
                valExpr = {node::ExprBoolLit{consume()}};
            }
            else
            {
                return {};
            }
        }
		else if (peek().value().type == Tokens::NOT)
        {
			consume();
			if (expectedType == BOOL_TYPE || ANY_TYPE)
			{
				if (auto nodeExpr = parseValExpr(ANY_TYPE))
				{
					valExpr = {node::NotCondition({new node::ValExpr(nodeExpr.value())})};
				}
				else
				{
					return {};
				}
			}
		}
        else if (peek().value().type == Tokens::IDENT)
        {
            valExpr = {node::ExprIdent{consume()}};
        }
        else
        {
            std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected Expression";
            exit(EXIT_FAILURE);
        }
        return valExpr;
    }
    else
    {
        return {};
    }
}

std::optional<node::Expr> parser::parseExpr(const std::string& expectedType, uint8_t minPriority)
{
    if (peek().has_value() && peek().value().type == Tokens::INPUT)
    {
        auto tmpInpStmt = parseInputStmt().value();
        return node::Expr{new node::StmtInput(tmpInpStmt)};
    }
    else
    {
        std::optional<node::ValExpr> valFvl = parseValExpr(expectedType);
        if (valFvl.has_value())
        {
            node::Expr exprFvl = {new node::ValExpr(valFvl.value())};

            while (true)
            {
                std::optional<uint8_t> priority;
                if (peek().has_value())
                {
                    priority = op_to_prior(peek().value().type);
                    if (!priority.has_value() || priority < minPriority)
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
                Token opr = consume();
                uint8_t nextMinPriority = priority.value() + 1;
                if (auto exprSvl = parseExpr(expectedType, nextMinPriority))
                {
					node::BinExpr expr;
					node::Expr exprFvl2;
					if (opr.type == Tokens::PLUS)
					{
						node::BinExprAdd add;
						exprFvl2 = exprFvl;
						add.fvl = new node::Expr(exprFvl2);
						add.svl = new node::Expr(exprSvl.value());
						expr.var = new node::BinExprAdd(add);
					}
					else if (opr.type == Tokens::MINUS)
					{
						node::BinExprSub sub;
						exprFvl2 = exprFvl;
						sub.fvl = new node::Expr(exprFvl2);
						sub.svl = new node::Expr(exprSvl.value());
						expr.var = new node::BinExprSub(sub);
					}
					else if (opr.type == Tokens::MULT)
					{
						node::BinExprMul mul;
						exprFvl2 = exprFvl;
						mul.fvl = new node::Expr(exprFvl2);
						mul.svl = new node::Expr(exprSvl.value());
						expr.var = new node::BinExprMul(mul);
					}
					else if (opr.type == Tokens::DIV)
					{
						node::BinExprDiv div;
						exprFvl2 = exprFvl;
						div.fvl = new node::Expr(exprFvl2);
						div.svl = new node::Expr(exprSvl.value());
						expr.var = new node::BinExprDiv(div);
					}
					else if (opr.type == Tokens::EQEQ)
					{
						node::EQCondition eq;
						exprFvl2 = exprFvl;
						eq.fvl = new node::Expr(exprFvl2);
						eq.svl = new node::Expr(exprSvl.value());
						expr.var = new node::EQCondition(eq);
					}
					else if (opr.type == Tokens::NOTEQ)
					{
						node::NotEQCondition notEq;
						exprFvl2 = exprFvl;
						notEq.fvl = new node::Expr(exprFvl2);
						notEq.svl = new node::Expr(exprSvl.value());
						expr.var = new node::NotEQCondition(notEq);
					}
					else if (opr.type == Tokens::LESS)
					{
						node::LessCondition less;
						exprFvl2 = exprFvl;
						less.fvl = new node::Expr(exprFvl2);
						less.svl = new node::Expr(exprSvl.value());
						expr.var = new node::LessCondition(less);
					}
					else if (opr.type == Tokens::LESSEQ)
					{
						node::EQLessCondition lessEq;
						exprFvl2 = exprFvl;
						lessEq.fvl = new node::Expr(exprFvl2);
						lessEq.svl = new node::Expr(exprSvl.value());
						expr.var = new node::EQLessCondition(lessEq);
					}
					else if (opr.type == Tokens::GREATER)
					{
						node::GreaterCondition greater;
						exprFvl2 = exprFvl;
						greater.fvl = new node::Expr(exprFvl2);
						greater.svl = new node::Expr(exprSvl.value());
						expr.var = new node::GreaterCondition(greater);
					}
					else if (opr.type == Tokens::GREATEQ)
					{
						node::EQGreaterCondition greatEq;
						exprFvl2 = exprFvl;
						greatEq.fvl = new node::Expr(exprFvl2);
						greatEq.svl = new node::Expr(exprSvl.value());
						expr.var = new node::EQGreaterCondition(greatEq);
					}
					else if (opr.type == Tokens::AND)
					{
						node::AndCondition And;
						exprFvl2 = exprFvl;
						And.fvl = new node::Expr(exprFvl2);
						And.svl = new node::Expr(exprSvl.value());
						expr.var = new node::AndCondition(And);
					}
					else if (opr.type == Tokens::OR)
					{
						node::OrCondition Or;
						exprFvl2 = exprFvl;
						Or.fvl = new node::Expr(exprFvl2);
						Or.svl = new node::Expr(exprSvl.value());
						expr.var = new node::OrCondition(Or);
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
        else
        {
	        std::cerr << "[Parse Error] ERR006 Value Doesn't Matches Type";
	        exit(EXIT_FAILURE);
        }
    }
}

std::optional<node::StmtIf> parser::parseIfStmt()
{
    consume();
    if (peek().has_value() && peek().value().type == Tokens::LPAREN)
    {
        consume();
        if (auto cond = parseExpr(ANY_TYPE))
        {
            if (peek().has_value() && peek().value().type == Tokens::RPAREN)
            {
                consume();
                if (peek().has_value() && peek().value().type == Tokens::LBRACKET)
                {
                    consume();
                    std::vector<node::Stmt> stmts;
                    while (peek().has_value() && peek().value().type != Tokens::RBRACKET)
                    {
                        if (auto stmt = parseStmt())
                        {
                            stmts.push_back(stmt.value());
                        }
                    }
                    if (peek().has_value() && peek().value().type == Tokens::RBRACKET)
                    {
                        consume(); // consume '}'
                        auto pred = parseIfPred();
                        if (pred.has_value())
                        {
                            return {node::StmtIf({new node::Expr(cond.value()), stmts, {new node::IfPred(pred.value())}})};
                        }
                        else
                        {
                            return {node::StmtIf({new node::Expr(cond.value()), stmts})};
                        }
                    }
                    else
                    {
                        std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected '}'";
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected '{'";
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                std::cerr << "ERR001 Invalid Syntax Expected ')'";
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            std::cerr << "[Parse Error] ERR007 Expected Condition";
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected '('";
        exit(EXIT_FAILURE);
    }
}

std::optional<node::IfPred> parser::parseIfPred()
{
    if (peek().has_value())
    {
        std::optional<node::IfPred> stmtPred;
        if (peek().value().type == Tokens::ELIF)
        {
            auto tempIfStmt = parseIfStmt().value();

            if (tempIfStmt.pred.has_value())
            {
                stmtPred = {new node::StmtElIf({tempIfStmt.Cond, tempIfStmt.statements, tempIfStmt.pred})};
            }
            else
            {
                stmtPred = {new node::StmtElIf({tempIfStmt.Cond, tempIfStmt.statements})};
            }
        }
        else if (peek().value().type == Tokens::ELSE)
        {
            consume();
            if (peek().has_value() && peek().value().type == Tokens::LBRACKET)
            {
                consume();
                std::vector<node::Stmt> stmts;
                while (peek().has_value() && peek().value().type != Tokens::RBRACKET)
                {
                    if (auto stmt = parseStmt())
                    {
                        stmts.push_back(stmt.value());
                    }
                }
                consume();
                stmtPred = {new node::StmtElse({stmts})};
            }
            else
            {
                std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected '{'";
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            return {};
        }
        return stmtPred;
    }
    else
    {
        return {};
    }
}

std::optional<node::StmtIntLet> parser::parseLet(const std::string& expectedType)
{
    consume();
    if (peek().has_value() && peek().value().type == Tokens::IDENT)
    {
        auto varIdent = consume();
        if (peek().has_value() && peek().value().type == Tokens::EQ)
        {
            consume();
            if (auto nodeExpr = parseExpr(expectedType))
            {
                if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                {
                    consume();
                    return node::StmtIntLet{varIdent, new node::Expr(nodeExpr.value())};
                }
                else
                {
                    std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected ';'";
                    exit(EXIT_FAILURE);
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
            if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
            {
                consume();
                return node::StmtIntLet{varIdent};
            }
            else
            {
                std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected ';'";
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        std::cerr << "[Parse Error] ERR002 Expected An Identifier";
        exit(EXIT_FAILURE);
    }
}

std::optional<node::StmtInput> parser::parseInputStmt()
{
    consume();
    if (peek().has_value() && peek().value().type == Tokens::LPAREN)
    {
        consume();
        if (auto nodeExpr = parseExpr(ANY_TYPE))
        {
            if (peek().has_value() && peek().value().type == Tokens::RPAREN)
            {
                consume();
                if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                {
                    return node::StmtInput{new node::Expr(nodeExpr.value())};
                }
                else
                {
                    std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected ';'";
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected ')'";
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected Expression";
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected '('";
        exit(EXIT_FAILURE);
    }
}

std::optional<node::Stmt> parser::parseStmt()
{
    std::optional<node::Stmt> stmtNode;
    if (peek().has_value())
    {
        if (peek().value().type == Tokens::RETURN)
        {
            consume();
            if (auto nodeExpr = parseExpr(ANY_TYPE))
            {
                stmtNode = {{node::StmtReturn{new node::Expr(nodeExpr.value())}}};

                if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                {
                    consume();
                }
                else
                {
                    std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected ';'";
                    exit(EXIT_FAILURE);
                }
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
                stmtNode = {node::StmtIf({tempIfStmt.Cond, tempIfStmt.statements, tempIfStmt.pred})};
            }
            else
            {
                stmtNode = {node::StmtIf({tempIfStmt.Cond, tempIfStmt.statements})};
            }
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
            if (peek().value().type == Tokens::LPAREN)
            {
                consume();
                if (auto nodeExpr = parseExpr(ANY_TYPE))
                {
                    if (peek().value().type == Tokens::RPAREN)
                    {
                        consume();
                        if (peek().value().type == Tokens::SEMICOLON)
                        {
                            consume();
                            stmtNode = {node::StmtOutput{new node::Expr(nodeExpr.value())}};
                        }
                        else
                        {
                            std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected ';'";
                            exit(EXIT_FAILURE);
                        }
                    }
                    else
                    {
                        std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected ')'";
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected Expression";
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected '('";
                exit(EXIT_FAILURE);
            }
        }
        else if (peek().has_value() && peek().value().type == Tokens::INPUT)
        {
            auto tmpInpStmt = parseInputStmt().value();
            if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
            {
                stmtNode = node::Stmt{tmpInpStmt};
            }
            else
            {
                std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected ';'";
            }
        }
        else if (peek().value().type == Tokens::INT_LET)
        {
            auto tempLetStmt = parseLet(INT_TYPE).value();
            if (tempLetStmt.Expr != nullptr)
            {
                stmtNode = {node::StmtIntLet{tempLetStmt.ident, tempLetStmt.Expr}};
            }
            else
            {
                stmtNode = {node::StmtIntLet{tempLetStmt.ident}};
            }
            m_vars.insert({tempLetStmt.ident.value.value(), INT_TYPE});
        }
        else if (peek().value().type == Tokens::STRING_LET)
        {
            auto tempLetStmt = parseLet(STR_TYPE).value();
            if (tempLetStmt.Expr != nullptr)
            {
                stmtNode = {node::StmtStrLet{tempLetStmt.ident, tempLetStmt.Expr}};
            }
            else
            {
                stmtNode = {node::StmtStrLet{tempLetStmt.ident}};
            }
            m_vars.insert({tempLetStmt.ident.value.value(), STR_TYPE});
        }
        else if (peek().value().type == Tokens::BOOL_LET)
        {
            auto tempLetStmt = parseLet(ANY_TYPE).value();
            if (tempLetStmt.Expr != nullptr)
            {
                stmtNode = {node::StmtBoolLet{tempLetStmt.ident, tempLetStmt.Expr}};
            }
            else
            {
                stmtNode = {node::StmtBoolLet{tempLetStmt.ident}};
            }
            m_vars.insert({tempLetStmt.ident.value.value(), BOOL_TYPE});
        }
        else if (peek().value().type == Tokens::CHAR_LET)
        {
            auto tempLetStmt = parseLet(CHAR_TYPE).value();
            if (tempLetStmt.Expr != nullptr)
            {
                stmtNode = {node::StmtCharLet{tempLetStmt.ident, tempLetStmt.Expr}};
            }
            else
            {
                stmtNode = {node::StmtCharLet{tempLetStmt.ident}};
            }
            m_vars.insert({tempLetStmt.ident.value.value(), CHAR_TYPE});
        }
        else if (peek().value().type == Tokens::IDENT)
        {
            auto varIdent = consume();
            if (m_vars.count(varIdent.value.value()))
            {
                if (peek().value().type == Tokens::EQ)
                {
                    consume();
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

                        if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                        {
                            consume();
                        }
                        else
                        {
                            std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected ';'";
                            exit(EXIT_FAILURE);
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
                    std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected '='";
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                std::cerr << "[Parse Error] ERR005 Undeclared Identifier '" << varIdent.value.value() << "'";
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            std::cerr << "[Parse Error] ERR001 Syntax Error";
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        return {};
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