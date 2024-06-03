#include "parser.hpp"

size_t parser::m_index = 0;
std::vector<Token> parser::m_tokens = {};
node::Prog parser::prog = {};

[[nodiscard]] std::optional<Token> parser::peek(const int &offset)
{
    if (m_index + offset >= m_tokens.size())
    {
        return {};
    }
    return m_tokens[m_index + offset];
}

Token parser::consume()
{
    return m_tokens[m_index++];
}

void parser::tryConsume(const char &charToConsume)
{
    if (peek().has_value() && peek().value().type != tokensMap[std::string(1, charToConsume)] || !peek().has_value())
    {
        std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected '" + std::string(1, charToConsume) + "'";
        exit(EXIT_FAILURE);
    }
    consume();
}

std::optional<node::StmtInput> parser::parseInputStmt()
{
    consume();
    tryConsume('(');
    if (auto const &nodeExpr = expressionParser::parseExpr(ANY_TYPE))
    {
        tryConsume(')');
        return node::StmtInput{new node::Expr(nodeExpr.value())};
    }
    std::cerr << "[Parse Error] ERR001 Invalid Syntax Expected Expression";
    exit(EXIT_FAILURE);
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
        if (auto const &nodeExpr = expressionParser::parseExpr(ANY_TYPE))
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
        stmtNode = {scopeParser::parseIfStmt()};
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
        if (auto const &nodeExpr = expressionParser::parseExpr(ANY_TYPE))
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
        stmtNode = {scopeParser::parseWhileLoop()};
        expectSemi = false;
    }
    else if (peek().value().type == Tokens::FOR)
    {
        stmtNode = {scopeParser::parseForLoop()};
        expectSemi = false;
    }
    else if ((peek().value().type == Tokens::INT_LET || peek().value().type == Tokens::FLOAT_LET ||
            peek().value().type == Tokens::STRING_LET || peek().value().type == Tokens::BOOL_LET ||
            peek().value().type == Tokens::CHAR_LET) || peek().value().type == Tokens::CONST &&
        (peek(1).value().type == Tokens::INT_LET ||
            peek(1).value().type == Tokens::FLOAT_LET ||
            peek(1).value().type == Tokens::STRING_LET ||
            peek(1).value().type == Tokens::BOOL_LET ||
            peek(1).value().type == Tokens::CHAR_LET))
    {
        auto letStmt = varParser::parseLet().value();
        stmtNode = {letStmt};
        varParser::m_vars.insert({letStmt.ident.value.value(), letToType[letStmt.letType]});
    }
    else if (peek().value().type == Tokens::INC || peek().value().type == Tokens::DEC ||
        peek(1).has_value() && (peek(1).value().type == Tokens::INC || peek(1).value().type == Tokens::DEC))
    {
        if (auto const &incDecStmt = expressionParser::parseIncDec())
        {
            stmtNode = {incDecStmt.value()};
        }
    }
    else if (peek().value().type == Tokens::IDENT)
    {
        const auto &varIdent = consume();
        if (!varParser::m_vars.contains(varIdent.value.value()))
        {
            std::cerr << "[Parse Error] ERR005 Undeclared Identifier '" << varIdent.value.value() << "'";
            exit(EXIT_FAILURE);
        }
        tryConsume('=');
        if (auto const &nodeExpr = expressionParser::parseExpr(varParser::m_vars[varIdent.value.value()]))
        {
            stmtNode = {
                {
                    node::StmtVar{
                        varIdent,
                        new node::Expr(nodeExpr.value()),
                        varParser::m_vars[varIdent.value.value()]
                    }
                }
            };
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
