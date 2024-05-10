#include "compile.hpp"

bool createObjectFile(const std::string& path)
{
    int res = system(("nasm -fwin64 " + path + ".asm").c_str());
    if (res)
    {
        std::cerr << "Error Creating Object File";
        exit(EXIT_FAILURE);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return true;
}



bool linkObjectFiles(const std::string& path)
{

    int res = system(("gcc.exe " + path + ".obj -g -o" + path + ".exe -m64").c_str());
    if (res)
    {
        std::cerr << "Error Linking File";
        exit(EXIT_FAILURE);
    }
    return true;
}

std::string compiler::createLabel()
{
    std::stringstream ss;
    ss << "label" << m_labelCount;
    m_labelCount++;
    return ss.str();
}

std::string compiler::createSCLabel()
{
    std::stringstream ss;
    ss << "SC" << m_SCCount;
    m_SCCount++;
    return ss.str();
}

size_t compiler::m_stackSize;
std::unordered_map<std::string, compiler::Var> compiler::m_vars;
uint64_t compiler::m_labelCount = 0;
uint64_t compiler::m_SCCount = 0;

std::stringstream compiler::m_output;
std::stringstream compiler::m_SC;
std::stringstream compiler::m_bssSC;

bool compiler::compValExpr(const node::ValExpr &expr, const std::string &expectedType)
{
    struct exprVisitor
    {
        std::string expectedType;

        explicit exprVisitor(std::string expectedType) : expectedType(std::move(expectedType)) {}

        bool operator()(const node::ExprIdent &exprIdent) const
        {
            if (!m_vars.count(exprIdent.ident.value.value()))
            {
	            std::cerr << "[Compile Error] ERR005 Undeclared Identifier '" << exprIdent.ident.value.value() << "'";
	            exit(EXIT_FAILURE);
			}
            const auto &var = m_vars[exprIdent.ident.value.value()];
            if (expectedType == var.Type)
            {
                push("QWORD [rsp + " + std::to_string((m_stackSize - var.stackLoc - 1) * 8) + "]");
            }
            else if (expectedType == BOOL_TYPE && var.Type != STR_TYPE)
            {
                compBoolExpr({"QWORD [rsp + " + std::to_string((m_stackSize - var.stackLoc - 1) * 8) + "]"});
            }
            else
            {
                return false;
            }
	        return true;
        }
        bool operator()(const node::ExprIntLit &exprInt) const
        {
            if (expectedType == INT_TYPE)
            {
                if ((exprInt.intLit.value.has_value()))
                {
                    m_output << "\tmov rdx, " << exprInt.intLit.value.value() << '\n';
                }
                push("rdx");
                m_output << "\txor rdx, rdx\n";
            }
            else if (expectedType == BOOL_TYPE)
            {
                compBoolExpr(exprInt.intLit.value);
            }
            else
            {
                return false;
            }
			return true;
        }
        bool operator()(const node::ExprCharLit &exprChar) const
        {
            if (expectedType == CHAR_TYPE)
            {
                if ((exprChar.charLit.value.has_value()))
                {
                    m_output << "\tmov rdx, '" << exprChar.charLit.value.value() << "'\n";
                }
                push("rdx");
                m_output << "\txor rdx, rdx\n";
            }
            else if (expectedType == BOOL_TYPE)
            {
                compBoolExpr({'\'' + exprChar.charLit.value.value() + '\''});
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
				std::string SC = createSCLabel();
				m_SC << SC << ": db '" << exprStr.strLit.value.value() << "',00H\n";
				m_output << "\tmov rdx, " << SC << '\n';
			}
			push("rdx");
			m_output << "\txor rdx, rdx\n";
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
                if (exprBool.boolLit.value.value() == "false")
                {
                    m_output << "\tmov rdx, 0\n";
                }
                else if (exprBool.boolLit.value.value() == "true")
                {
                    m_output << "\tmov rdx, 1\n";
                }
            }
            push("rdx");
            m_output << "\txor rdx, rdx\n";
			return true;
        }
	    bool operator()(const node::NotCondition &exprNotCond) const
	    {
			if (!compValExpr(*exprNotCond.val, expectedType))
			{
				return false;
			}
			pop("rdx");
			compBoolExpr("rdx", true);
			return true;
		}
    };

    exprVisitor visitor(expectedType);
    return std::visit(visitor, expr.var);
}

