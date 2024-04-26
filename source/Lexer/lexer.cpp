#include "lexer.hpp"

std::optional<Token> parser::peek(int offset) const
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

std::optional<node::ValExpr> parser::parseValExpr(std::string ExpectedType)
{
    if (peek().has_value())
    {
        node::ValExpr valExpr;
        if (peek().value().type == Tokens::INT_LITERAL)
        {
            if (ExpectedType == INT_TYPE || ExpectedType == ANY_TYPE)
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
            consume(); // consume '"'
            if (ExpectedType == STR_TYPE || ExpectedType == ANY_TYPE)
            {
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
            consume(); // consume '
            if (ExpectedType == CHAR_TYPE || ExpectedType == ANY_TYPE)
            {
                auto x = peek().value().type;
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
            if (ExpectedType == BOOL_TYPE || ExpectedType == ANY_TYPE)
            {
                valExpr = {node::ExprBoolLit{consume()}};
            }
            else
            {
                return {};
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

std::optional<node::Expr> parser::parseExpr(std::string ExpectedType, uint8_t min_priority)
{
    if (peek().has_value() && peek().value().type == Tokens::INPUT)
    {
        auto TmpInpStmt = parseInputStmt().value();
        return node::Expr{new node::StmtInput(TmpInpStmt)};
    }
    else
    {
        std::optional<node::ValExpr> val_fvl = parseValExpr(ExpectedType);
        if (val_fvl.has_value())
        {
            node::Expr expr_fvl = {new node::ValExpr(val_fvl.value())};

            while (1)
            {
                std::optional<uint8_t> priority;
                if (peek().has_value())
                {
                    priority = op_to_prior(peek().value().type);
                    if (!priority.has_value() || priority < min_priority)
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
                Token opr = consume();
                uint8_t next_min_priority = priority.value() + 1;
                if (auto expr_svl = parseExpr(ExpectedType, next_min_priority))
                {
                    if (expr_svl.has_value())
                    {
                        node::BinExpr expr;
                        node::Expr expr_fvl2;
                        if (opr.type == Tokens::PLUS)
                        {
                            node::BinExprAdd add;
                            expr_fvl2 = expr_fvl;
                            add.fvl = new node::Expr(expr_fvl2);
                            add.svl = new node::Expr(expr_svl.value());
                            expr.var = new node::BinExprAdd(add);
                        }
                        else if (opr.type == Tokens::MINUS)
                        {
                            node::BinExprSub sub;
                            expr_fvl2 = expr_fvl;
                            sub.fvl = new node::Expr(expr_fvl2);
                            sub.svl = new node::Expr(expr_svl.value());
                            expr.var = new node::BinExprSub(sub);
                        }
                        else if (opr.type == Tokens::MULT)
                        {
                            node::BinExprMul mul;
                            expr_fvl2 = expr_fvl;
                            mul.fvl = new node::Expr(expr_fvl2);
                            mul.svl = new node::Expr(expr_svl.value());
                            expr.var = new node::BinExprMul(mul);
                        }
                        else if (opr.type == Tokens::DIV)
                        {
                            node::BinExprDiv div;
                            expr_fvl2 = expr_fvl;
                            div.fvl = new node::Expr(expr_fvl2);
                            div.svl = new node::Expr(expr_svl.value());
                            expr.var = new node::BinExprDiv(div);
                        }
                        else if (opr.type == Tokens::EQEQ)
                        {
                            node::EQCondition eq;
                            expr_fvl2 = expr_fvl;
                            eq.fvl = new node::Expr(expr_fvl2);
                            eq.svl = new node::Expr(expr_svl.value());
                            expr.var = new node::EQCondition(eq);
                        }
                        else if (opr.type == Tokens::NOTEQ)
                        {
                            node::NotEQCondition NotEq;
                            expr_fvl2 = expr_fvl;
                            NotEq.fvl = new node::Expr(expr_fvl2);
                            NotEq.svl = new node::Expr(expr_svl.value());
                            expr.var = new node::NotEQCondition(NotEq);
                        }
                        else if (opr.type == Tokens::LESS)
                        {
                            node::LessCondition less;
                            expr_fvl2 = expr_fvl;
                            less.fvl = new node::Expr(expr_fvl2);
                            less.svl = new node::Expr(expr_svl.value());
                            expr.var = new node::LessCondition(less);
                        }
                        else if (opr.type == Tokens::LESSEQ)
                        {
                            node::EQLessCondition lessEq;
                            expr_fvl2 = expr_fvl;
                            lessEq.fvl = new node::Expr(expr_fvl2);
                            lessEq.svl = new node::Expr(expr_svl.value());
                            expr.var = new node::EQLessCondition(lessEq);
                        }
                        else if (opr.type == Tokens::GREATER)
                        {
                            node::GreaterCondition greater;
                            expr_fvl2 = expr_fvl;
                            greater.fvl = new node::Expr(expr_fvl2);
                            greater.svl = new node::Expr(expr_svl.value());
                            expr.var = new node::GreaterCondition(greater);
                        }
                        else if (opr.type == Tokens::GREATEQ)
                        {
                            node::EQGreaterCondition greatEq;
                            expr_fvl2 = expr_fvl;
                            greatEq.fvl = new node::Expr(expr_fvl2);
                            greatEq.svl = new node::Expr(expr_svl.value());
                            expr.var = new node::EQGreaterCondition(greatEq);
                        }
                        else if (opr.type == Tokens::AND)
                        {
                            node::AndCondition And;
                            expr_fvl2 = expr_fvl;
                            And.fvl = new node::Expr(expr_fvl2);
                            And.svl = new node::Expr(expr_svl.value());
                            expr.var = new node::AndCondition(And);
                        }
                        else if (opr.type == Tokens::OR)
                        {
                            node::OrCondition Or;
                            expr_fvl2 = expr_fvl;
                            Or.fvl = new node::Expr(expr_fvl2);
                            Or.svl = new node::Expr(expr_svl.value());
                            expr.var = new node::OrCondition(Or);
                        }
                        expr_fvl.var = new node::BinExpr(expr);
                    }
                    else
                    {
                        std::cerr << "[Parse Error] Expected Expression After Operator";
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    std::cerr << "[Parse Error] ERR006 Value Doesn't Matches Type";
                    exit(EXIT_FAILURE);
                }
            }
            return expr_fvl;
        }
        else
        {
            return {};
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
        std::optional<node::IfPred> stmt_pred;
        if (peek().value().type == Tokens::ELIF)
        {
            auto temp_if_stmt = parseIfStmt().value();

            if (temp_if_stmt.pred.has_value())
            {
                stmt_pred = {new node::StmtElIf({temp_if_stmt.Cond, temp_if_stmt.statements, temp_if_stmt.pred})};
            }
            else
            {
                stmt_pred = {new node::StmtElIf({temp_if_stmt.Cond, temp_if_stmt.statements})};
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
                stmt_pred = {new node::StmtElse({stmts})};
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
        return stmt_pred;
    }
    else
    {
        return {};
    }
}

std::optional<node::StmtIntLet> parser::parseLet(std::string ExpectedType)
{
    consume();
    if (peek().has_value() && peek().value().type == Tokens::IDENT)
    {
        auto var_ident = consume();
        if (peek().has_value() && peek().value().type == Tokens::EQ)
        {
            consume();
            if (auto node_expr = parseExpr(ExpectedType))
            {
                if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                {
                    consume();
                    return node::StmtIntLet{var_ident, new node::Expr(node_expr.value())};
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
                return node::StmtIntLet{var_ident};
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
        if (auto node_expr = parseExpr(ANY_TYPE))
        {
            if (peek().has_value() && peek().value().type == Tokens::RPAREN)
            {
                consume();
                if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
                {
                    return node::StmtInput{new node::Expr(node_expr.value())};
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
    std::optional<node::Stmt> stmt_node;
    if (peek().has_value())
    {
        if (peek().value().type == Tokens::RETURN)
        {
            consume();
            if (auto node_expr = parseExpr(ANY_TYPE))
            {
                stmt_node = {{node::StmtReturn{new node::Expr(node_expr.value())}}};

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
            auto temp_if_stmt = parseIfStmt().value();

            if (temp_if_stmt.pred.has_value())
            {
                stmt_node = {node::StmtIf({temp_if_stmt.Cond, temp_if_stmt.statements, temp_if_stmt.pred})};
            }
            else
            {
                stmt_node = {node::StmtIf({temp_if_stmt.Cond, temp_if_stmt.statements})};
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
                if (auto node_expr = parseExpr(ANY_TYPE))
                {
                    if (peek().value().type == Tokens::RPAREN)
                    {
                        consume();
                        if (peek().value().type == Tokens::SEMICOLON)
                        {
                            consume();
                            stmt_node = {node::StmtOutput{new node::Expr(node_expr.value())}};
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
            auto TmpInpStmt = parseInputStmt().value();
            if (peek().has_value() && peek().value().type == Tokens::SEMICOLON)
            {
                stmt_node = node::Stmt{TmpInpStmt};
            }
            else
            {
                std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected ';'";
            }
        }
        else if (peek().value().type == Tokens::INT_LET)
        {
            auto temp_let_stmt = parseLet(INT_TYPE).value();
            if (temp_let_stmt.Expr != nullptr)
            {
                stmt_node = {node::StmtIntLet{temp_let_stmt.ident, temp_let_stmt.Expr}};
            }
            else
            {
                stmt_node = {node::StmtIntLet{temp_let_stmt.ident}};
            }
        }
        else if (peek().value().type == Tokens::STRING_LET)
        {
            auto temp_let_stmt = parseLet(STR_TYPE).value();
            if (temp_let_stmt.Expr != nullptr)
            {
                stmt_node = {node::StmtStrLet{temp_let_stmt.ident, temp_let_stmt.Expr}};
            }
            else
            {
                stmt_node = {node::StmtStrLet{temp_let_stmt.ident}};
            }
        }
        else if (peek().value().type == Tokens::BOOL_LET)
        {
            auto temp_let_stmt = parseLet(ANY_TYPE).value();
            if (temp_let_stmt.Expr != nullptr)
            {
                stmt_node = {node::StmtBoolLet{temp_let_stmt.ident, temp_let_stmt.Expr}};
            }
            else
            {
                stmt_node = {node::StmtBoolLet{temp_let_stmt.ident}};
            }
        }
        else if (peek().value().type == Tokens::CHAR_LET)
        {
            auto temp_let_stmt = parseLet(CHAR_TYPE).value();
            if (temp_let_stmt.Expr != nullptr)
            {
                stmt_node = {node::StmtCharLet{temp_let_stmt.ident, temp_let_stmt.Expr}};
            }
            else
            {
                stmt_node = {node::StmtCharLet{temp_let_stmt.ident}};
            }
        }
        else if (peek().value().type == Tokens::IDENT)
        {
            auto var_ident = consume();
            if (peek().value().type == Tokens::EQ)
            {
                consume();
                if (auto node_expr = parseExpr(STR_TYPE))
                {
                    stmt_node = {{node::StmtStrVar{var_ident, new node::Expr(node_expr.value())}}};
                }
                else if (auto node_expr = parseExpr(INT_TYPE))
                {
                    stmt_node = {{node::StmtIntVar{var_ident, new node::Expr(node_expr.value())}}};
                }
                else if (auto node_expr = parseExpr(BOOL_TYPE))
                {
                    stmt_node = {{node::StmtBoolVar{var_ident, new node::Expr(node_expr.value())}}};
                }
                else
                {
                    std::cerr << "[Parse Error] ERR006 Value Doesn't Matches Type";
                    exit(EXIT_FAILURE);
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
                std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected '='";
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
    return stmt_node;
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