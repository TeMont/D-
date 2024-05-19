#include "exprCompiler.hpp"

bool expressionCompiler::compValExpr(const node::ValExpr &expr, const std::string &expectedType)
{
	struct exprVisitor
	{
		std::string expectedType;

		explicit exprVisitor(std::string expectedType) : expectedType(std::move(expectedType))
		{
		}

		bool operator()(const node::ExprIdent &exprIdent) const
		{
			if (!varCompiler::m_vars.count(exprIdent.ident.value.value()))
			{
				std::cerr << "[Compile Error] ERR005 Undeclared Identifier '" << exprIdent.ident.value.value() << "'";
				exit(EXIT_FAILURE);
			}
			const auto &var = varCompiler::m_vars[exprIdent.ident.value.value()];
			if (expectedType == var.Type)
			{
				varCompiler::push("QWORD [rsp + " + std::to_string((varCompiler::m_stackSize - var.stackLoc - 1) * 8) + "]");
			}
			else if (expectedType == BOOL_TYPE && var.Type != STR_TYPE)
			{
				compBoolExpr({"QWORD [rsp + " + std::to_string((varCompiler::m_stackSize - var.stackLoc - 1) * 8) + "]"});
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
					compiler::m_output << "\tmov rdx, " << exprInt.intLit.value.value() << '\n';
				}
				varCompiler::push("rdx");
				compiler::m_output << "\txor rdx, rdx\n";
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
					compiler::m_output << "\tmov rdx, '" << exprChar.charLit.value.value() << "'\n";
				}
				varCompiler::push("rdx");
				compiler::m_output << "\txor rdx, rdx\n";
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
				std::string SC = compiler::createSCLabel();
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
				if (exprBool.boolLit.value.value() == "false")
				{
					compiler::m_output << "\tmov rdx, 0\n";
				}
				else if (exprBool.boolLit.value.value() == "true")
				{
					compiler::m_output << "\tmov rdx, 1\n";
				}
			}
			varCompiler::push("rdx");
			compiler::m_output << "\txor rdx, rdx\n";
			return true;
		}

		bool operator()(const node::NotCondition &exprNotCond) const
		{
			if (!compValExpr(*exprNotCond.val, expectedType))
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
				if (!compValExpr(node::ValExpr{node::ExprIdent{incDec.ident}}, expectedType))
				{
					return false;
				}
			}
			compIncDec(incDec.ident, incDec.isInc, expectedType);
			if (incDec.isPref)
			{
				return compValExpr(node::ValExpr{node::ExprIdent{incDec.ident}}, expectedType);
			}
			return true;
		}
	};
	exprVisitor visitor(expectedType);
	return std::visit(visitor, expr.var);
}

