#include <gtest/gtest.h>
#include "../source/Lexer/lexer.cpp"

std::vector<std::vector<std::string>> typeArr =
    {
        {INT_TYPE, STR_TYPE, BOOL_TYPE, CHAR_TYPE},
        {STR_TYPE, BOOL_TYPE, CHAR_TYPE, INT_TYPE},
        {BOOL_TYPE, CHAR_TYPE, INT_TYPE, STR_TYPE},
        {CHAR_TYPE, INT_TYPE, STR_TYPE, BOOL_TYPE},
        {INT_TYPE, STR_TYPE, BOOL_TYPE, CHAR_TYPE},
};

std::vector<std::vector<Token>> litArr =
    {
        {{INT_LITERAL, "val"}},
        {{QOUTE}, {STRING_LITERAL, "val"}, {QOUTE}},
        {{BOOL_LITERAL, "val"}},
        {{APOST}, {CHAR_LITERAL, "val"}, {APOST}},
        {{IDENT, "val"}},
};

std::vector<Tokens> binOpArr = {PLUS, MINUS, MULT, DIV, EQEQ, NOTEQ, LESS, LESSEQ, GREATER, GREATEQ, AND, OR};

struct valExprVisitor
{
    std::string expectedType;
    explicit valExprVisitor(std::string expectedType) : expectedType(std::move(expectedType)) {}
    void operator()(const node::ExprIntLit &exprInt) const
    {
        if (expectedType == INT_TYPE || expectedType == BOOL_TYPE)
        {
            ASSERT_EQ(exprInt.intLit.type, INT_LITERAL) << "ERROR Incorrect token type for INT_TYPE\n";
            ASSERT_EQ(exprInt.intLit.value, "val") << "ERROR Incorrect token value for INT_TYPE\n";
        }
        else
        {
            FAIL() << "ERROR Incorrect type\n";
        }
    }
    void operator()(const node::ExprStrLit &exprStr) const
    {
        if (expectedType == STR_TYPE)
        {
            ASSERT_EQ(exprStr.strLit.type, STRING_LITERAL) << "ERROR Incorrect token type for STR_TYPE\n";
            ASSERT_EQ(exprStr.strLit.value, "val") << "ERROR Incorrect token value for STR_TYPE\n";
        }
        else
        {
            FAIL() << "ERROR Incorrect type\n";
        }
    }
    void operator()(const node::ExprCharLit &exprChar) const
    {
        if (expectedType == CHAR_TYPE || expectedType == BOOL_TYPE)
        {
            ASSERT_EQ(exprChar.charLit.type, CHAR_LITERAL) << "ERROR Incorrect token type for CHAR_TYPE\n";
            ASSERT_EQ(exprChar.charLit.value, "val") << "ERROR Incorrect token value for CHAR_TYPE\n";
        }
        else
        {
            FAIL() << "ERROR Incorrect type\n";
        }
    }
    void operator()(const node::ExprBoolLit &exprBool) const
    {
        if (expectedType == BOOL_TYPE || expectedType == BOOL_TYPE)
        {
            ASSERT_EQ(exprBool.boolLit.type, BOOL_LITERAL) << "ERROR Incorrect token type for BOOL_TYPE\n";
            ASSERT_EQ(exprBool.boolLit.value, "val") << "ERROR Incorrect token value for BOOL_TYPE\n";
        }
        else
        {
            FAIL() << "ERROR Incorrect type\n";
        }
    }
    void operator()(const node::ExprIdent &exprIdent) const
    {
        ASSERT_EQ(exprIdent.ident.type, IDENT) << "ERROR Incorrect token type for IDENT\n";
        ASSERT_EQ(exprIdent.ident.value, "val") << "ERROR Incorrect token value for IDENT\n";
    }
	void operator()(const node::NotCondition &exprNotCond) const
	{
	}
};

