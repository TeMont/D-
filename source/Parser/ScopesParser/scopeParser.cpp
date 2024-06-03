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

node::StmtIf scopeParser::parseIfStmt()
{
    parser::consume();
    parser::tryConsume('(');
    if (auto const &cond = expressionParser::parseExpr(ANY_TYPE))
    {
        parser::tryConsume(')');
        const node::Scope &scope = parseScope();
        if (auto const &pred = parseIfPred(); pred.has_value())
        {
            return node::StmtIf({new node::Expr(cond.value()), scope, {new node::IfPred(pred.value())}});
        }
        return node::StmtIf({new node::Expr(cond.value()), scope});
    }
    std::cerr << "[Parse Error] ERR007 Expected Condition";
    exit(EXIT_FAILURE);
}

std::optional<node::IfPred> scopeParser::parseIfPred()
{
    if (!parser::peek().has_value())
    {
        return {};
    }
    std::optional<node::IfPred> stmtPred;
    if (parser::peek().value().type == Tokens::ELIF)
    {
        auto const &tempIfStmt = parseIfStmt();
        if (tempIfStmt.pred.has_value())
        {
            stmtPred = {new node::StmtElIf({tempIfStmt.cond, tempIfStmt.scope, tempIfStmt.pred})};
        }
        else
        {
            stmtPred = {new node::StmtElIf({tempIfStmt.cond, tempIfStmt.scope})};
        }
    }
    else if (parser::peek().value().type == Tokens::ELSE)
    {
        parser::consume();
        auto const &scope = parseScope();
        stmtPred = {new node::StmtElse({scope})};
    }
    else
    {
        return {};
    }
    return stmtPred;
}

node::StmtForLoop scopeParser::parseForLoop()
{
    parser::consume();
    parser::tryConsume('(');
    const std::optional<node::Stmt *> &initStmt = {new node::Stmt(parser::parseStmt().value())};
    if (!initStmt.has_value())
    {
        parser::tryConsume(';');
    }
    const std::optional<node::Expr *> &cond = {new node::Expr(expressionParser::parseExpr(ANY_TYPE, false).value())};
    parser::tryConsume(';');
    const std::optional<node::Stmt *> &itStmt = {new node::Stmt(parser::parseStmt(false).value())};
    parser::tryConsume(')');
    auto const &scope = parseScope();
    return {initStmt, cond, itStmt, scope};
}

node::StmtWhileLoop scopeParser::parseWhileLoop()
{
    parser::consume();
    parser::tryConsume('(');
    if (auto cond = expressionParser::parseExpr(ANY_TYPE))
    {
        parser::tryConsume(')');
        auto const &scope = parseScope();
        return node::StmtWhileLoop{new node::Expr({cond.value()}), scope};
    }
    std::cerr << "[Parse Error] ERR007 Expected Condition";
    exit(EXIT_FAILURE);
}
