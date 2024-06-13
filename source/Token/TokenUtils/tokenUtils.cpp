#include "tokenUtils.hpp"

std::optional<uint8_t> op_to_prior(Tokens op)
{
	switch (op)
	{
		case OR:
			return 1;
		case AND:
			return 2;
		case EQEQ:
		case NOTEQ:
			return 3;
		case LESS:
		case LESSEQ:
		case GREATER:
		case GREATEQ:
			return 4;
		case PLUS:
		case MINUS:
			return 5;
		case MULT:
		case DIV:
			return 6;

		default:
			return {};
	}
}

std::unordered_map<Tokens, std::string> letToType = {{INT_LET, INT_TYPE},
                                                     {STRING_LET, STR_TYPE},
                                                     {CHAR_LET, CHAR_TYPE},
                                                     {BOOL_LET, BOOL_TYPE},
                                                     {FLOAT_LET, FLOAT_TYPE},};