struct exprVisitor
{
    Tokens op;
    std::string expectedType;
    exprVisitor(Tokens opr, std::string expectedType) : op(opr), expectedType(std::move(expectedType)) {}
    explicit exprVisitor(std::string expectedType) : expectedType(std::move(expectedType)) {}
    void operator()(node::ValExpr *valExpr) const
    {
        valExprVisitor valExprVisit(expectedType);
        std::visit(valExprVisit, valExpr->var);
    }
    void operator()(node::BinExpr *binExpr) const
    {
        auto checkExprAdd = [&]()
        {
            return *std::get_if<node::BinExprAdd *>(&binExpr->var);
        };
        auto checkExprSub = [&]()
        {
            return *std::get_if<node::BinExprSub *>(&binExpr->var);
        };
        auto checkExprMul = [&]()
        {
            return *std::get_if<node::BinExprMul *>(&binExpr->var);
        };
        auto checkExprDiv = [&]()
        {
            return *std::get_if<node::BinExprDiv *>(&binExpr->var);
        };
        auto checkEqCondition = [&]()
        {
            return *std::get_if<node::EQCondition *>(&binExpr->var);
        };
        auto checkNotEqCondition = [&]()
        {
            return *std::get_if<node::NotEQCondition *>(&binExpr->var);
        };
        auto checkLessCondition = [&]()
        {
            return *std::get_if<node::LessCondition *>(&binExpr->var);
        };
        auto checkEqLessCondition = [&]()
        {
            return *std::get_if<node::EQLessCondition *>(&binExpr->var);
        };
        auto checkGreaterCondition = [&]()
        {
            return *std::get_if<node::GreaterCondition *>(&binExpr->var);
        };
        auto checkEqGreaterCondition = [&]()
        {
            return *std::get_if<node::EQGreaterCondition *>(&binExpr->var);
        };
        auto checkAndCondition = [&]()
        {
            return *std::get_if<node::AndCondition *>(&binExpr->var);
        };
        auto checkOrCondition = [&]()
        {
            return *std::get_if<node::OrCondition *>(&binExpr->var);
        };
        node::Expr fValExpr;
        node::Expr sValExpr;
        switch (op)
        {
        case PLUS:
            fValExpr.var = checkExprAdd()->fvl->var;
            sValExpr.var = checkExprAdd()->svl->var;
            break;
        case MINUS:
            fValExpr.var = checkExprSub()->fvl->var;
            sValExpr.var = checkExprSub()->svl->var;
            break;
        case MULT:
            fValExpr.var = checkExprMul()->fvl->var;
            sValExpr.var = checkExprMul()->svl->var;
            break;
        case DIV:
            fValExpr.var = checkExprDiv()->fvl->var;
            sValExpr.var = checkExprDiv()->svl->var;
            break;
        case EQEQ:
            fValExpr.var = checkEqCondition()->fvl->var;
            sValExpr.var = checkEqCondition()->svl->var;
            break;
        case NOTEQ:
            fValExpr.var = checkNotEqCondition()->fvl->var;
            sValExpr.var = checkNotEqCondition()->svl->var;
            break;
        case LESS:
            fValExpr.var = checkLessCondition()->fvl->var;
            sValExpr.var = checkLessCondition()->svl->var;
            break;
        case LESSEQ:
            fValExpr.var = checkEqLessCondition()->fvl->var;
            sValExpr.var = checkEqLessCondition()->svl->var;
            break;
        case GREATER:
            fValExpr.var = checkGreaterCondition()->fvl->var;
            sValExpr.var = checkGreaterCondition()->svl->var;
            break;
        case GREATEQ:
            fValExpr.var = checkEqGreaterCondition()->fvl->var;
            sValExpr.var = checkEqGreaterCondition()->svl->var;
            break;
        case AND:
            fValExpr.var = checkAndCondition()->fvl->var;
            sValExpr.var = checkAndCondition()->svl->var;
            break;
        case OR:
            fValExpr.var = checkOrCondition()->fvl->var;
            sValExpr.var = checkOrCondition()->svl->var;
            break;
        default:
            FAIL() << "ERROR Incorrect operator\n";
        }
        exprVisitor exprVisit(op, expectedType);
        std::visit(exprVisit, fValExpr.var);
        std::visit(exprVisit, sValExpr.var);
    }
    void operator()([[maybe_unused]] node::StmtInput *input)
    {
        FAIL() << "ERROR Unexpected input statement\n";
    }
};
TEST(ParserTest, ParseInputStmtTest)
{
    for (int i = 0; i < litArr.size(); ++i)
    {
        parser p({});
        p.pushToken(Token{INPUT});
	    ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
		<< "ERROR Successfully parsed without all tokens";
        p.pushToken(Token{LPAREN});
	    ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
		<< "ERROR Successfully parsed without all tokens";
        for (const auto & k : litArr[i])
        {
            p.pushToken(k);
        }
	    ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
		<< "ERROR Successfully parsed without all tokens";
        p.pushToken(Token{RPAREN});
	    ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
		<< "ERROR Successfully parsed without all tokens";
        p.pushToken(Token{SEMICOLON});
        auto parsedInput = p.parseInputStmt();
        ASSERT_TRUE(parsedInput.has_value()) << "ERROR Parsed input statement has not value\n";
        exprVisitor exprVisit(typeArr[i][0]);
        std::visit(exprVisit, parsedInput.value().msg->var);
    }
}
TEST(ParserTest, parseValExprTest)
{
    for (int i = 0; i < typeArr.size(); ++i)
    {
        for (int j = 0; j < typeArr[i].size(); ++j)
        {
            parser p({});
			ASSERT_FALSE(p.parseValExpr(typeArr[i][j]))
			<< "ERROR Successfully parsed without literal";
            for (const auto & k : litArr[i])
            {
                p.pushToken(k);
            }
            if (j == 0 || litArr[i][0].type == IDENT || typeArr[i][j] == BOOL_TYPE && typeArr[i][0] != STR_TYPE)
            {
                auto parsedValExpr = p.parseValExpr(typeArr[i][j]);
                ASSERT_TRUE(parsedValExpr.has_value()) << "ERROR Parsed value expression has not value\n";
                valExprVisitor valExprVisit(typeArr[i][j]);
                std::visit(valExprVisit, parsedValExpr.value().var);
            }
            else
            {
                ASSERT_FALSE(p.parseValExpr(typeArr[i][j]).has_value())
                << "ERROR Incorrect parsed value expression has value\n";
            }
        }
    }
}
TEST(ParserTest, ParseExprTest)
{
	for (int i = 0; i < typeArr.size(); ++i)
	{
		for (int j = 0; j < typeArr.size(); ++j)
		{
			for (int k = 0; k < typeArr[i].size(); ++k)
			{
				for (auto & l: binOpArr)
	            {
		            parser p({});
		            ASSERT_EXIT(p.parseExpr(typeArr[i][k]), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
					<< "ERROR Was not thrown error without first literal";
		            for (const auto & o : litArr[i])
	                {
	                    p.pushToken(o);
	                }
	                p.pushToken(Token{l});
		            ASSERT_EXIT(p.parseExpr(typeArr[i][k]), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
					<< "ERROR Was not thrown error without second literal";
	                for (const auto & o : litArr[j])
	                {
	                    p.pushToken(o);
	                }
					if (litArr[i][0].type == IDENT && litArr[j][0].type == IDENT || i == j && k == 0 || typeArr[i][k] == BOOL_TYPE && typeArr[i][0] != STR_TYPE && typeArr[j][0] != STR_TYPE)
	                {
	                    auto parsedExpr = p.parseExpr(typeArr[i][k]);
	                    ASSERT_TRUE(parsedExpr.has_value()) << "ERROR Parsed expression has not value\n";
	                    exprVisitor exprVisit(l, typeArr[i][k]);
	                    std::visit(exprVisit, parsedExpr.value().var);
	                }
					else if (litArr[i][0].type == IDENT || litArr[j][0].type == IDENT)
					{
						if (litArr[i][0].type == IDENT && typeArr[i][k] == typeArr[j][0] || litArr[j][0].type == IDENT && typeArr[i][k] == typeArr[i][0])
						{
							auto parsedExpr = p.parseExpr(typeArr[i][k]);
							ASSERT_TRUE(parsedExpr.has_value()) << "ERROR Parsed expression has not value(IDENT)\n";
							exprVisitor exprVisit(l, typeArr[i][k]);
							std::visit(exprVisit, parsedExpr.value().var);
						}
						else
						{
							ASSERT_EXIT(p.parseExpr(typeArr[i][k]), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
							<< "ERROR Was not thrown error while parsing incorrect expression(IDENT)\n";
						}
					}
					else
	                {
						ASSERT_EXIT(p.parseExpr(typeArr[i][k]), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
						<< "ERROR Was not thrown error while parsing incorrect expression\n";
	                }
				}
			}
		}
	}
}
TEST(ParserTest, ParseLetStmtTest)
{
    std::vector<Tokens> letArr = {INT_LET, STRING_LET, BOOL_LET, CHAR_LET, INT_LET};
    for (int i = 0; i < typeArr.size(); ++i)
    {
        for (int j = 0; j < typeArr[i].size(); ++j)
        {
            parser p({});
            p.pushToken({letArr[i]});
	        ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
			<< "ERROR Successfully parsed without all tokens";
            p.pushToken({IDENT, "val"});
	        ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
			<< "ERROR Successfully parsed without all tokens";
            p.pushToken({EQ});
	        ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
			<< "ERROR Successfully parsed without all tokens";
            for (const auto & k : litArr[i])
            {
                p.pushToken(k);
            }
            p.pushToken({SEMICOLON});
            if (j == 0 || litArr[i][0].type == IDENT || typeArr[i][j] == BOOL_TYPE && typeArr[i][0] != STR_TYPE)
            {
                auto parsedLet = p.parseLet(typeArr[i][j]);
                ASSERT_TRUE(parsedLet.has_value()) << "ERROR Parsed let statement has not value\n";
                exprVisitor exprVisit(typeArr[i][j]);
                std::visit(exprVisit, parsedLet.value().Expr->var);
                ASSERT_EQ(parsedLet.value().ident.type, IDENT)
                << "ERROR Incorrect ident type in parsed let statement\n";
                ASSERT_EQ(parsedLet.value().ident.value.value(), "val")
                << "ERROR Incorrect value type in parsed let statement\n";
            }
            else
            {
                ASSERT_EXIT(p.parseLet(typeArr[i][j]), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
                << "ERROR Was not thrown error while parsing incorrect let statement\n";
            }
        }
    }
}
TEST(ParserTest, ParseIfStmtTest)
{
    for (int i = 0; i < litArr.size(); ++i)
    {
        parser p({});
        p.pushToken({IF});
	    ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
		<< "ERROR Successfully parsed without all tokens";
        p.pushToken({LPAREN});
		ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
		<< "ERROR Successfully parsed without all tokens";
        for (const auto & j : litArr[i])
        {
            p.pushToken(j);
        }
        p.pushToken({RPAREN});
		ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
		<< "ERROR Successfully parsed without all tokens";
        p.pushToken({LBRACKET});
		ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
		<< "ERROR Successfully parsed without all tokens";
        p.pushToken({RBRACKET});
        auto parsedIf = p.parseIfStmt();
        ASSERT_TRUE(parsedIf.has_value()) << "ERROR Parsed if statement has not value\n";
        exprVisitor exprVisit(typeArr[i][0]);
        std::visit(exprVisit, parsedIf.value().cond->var);
        ASSERT_FALSE(parsedIf.value().pred.has_value()) << "ERROR Unused if pred in if statement has value\n";
        ASSERT_TRUE(parsedIf.value().statements.empty()) << "ERROR Unused statements in if statement has value\n";
    }
}
TEST(ParserTest, ParseIfPredTest)
{
    struct ifPredVisitor
    {
        std::string expectedType;
        explicit ifPredVisitor(std::string expectedType) : expectedType(std::move(expectedType)) {}
        ifPredVisitor() = default;
        void operator()(const node::StmtElIf *stmtElif) const
        {
            exprVisitor exprVisit(expectedType);
            std::visit(exprVisit, stmtElif->cond->var);
            ASSERT_FALSE(stmtElif->pred.has_value()) << "ERROR Unused if pred in elif statement has value\n";
            ASSERT_TRUE(stmtElif->statements.empty()) << "ERROR Unused statements in elif statement has value\n";
        }
        void operator()(const node::StmtElse *stmtElse)
        {
            ASSERT_TRUE(stmtElse->statements.empty()) << "ERROR Unused statements in else statement has value\n";
        }
    };
    for (int i = 0; i < litArr.size(); ++i)
    {
        parser p({});
        p.pushToken({ELIF});
		ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
		<< "ERROR Successfully parsed without all tokens";
        p.pushToken({LPAREN});
		ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
		<< "ERROR Successfully parsed without all tokens";
        for (const auto & j : litArr[i])
        {
            p.pushToken(j);
        }
        p.pushToken({RPAREN});
		ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
		<< "ERROR Successfully parsed without all tokens";
        p.pushToken({LBRACKET});
		ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
		<< "ERROR Successfully parsed without all tokens";
        p.pushToken({RBRACKET});
        auto parsedIfPred = p.parseIfPred();
        ASSERT_TRUE(parsedIfPred.has_value()) << "ERROR Parsed if pred(ELIF) has not value\n";
        ifPredVisitor ifPredVisit(typeArr[i][0]);
        std::visit(ifPredVisit, parsedIfPred.value().var);
    }
    parser p({});
    p.pushToken({ELSE});
	ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
	<< "ERROR Successfully parsed without all tokens";
    p.pushToken({LBRACKET});
	ASSERT_EXIT(p.parseInputStmt(), ::testing::ExitedWithCode(EXIT_FAILURE), ".*")
	<< "ERROR Successfully parsed without all tokens";
    p.pushToken({RBRACKET});
    auto parsedIfPred = p.parseIfPred();
    ASSERT_TRUE(parsedIfPred.has_value()) << "ERROR Parsed if pred(ELSE) has not value\n";
    ifPredVisitor ifPredVisit;
    std::visit(ifPredVisit, parsedIfPred.value().var);
}