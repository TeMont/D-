#pragma once
#include "../token.hpp"
#include <unordered_map>

extern std::unordered_map<Tokens, std::string> letToType;
std::optional<uint8_t> op_to_prior(Tokens op);