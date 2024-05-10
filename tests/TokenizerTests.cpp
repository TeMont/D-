#include <gtest/gtest.h>
#include "../source/Token/token.cpp"

TEST(TokenizerTest, TokenizeTest)
{
    std::string source = "//int x = 10 string \"string\" bool true char 'a' + - * / ( ) ; { } return stdOut stdInput ==  < > >= <= ! != || && if elif else const\n"
                         "int x = 10 string \"string\" bool true char 'a' + - * / ( ) ; { } return stdOut stdInput ==  < > >= <= ! != || && if elif else const\n"
                         "/*int x = 10 string \"string\" bool true char 'a' + - * / ( ) ; { } return stdOut stdInput ==  < > >= <= ! != || && if elif else const\n"
                         "int x = 10 string \"string\" bool true char 'a' + - * / ( ) ; { } return stdOut stdInput ==  < > >= <= ! != || && if elif else const\n"
                         "int x = 10 string \"string\" bool true char 'a' + - * / ( ) ; { } return stdOut stdInput ==  < > >= <= ! != || && if elif else const*/\n";

    tokenizer tknzr(source);

    std::vector<Token> tokens = tknzr.tokenize();

    ASSERT_EQ(tokens.size(), 39) << "ERROR Incorrect tokens array size\n";
    ASSERT_EQ(tokens[0].type, INT_LET) << "ERROR Incorrect type at index 0: " << tokens[0].type << "\n";
    ASSERT_EQ(tokens[1].type, IDENT) << "ERROR Incorrect type at index 1: " << tokens[1].type << "\n";
    ASSERT_EQ(tokens[1].value, "x") << "ERROR Incorrect value at index 1: " << tokens[1].value.value() << "\n";
    ASSERT_EQ(tokens[2].type, EQ) << "ERROR Incorrect type at index 2: " << tokens[2].type << "\n";
    ASSERT_EQ(tokens[3].type, INT_LITERAL) << "ERROR Incorrect type at index 3: " << tokens[3].type << "\n";
    ASSERT_EQ(tokens[3].value, "10") << "ERROR Incorrect value at index 3: " << tokens[3].value.value() << "\n";
    ASSERT_EQ(tokens[4].type, STRING_LET) << "ERROR Incorrect type at index 4: " << tokens[4].type << "\n";
    ASSERT_EQ(tokens[5].type, QOUTE) << "ERROR Incorrect type at index 5: " << tokens[5].type << "\n";
    ASSERT_EQ(tokens[6].type, STRING_LITERAL) << "ERROR Incorrect type at index 6: " << tokens[6].type << "\n";
    ASSERT_EQ(tokens[6].value, "string") << "ERROR Incorrect value at index 6: " << tokens[6].value.value() << "\n";
    ASSERT_EQ(tokens[7].type, QOUTE) << "ERROR Incorrect type at index 7: " << tokens[7].type << "\n";
    ASSERT_EQ(tokens[8].type, BOOL_LET) << "ERROR Incorrect type at index 8: " << tokens[8].type << "\n";
    ASSERT_EQ(tokens[9].type, BOOL_LITERAL) << "ERROR Incorrect type at index 9: " << tokens[9].type << "\n";
    ASSERT_EQ(tokens[9].value, "true") << "ERROR Incorrect value at index 9: " << tokens[9].value.value() << "\n";
    ASSERT_EQ(tokens[10].type, CHAR_LET) << "ERROR Incorrect type at index 10: " << tokens[10].type << "\n";
    ASSERT_EQ(tokens[11].type, APOST) << "ERROR Incorrect type at index 11: " << tokens[11].type << "\n";
    ASSERT_EQ(tokens[12].type, CHAR_LITERAL) << "ERROR Incorrect type at index 12: " << tokens[12].type << "\n";
    ASSERT_EQ(tokens[12].value, "a") << "ERROR Incorrect value at index 12: " << tokens[12].value.value() << "\n";
    ASSERT_EQ(tokens[13].type, APOST) << "ERROR Incorrect type at index 13: " << tokens[13].type << "\n";
    ASSERT_EQ(tokens[14].type, PLUS) << "ERROR Incorrect type at index 14: " << tokens[14].type << "\n";
    ASSERT_EQ(tokens[15].type, MINUS) << "ERROR Incorrect type at index 15: " << tokens[15].type << "\n";
    ASSERT_EQ(tokens[16].type, MULT) << "ERROR Incorrect type at index 16: " << tokens[16].type << "\n";
    ASSERT_EQ(tokens[17].type, DIV) << "ERROR Incorrect type at index 17: " << tokens[17].type << "\n";
    ASSERT_EQ(tokens[18].type, LPAREN) << "ERROR Incorrect type at index 18: " << tokens[18].type << "\n";
    ASSERT_EQ(tokens[19].type, RPAREN) << "ERROR Incorrect type at index 19: " << tokens[19].type << "\n";
    ASSERT_EQ(tokens[20].type, SEMICOLON) << "ERROR Incorrect type at index 20: " << tokens[20].type << "\n";
    ASSERT_EQ(tokens[21].type, LBRACKET) << "ERROR Incorrect type at index 21: " << tokens[21].type << "\n";
    ASSERT_EQ(tokens[22].type, RBRACKET) << "ERROR Incorrect type at index 22: " << tokens[22].type << "\n";
    ASSERT_EQ(tokens[23].type, RETURN) << "ERROR Incorrect type at index 23: " << tokens[23].type << "\n";
    ASSERT_EQ(tokens[24].type, OUTPUT) << "ERROR Incorrect type at index 24: " << tokens[24].type << "\n";
    ASSERT_EQ(tokens[25].type, INPUT) << "ERROR Incorrect type at index 25: " << tokens[25].type << "\n";
    ASSERT_EQ(tokens[26].type, EQEQ) << "ERROR Incorrect type at index 26: " << tokens[26].type << "\n";
    ASSERT_EQ(tokens[27].type, LESS) << "ERROR Incorrect type at index 27: " << tokens[27].type << "\n";
    ASSERT_EQ(tokens[28].type, GREATER) << "ERROR Incorrect type at index 28: " << tokens[28].type << "\n";
    ASSERT_EQ(tokens[29].type, GREATEQ) << "ERROR Incorrect type at index 29: " << tokens[29].type << "\n";
    ASSERT_EQ(tokens[30].type, LESSEQ) << "ERROR Incorrect type at index 30: " << tokens[30].type << "\n";
    ASSERT_EQ(tokens[31].type, NOT) << "ERROR Incorrect type at index 31: " << tokens[31].type << "\n";
    ASSERT_EQ(tokens[32].type, NOTEQ) << "ERROR Incorrect type at index 32: " << tokens[32].type << "\n";
    ASSERT_EQ(tokens[33].type, OR) << "ERROR Incorrect type at index 33: " << tokens[33].type << "\n";
    ASSERT_EQ(tokens[34].type, AND) << "ERROR Incorrect type at index 34: " << tokens[34].type << "\n";
    ASSERT_EQ(tokens[35].type, IF) << "ERROR Incorrect type at index 35: " << tokens[35].type << "\n";
    ASSERT_EQ(tokens[36].type, ELIF) << "ERROR Incorrect type at index 36: " << tokens[36].type << "\n";
	ASSERT_EQ(tokens[37].type, ELSE) << "ERROR Incorrect type at index 37: " << tokens[37].type << "\n";
	ASSERT_EQ(tokens[38].type, CONST) << "ERROR Incorrect type at index 38: " << tokens[37].type << "\n";
}
