#include "varCompiler.hpp"

size_t varCompiler::m_stackSize = 0;

std::unordered_map<std::string, varCompiler::Var> varCompiler::m_vars;

void varCompiler::compVar(const node::StmtVar &stmtVar)
{
    if (!m_vars.contains(stmtVar.ident.value.value()))
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
        if (!expressionCompiler::compExpr(*stmtVar.Expr, FLOAT_TYPE, false) &&
            !expressionCompiler::compExpr(*stmtVar.Expr, INT_TYPE) &&
            !expressionCompiler::compExpr(*stmtVar.Expr, CHAR_TYPE) &&
            !expressionCompiler::compExpr(*stmtVar.Expr, BOOL_TYPE))
        {
            std::cerr << "[Compile Error] ERR010 Expression Must Have Bool Type (Or Convertable To It)";
            exit(EXIT_FAILURE);
        }
        pop("rdx");
        expressionCompiler::compBoolExpr("rdx");
        pop("rdx");
        compiler::m_output << "\tmov [rsp + " + std::to_string((m_stackSize - var.stackLoc - 1) * 8) + "], rdx\n";
        compiler::m_output << "\txor rdx, rdx\n";
    }
    else if (var.Type == stmtVar.type)
    {
        if (!expressionCompiler::compExpr(*stmtVar.Expr, stmtVar.type))
        {
            std::cerr << "[Compile Error] ERR006 Value Doesnt Matches Type";
            exit(EXIT_FAILURE);
        }
        pop("rdx");
        compiler::m_output << "\tmov [rsp + " + std::to_string((m_stackSize - var.stackLoc - 1) * 8) + "], rdx\n";
        compiler::m_output << "\txor rdx, rdx\n";
    }
    else if (var.Type == FLOAT_TYPE && stmtVar.type == INT_TYPE || var.Type == INT_TYPE && stmtVar.type == FLOAT_TYPE)
    {
        if (!expressionCompiler::compExpr(*stmtVar.Expr, var.Type))
        {
            std::cerr << "[Compile Error] ERR006 Value Doesnt Matches Type";
            exit(EXIT_FAILURE);
        }
        pop("rdx");
        compiler::m_output << "\tmov [rsp + " + std::to_string((m_stackSize - var.stackLoc - 1) * 8) + "], rdx\n";
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
    compiler::m_output << ";;\tlet\n";
    if (m_vars.contains(stmtLet.ident.value.value()))
    {
        std::cerr << "[Compile Error] ERR004 Identifier '" << stmtLet.ident.value.value() << "' Is Already Declared";
        exit(EXIT_FAILURE);
    }
    const std::string varType = letToType[stmtLet.letType];
    m_vars.insert({stmtLet.ident.value.value(), Var{m_stackSize, varType, stmtLet.isConst}});
    if (varType == BOOL_TYPE)
    {
        if (stmtLet.Expr != nullptr)
        {
            if (!expressionCompiler::compExpr(*stmtLet.Expr, FLOAT_TYPE, false) &&
                !expressionCompiler::compExpr(*stmtLet.Expr, INT_TYPE) &&
                !expressionCompiler::compExpr(*stmtLet.Expr, CHAR_TYPE) &&
                !expressionCompiler::compExpr(*stmtLet.Expr, BOOL_TYPE))
            {
                std::cerr << "[Compile Error] ERR010 Expression Must Have Bool Type (Or Convertable To It)";
                exit(EXIT_FAILURE);
            }
            pop("rdx");
            expressionCompiler::compBoolExpr("rdx");
        }
        else
        {
            push("rdx");
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
            push("rdx");
        }
    }
    compiler::m_output << ";;\t/let\n";
}

void varCompiler::push(const std::string &reg)
{
    compiler::m_output << "\tpush " << reg << "\n";
    ++m_stackSize;
}

void varCompiler::pop(const std::string &reg)
{
    compiler::m_output << "\tpop " << reg << "\n";
    --m_stackSize;
}