void expressionCompiler::compBoolExpr(const std::optional<std::string> &literal, bool isReversed)
{
	std::string endLabel = compiler::createLabel();
	std::string falseLabel = compiler::createLabel();
	if ((literal.has_value()))
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

bool expressionCompiler::compBinExpr(const node::BinExpr &expr, const std::string &expectedType)
{
	if (compExpr(*expr.fvl, STR_TYPE) || compExpr(*expr.svl, STR_TYPE))
	{
		std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
		exit(EXIT_FAILURE);
	}
	Tokens op = expr.oper;
	if (op == PLUS || op == MINUS || op == MULT || op == DIV)
	{
		if (compExpr(*expr.fvl, CHAR_TYPE) || compExpr(*expr.svl, CHAR_TYPE))
		{
			std::cerr << "[Compile Error] ERR009 Arithmetic Operations Cannot Be Used With Char Constants";
			exit(EXIT_FAILURE);
		}
	}
	if (!compExpr(*expr.fvl, expectedType) || !compExpr(*expr.svl, expectedType))
	{
		return false;
	}
	varCompiler::pop("rdi");
	varCompiler::pop("rdx");
	std::string trueLabel = compiler::createLabel();
	std::string endLabel = compiler::createLabel();
	if (op == Tokens::PLUS)
	{
		compiler::m_output << "\tadd rdx, rdi\n";
	}
	else if (op == Tokens::MINUS)
	{
		compiler::m_output << "\tsub rdx, rdi\n";
	}
	else if (op == Tokens::MULT)
	{
		compiler::m_output << "\timul rdx, rdi\n";
	}
	else if (op == Tokens::DIV)
	{
		compiler::m_output << "\tmov rax, rdx\n";
		compiler::m_output << "\txor rdx, rdx\n";
		compiler::m_output << "\tcqo\n";
		compiler::m_output << "\tidiv rdi\n";
		compiler::m_output << "\tmov rdx, rax\n";
	}
	else if (op == Tokens::EQEQ)
	{
		compiler::m_output << "\tcmp rdx, rdi\n";
		compiler::m_output << "\tje " << trueLabel << "\n";
		compiler::m_output << "\tmov rdx, 0\n";
		compiler::m_output << "\tjmp " << endLabel << "\n";
		compiler::m_output << "\t" << trueLabel << ":\n";
		compiler::m_output << "\tmov rdx, 1\n";
		compiler::m_output << "\t" << endLabel << ":\n";
	}
	else if (op == Tokens::NOTEQ)
	{
		compiler::m_output << "\tcmp rdx, rdi\n";
		compiler::m_output << "\tjne " << trueLabel << "\n";
		compiler::m_output << "\tmov rdx, 0\n";
		compiler::m_output << "\tjmp " << endLabel << "\n";
		compiler::m_output << "\t" << trueLabel << ":\n";
		compiler::m_output << "\tmov rdx, 1\n";
		compiler::m_output << "\t" << endLabel << ":\n";
	}
	else if (op == Tokens::LESS)
	{
		compiler::m_output << "\tcmp rdx, rdi\n";
		compiler::m_output << "\tjl " << trueLabel << "\n";
		compiler::m_output << "\tmov rdx, 0\n";
		compiler::m_output << "\tjmp " << endLabel << "\n";
		compiler::m_output << "\t" << trueLabel << ":\n";
		compiler::m_output << "\tmov rdx, 1\n";
		compiler::m_output << "\t" << endLabel << ":\n";
	}
	else if (op == Tokens::GREATER)
	{
		compiler::m_output << "\tcmp rdx, rdi\n";
		compiler::m_output << "\tjg " << trueLabel << "\n";
		compiler::m_output << "\tmov rdx, 0\n";
		compiler::m_output << "\tjmp " << endLabel << "\n";
		compiler::m_output << "\t" << trueLabel << ":\n";
		compiler::m_output << "\tmov rdx, 1\n";
		compiler::m_output << "\t" << endLabel << ":\n";
	}
	else if (op == Tokens::LESSEQ)
	{
		compiler::m_output << "\tcmp rdx, rdi\n";
		compiler::m_output << "\tjle " << trueLabel << "\n";
		compiler::m_output << "\tmov rdx, 0\n";
		compiler::m_output << "\tjmp " << endLabel << "\n";
		compiler::m_output << "\t" << trueLabel << ":\n";
		compiler::m_output << "\tmov rdx, 1\n";
		compiler::m_output << "\t" << endLabel << ":\n";
	}
	else if (op == Tokens::GREATEQ)
	{
		compiler::m_output << "\tcmp rdx, rdi\n";
		compiler::m_output << "\tjge " << trueLabel << "\n";
		compiler::m_output << "\tmov rdx, 0\n";
		compiler::m_output << "\tjmp " << endLabel << "\n";
		compiler::m_output << "\t" << trueLabel << ":\n";
		compiler::m_output << "\tmov rdx, 1\n";
		compiler::m_output << "\t" << endLabel << ":\n";
	}
	else if (op == Tokens::AND)
	{
		std::string falseLabel = compiler::createLabel();
		compiler::m_output << "\tcmp rdx, 0\n";
		compiler::m_output << "\tje " << falseLabel << "\n";
		compiler::m_output << "\tcmp rdi, 0\n";
		compiler::m_output << "\tje " << falseLabel << "\n";
		compiler::m_output << "\tmov rdx, 1\n";
		compiler::m_output << "\tjmp " << endLabel << "\n";
		compiler::m_output << "\t" << falseLabel << ":\n";
		compiler::m_output << "\tmov rdx, 0\n";
		compiler::m_output << "\t" << endLabel << ":\n";
	}
	else if (op == Tokens::OR)
	{
		compiler::m_output << "\tcmp rdx, 0\n";
		compiler::m_output << "\tjne " << trueLabel << "\n";
		compiler::m_output << "\tcmp rdi, 0\n";
		compiler::m_output << "\tjne " << trueLabel << "\n";
		compiler::m_output << "\tmov rdx, 0\n";
		compiler::m_output << "\tjmp " << endLabel << "\n";
		compiler::m_output << "\t" << trueLabel << ":\n";
		compiler::m_output << "\tmov rdx, 1\n";
		compiler::m_output << "\t" << endLabel << ":\n";
	}
	varCompiler::push("rdx");
	compiler::m_output << "\txor rdx, rdx\n";
	compiler::m_output << "\txor rdi, rdi\n";
	return true;
}

bool expressionCompiler::compExpr(const node::Expr &expr, const std::string &expectedType)
{
	struct exprVisitor
	{
		std::string expectedType;

		explicit exprVisitor(std::string expectedType) : expectedType(std::move(expectedType))
		{
		}

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
			compiler::m_output << ";;\tInput\n";
			compiler::compInput(*inputExpr);
			if (expectedType == STR_TYPE)
			{
				std::string falseLabel1 = compiler::createLabel();
				std::string trueLabel1 = compiler::createLabel();
				std::string endLabel1 = compiler::createLabel();
				std::string falseLabel2 = compiler::createLabel();
				std::string trueLabel2 = compiler::createLabel();
				std::string endLabel2 = compiler::createLabel();
				std::string SC = compiler::createSCLabel();
				compiler::m_bssSC << "\t" << SC << " resb 256\n";
				compiler::m_output << "\tmov rdi, " << SC << "\n";
				compiler::m_output << "\tmov rcx, 256\n";
				compiler::m_output << "\trep movsb\n";
				compiler::m_output << "\tmov rdx, " << SC << "\n";
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
				varCompiler::push("rdx");
			}
			else if (expectedType == CHAR_TYPE)
			{
				compiler::m_output << "\tmovzx rdx, byte [rsi]\n";
				varCompiler::push("rdx");
			}
			else if (expectedType == INT_TYPE || expectedType == BOOL_TYPE)
			{
				compiler::m_output << "\tcall _stoi\n";
				if (expectedType == BOOL_TYPE)
				{
					compBoolExpr("rdi");
				}
				else
				{
					varCompiler::push("rdi");
				}
			}
			else
			{
				return false;
			}

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
	exprVisitor visitor(expectedType);
	return std::visit(visitor, expr.var);
}

void expressionCompiler::compIncDec(const Token &ident, bool isInc, const std::string &expectedType)
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