#pragma once

#include "../parser.hpp"

class varParser
{
public:
    static std::optional<node::StmtLet> parseLet();
    static std::unordered_map<std::string, std::string> m_vars;
};
