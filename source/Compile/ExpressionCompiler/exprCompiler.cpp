#include "exprCompiler.hpp"

#include <utility>

bool expressionCompiler::compValExpr(const node::ValExpr &expr, const std::string &expectedType, const bool &isConvertable)
{
    struct exprVisitor
    {
        std::string expectedType;
        bool isConvertable;

        explicit exprVisitor(std::string expectedType, const bool &isConvertable) : expectedType(std::move(expectedType)),
                                                                                    isConvertable(isConvertable)
        {
        }

        bool operator()(const node::ExprIdent &exprIdent) const
        {
            if (!varCompiler::m_vars.contains(exprIdent.ident.value.value()))
            {
                std::cerr << "[Compile Error] ERR005 Undeclared Identifier '" << exprIdent.ident.value.value() << "'";
                exit(EXIT_FAILURE);
            }
            if (auto const &var = varCompiler::m_vars[exprIdent.ident.value.value()]; expectedType == var.Type)
            {
                compiler::m_output << "\tmov rdx, QWORD [rsp + " + std::to_string((varCompiler::m_stackSize - var.stackLoc - 1) * 8) +
                    "]\n";
            }
            else if (isConvertable)
            {
                if (var.Type == FLOAT_TYPE && expectedType == INT_TYPE)
                {
                    floatToInt("QWORD [rsp + " + std::to_string((varCompiler::m_stackSize - var.stackLoc - 1) * 8) + "]");
                }
                else if (var.Type == INT_TYPE && expectedType == FLOAT_TYPE)
                {
                    intToFloat("QWORD [rsp + " + std::to_string((varCompiler::m_stackSize - var.stackLoc - 1) * 8) + "]");
                }
                else if (expectedType == BOOL_TYPE)
                {
                    if (var.Type == FLOAT_TYPE)
                    {
                        compiler::m_output << "\tmovq xmm0, "
                            << "QWORD [rsp + " + std::to_string((varCompiler::m_stackSize - var.stackLoc - 1) * 8) + "]" << "rax\n";
                        compBoolExprFloat32({"xmm0"}, false, true);
                    }
                    else if (var.Type == CHAR_TYPE || var.Type == BOOL_TYPE || var.Type == INT_TYPE)
                    {
                        compBoolExpr({"QWORD [rsp + " + std::to_string((varCompiler::m_stackSize - var.stackLoc - 1) * 8) + "]"});
                    }
                    else
                    {
                        return false;
                    }
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
            varCompiler::push("rdx");
            return true;
        }

        bool operator()(const node::ExprIntLit &exprInt) const
        {
            if (expectedType == INT_TYPE)
            {
                if ((exprInt.intLit.value.has_value()))
                {
                    compiler::m_output << "\tmov rdx, " << exprInt.intLit.value.value() << '\n';
                }
            }
            else if (isConvertable)
            {
                if (expectedType == FLOAT_TYPE)
                {
                    compiler::m_output << ((exprInt.intLit.value.has_value()) ? "\tmov rdx, " + exprInt.intLit.value.value() + "\n" : "");
                    intToFloat("rdx");
                }
                else if (expectedType == BOOL_TYPE)
                {
                    compBoolExpr(exprInt.intLit.value);
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
            varCompiler::push("rdx");
            compiler::m_output << "\txor rdx, rdx\n";
            return true;
        }

        bool operator()(const node::ExprCharLit &exprChar) const
        {
            if (expectedType == CHAR_TYPE)
            {
                if ((exprChar.charLit.value.has_value()))
                {
                    compiler::m_output << "\tmov rdx, '" << exprChar.charLit.value.value() << "'\n";
                }
                varCompiler::push("rdx");
                compiler::m_output << "\txor rdx, rdx\n";
            }
            else if (isConvertable)
            {
                if (expectedType == BOOL_TYPE)
                {
                    compBoolExpr({'\'' + exprChar.charLit.value.value() + '\''});
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
            return true;
        }

        bool operator()(const node::ExprStrLit &exprStr) const
        {
            if (expectedType != STR_TYPE)
            {
                return false;
            }
            if ((exprStr.strLit.value.has_value()))
            {
                const std::string SC = compiler::createSCLabel();
                compiler::m_SC << SC << ": db '" << exprStr.strLit.value.value() << "',00H\n";
                compiler::m_output << "\tmov rdx, " << SC << '\n';
            }
            varCompiler::push("rdx");
            compiler::m_output << "\txor rdx, rdx\n";
            return true;
        }

        bool operator()(const node::ExprBoolLit &exprBool) const
        {
            if (expectedType != BOOL_TYPE)
            {
                return false;
            }
            if ((exprBool.boolLit.value.has_value()))
            {
                compiler::m_output << "\tmov rdx, " + std::to_string(exprBool.boolLit.value.value() == "true") + "\n";
            }
            varCompiler::push("rdx");
            compiler::m_output << "\txor rdx, rdx\n";
            return true;
        }

        bool operator()(const node::ExprFloatLit &exprFloat) const
        {
            if (expectedType == FLOAT_TYPE)
            {
                if (exprFloat.floatLit.value.has_value())
                {
                    compiler::m_output << "\tmov rdx,__?float32?__(" + exprFloat.floatLit.value.value() + ")\n";
                }
            }
            else if (isConvertable)
            {
                if (expectedType == INT_TYPE)
                {
                    compiler::m_output
                        << ((exprFloat.floatLit.value.has_value())
                                ? "\tmov rdx, __?float32?__(" + exprFloat.floatLit.value.value() + ")\n"
                                : "");
                    floatToInt("rdx");
                }
                else if (expectedType == BOOL_TYPE)
                {
                    compBoolExprFloat32(exprFloat.floatLit.value);
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
            varCompiler::push("rdx");
            compiler::m_output << "\txor rdx, rdx\n";
            return true;
        }

        bool operator()(const node::NotCondition &exprNotCond) const
        {
            if (expectedType == STR_TYPE)
            {
                return false;
            }
            if (!compValExpr(*exprNotCond.val, expectedType, isConvertable))
            {
                return false;
            }
            varCompiler::pop("rdx");
            compBoolExpr("rdx", true);
            return true;
        }

        bool operator()(const node::IncDec &incDec) const
        {
            if (expectedType != INT_TYPE)
            {
                return false;
            }
            if (!incDec.isPref)
            {
                if (!compValExpr(node::ValExpr{node::ExprIdent{incDec.ident}}, expectedType, isConvertable))
                {
                    return false;
                }
            }
            compIncDec(incDec.ident, incDec.isInc, expectedType);
            if (incDec.isPref)
            {
                return compValExpr(node::ValExpr{node::ExprIdent{incDec.ident}}, expectedType, isConvertable);
            }
            return true;
        }
    };
    exprVisitor visitor(expectedType, isConvertable);
    return std::visit(visitor, expr.var);
}

void expressionCompiler::compBoolExpr(const std::optional<std::string> &literal, const bool &isReversed)
{
    const std::string &endLabel = compiler::createLabel();
    const std::string &falseLabel = compiler::createLabel();
    if (literal.has_value())
    {
        compiler::m_output << "\tmov rdx, " << literal.value() << '\n';
    }
    compiler::m_output << "\tcmp rdx, 0\n";
    compiler::m_output << (isReversed ? "\tjne " : "\tje ");
    compiler::m_output << falseLabel << "\n";
    compiler::m_output << "\tmov rdx, 1\n";
    compiler::m_output << "\tjmp " << endLabel << "\n";
    compiler::m_output << "\t" << falseLabel << ":\n";
    compiler::m_output << "\tmov rdx, 0\n";
    compiler::m_output << "\t" << endLabel << ":\n";
    varCompiler::push("rdx");
    compiler::m_output << "\txor rdx, rdx\n";
}

void expressionCompiler::compBoolExprFloat32(const std::optional<std::string> &literal, const bool &isReversed, const bool &isRegister)
{
    const std::string &endLabel = compiler::createLabel();
    const std::string &falseLabel = compiler::createLabel();
    if (literal.has_value())
    {
        if (isRegister)
        {
            compiler::m_output << "\tmovq rdx, " << literal.value() << "\n";
        }
        else
        {
            compiler::m_output << "\tmov rdx,__?float32?__(" << literal.value() << ")\n";
        }
    }
    compiler::m_output << "\tmovq xmm0, rdx\n";
    compiler::m_output << "\tmov rdx, 0\n";
    compiler::m_output << "\tcvtsi2ss xmm1, rdx\n";
    compiler::m_output << "\tcomiss xmm1, xmm0\n";
    compiler::m_output << "\tje " << falseLabel << "\n";
    compiler::m_output << "\tmov rdx, 1\n";
    compiler::m_output << "\tjmp " << endLabel << "\n";
    compiler::m_output << "\t" << falseLabel << ":\n";
    compiler::m_output << "\tmov rdx, 0\n";
    compiler::m_output << "\t" << endLabel << ":\n";
    varCompiler::push("rdx");
    compiler::m_output << "\txor rdx, rdx\n";
}

bool expressionCompiler::compBinExpr(const node::BinExpr &expr, const std::string &expectedType, const bool &isConvertable)
{
    if (compExpr(*expr.fvl, STR_TYPE) || compExpr(*expr.svl, STR_TYPE))
    {
        std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
        exit(EXIT_FAILURE);
    }
    const Tokens &op = expr.oper;
    if (op == PLUS || op == MINUS || op == MULT || op == DIV)
    {
        if (compExpr(*expr.fvl, CHAR_TYPE) || compExpr(*expr.svl, CHAR_TYPE))
        {
            std::cerr << "[Compile Error] ERR009 Arithmetic Operations Cannot Be Used With Char Constants";
            exit(EXIT_FAILURE);
        }
    }

    if (!isConvertable)
    {
        if (!compExpr(*expr.fvl, expectedType, isConvertable))
        {
            if (!compExpr(*expr.svl, expectedType, isConvertable))
            {
                return false;
            }
            compExpr(*expr.fvl, expectedType);
            varCompiler::pop("rdi");
            varCompiler::pop("rdx");
            varCompiler::push("rdi");
            varCompiler::push("rdx");
        }
        else if (!compExpr(*expr.svl, expectedType, isConvertable))
        {
            if (!compExpr(*expr.fvl, expectedType, isConvertable))
            {
                return false;
            }
            compExpr(*expr.svl, expectedType);
        }
    }
    else if (!compExpr(*expr.fvl, expectedType) || !compExpr(*expr.svl, expectedType))
    {
        return false;
    }
    varCompiler::pop("rdi");
    varCompiler::pop("rdx");
    if (expectedType == FLOAT_TYPE)
    {
        compiler::m_output << "\tmovq xmm0, rdx\n";
        compiler::m_output << "\tmovq xmm1, rdi\n";
    }
    const std::string &trueLabel = compiler::createLabel();
    const std::string &endLabel = compiler::createLabel();
    if (op == Tokens::PLUS)
    {
        compiler::m_output << ((expectedType == FLOAT_TYPE) ? "\taddss xmm0, xmm1\n" : "\tadd rdx, rdi\n");
    }
    else if (op == Tokens::MINUS)
    {
        compiler::m_output << ((expectedType == FLOAT_TYPE) ? "\tsubss xmm0, xmm1\n" : "\tsub rdx, rdi\n");
    }
    else if (op == Tokens::MULT)
    {
        compiler::m_output << ((expectedType == FLOAT_TYPE) ? "\tmulss xmm0, xmm1\n" : "\timul rdx, rdi\n");
    }
    else if (op == Tokens::DIV)
    {
        if (expectedType == FLOAT_TYPE)
        {
            compiler::m_output << "\tdivss xmm0, xmm1\n";
        }
        else
        {
            compiler::m_output << "\tmov rax, rdx\n";
            compiler::m_output << "\txor rdx, rdx\n";
            compiler::m_output << "\tcqo\n";
            compiler::m_output << "\tidiv rdi\n";
            compiler::m_output << "\tmov rdx, rax\n";
        }
    }
    else if (op == Tokens::EQEQ)
    {
        compiler::m_output << ((expectedType == FLOAT_TYPE) ? "\tcomiss xmm0, xmm1\n" : "\tcmp rdx, rdi\n");
        compiler::m_output << "\tje " << trueLabel << "\n";
        compiler::m_output << "\tmov rdx, 0\n";
        compiler::m_output << "\tjmp " << endLabel << "\n";
        compiler::m_output << "\t" << trueLabel << ":\n";
        compiler::m_output << "\tmov rdx, 1\n";
        compiler::m_output << "\t" << endLabel << ":\n";
    }
    else if (op == Tokens::NOTEQ)
    {
        compiler::m_output << ((expectedType == FLOAT_TYPE) ? "\tcomiss xmm0, xmm1\n" : "\tcmp rdx, rdi\n");
        compiler::m_output << "\tjne " << trueLabel << "\n";
        compiler::m_output << "\tmov rdx, 0\n";
        compiler::m_output << "\tjmp " << endLabel << "\n";
        compiler::m_output << "\t" << trueLabel << ":\n";
        compiler::m_output << "\tmov rdx, 1\n";
        compiler::m_output << "\t" << endLabel << ":\n";
    }
    else if (op == Tokens::LESS)
    {
        compiler::m_output << ((expectedType == FLOAT_TYPE) ? "\tcomiss xmm0, xmm1\n" : "\tcmp rdx, rdi\n");
        compiler::m_output << "\tjb " << trueLabel << "\n";
        compiler::m_output << "\tmov rdx, 0\n";
        compiler::m_output << "\tjmp " << endLabel << "\n";
        compiler::m_output << "\t" << trueLabel << ":\n";
        compiler::m_output << "\tmov rdx, 1\n";
        compiler::m_output << "\t" << endLabel << ":\n";
    }
    else if (op == Tokens::GREATER)
    {
        compiler::m_output << ((expectedType == FLOAT_TYPE) ? "\tcomiss xmm0, xmm1\n" : "\tcmp rdx, rdi\n");
        compiler::m_output << "\tja " << trueLabel << "\n";
        compiler::m_output << "\tmov rdx, 0\n";
        compiler::m_output << "\tjmp " << endLabel << "\n";
        compiler::m_output << "\t" << trueLabel << ":\n";
        compiler::m_output << "\tmov rdx, 1\n";
        compiler::m_output << "\t" << endLabel << ":\n";
    }
    else if (op == Tokens::LESSEQ)
    {
        compiler::m_output << ((expectedType == FLOAT_TYPE) ? "\tcomiss xmm0, xmm1\n" : "\tcmp rdx, rdi\n");
        compiler::m_output << "\tjbe " << trueLabel << "\n";
        compiler::m_output << "\tmov rdx, 0\n";
        compiler::m_output << "\tjmp " << endLabel << "\n";
        compiler::m_output << "\t" << trueLabel << ":\n";
        compiler::m_output << "\tmov rdx, 1\n";
        compiler::m_output << "\t" << endLabel << ":\n";
    }
    else if (op == Tokens::GREATEQ)
    {
        compiler::m_output << ((expectedType == FLOAT_TYPE) ? "\tcomiss xmm0, xmm1\n" : "\tcmp rdx, rdi\n");
        compiler::m_output << "\tjae " << trueLabel << "\n";
        compiler::m_output << "\tmov rdx, 0\n";
        compiler::m_output << "\tjmp " << endLabel << "\n";
        compiler::m_output << "\t" << trueLabel << ":\n";
        compiler::m_output << "\tmov rdx, 1\n";
        compiler::m_output << "\t" << endLabel << ":\n";
    }
    else if (op == Tokens::AND)
    {
        compiler::m_output << (expectedType == FLOAT_TYPE ? "\tmov rdx,__?float32?__(0.0)\n\tmovq xmm2, rdx\n" : "");
        const std::string &falseLabel = compiler::createLabel();
        compiler::m_output << (expectedType == FLOAT_TYPE ? "\tcomiss xmm0, xmm2\n" : "\tcmp rdx, 0\n");
        compiler::m_output << "\tje " << falseLabel << "\n";
        compiler::m_output << ((expectedType == FLOAT_TYPE) ? "\tcomiss xmm1, xmm2\n" : "\tcmp rdi, 0\n");
        compiler::m_output << "\tje " << falseLabel << "\n";
        compiler::m_output << "\tmov rdx, 1\n";
        compiler::m_output << "\tjmp " << endLabel << "\n";
        compiler::m_output << "\t" << falseLabel << ":\n";
        compiler::m_output << "\tmov rdx, 0\n";
        compiler::m_output << "\t" << endLabel << ":\n";
    }
    else if (op == Tokens::OR)
    {
        compiler::m_output << ((expectedType == FLOAT_TYPE) ? "\tmov rdx,__?float32?__(0.0)\n\tmovq xmm2, rdx\n" : "");
        compiler::m_output << ((expectedType == FLOAT_TYPE) ? "\tcomiss xmm0, xmm2\n" : "\tcmp rdx, 0\n");
        compiler::m_output << "\tjne " << trueLabel << "\n";
        compiler::m_output << ((expectedType == FLOAT_TYPE) ? "\tcomiss xmm1, xmm2\n" : "\tcmp rdi, 0\n");
        compiler::m_output << "\tjne " << trueLabel << "\n";
        compiler::m_output << "\tmov rdx, 0\n";
        compiler::m_output << "\tjmp " << endLabel << "\n";
        compiler::m_output << "\t" << trueLabel << ":\n";
        compiler::m_output << "\tmov rdx, 1\n";
        compiler::m_output << "\t" << endLabel << ":\n";
    }
    if (expectedType == FLOAT_TYPE && (op == Tokens::PLUS || op == Tokens::MINUS || op == Tokens::MULT || op == Tokens::DIV))
    {
        compiler::m_output << "\tmovq rdx, xmm0\n";
    }
    varCompiler::push("rdx");
    compiler::m_output << "\txor rdx, rdx\n";
    compiler::m_output << "\txor rdi, rdi\n";
    return true;
}

bool expressionCompiler::compExpr(const node::Expr &expr, const std::string &expectedType, const bool &isConvertable)
{
    struct exprVisitor
    {
        std::string expectedType;
        bool isConvertable;

        explicit exprVisitor(std::string expectedType, const bool &isConvertable) : expectedType(std::move(expectedType)),
                                                                                    isConvertable(isConvertable)
        {
        }

        bool operator()(const node::BinExpr *binExpr) const
        {
            return compBinExpr(*binExpr, expectedType, isConvertable);
        }

        bool operator()(const node::ValExpr *valExpr) const
        {
            return compValExpr(*valExpr, expectedType, isConvertable);
        }

        bool operator()(const node::StmtInput *inputExpr) const
        {
            compiler::m_output << ";;\tInput\n";
            compiler::compInput(*inputExpr);
            const std::string &trueLabel1 = compiler::createLabel();
            const std::string &endLabel1 = compiler::createLabel();
            const std::string &trueLabel2 = compiler::createLabel();
            const std::string &endLabel2 = compiler::createLabel();
            compiler::m_output << "\tmov rdx, rsi\n";
            compiler::m_output << "\tcall _countStrLen\n";
            compiler::m_output << "\tcmp byte [rdx+rcx-1], 10\n";
            compiler::m_output << "\tje " << trueLabel1 << "\n";
            compiler::m_output << "\tjmp " << endLabel1 << "\n";
            compiler::m_output << "\t" << trueLabel1 << ":\n";
            compiler::m_output << "\tmov byte [rdx+rcx-1], 00H\n";
            compiler::m_output << "\t" << endLabel1 << ":\n";
            compiler::m_output << "\tcmp byte [rdx+rcx-2], 13\n";
            compiler::m_output << "\tje " << trueLabel2 << "\n";
            compiler::m_output << "\tjmp " << endLabel2 << "\n";
            compiler::m_output << "\t" << trueLabel2 << ":\n";
            compiler::m_output << "\tmov byte [rdx+rcx-2], 00H\n";
            compiler::m_output << "\t" << endLabel2 << ":\n";
            if (expectedType == STR_TYPE)
            {
                const std::string &SC = compiler::createSCLabel();
                compiler::m_bssSC << "\t" << SC << " resb 256\n";
                compiler::m_output << "\tmov rdi, " << SC << "\n";
                compiler::m_output << "\tmov rcx, 256\n";
                compiler::m_output << "\trep movsb\n";
                compiler::m_output << "\tmov rdx, " << SC << "\n";
            }
            else if (expectedType == CHAR_TYPE)
            {
                compiler::m_output << "\tmovzx rdx, byte [rsi]\n";
            }
            else if (expectedType == INT_TYPE)
            {
                compiler::m_output << "\tcall _stoi\n";
                compiler::m_output << "\tmov rdx, rdi\n";
            }
            else if (expectedType == FLOAT_TYPE || expectedType == BOOL_TYPE)
            {
                compiler::m_output << "\tcall _stof\n";
                if (expectedType == BOOL_TYPE)
                {
                    compBoolExprFloat32("xmm0", false, true);
                }
                else
                {
                    compiler::m_output << "\tmovq rdx, xmm0\n";
                }
            }
            else
            {
                return false;
            }
            varCompiler::push("rdx");
            compiler::m_output << "\tmov rsi, OutputBuffer\n";
            compiler::m_output << "\tmov rdx, 20\n";
            compiler::m_output << "\tcall _clearBuffer\n";
            compiler::m_output << "\tmov rsi, InputBuffer\n";
            compiler::m_output << "\tmov rdx, 256\n";
            compiler::m_output << "\tcall _clearBuffer\n";
            compiler::m_output << ";;\t/Input\n";
            return true;
        }
    };
    exprVisitor visitor(expectedType, isConvertable);
    return std::visit(visitor, expr.var);
}

void expressionCompiler::compIncDec(const Token &ident, const bool &isInc, const std::string &expectedType)
{
    auto *fvl = new node::Expr{new node::ValExpr{node::ExprIdent{ident}}};
    auto *svl = new node::Expr{new node::ValExpr{node::ExprIntLit{INT_LITERAL, "1"}}};
    if (isInc)
    {
        varCompiler::compVar({ident, new node::Expr{new node::BinExpr{fvl, svl, Tokens::PLUS}}, expectedType});
    }
    else
    {
        varCompiler::compVar({ident, new node::Expr{new node::BinExpr{fvl, svl, Tokens::MINUS}}, expectedType});
    }
}

void expressionCompiler::intToFloat(const std::string &reg)
{
    compiler::m_output << "\tmov rdx, " << reg << '\n';
    compiler::m_output << "\tcvtsi2ss xmm0, rdx\n";
    compiler::m_output << "\tmovq rdx, xmm0\n";
}

void expressionCompiler::floatToInt(const std::string &reg)
{
    compiler::m_output << "\tmov rdx, " << reg << '\n';
    compiler::m_output << "\tmovq xmm0, rdx\n";
    compiler::m_output << "\tcvttss2si rdx, xmm0\n";
}