void compiler::compBoolExpr(const std::optional<std::string> &literal, bool isReversed)
{
    std::string endLabel = createLabel();
    std::string falseLabel = createLabel();
    if ((literal.has_value()))
    {
        m_output << "\tmov rdx, " << literal.value() << '\n';
    }
    m_output << "\tcmp rdx, 0\n";
	m_output << (isReversed ? "\tjne " : "\tje ");
	m_output << falseLabel << "\n";
    m_output << "\tmov rdx, 1\n";
    m_output << "\tjmp " << endLabel << "\n";
    m_output << "\t" << falseLabel << ":\n";
    m_output << "\tmov rdx, 0\n";
    m_output << "\t" << endLabel << ":\n";
    push("rdx");
    m_output << "\txor rdx, rdx\n";
}

bool compiler::compBinExpr(const node::BinExpr &expr, const std::string &expectedType)
{
    struct exprVisitor
    {
        std::string expectedType;

        explicit exprVisitor(std::string expectedType) : expectedType(std::move(expectedType)) {}

        bool operator()(const node::BinExprAdd *binExprAdd) const
        {
            if (compExpr(*binExprAdd->fvl, STR_TYPE) || compExpr(*binExprAdd->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            else if (compExpr(*binExprAdd->fvl, CHAR_TYPE) || compExpr(*binExprAdd->svl, CHAR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Arithmetic Operations Cannot Be Used With Char Constants";
                exit(EXIT_FAILURE);
            }
            if (compExpr(*binExprAdd->fvl, expectedType) && compExpr(*binExprAdd->svl, expectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tadd rdx, rdi\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return true;
            }
            else
            {
                return false;
            }
        }
        bool operator()(const node::BinExprSub *binExprSub) const
        {
            if (compExpr(*binExprSub->fvl, STR_TYPE) || compExpr(*binExprSub->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            else if (compExpr(*binExprSub->fvl, CHAR_TYPE) || compExpr(*binExprSub->svl, CHAR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Arithmetic Operations Cannot Be Used With Char Constants";
                exit(EXIT_FAILURE);
            }
            if (compExpr(*binExprSub->fvl, expectedType) && compExpr(*binExprSub->svl, expectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tsub rdx, rdi\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return true;
            }
            else
            {
                return false;
            }
        }
        bool operator()(const node::BinExprMul *binExprMul) const
        {
            if (compExpr(*binExprMul->fvl, STR_TYPE) || compExpr(*binExprMul->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            else if (compExpr(*binExprMul->fvl, CHAR_TYPE) || compExpr(*binExprMul->svl, CHAR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Arithmetic Operations Cannot Be Used With Char Constants";
                exit(EXIT_FAILURE);
            }
            if (compExpr(*binExprMul->fvl, expectedType) && compExpr(*binExprMul->svl, expectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\timul rdx, rdi\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return true;
            }
            else
            {
                return false;
            }
        }
        bool operator()(const node::BinExprDiv *binExprDiv) const
        {
            if (compExpr(*binExprDiv->fvl, STR_TYPE) || compExpr(*binExprDiv->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            else if (compExpr(*binExprDiv->fvl, CHAR_TYPE) || compExpr(*binExprDiv->svl, CHAR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Arithmetic Operations Cannot Be Used With Char Constants";
                exit(EXIT_FAILURE);
            }
            if (compExpr(*binExprDiv->fvl, expectedType) && compExpr(*binExprDiv->svl, expectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tmov rax, rdx\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tidiv rdi\n";
                m_output << "\tmov rdx, rax\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return true;
            }
            else
            {
                return false;
            }
        }
        bool operator()(const node::EQCondition *binEqCond) const
        {
            if (compExpr(*binEqCond->fvl, STR_TYPE) || compExpr(*binEqCond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string trueLabel = createLabel();
            std::string endLabel = createLabel();
            if (compExpr(*binEqCond->fvl, expectedType) && compExpr(*binEqCond->svl, expectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, rdi\n";
                m_output << "\tje " << trueLabel << "\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tjmp " << endLabel << "\n";
                m_output << "\t" << trueLabel << ":\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\t" << endLabel << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return true;
            }
            else
            {
                return false;
            }
        }
        bool operator()(const node::NotEQCondition *binNotEqCond) const
        {
            if (compExpr(*binNotEqCond->fvl, STR_TYPE) || compExpr(*binNotEqCond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string trueLabel = createLabel();
            std::string endLabel = createLabel();
            if (compExpr(*binNotEqCond->fvl, expectedType) && compExpr(*binNotEqCond->svl, expectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, rdi\n";
                m_output << "\tjne " << trueLabel << "\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tjmp " << endLabel << "\n";
                m_output << "\t" << trueLabel << ":\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\t" << endLabel << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return true;
            }
            else
            {
                return false;
            }
        }
        bool operator()(const node::LessCondition *binLessCond) const
        {
            if (compExpr(*binLessCond->fvl, STR_TYPE) || compExpr(*binLessCond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string trueLabel = createLabel();
            std::string endLabel = createLabel();
            if (compExpr(*binLessCond->fvl, expectedType) && compExpr(*binLessCond->svl, expectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, rdi\n";
                m_output << "\tjl " << trueLabel << "\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tjmp " << endLabel << "\n";
                m_output << "\t" << trueLabel << ":\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\t" << endLabel << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return true;
            }
            else
            {
                return false;
            }
        }
        bool operator()(const node::GreaterCondition *binGreaterCond) const
        {
            if (compExpr(*binGreaterCond->fvl, STR_TYPE) || compExpr(*binGreaterCond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string trueLabel = createLabel();
            std::string endLabel = createLabel();
            if (compExpr(*binGreaterCond->fvl, expectedType) && compExpr(*binGreaterCond->svl, expectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, rdi\n";
                m_output << "\tjg " << trueLabel << "\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tjmp " << endLabel << "\n";
                m_output << "\t" << trueLabel << ":\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\t" << endLabel << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return true;
            }
            else
            {
                return false;
            }
        }
        bool operator()(const node::EQLessCondition *binLessEqCond) const
        {
            if (compExpr(*binLessEqCond->fvl, STR_TYPE) || compExpr(*binLessEqCond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string trueLabel = createLabel();
            std::string endLabel = createLabel();
            if (compExpr(*binLessEqCond->fvl, expectedType) && compExpr(*binLessEqCond->svl, expectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, rdi\n";
                m_output << "\tjle " << trueLabel << "\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tjmp " << endLabel << "\n";
                m_output << "\t" << trueLabel << ":\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\t" << endLabel << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return true;
            }
            else
            {
                return false;
            }
        }
        bool operator()(const node::EQGreaterCondition *binGreatEqCond) const
        {
            if (compExpr(*binGreatEqCond->fvl, STR_TYPE) || compExpr(*binGreatEqCond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string trueLabel = createLabel();
            std::string endLabel = createLabel();
            if (compExpr(*binGreatEqCond->fvl, expectedType) && compExpr(*binGreatEqCond->svl, expectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, rdi\n";
                m_output << "\tjge " << trueLabel << "\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tjmp " << endLabel << "\n";
                m_output << "\t" << trueLabel << ":\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\t" << endLabel << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return true;
            }
            else
            {
                return false;
            }
        }
        bool operator()(const node::AndCondition *binAndCond) const
        {
            if (compExpr(*binAndCond->fvl, STR_TYPE) || compExpr(*binAndCond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string falseLabel = createLabel();
            std::string endLabel = createLabel();
            if (compExpr(*binAndCond->fvl, expectedType) && compExpr(*binAndCond->svl, expectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, 0\n";
                m_output << "\tjle " << falseLabel << "\n";
                m_output << "\tcmp rdi, 0\n";
                m_output << "\tjle " << falseLabel << "\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\tjmp " << endLabel << "\n";
                m_output << "\t" << falseLabel << ":\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\t" << endLabel << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return true;
            }
            else
            {
                return false;
            }
        }
	    bool operator()(const node::OrCondition *binOrCond) const
	    {
		    if (compExpr(*binOrCond->fvl, STR_TYPE) || compExpr(*binOrCond->svl, STR_TYPE))
		    {
			    std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
			    exit(EXIT_FAILURE);
		    }
		    std::string trueLabel = createLabel();
		    std::string endLabel = createLabel();
		    if (compExpr(*binOrCond->fvl, expectedType) && compExpr(*binOrCond->svl, expectedType))
		    {
			    pop("rdi");
			    pop("rdx");
			    m_output << "\tcmp rdx, 0\n";
			    m_output << "\tjg " << trueLabel << "\n";
			    m_output << "\tcmp rdi, 0\n";
			    m_output << "\tjg " << trueLabel << "\n";
			    m_output << "\tmov rdx, 0\n";
			    m_output << "\tjmp " << endLabel << "\n";
			    m_output << "\t" << trueLabel << ":\n";
			    m_output << "\tmov rdx, 1\n";
			    m_output << "\t" << endLabel << ":\n";
			    push("rdx");
			    m_output << "\txor rdx, rdx\n";
			    m_output << "\txor rdi, rdi\n";
			    return true;
		    }
		    else
		    {
			    return false;
		    }
	    }
    };

    exprVisitor visitor(expectedType);
    return std::visit(visitor, expr.var);
}

bool compiler::compExpr(const node::Expr &expr, const std::string &expectedType)
{
    struct exprVisitor
    {
        std::string expectedType;

        explicit exprVisitor(std::string expectedType) : expectedType(std::move(expectedType)) {}

        bool operator()(const node::BinExpr *binExpr) const
        {
            return compBinExpr(*binExpr, expectedType);
        }
        bool operator()(const node::ValExpr *valExpr) const
        {
            return compValExpr(*valExpr, expectedType);
        }
        bool operator()(const node::StmtInput *inputExpr) const
        {
            m_output << ";;\tInput\n";
            compInput(*inputExpr);
            if (expectedType == STR_TYPE)
            {
                std::string falseLabel1 = createLabel();
                std::string trueLabel1 = createLabel();
                std::string endLabel1 = createLabel();
                std::string falseLabel2 = createLabel();
                std::string trueLabel2 = createLabel();
                std::string endLabel2 = createLabel();
                std::string SC = createSCLabel();
                m_bssSC << "\t" << SC << " resb 256\n";
                m_output << "\tmov rdi, " << SC << "\n";
                m_output << "\tmov rcx, 256\n";
                m_output << "\trep movsb\n";
                m_output << "\tmov rdx, " << SC << "\n";
                m_output << "\tcall _countStrLen\n";
                m_output << "\tcmp byte [rdx+rcx-1], 10\n";
                m_output << "\tje " << trueLabel1 << "\n";
                m_output << "\tjmp " << endLabel1 << "\n";
                m_output << "\t" << trueLabel1 << ":\n";
                m_output << "\tmov byte [rdx+rcx-1], 00H\n";
                m_output << "\t" << endLabel1 << ":\n";
                m_output << "\tcmp byte [rdx+rcx-2], 13\n";
                m_output << "\tje " << trueLabel2 << "\n";
                m_output << "\tjmp " << endLabel2 << "\n";
                m_output << "\t" << trueLabel2 << ":\n";
                m_output << "\tmov byte [rdx+rcx-2], 00H\n";
                m_output << "\t" << endLabel2 << ":\n";
                push("rdx");
            }
            else if (expectedType == CHAR_TYPE)
            {
                m_output << "\tmovzx rdx, byte [rsi]\n";
                push("rdx");
            }
            else if (expectedType == INT_TYPE || expectedType == BOOL_TYPE)
            {
                m_output << "\tcall _stoi\n";
                if (expectedType == BOOL_TYPE)
                {
                    compBoolExpr("rdi");
                }
                else
                {
                    push("rdi");
                }
            }
            else 
            {
                return false;
            }

            m_output << "\tmov rsi, OutputBuffer\n";
            m_output << "\tmov rdx, 20\n";
            m_output << "\tcall _clearBuffer\n";
            m_output << "\tmov rsi, InputBuffer\n";
            m_output << "\tmov rdx, 256\n";
            m_output << "\tcall _clearBuffer\n";
            m_output << ";;\t/Input\n";
            return true;
        }
    };

    exprVisitor visitor(expectedType);
    return std::visit(visitor, expr.var);
}

void compiler::compInput(const node::StmtInput &stmtInput)
{
    if (compExpr(*stmtInput.msg, STR_TYPE))
    {
        pop("rdx");
        m_output << "\tmov rsi, InputBuffer\n";
        m_output << "\tmov rax, 256\n";
        m_output << "\tcall _scanf\n";
    }
    else if (compExpr(*stmtInput.msg, CHAR_TYPE))
    {
        pop("rdx");
        m_output << "\tmov rsi, OutputBuffer\n";
        m_output << "\tmov [rsi], dx\n";
        m_output << "\tmov rdx, rsi\n";
        m_output << "\tmov rsi, InputBuffer\n";
        m_output << "\tmov rax, 256\n";
        m_output << "\tcall _scanf\n";
    }
    else if (compExpr(*stmtInput.msg, INT_TYPE) || compExpr(*stmtInput.msg, BOOL_TYPE))
    {
        pop("rdx");
        m_output << "\tmov rax, rdx\n";
        m_output << "\tmov rsi, OutputBuffer\n";
        m_output << "\tcall _itoa\n";
        m_output << "\tmov rdx, rsi\n";
        m_output << "\tmov rsi, InputBuffer\n";
        m_output << "\tmov rax, 256\n";
        m_output << "\tcall _scanf\n";
    }
}

void compiler::compIfPred(const node::IfPred &pred, const std::string &endLabel)
{
    struct predVisitor
    {
        std::string endLabel;

        explicit predVisitor(std::string endLabel) : endLabel(std::move(endLabel)) {}

        void operator()(const node::StmtElIf *elIf) const
        {
            m_output << ";;\telif\n";
            std::string falseLabel = createLabel();
            if (!compExpr(*elIf->Cond, INT_TYPE) && !compExpr(*elIf->Cond, CHAR_TYPE) &&
                !compExpr(*elIf->Cond, BOOL_TYPE))
            {
	            std::cerr << "[Compile Error] ERR010 Expression Must Have Bool Type (Or Convertable To It)";
	            exit(EXIT_FAILURE);
			}
            pop("rdx");
            m_output << "\tcmp rdx, 0\n";
            m_output << "\tje " << falseLabel << "\n";
            for (auto const &i : elIf->statements)
            {
                compStmt(i);
            }
			m_output << "\tjmp " << endLabel << "\n";
            m_output << ";;\t/elif\n";
            if (elIf->pred.has_value())
            {
                m_output << "\t" << falseLabel << ":\n";
                compIfPred(*elIf->pred.value(), endLabel);
            }
            else
            {
                m_output << falseLabel << ":\n";
            }
            m_output << "\txor rdx, rdx\n";
        }
        void operator()(const node::StmtElse *Else)
        {
            m_output << ";;\telse\n";
            for (auto const& i : Else->statements)
            {
                compStmt(i);
            }
            m_output << ";;\t/else\n";
        }
    };

    predVisitor visitor(endLabel);
    std::visit(visitor, pred.var);
}

void compiler::compVar(Token ident, node::Expr *expr, const std::string &expectedType)
{
    if (!m_vars.count(ident.value.value()))
    {
	    std::cerr << "[Compile Error] ERR004 Identifier '" << ident.value.value() << "' Was Not Declared";
	    exit(EXIT_FAILURE);
	}
    const auto &var = m_vars[ident.value.value()];
	if (var.isConst)
	{
		std::cerr << "[Compile Error] ERR012 Cannot Change Value Of Const Variables";
		exit(EXIT_FAILURE);
	}
    if (var.Type == BOOL_TYPE)
    {
        if (!compExpr(*expr, INT_TYPE) && !compExpr(*expr, CHAR_TYPE) && !compExpr(*expr, BOOL_TYPE))
        {
	        std::cerr << "[Compile Error] ERR010 Expression Must Have Bool Type (Or Convertable To It)";
	        exit(EXIT_FAILURE);
		}
        pop("rdx");
        compBoolExpr("rdx");
        pop("rdx");
        m_output << "\tmov [rsp + " + std::to_string((m_stackSize - var.stackLoc - 1) * 8) + "], rdx\n";
        m_output << "\txor rdx, rdx\n";
    }
    else if (var.Type == expectedType)
    {
        if (!compExpr(*expr, expectedType))
        {
	        std::cerr << "[Compile Error] ERR006 Value Doesnt Matches Type";
	        exit(EXIT_FAILURE);
		}
		pop("rdx");
		m_output << "\tmov [rsp + " + std::to_string((m_stackSize - var.stackLoc - 1) * 8) + "], rdx\n";
		m_output << "\txor rdx, rdx\n";
    }
    else
    {
        std::cerr << "[Compile Error] ERR006 Value Doesnt Matches Type";
        exit(EXIT_FAILURE);
    }
}

void compiler::compLet(Token ident, node::Expr *expr, const std::string &expectedType, bool isConst)
{
    if (m_vars.count(ident.value.value()))
    {
        std::cerr << "[Compile Error] ERR004 Identifier '" << ident.value.value() << "' Is Already Declared";
        exit(EXIT_FAILURE);
    }
    m_vars.insert({ident.value.value(), Var{m_stackSize, expectedType, isConst}});
    if (expectedType == BOOL_TYPE)
    {
        if (expr != nullptr)
        {
            if (!compExpr(*expr, INT_TYPE) && !compExpr(*expr, CHAR_TYPE) && !compExpr(*expr, BOOL_TYPE))
            {
	            std::cerr << "[Compile Error] ERR010 Expression Must Have Bool Type (Or Convertable To It)";
	            exit(EXIT_FAILURE);
			}
            pop("rdx");
            compBoolExpr("rdx");
        }
        else
        {
            push("rdx");
        }
    }
    else
    {
        if (expr != nullptr)
        {
            if (!compExpr(*expr, expectedType))
            {
                std::cerr << "[Compile Error] ERR006 Value Doesnt Matches Type";
                exit(EXIT_FAILURE);
            }
        }
        else
        {
			if (isConst)
			{
				std::cerr << "[Compile Error] ERR011 Const Variables Cannot Be Declared Without Value";
				exit(EXIT_FAILURE);
			}
            push("rdx");
        }
    }
}

void compiler::compStmt(const node::Stmt &stmt)
{
    struct stmtVisitor
    {
        void operator()(const node::StmtReturn &stmtRet)
        {
            m_output << ";;\treturn\n";
            if (!compExpr(*stmtRet.Expr, INT_TYPE) && !compExpr(*stmtRet.Expr, CHAR_TYPE) &&
				!compExpr(*stmtRet.Expr, BOOL_TYPE))
            {
	            std::cerr << "[Compile Error] ERR006 Value Doesnt Matches Type";
	            exit(EXIT_FAILURE);
			}
            pop("rcx");
            m_output << "\tcall ExitProcess"
                     << "\n";
            m_output << ";;\t/return\n";
        }
        void operator()(const node::StmtIf &stmtIf)
        {
            m_output << ";;\tif\n";
            std::string falseLabel = createLabel();
            if (!compExpr(*stmtIf.Cond, INT_TYPE) && !compExpr(*stmtIf.Cond, CHAR_TYPE) &&
                !compExpr(*stmtIf.Cond, BOOL_TYPE))
            {
	            std::cerr << "[Compile Error] ERR010 Expression Must Have Bool Type (Or Convertable To It)";
	            exit(EXIT_FAILURE);
			}
            pop("rdx");
            m_output << "\tcmp rdx, 0\n";
            m_output << "\tje " << falseLabel << "\n";
            for (auto const &i : stmtIf.statements)
            {
                compStmt(i);
            }
            m_output << ";;\t/if\n";
            if (stmtIf.pred.has_value())
            {
                std::string endLabel = createLabel();
                m_output << "\tjmp " << endLabel << "\n";
                m_output << "\t" << falseLabel << ":\n";
                compIfPred(*stmtIf.pred.value(), endLabel);
                m_output << "\t" << endLabel << ":\n";
            }
            else
            {
                m_output << "\t" << falseLabel << ":\n";
            }
            m_output << "\txor rdx, rdx\n";
        }
        void operator()(const node::StmtOutput &stmtOutput)
        {
            m_output << ";;\tOutput\n";
            if (compExpr(*stmtOutput.Expr, STR_TYPE))
            {
                pop("rdx");
                m_output << "\tcall _printf\n";
            }
            else if (compExpr(*stmtOutput.Expr, CHAR_TYPE))
            {
                pop("rdx");
                m_output << "\tmov rsi, OutputBuffer\n";
                m_output << "\tmov [rsi], dx\n";
                m_output << "\tmov rdx, rsi\n";
                m_output << "\tcall _printf\n";
                m_output << "\tmov rsi, OutputBuffer\n";
                m_output << "\tmov rdx, 20\n";
                m_output << "\tcall _clearBuffer\n";
            }
            else if (compExpr(*stmtOutput.Expr, INT_TYPE) || compExpr(*stmtOutput.Expr, BOOL_TYPE))
            {
                pop("rdx");
                m_output << "\tmov rax, rdx\n";
                m_output << "\tmov rsi, OutputBuffer\n";
                m_output << "\tcall _itoa\n";
                m_output << "\tmov rdx, rsi\n";
                m_output << "\tcall _printf\n";
                m_output << "\tmov rsi, OutputBuffer\n";
                m_output << "\tmov rdx, 20\n";
                m_output << "\tcall _clearBuffer\n";
            }
            m_output << ";;\t/Output\n";
        }
        void operator()(const node::StmtInput &stmtInput)
        {
            m_output << ";;\tInput\n";
            compInput(stmtInput);
            m_output << "\tmov rsi, OutputBuffer\n";
            m_output << "\tmov rdx, 20\n";
            m_output << "\tcall _clearBuffer\n";
            m_output << "\tmov rsi, InputBuffer\n";
            m_output << "\tmov rdx, 256\n";
            m_output << "\tcall _clearBuffer\n";
            m_output << ";;\t/Input\n";
        }
        void operator()(const node::StmtIntLet &stmtIntLet)
        {
            m_output << ";;\tint let\n";
            compLet(stmtIntLet.ident, stmtIntLet.Expr, INT_TYPE, stmtIntLet.isConst);
            m_output << ";;\t/int let\n";
        }
        void operator()(const node::StmtStrLet &stmtStrLet)
        {
            m_output << ";;\tstr let\n";
            compLet(stmtStrLet.ident, stmtStrLet.Expr, STR_TYPE, stmtStrLet.isConst);
            m_output << ";;\t/str let\n";
        }
        void operator()(const node::StmtBoolLet &stmtBoolLet)
        {
            m_output << ";;\tbool let\n";
            compLet(stmtBoolLet.ident, stmtBoolLet.Expr, BOOL_TYPE, stmtBoolLet.isConst);
            m_output << ";;\t/bool let\n";
        }
        void operator()(const node::StmtCharLet &stmtCharLet)
        {
            m_output << ";;\tchar let\n";
            compLet(stmtCharLet.ident, stmtCharLet.Expr, CHAR_TYPE, stmtCharLet.isConst);
            m_output << ";;\t/char let\n";
        }
        void operator()(const node::StmtIntVar &stmtIntVar)
        {
            compVar(stmtIntVar.ident, stmtIntVar.Expr, INT_TYPE);
        }
        void operator()(const node::StmtStrVar &stmtStrVar)
        {
            compVar(stmtStrVar.ident, stmtStrVar.Expr, STR_TYPE);
        }
        void operator()(const node::StmtBoolVar &stmtBoolVar)
        {
            compVar(stmtBoolVar.ident, stmtBoolVar.Expr, BOOL_TYPE);
        }
        void operator()(const node::StmtCharVar &stmtCharVar)
        {
            compVar(stmtCharVar.ident, stmtCharVar.Expr, CHAR_TYPE);
        }
    };
    stmtVisitor visitor;
    std::visit(visitor, stmt.var);
}

std::stringstream compiler::compile()
{
    m_output << "extern GetStdHandle, WriteConsoleA, ReadConsoleA, ExitProcess\n\n"
                "stdout_query equ -11\n"
                "stdin_query equ -10\n"
                "section .data\n"
                "\tstdout dw 0\n"
                "\tstdin dw 0\n"
                "\tbytesWritten dw 0\n"
                "\tbytesRead dw 0\n\n"
                "section .bss\n"
                "\tOutputBuffer resb 20\n"
                "\tInputBuffer resb 256\n\n"
                "section .text\n"
                "global main\n"
                "main:\n";

    for (const node::Stmt &stmt : m_prog.statements)
    {
        compStmt(stmt);
    }

    m_output << "\n"
                "_printf:\n"
                "\t; INPUT:\n"
                "\t; RDX - string\n"
                "\tcall _countStrLen\n"
                "\tmov r8, rcx\n"
                "\tmov rcx, stdout_query\n"
                "\tcall GetStdHandle\n"
                "\tmov [rel stdout], rax\n"
                "\tmov rcx, [rel stdout]\n"
                "\tmov r9, bytesWritten\n"
                "\txor r10, r10\n"
                "\tcall WriteConsoleA\n"
                "\tret\n"
                "\n"
                "_scanf:\n"
                "\t; INPUT:\n"
                "\t; RDX - message\n"
                "\t; RSI - buffer for input\n"
                "\t; RAX - buffer size\n"
                "\t; OUTPUT:\n"
                "\t; RSI - buffer with user input\n"
                "\tpush rax\n"
                "\tpush rsi\n"
                "\tpush rdx\n"
                "\tmov rdx, rax\n"
                "\tcall _clearBuffer\n"
                "\tpop rdx\n"
                "\tcall _printf\n"
                "\tmov rcx, stdin_query\n"
                "\tcall GetStdHandle\n"
                "\tmov [rel stdin], rax\n"
                "\tmov rcx, [rel stdin]\n"
                "\tpop rdx\n"
                "\tpop r8\n"
                "\tmov r9, bytesRead\n"
                "\tcall ReadConsoleA\n"
                "\tret\n"
                "\n"
                "_countStrLen:\n"
                "\t; INPUT:\n"
                "\t; RDX - string\n"
                "\t; OUTPUT:\n"
                "\t; RCX - string length\n"
                "\txor rcx, rcx\n"
                "\tcontinue_count:\n"
                "\tmov al, byte [rdx + rcx]\n"
                "\tcmp al, 0\n"
                "\tje end_len_count\n"
                "\tinc rcx\n"
                "\tjmp continue_count\n"
                "\tend_len_count:\n"
                "\tret\n"
                "\n"
                "_itoa:\n"
                "\t; INPUT:\n"
                "\t; RSI - output string\n"
                "\t; RAX - integer\n"
                "\t; OUTPUT:\n"
                "\t; RSI - string\n"
                "\tpush rsi\n"
                "\tpush rax\n"
                "\tmov rdi, 1\n"
                "\tmov rcx, 1\n"
                "\tmov rbx, 10\n"
                "\t.get_divisor:\n"
                "\txor rdx, rdx\n"
                "\tdiv rbx\n"
                "\tcmp rax, 0\n"
                "\tje ._after\n"
                "\timul rcx, 10\n"
                "\tinc rdi\n"
                "\tjmp .get_divisor\n"
                "\t._after:\n"
                "\tpop rax\n"
                "\tpush rdi\n"
                "\t.to_string:\n"
                "\txor rdx, rdx\n"
                "\tdiv rcx\n"
                "\tadd al, '0'\n"
                "\tmov [rsi], al\n"
                "\tinc rsi\n"
                "\tpush rdx\n"
                "\txor rdx, rdx\n"
                "\tmov rax, rcx\n"
                "\tmov rbx, 10\n"
                "\tdiv rbx\n"
                "\tmov rcx, rax\n"
                "\tpop rax\n"
                "\tcmp rcx, 0\n"
                "\tjg .to_string\n"
                "\tpop rdx\n"
                "\tpop rsi\n"
                "\tret\n"
                "\n"
                "_stoi:\n"
                "\t; INPUT:\n"
                "\t; RSI - buffer to convert\n"
                "\t; OUTPUT:\n"
                "\t; RDI - integer\n"
                "\txor rdi, rdi\n"
                "\tmov rbx, 10\n"
                "\txor rax, rax\n"
                "\tnext_digit:\n"
                "\tmovzx rdx, byte[rsi]\n"
                "\ttest rdx, rdx\n"
                "\tjz done\n"
                "\tcmp rdx, 13\n"
                "\tje done\n"
                "\tcmp rdx, '0'\n"
                "\tjl error\n"
                "\tcmp rdx, '9'\n"
                "\tjg error\n"
                "\timul rdi, rbx\n"
                "\tsub rdx, '0'\n"
                "\tadd rdi, rdx\n"
                "\tinc rsi\n"
                "\tjmp next_digit\n"
                "\terror:\n"
                "\tmov rdx, WAR1\n"
                "\tcall _printf\n"
                "\tmov rdx, 0\n"
                "\tdone:\n"
                "\tmov rsi, rdx\n"
                "\tret\n"
                "\n"
                "_clearBuffer:\n"
                "\t; INPUT:\n"
                "\t; RSI - buffer to clear\n"
                "\t; RDX - buffer size\n"
                "\tclear:\n"
                "\tcmp rdx, 0\n"
                "\tje end\n"
                "\tcmp BYTE [rsi], 00H\n"
                "\tje end\n"
                "\tmov al, 00H\n"
                "\tmov [rsi], al\n"
                "\tinc rsi\n"
                "\tdec rdx\n"
                "\tjmp clear\n"
                "\tend:\n"
                "\tret\n"
                "\n"
                "WAR1: db 'Runtime Warning. Cannot Convert String To Integer. Assigned 0',7,10,00H\n";

    std::stringstream output;
    output << m_SC.str();
    output << "section .bss\n";
    output << m_bssSC.str();
    output << m_output.str();

    return output;
}

void compiler::push(const std::string &reg)
{
    m_output << "\tpush " << reg << "\n";
    ++m_stackSize;
}

void compiler::pop(const std::string &reg)
{
    m_output << "\tpop " << reg << "\n";
    --m_stackSize;
}

