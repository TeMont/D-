#include <gtest/gtest.h>
#include "../source/Compile/compile.cpp"

extern std::vector<std::vector<std::string>> typeArr;
extern std::vector<Tokens> binOpArr;

std::vector<node::ValExpr> valExprArr =
	{
		{node::ExprIntLit{Token{INT_LITERAL, "10"}}},
		{node::ExprStrLit{Token{STRING_LITERAL, "str"}}},
		{node::ExprBoolLit{Token{BOOL_LITERAL, "false"}}},
		{node::ExprCharLit{Token{CHAR_LITERAL, "c"}}},
		{node::ExprIdent{Token{IDENT, "ident"}}},
};
struct valExprVisitor
{
	Token operator()(const node::ExprIdent &exprIdent)
	{
		return exprIdent.ident;
	}
	Token operator()(const node::ExprIntLit &exprInt)
	{
		return exprInt.intLit;
	}
	Token operator()(const node::ExprStrLit &exprStr)
	{
		return exprStr.strLit;
	}
	Token operator()(const node::ExprBoolLit &exprBool)
	{
		return exprBool.boolLit;
	}
	Token operator()(const node::ExprCharLit &exprChar)
	{
		return exprChar.charLit;
	}
};
node::BinExprAdd createBinExpr(node::ValExpr *fvl, node::ValExpr *svl)
{
	valExprVisitor valExprVisit;
	if (std::visit(valExprVisit, fvl->var).type == IDENT && std::visit(valExprVisit, svl->var).type == IDENT)
	{
		return {
			new node::Expr({new node::ValExpr{node::ExprIdent{std::visit(valExprVisit, fvl->var).type, std::visit(valExprVisit, fvl->var).value.value() + "1"}}}),
			new node::Expr({svl})};
	}
	else
	{
		return {new node::Expr({fvl}), new node::Expr({svl})};
	}
}
std::string literalToType(const Tokens &lit)
{
	switch (lit)
	{
	case INT_LITERAL:
		return INT_TYPE;
	case STRING_LITERAL:
		return STR_TYPE;
	case BOOL_LITERAL:
		return BOOL_TYPE;
	case CHAR_LITERAL:
		return CHAR_TYPE;
	case IDENT:
		return ANY_TYPE;
	default:
		std::cerr << "Unexpected Literal";
		exit(EXIT_FAILURE);
	}
}
TEST(CompilerTest, CompValExprTest)
{
	valExprVisitor valExprVisit;
	for (int i = 0; i < typeArr.size(); ++i)
	{
		for (int j = 0; j < typeArr[i].size(); ++j)
		{
			Token valExprToken = std::visit(valExprVisit, valExprArr[i].var);
			if (valExprToken.type != IDENT)
			{
				if (j == 0)
				{
					compiler::clearM_output();
					compiler::resetM_labelCount();
					ASSERT_TRUE(compiler::compValExpr(valExprArr[i], typeArr[i][j]))
						<< "ERROR Incorrect type error while compiling value expression(DEFAULT TYPE)\n";
					if (typeArr[i][j] == STR_TYPE)
					{
						ASSERT_EQ(compiler::getM_output(), "\tmov rdx, SC0\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output for STR_TYPE(DEFAULT TYPE)\n";
					}
					else if (typeArr[i][j] == CHAR_TYPE)
					{
						ASSERT_EQ(compiler::getM_output(), "\tmov rdx, '" + valExprToken.value.value() + "'\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output for CHAR_TYPE(DEFAULT TYPE)\n";
					}
					else if (typeArr[i][j] == BOOL_TYPE)
					{
						ASSERT_EQ(compiler::getM_output(), "\tmov rdx, " + std::to_string(valExprToken.value.value() == "true") +
															   "\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output for BOOL_TYPE(DEFAULT TYPE)\n";
					}
					else if (typeArr[i][j] == INT_TYPE)
					{
						ASSERT_EQ(compiler::getM_output(), "\tmov rdx, " + valExprToken.value.value() +
															   "\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output for INT_TYPE(DEFAULT TYPE)\n";
					}
					else
					{
						FAIL() << "ERROR Incorrect type(DEFAULT TYPE)\n";
					}
				}
				else if (typeArr[i][j] == BOOL_TYPE && typeArr[i][0] != STR_TYPE)
				{
					compiler::clearM_output();
					compiler::resetM_labelCount();
					ASSERT_TRUE(compiler::compValExpr(valExprArr[i], typeArr[i][j]))
						<< "ERROR Incorrect type error while compiling value expression(BOOLEAN)\n";
					if (typeArr[i][0] == INT_TYPE)
					{
						ASSERT_EQ(compiler::getM_output(), "\tmov rdx, " + valExprToken.value.value() +
															   "\n\tcmp rdx, 0\n\tjle label1\n\tmov rdx, 1\n\tjmp label0\n\tlabel1:\n\tmov rdx, 0\n\tlabel0:\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output for INT_TYPE(BOOLEAN)\n";
					}
					else if (typeArr[i][0] == CHAR_TYPE)
					{
						ASSERT_EQ(compiler::getM_output(), "\tmov rdx, '" + valExprToken.value.value() +
															   "'\n\tcmp rdx, 0\n\tjle label1\n\tmov rdx, 1\n\tjmp label0\n\tlabel1:\n\tmov rdx, 0\n\tlabel0:\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output for CHAR_TYPE(BOOLEAN)\n";
					}
					else
					{
						FAIL() << "ERROR Incorrect type(BOOLEAN)\n";
					}
				}
				else
				{
					ASSERT_FALSE(compiler::compValExpr(valExprArr[i], typeArr[i][j]))
						<< "ERROR Successfully compiled with incorrect value expression\n";
				}
				compiler::resetM_labelCount();
			}
			else
			{
				for (int k = 0; k < typeArr[i].size(); ++k)
				{
					compiler::clearM_vars();
					compiler::clearM_output();
					compiler::resetM_stackSize();
					compiler::pushVar(valExprToken.value.value(), typeArr[i][j]);
					compiler::push("rdx");
					compiler::resetM_labelCount();
					if (typeArr[i][k] == typeArr[i][j])
					{
						ASSERT_TRUE(compiler::compValExpr(valExprArr[i], typeArr[i][k]))
							<< "ERROR Incorrect type error while compiling value expression(DEFAULT TYPE IDENT)\n";
						ASSERT_EQ(compiler::getM_output(), "\tpush rdx\n\tpush QWORD [rsp + 0]\n")
							<< "ERROR Incorrect output (DEFAULT TYPE IDENT)\n";
					}
					else if (typeArr[i][k] == BOOL_TYPE && typeArr[i][j] != STR_TYPE)
					{
						ASSERT_TRUE(compiler::compValExpr(valExprArr[i], typeArr[i][k]))
							<< "ERROR Incorrect type error while compiling value expression(BOOLEAN IDENT)\n";
						ASSERT_EQ(compiler::getM_output(), "\tpush rdx\n\tmov rdx, QWORD [rsp + 0]\n\tcmp rdx, 0"
														   "\n\tjle label1\n\tmov rdx, 1\n\tjmp label0\n\tlabel1:\n\tmov rdx, 0\n\tlabel0:\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output (BOOLEAN IDENT)\n";
					}
					else
					{
						ASSERT_FALSE(compiler::compValExpr(valExprArr[i], typeArr[i][k]))
							<< "ERROR Successfully compiled with incorrect value expression(IDENT)\n";
					}
				}
			}
		}
	}
}
TEST(CompilerTest, CompBinExprTest)
{
	valExprVisitor valExprVisit;
	for (int i = 0; i < typeArr.size(); ++i)
	{
		for (int j = 0; j < typeArr.size(); ++j)
		{
			for (int k = 0; k < typeArr[i].size(); ++k)
			{
				for (int l = 0; l < binOpArr.size(); ++l)
				{
					Token fvlToken = std::visit(valExprVisit, valExprArr[i].var);
					Token svlToken = std::visit(valExprVisit, valExprArr[j].var);
					node::BinExpr binExpr;
					auto tmpBinExpr = createBinExpr(&valExprArr[i], &valExprArr[j]);
					switch (binOpArr[l])
					{
					case PLUS:
						binExpr.var = {new node::BinExprAdd({tmpBinExpr.fvl, tmpBinExpr.svl})};
						break;
					case MINUS:
						binExpr.var = {new node::BinExprSub({tmpBinExpr.fvl, tmpBinExpr.svl})};
						break;
					case MULT:
						binExpr.var = {new node::BinExprMul({tmpBinExpr.fvl, tmpBinExpr.svl})};
						break;
					case DIV:
						binExpr.var = {new node::BinExprDiv({tmpBinExpr.fvl, tmpBinExpr.svl})};
						break;
					case EQEQ:
						binExpr.var = {new node::EQCondition({tmpBinExpr.fvl, tmpBinExpr.svl})};
						break;
					case NOTEQ:
						binExpr.var = {new node::NotEQCondition({tmpBinExpr.fvl, tmpBinExpr.svl})};
						break;
					case LESS:
						binExpr.var = {new node::LessCondition({tmpBinExpr.fvl, tmpBinExpr.svl})};
						break;
					case LESSEQ:
						binExpr.var = {new node::EQLessCondition({tmpBinExpr.fvl, tmpBinExpr.svl})};
						break;
					case GREATER:
						binExpr.var = {new node::GreaterCondition({tmpBinExpr.fvl, tmpBinExpr.svl})};
						break;
					case GREATEQ:
						binExpr.var = {new node::EQGreaterCondition({tmpBinExpr.fvl, tmpBinExpr.svl})};
						break;
					case AND:
						binExpr.var = {new node::AndCondition({tmpBinExpr.fvl, tmpBinExpr.svl})};
						break;
					case OR:
						binExpr.var = {new node::OrCondition({tmpBinExpr.fvl, tmpBinExpr.svl})};
						break;
					default:
						FAIL() << "ERROR Unknown Operator\n";
					}
					if (fvlToken.type == STRING_LITERAL || svlToken.type == STRING_LITERAL)
					{
						ASSERT_EXIT(compiler::compBinExpr(binExpr, typeArr[i][k]), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
							<< "ERROR Was not thrown error while compiling binary string expression(DEFAULT TYPE)\n";
					}
					else if ((fvlToken.type == CHAR_LITERAL || svlToken.type == CHAR_LITERAL) && l < 4)
					{
						ASSERT_EXIT(compiler::compBinExpr(binExpr, typeArr[i][k]), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
							<< "ERROR Was not thrown error while compiling binary char expression(DEFAULT TYPE)\n";
					}
					else if (fvlToken.type == IDENT && svlToken.type == IDENT)
					{
						for (int m = 0; m < typeArr[i].size(); ++m)
						{
							for (int n = 0; n < typeArr[i].size(); ++n)
							{
								compiler::clearM_vars();
								compiler::pushVar(fvlToken.value.value() + "1", typeArr[i][m]);
								compiler::pushVar(svlToken.value.value(), typeArr[i][n]);
								if (typeArr[i][m] == STR_TYPE || typeArr[i][n] == STR_TYPE)
								{
									ASSERT_EXIT(compiler::compBinExpr(binExpr, typeArr[i][k]), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
										<< "ERROR Was not thrown error while compiling binary string expression(IDENT DEFAULT TYPE)\n";
								}
								else if ((typeArr[i][m] == CHAR_TYPE || typeArr[i][n] == CHAR_TYPE) && l < 4)
								{
									ASSERT_EXIT(compiler::compBinExpr(binExpr, typeArr[i][k]), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
										<< "ERROR Was not thrown error while compiling binary char expression(IDENT DEFAULT TYPE)\n";
								}
								else
								{
									if (m == n && n == k)
									{
										ASSERT_TRUE(compiler::compBinExpr(binExpr, typeArr[i][k]))
											<< "ERROR Incorrect type error while compiling value expression(IDENT DEFAULT TYPE)\n";
									}
									else if (typeArr[i][k] == BOOL_TYPE)
									{
										ASSERT_TRUE(compiler::compBinExpr(binExpr, typeArr[i][k]))
											<< "ERROR Incorrect type error while compiling value expression(BOOLEAN IDENT)\n";
									}
									else
									{
										ASSERT_FALSE(compiler::compBinExpr(binExpr, typeArr[i][k]))
											<< "ERROR Successfully compiled incorrect binary expression(IDENT DEFAULT TYPE)\n";
									}
								}
							}
						}
					}
					else if (fvlToken.type == IDENT || svlToken.type == IDENT)
					{
						for (int m = 0; m < typeArr[i].size(); ++m)
						{
							compiler::clearM_vars();
							compiler::pushVar((svlToken.type == IDENT) ? svlToken.value.value() : fvlToken.value.value(), typeArr[i][m]);

							if (typeArr[i][m] == STR_TYPE || typeArr[i][m] == CHAR_TYPE && l < 4)
							{
								ASSERT_EXIT(compiler::compBinExpr(binExpr, typeArr[i][k]), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
									<< "ERROR Was not thrown error while compiling binary string expression(IDENT DEFAULT TYPE)\n";
							}
							else
							{
								if (fvlToken.type == IDENT && literalToType(svlToken.type) == typeArr[i][k] || svlToken.type == IDENT && literalToType(fvlToken.type) == typeArr[i][k])
								{
									if (typeArr[i][m] == typeArr[i][k])
									{
										ASSERT_TRUE(compiler::compBinExpr(binExpr, typeArr[i][k]))
											<< "ERROR Incorrect type error while compiling value expression(IDENT DEFAULT TYPE)\n";
									}
									else if (typeArr[i][k] == BOOL_TYPE)
									{
										ASSERT_TRUE(compiler::compBinExpr(binExpr, typeArr[i][k]))
											<< "ERROR Incorrect type error while compiling value expression(IDENT BOOLEAN)\n";
									}
									else
									{
										ASSERT_FALSE(compiler::compBinExpr(binExpr, typeArr[i][k]))
											<< "ERROR Successfully compiled incorrect binary expression(IDENT DEFAULT TYPE)\n";
									}
								}
								else
								{
									if (i == j && k == 0)
									{
										ASSERT_FALSE(compiler::compBinExpr(binExpr, typeArr[i][k]))
											<< "ERROR Successfully compiled incorrect binary expression(IDENT DEFAULT TYPE)\n";
									}
									else if (typeArr[i][k] == BOOL_TYPE)
									{
										ASSERT_TRUE(compiler::compBinExpr(binExpr, typeArr[i][k]))
											<< "ERROR Incorrect type error while compiling binary expression(BOOLEAN)\n";
									}
									else
									{
										ASSERT_FALSE(compiler::compBinExpr(binExpr, typeArr[i][k]))
											<< "ERROR Successfully compiled incorrect binary expression(DEFAULT TYPE)\n";
									}
								}
							}
						}
					}
					else
					{
						if (i == j && k == 0)
						{
							ASSERT_TRUE(compiler::compBinExpr(binExpr, typeArr[i][k]))
								<< "ERROR Incorrect type error while compiling value expression(DEFAULT TYPE)\n";
						}
						else if (typeArr[i][k] == BOOL_TYPE)
						{
							ASSERT_TRUE(compiler::compBinExpr(binExpr, typeArr[i][k]))
								<< "ERROR Incorrect type error while compiling binary expression(BOOLEAN)\n";
						}
						else
						{
							ASSERT_FALSE(compiler::compBinExpr(binExpr, typeArr[i][k]))
								<< "ERROR Successfully compiled incorrect binary expression(DEFAULT TYPE)\n";
						}
					}
				}
			}
		}
	}
}
TEST(CompilerTest, CompBoolExprTest)
{
	std::vector<std::optional<std::string>> literalArr =
		{
			{"10"},
			{"'c'"},
			{"rdx"},
			{},
		};
	for (auto const &i : literalArr)
	{
		compiler::clearM_output();
		compiler::resetM_labelCount();
		compiler::compBoolExpr(i);
		if (i.has_value())
		{
			ASSERT_EQ(compiler::getM_output(),
					  "\tmov rdx, " + i.value() + "\n\tcmp rdx, 0\n\tjle label1\n\tmov rdx, 1\n\tjmp label0\n\tlabel1:\n\tmov rdx, 0\n\tlabel0:\n\tpush rdx\n\txor rdx, rdx\n")
				<< "ERROR Incorrect output with value";
		}
		else
		{
			ASSERT_EQ(compiler::getM_output(),
					  "\tcmp rdx, 0\n\tjle label1\n\tmov rdx, 1\n\tjmp label0\n\tlabel1:\n\tmov rdx, 0\n\tlabel0:\n\tpush rdx\n\txor rdx, rdx\n")
				<< "ERROR Incorrect output without value";
		}
	}
}
TEST(CompilerTest, CompInputTest)
{
	valExprVisitor valExprVisit;
	for (auto const &i : valExprArr)
	{
		Token token = std::visit(valExprVisit, i.var);
		node::StmtInput inputStmt = {new node::Expr{new node::ValExpr({i})}};
		compiler::clearM_output();
		if (token.type == IDENT)
		{
			for (auto const &j : typeArr[0])
			{
				compiler::clearM_vars();
				compiler::clearM_output();
				compiler::resetM_stackSize();
				compiler::pushVar(token.value.value(), j);
				compiler::push("rdx");
				compiler::resetM_labelCount();

				if (j == STR_TYPE)
				{
					compiler::compInput(inputStmt);
					ASSERT_EQ(compiler::getM_output(), "\tpush rdx\n\tpush QWORD [rsp + 0]\n\tpop rdx\n\tmov rsi, InputBuffer\n\tmov rax, 256\n\tcall _scanf\n")
						<< "ERROR Incorrect output (IDENT STR_TYPE)";
				}
				else if (j == CHAR_TYPE)
				{
					compiler::compInput(inputStmt);
					ASSERT_EQ(compiler::getM_output(),
							  "\tpush rdx\n\tpush QWORD [rsp + 0]\n\tpop rdx\n\tmov rsi, OutputBuffer\n\tmov [rsi], dx\n\tmov rdx, rsi\n\tmov rsi, InputBuffer\n\tmov rax, 256\n\tcall _scanf\n")
						<< "ERROR Incorrect output (IDENT CHAR_TYPE)";
				}
				else if (j == INT_TYPE || j == BOOL_TYPE)
				{
					compiler::compInput(inputStmt);
					ASSERT_EQ(compiler::getM_output(),
							  "\tpush rdx\n\tpush QWORD [rsp + 0]\n\tpop rdx\n\tmov rax, rdx\n\tmov rsi, OutputBuffer\n\tcall _itoa\n\tmov rdx, rsi\n\tmov rsi, InputBuffer\n"
							  "\tmov rax, 256\n\tcall _scanf\n")
						<< "ERROR Incorrect output (IDENT BOOL_TYPE | INT_TYPE)";
				}
			}
		}
		else
		{
			compiler::compInput(inputStmt);
			if (token.type == STRING_LITERAL)
			{
				ASSERT_EQ(compiler::getM_output(), "\tmov rdx, SC0\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov rsi, InputBuffer\n\tmov rax, 256\n\tcall _scanf\n")
					<< "ERROR Incorrect output (STR_LITERAL)";
			}
			else if (token.type == CHAR_LITERAL)
			{
				ASSERT_EQ(compiler::getM_output(),
						  "\tmov rdx, '" + token.value.value() + "'\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov rsi, OutputBuffer\n\tmov [rsi], dx\n\tmov rdx, rsi\n\tmov rsi, InputBuffer"
																 "\n\tmov rax, 256\n\tcall _scanf\n")
					<< "ERROR Incorrect output (CHAR_LITERAL)";
			}
			else if (token.type == INT_LITERAL)
			{
				ASSERT_EQ(compiler::getM_output(),
						  "\tmov rdx, " + token.value.value() + "\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov rax, rdx\n\tmov rsi, OutputBuffer\n\tcall _itoa\n\tmov rdx, rsi\n"
																"\tmov rsi, InputBuffer\n\tmov rax, 256\n\tcall _scanf\n")
					<< "ERROR Incorrect output (INT_LITERAL)";
			}
			else if (token.type == BOOL_LITERAL)
			{
				ASSERT_EQ(compiler::getM_output(),
						  "\tmov rdx, " + std::to_string(token.value.value() == "true") + "\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov rax, rdx\n\tmov rsi, OutputBuffer"
																						  "\n\tcall _itoa\n\tmov rdx, rsi\n\tmov rsi, InputBuffer\n\tmov rax, 256\n\tcall _scanf\n")
					<< "ERROR Incorrect output (BOOL_LITERAL)";
			}
		}
	}
}
TEST(CompilerTest, CompVarTest)
{
	valExprVisitor valExprVisit;
	Token ident = {IDENT, {"ident"}};
	for (int i = 0; i < typeArr.size(); ++i)
	{
		for (int j = 0; j < typeArr[i].size(); ++j)
		{
			for (int k = 0; k < typeArr[i].size(); ++k)
			{
				compiler::clearM_output();
				compiler::clearM_vars();
				compiler::resetM_labelCount();
				compiler::resetM_stackSize();
				compiler::pushVar(ident.value.value(), typeArr[i][j]);
				compiler::push("rdx");
				Token valExprToken = std::visit(valExprVisit, valExprArr[i].var);
				if (typeArr[i][j] == BOOL_TYPE)
				{
					if (valExprToken.type != STRING_LITERAL)
					{
						if (valExprToken.type != IDENT)
						{
							compiler::compVar(ident, new node::Expr{new node::ValExpr({valExprArr[i]})}, typeArr[i][k]);
							if (valExprToken.type == BOOL_LITERAL)
							{
								ASSERT_EQ(compiler::getM_output(),
										  "\tpush rdx\n\tmov rdx, " + std::to_string(valExprToken.value.value() == "true") +
											  "\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov rdx, rdx"
											  "\n\tcmp rdx, 0\n\tjle label1\n\tmov rdx, 1\n\tjmp label0\n\tlabel1:\n\tmov rdx, 0"
											  "\n\tlabel0:\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov [rsp + 0], rdx\n\txor rdx, rdx\n")
									<< "ERROR Incorrect output (BOOLEAN BOOL_LITERAL)";
							}
							else if (valExprToken.type == CHAR_LITERAL)
							{
								ASSERT_EQ(compiler::getM_output(),
										  "\tpush rdx\n\tmov rdx, '" + valExprToken.value.value() +
											  "'\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov rdx, rdx"
											  "\n\tcmp rdx, 0\n\tjle label1\n\tmov rdx, 1\n\tjmp label0\n\tlabel1:\n\tmov rdx, 0"
											  "\n\tlabel0:\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov [rsp + 0], rdx\n\txor rdx, rdx\n")
									<< "ERROR Incorrect output (BOOLEAN CHAR_LITERAL)";
							}
							else if (valExprToken.type == INT_LITERAL)
							{
								ASSERT_EQ(compiler::getM_output(),
										  "\tpush rdx\n\tmov rdx, " + valExprToken.value.value() +
											  "\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov rdx, rdx"
											  "\n\tcmp rdx, 0\n\tjle label1\n\tmov rdx, 1\n\tjmp label0\n\tlabel1:\n\tmov rdx, 0"
											  "\n\tlabel0:\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov [rsp + 0], rdx\n\txor rdx, rdx\n")
									<< "ERROR Incorrect output (BOOLEAN INT_LITERAL)";
							}
							else
							{
								FAIL() << "ERROR Unexpected literal (BOOLEAN)";
							}
						}
						else
						{
							for (int l = 0; l < typeArr[i].size(); ++l)
							{
								compiler::clearM_output();
								compiler::clearM_vars();
								compiler::resetM_labelCount();
								compiler::resetM_stackSize();
								compiler::pushVar(ident.value.value(), typeArr[i][j]);
								compiler::push("rdx");
								compiler::pushVar(ident.value.value() + "1", typeArr[i][l]);
								compiler::push("rdx");
								if (typeArr[i][l] != STR_TYPE)
								{
									compiler::compVar(ident,
													  new node::Expr{new node::ValExpr({node::ExprIdent{IDENT, ident.value.value() + "1"}})}, typeArr[i][k]);
									ASSERT_EQ(compiler::getM_output(), "\tpush rdx\n\tpush rdx\n\tpush QWORD [rsp + 0]\n\tpop rdx\n\tmov rdx, rdx"
																	   "\n\tcmp rdx, 0\n\tjle label1\n\tmov rdx, 1\n\tjmp label0\n\tlabel1:"
																	   "\n\tmov rdx, 0\n\tlabel0:\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov [rsp + 8], rdx\n\txor rdx, rdx\n")
										<< "ERROR Incorrect output (IDENT BOOLEAN)";
								}
								else
								{
									ASSERT_EXIT(compiler::compVar(ident,
																  new node::Expr{new node::ValExpr({node::ExprIdent{IDENT, ident.value.value() + "1"}})}, typeArr[i][k]),
												::testing::ExitedWithCode(EXIT_FAILURE), ".*")
										<< "ERROR Was not thrown error with STR_TYPE (IDENT BOOLEAN)";
								}
							}
						}
					}
					else
					{
						ASSERT_EXIT(compiler::compVar(ident,
													  new node::Expr{new node::ValExpr({valExprArr[i]})},
													  typeArr[i][k]),
									::testing::ExitedWithCode(EXIT_FAILURE), ".*")
							<< "ERROR Was not thrown error with STR_TYPE (BOOLEAN)";
					}
				}
				else if (j == k)
				{
					if (valExprToken.type != IDENT)
					{
						if (literalToType(valExprToken.type) == typeArr[i][k])
						{
							compiler::compVar(ident, new node::Expr{new node::ValExpr({valExprArr[i]})}, typeArr[i][k]);
							if (valExprToken.type == CHAR_LITERAL)
							{
								ASSERT_EQ(compiler::getM_output(),
										  "\tpush rdx\n\tmov rdx, '" + valExprToken.value.value() + "'\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov [rsp + 0], rdx\n\txor rdx, rdx\n")
									<< "ERROR Incorrect output (DEFAULT TYPE CHAR_LITERAL)";
							}
							else if (valExprToken.type == STRING_LITERAL)
							{
								ASSERT_EQ(compiler::getM_output(),
										  "\tpush rdx\n\tmov rdx, SC0\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov [rsp + 0], rdx\n\txor rdx, rdx\n")
									<< "ERROR Incorrect output (DEFAULT TYPE STRING_LITERAL)";
							}
							else if (valExprToken.type == INT_LITERAL)
							{
								ASSERT_EQ(compiler::getM_output(), "\tpush rdx\n\tmov rdx, " + valExprToken.value.value() +
																	   "\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov [rsp + 0], rdx\n\txor rdx, rdx\n")
									<< "ERROR Incorrect output (DEFAULT TYPE INT_LITERAL)";
							}
							else
							{
								FAIL() << "ERROR Unexpected literal(DEFAULT TYPE)";
							}
						}
						else
						{
							ASSERT_EXIT(compiler::compVar(ident,
														  new node::Expr{new node::ValExpr({valExprArr[i]})},
														  typeArr[i][k]),
										::testing::ExitedWithCode(EXIT_FAILURE), ".*")
								<< "ERROR Was not thrown error with incorrect types(DEFAULT TYPE)";
						}
					}
					else
					{
						for (int l = 0; l < typeArr[i].size(); ++l)
						{
							compiler::clearM_output();
							compiler::clearM_vars();
							compiler::resetM_labelCount();
							compiler::resetM_stackSize();
							compiler::pushVar(ident.value.value(), typeArr[i][j]);
							compiler::push("rdx");
							compiler::pushVar(ident.value.value() + "1", typeArr[i][l]);
							compiler::push("rdx");
							if (j == l && l == k)
							{
								compiler::compVar(ident,
												  new node::Expr{new node::ValExpr({node::ExprIdent{IDENT, ident.value.value() + "1"}})}, typeArr[i][k]);
								ASSERT_EQ(compiler::getM_output(), "\tpush rdx\n\tpush rdx\n\tpush QWORD [rsp + 0]\n\tpop rdx\n\tmov [rsp + 8], rdx\n\txor rdx, rdx\n")
									<< "ERROR Incorrect output (IDENT DEFAULT TYPE)";
							}
							else
							{
								ASSERT_EXIT(compiler::compVar(ident,
															  new node::Expr{new node::ValExpr({node::ExprIdent{IDENT, ident.value.value() + "1"}})}, typeArr[i][k]),
											::testing::ExitedWithCode(EXIT_FAILURE), ".*")
									<< "ERROR Was not thrown error with incorrect types(IDENT)";
							}
						}
					}
				}
				else
				{
					ASSERT_EXIT(compiler::compVar(ident,
												  new node::Expr{new node::ValExpr({valExprArr[i]})},
												  typeArr[i][k]),
								::testing::ExitedWithCode(EXIT_FAILURE), ".*")
						<< "ERROR Was not thrown error with incorrect types";
				}
			}
		}
	}
}
TEST(CompilerTest, CompLetTest)
{
	valExprVisitor valExprVisit;
	Token ident = {IDENT, {"ident"}};
	for (int i = 0; i < typeArr.size(); ++i)
	{
		for (int j = 0; j < typeArr[j].size(); ++j)
		{
			compiler::clearM_output();
			compiler::clearM_vars();
			compiler::resetM_labelCount();
			compiler::resetM_stackSize();
			Token valExprToken = std::visit(valExprVisit, valExprArr[i].var);
			if (typeArr[i][j] == BOOL_TYPE)
			{
				if (valExprToken.type != IDENT)
				{
					if (valExprToken.type != STRING_LITERAL)
					{
						compiler::compLet(ident, new node::Expr{new node::ValExpr({valExprArr[i]})}, typeArr[i][j]);
						if (valExprToken.type == INT_LITERAL)
						{
							ASSERT_EQ(compiler::getM_output(), "\tmov rdx, " + valExprToken.value.value() + "\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov rdx, rdx"
															   "\n\tcmp rdx, 0\n\tjle label1\n\tmov rdx, 1\n\tjmp label0\n\tlabel1:"
															   "\n\tmov rdx, 0\n\tlabel0:\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output (BOOLEAN INT_LITERAL)";
						}
						else if (valExprToken.type == CHAR_LITERAL)
						{
							ASSERT_EQ(compiler::getM_output(), "\tmov rdx, '" + valExprToken.value.value() + "'\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov rdx, rdx"
							                                   "\n\tcmp rdx, 0\n\tjle label1\n\tmov rdx, 1\n\tjmp label0\n\tlabel1:"
							                                   "\n\tmov rdx, 0\n\tlabel0:\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output (BOOLEAN CHAR_LITERAL)";
						}
						else if (valExprToken.type == BOOL_LITERAL)
						{
							ASSERT_EQ(compiler::getM_output(), "\tmov rdx, " + std::to_string(valExprToken.value.value() == "true") +
															   "\n\tpush rdx\n\txor rdx, rdx\n\tpop rdx\n\tmov rdx, rdx"
							                                   "\n\tcmp rdx, 0\n\tjle label1\n\tmov rdx, 1\n\tjmp label0\n\tlabel1:"
							                                   "\n\tmov rdx, 0\n\tlabel0:\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output (BOOLEAN BOOL_LITERAL)";
						}
						else
						{
							FAIL() << "ERROR Unexpected literal";
						}
					}
					else
					{
						ASSERT_EXIT(compiler::compLet(ident, new node::Expr{new node::ValExpr({valExprArr[i]})}, typeArr[i][j]),
						            ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
						<< "ERROR Was not thrown error with STR_LITERAL (BOOLEAN)";
					}
				}
				else
				{
					for (int k = 0; k < typeArr[i].size(); ++k)
					{
						compiler::clearM_output();
						compiler::clearM_vars();
						compiler::resetM_labelCount();
						compiler::resetM_stackSize();
						compiler::pushVar(ident.value.value()+"1", typeArr[i][k]);
						compiler::push("rdx");
						if (typeArr[i][k] != STR_TYPE)
						{
							compiler::compLet(ident, new node::Expr{new node::ValExpr(node::ExprIdent{valExprToken.type, valExprToken.value.value()+"1"})}, typeArr[i][j]);
							ASSERT_EQ(compiler::getM_output(), "\tpush rdx\n\tpush QWORD [rsp + 0]\n\tpop rdx\n\tmov rdx, rdx"
															   "\n\tcmp rdx, 0\n\tjle label1\n\tmov rdx, 1\n\tjmp label0\n\tlabel1:"
															   "\n\tmov rdx, 0\n\tlabel0:\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output (IDENT BOOLEAN)";
						}
						else
						{
							ASSERT_EXIT(compiler::compLet(ident, new node::Expr{new node::ValExpr(node::ExprIdent{valExprToken.type, valExprToken.value.value()+"1"})}, typeArr[i][j]),
							            ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
							<< "ERROR Was not thrown error with STR_LITERAL (IDENT BOOLEAN)";
						}
					}
				}
			}
			else
			{
				if (valExprToken.type != IDENT)
				{
					if (literalToType(valExprToken.type) == typeArr[i][j])
					{
						compiler::compLet(ident, new node::Expr{new node::ValExpr({valExprArr[i]})}, typeArr[i][j]);
						if (valExprToken.type == INT_LITERAL)
						{
							ASSERT_EQ(compiler::getM_output(), "\tmov rdx, " + valExprToken.value.value() + "\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output INT_LITERAL(DEFAULT TYPE)";
						}
						else if (valExprToken.type == CHAR_LITERAL)
						{
							ASSERT_EQ(compiler::getM_output(), "\tmov rdx, '" + valExprToken.value.value() + "'\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output CHAR_LITERAL(DEFAULT TYPE)";
						}
						else if (valExprToken.type == STRING_LITERAL)
						{
							ASSERT_EQ(compiler::getM_output(), "\tmov rdx, SC0\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output STRING_LITERAL(DEFAULT TYPE)";
						}
						else if (valExprToken.type == BOOL_LITERAL)
						{
							ASSERT_EQ(compiler::getM_output(), "\tmov rdx, " + std::to_string(valExprToken.value.value() == "true") + "\n\tpush rdx\n\txor rdx, rdx\n")
							<< "ERROR Incorrect output BOOL_LITERAL(DEFAULT TYPE)";
						}
						else
						{
							FAIL() << "ERROR Unexpected literal";
						}
					}
					else
					{
						ASSERT_EXIT(compiler::compLet(ident, new node::Expr{new node::ValExpr({valExprArr[i]})}, typeArr[i][j]),
									::testing::ExitedWithCode(EXIT_FAILURE), ".*")
						<< "ERROR Was not thrown error with incorrect type(DEFAULT TYPE)";
					}
				}
				else
				{
					for(int k = 0; k < typeArr[i].size(); ++k)
					{
						compiler::clearM_output();
						compiler::clearM_vars();
						compiler::resetM_labelCount();
						compiler::resetM_stackSize();
						compiler::pushVar(ident.value.value()+"1", typeArr[i][k]);
						compiler::push("rdx");
						if (k == j)
						{
							compiler::compLet(ident,
											  new node::Expr{new node::ValExpr(
											  node::ExprIdent{valExprToken.type, valExprToken.value.value()+"1"})},
											  typeArr[i][j]);
							ASSERT_EQ(compiler::getM_output(), "\tpush rdx\n\tpush QWORD [rsp + 0]\n")
							<< "ERROR Incorrect output (IDENT DEFAULT TYPE)";
						}
						else
						{
							ASSERT_EXIT(compiler::compLet(ident, new node::Expr{new node::ValExpr(node::ExprIdent{valExprToken.type, valExprToken.value.value()+"1"})}, typeArr[i][j]);,
							            ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
							<< "ERROR Was not thrown error with incorrect type(IDENT DEFAULT TYPE)";
						}
					}
				}
			}
		}

	}
}
