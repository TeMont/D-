#include "scopeCompiler.hpp"

std::vector<std::string> scopeCompiler::lastLoopBegin;
std::vector<std::string> scopeCompiler::lastLoopEnd;

void scopeCompiler::compScope(const node::Scope &scope)
{
    const auto beginVars = varCompiler::m_vars;
    const size_t beginStackSize = varCompiler::m_stackSize;
    for (const auto &stmt : scope.statements)
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

void scopeCompiler::compIfPred(const node::IfPred &pred, const std::string &endLabel)
{
    struct predVisitor
    {
        std::string endLabel;

        explicit predVisitor(std::string endLabel) : endLabel(std::move(endLabel))
        {
        }

        void operator()(const node::StmtElIf *elIf) const
        {
            compiler::m_output << ";;\telif\n";
            const std::string &falseLabel = compiler::createLabel();
            if (!expressionCompiler::compExpr(*elIf->cond, INT_TYPE, false) && !expressionCompiler::compExpr(*elIf->cond, FLOAT_TYPE, false)
                &&
                !expressionCompiler::compExpr(*elIf->cond, CHAR_TYPE, false) && !expressionCompiler::compExpr(*elIf->cond, BOOL_TYPE))
            {
                std::cerr << "[Compile Error] ERR010 Expression Must Have Bool Type (Or Convertable To It)";
                exit(EXIT_FAILURE);
            }
            varCompiler::pop("rdx");
            compiler::m_output << "\tcmp rdx, 0\n";
            compiler::m_output << "\tje " << falseLabel << "\n";
            compScope(elIf->scope);
            compiler::m_output << "\txor rdx, rdx\n";
            compiler::m_output << "\tjmp " << endLabel << "\n";
            compiler::m_output << ";;\t/elif\n";
            compiler::m_output << "\t" << falseLabel << ":\n";
            if (elIf->pred.has_value())
            {
                compIfPred(*elIf->pred.value(), endLabel);
            }
            compiler::m_output << "\txor rdx, rdx\n";
        }

        void operator()(const node::StmtElse *Else) const
        {
            compiler::m_output << ";;\telse\n";
            compScope(Else->scope);
            compiler::m_output << "\txor rdx, rdx\n";
            compiler::m_output << ";;\t/else\n";
        }
    };
    predVisitor visitor(endLabel);
    std::visit(visitor, pred.var);
}

void scopeCompiler::compIfStmt(const node::StmtIf &stmtIf)
{
    compiler::m_output << ";;\tif\n";
    const std::string &falseLabel = compiler::createLabel();
    if (!expressionCompiler::compExpr(*stmtIf.cond, INT_TYPE, false) && !expressionCompiler::compExpr(*stmtIf.cond, FLOAT_TYPE, false) &&
        !expressionCompiler::compExpr(*stmtIf.cond, CHAR_TYPE, false) && !expressionCompiler::compExpr(*stmtIf.cond, BOOL_TYPE))
    {
        std::cerr << "[Compile Error] ERR010 Expression Must Have Bool Type (Or Convertable To It)";
        exit(EXIT_FAILURE);
    }
    varCompiler::pop("rdx");
    compiler::m_output << "\tcmp rdx, 0\n";
    compiler::m_output << "\tje " << falseLabel << "\n";
    compScope(stmtIf.scope);
    compiler::m_output << "\txor rdx, rdx\n";
    compiler::m_output << ";;\t/if\n";
    if (stmtIf.pred.has_value())
    {
        const std::string &endLabel = compiler::createLabel();
        compiler::m_output << "\tjmp " << endLabel << "\n";
        compiler::m_output << "\t" << falseLabel << ":\n";
        compIfPred(*stmtIf.pred.value(), endLabel);
        compiler::m_output << "\t" << endLabel << ":\n";
    }
    else
    {
        compiler::m_output << "\t" << falseLabel << ":\n";
    }
    compiler::m_output << "\txor rdx, rdx\n";
}

void scopeCompiler::compForLoop(const node::StmtForLoop &forLoop)
{
    compiler::m_output << ";;\tfor loop\n";
    const std::string &startLabel = compiler::createLabel();
    const std::string &iterLabel = compiler::createLabel();
    const std::string &endLabel = compiler::createLabel();
    lastLoopBegin.push_back(iterLabel);
    lastLoopEnd.push_back(endLabel);
    const size_t beginStackSize = varCompiler::m_stackSize;
    const auto beginVars = varCompiler::m_vars;
    if (forLoop.initStmt.has_value())
    {
        compiler::compStmt(*forLoop.initStmt.value());
    }
    compiler::m_output << "\t" << startLabel << ":\n";
    if (forLoop.cond.has_value())
    {
        if (!expressionCompiler::compExpr(*forLoop.cond.value(), INT_TYPE, false) &&
            !expressionCompiler::compExpr(*forLoop.cond.value(), FLOAT_TYPE, false) &&
            !expressionCompiler::compExpr(*forLoop.cond.value(), CHAR_TYPE, false) && !expressionCompiler::compExpr(
                *forLoop.cond.value(), BOOL_TYPE))
        {
            std::cerr << "[Compile Error] ERR010 Expression Must Have Bool Type (Or Convertable To It)";
            exit(EXIT_FAILURE);
        }
        varCompiler::pop("rdx");
    }
    else
    {
        compiler::m_output << "\tmov rdx, 1\n";
    }
    compiler::m_output << "\tcmp rdx, 0\n";
    compiler::m_output << "\tje " << endLabel << "\n";
    compScope(forLoop.scope);
    compiler::m_output << "\t" << iterLabel << ":\n";
    if (forLoop.iterationStmt.has_value())
    {
        compiler::compStmt(*forLoop.iterationStmt.value());
    }
    compiler::m_output << "\txor rdx, rdx\n";
    compiler::m_output << "\tjmp " << startLabel << "\n";
    compiler::m_output << "\t" << endLabel << ":\n";
    if (const size_t popCount = varCompiler::m_stackSize - beginStackSize; popCount != 0)
    {
        compiler::m_output << "\tadd rsp, " << popCount * 8 << "\n";
        varCompiler::m_stackSize -= popCount;
    }
    varCompiler::m_vars = beginVars;
    compiler::m_output << "\txor rdx, rdx\n";
    lastLoopBegin.pop_back();
    lastLoopEnd.pop_back();
    compiler::m_output << ";;\t/for loop\n";
}

void scopeCompiler::compWhileLoop(const node::StmtWhileLoop &whileLoop)
{
    compiler::m_output << ";;\twhile loop\n";
    const std::string &startLabel = compiler::createLabel();
    const std::string &endLabel = compiler::createLabel();
    lastLoopBegin.push_back(startLabel);
    lastLoopEnd.push_back(endLabel);
    compiler::m_output << "\t" << startLabel << ":\n";
    if (!expressionCompiler::compExpr(*whileLoop.cond, INT_TYPE, false) && !expressionCompiler::compExpr(*whileLoop.cond, FLOAT_TYPE, false)
        &&
        !expressionCompiler::compExpr(*whileLoop.cond, CHAR_TYPE, false) && !expressionCompiler::compExpr(*whileLoop.cond, BOOL_TYPE))
    {
        std::cerr << "[Compile Error] ERR010 Expression Must Have Bool Type (Or Convertable To It)";
        exit(EXIT_FAILURE);
    }
    varCompiler::pop("rdx");
    compiler::m_output << "\tcmp rdx, 0\n";
    compiler::m_output << "\tje " << endLabel << "\n";
    compScope(whileLoop.scope);
    compiler::m_output << "\txor rdx, rdx\n";
    compiler::m_output << "\tjmp " << startLabel << "\n";
    compiler::m_output << "\t" << endLabel << ":\n";
    lastLoopBegin.pop_back();
    lastLoopEnd.pop_back();
    compiler::m_output << ";;\t/while loop\n";
}

void scopeCompiler::compBreakStmt()
{
    if (lastLoopEnd.empty())
    {
        std::cerr << "[Compile Error] ERR13 You Can Use 'break' Only Within Loops Or Switch-Case Statements";
        exit(EXIT_FAILURE);
    }
    compiler::m_output << "\tjmp " << lastLoopEnd[lastLoopEnd.size()-1] << "\n";
}

void scopeCompiler::compContinueStmt()
{
    if (lastLoopBegin.empty())
    {
        std::cerr << "[Compile Error] ERR13 You Can Use 'continue' Only Within Loops";
        exit(EXIT_FAILURE);
    }
    compiler::m_output << "\tjmp " << lastLoopBegin[lastLoopBegin.size()-1] << "\n";
}