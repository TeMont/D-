#pragma once
#include "../Lexer/lexer.hpp"
#include <thread>
#include <chrono>
#include <iostream>

std::stringstream TokensToAsm(std::vector<Token> TokensVec);

bool CreateObjectFile(std::string Path);

bool LinkObjectFiles(std::string Path);

class compiler
{
public:
    compiler(node::RETURN root) : m_root(std::move(root)){}

    std::stringstream compile() const
    {
        std::stringstream output;
        output << "section .text\nglobal _start\n_start:\n";
        output << "\tmov rax, " << m_root.Expr.int_lit.value << '\n';
        output << "\tret\n";

        return output;
    }        

private:
    const node::RETURN m_root;


};

