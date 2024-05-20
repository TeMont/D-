#include "tokenUtils.hpp"

std::optional<uint8_t> op_to_prior(Tokens op)
{
	switch (op)
	{
		case Tokens::OR:
			return 1;
		case Tokens::AND:
			return 2;
		case Tokens::EQEQ:
		case Tokens::NOTEQ:
			return 3;
		case Tokens::LESS:
		case Tokens::LESSEQ:
		case Tokens::GREATER:
		case Tokens::GREATEQ:
			return 4;
		case Tokens::PLUS:
		case Tokens::MINUS:
			return 5;
		case Tokens::MULT:
		case Tokens::DIV:
			return 6;

		default:
			return {};
	}
}

std::unordered_map<Tokens, std::string> letToType = {{Tokens::INT_LET, INT_TYPE},
                                                     {Tokens::STRING_LET, STR_TYPE},
                                                     {Tokens::CHAR_LET, CHAR_TYPE},
                                                     {Tokens::BOOL_LET, BOOL_TYPE},
                                                     {Tokens::FLOAT_LET, FLOAT_TYPE},};