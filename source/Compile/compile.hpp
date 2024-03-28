#pragma once
#include "../Lexer/lexer.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <unordered_map>

bool CreateObjectFile(std::string Path);

bool LinkObjectFiles(std::string Path);

class compiler
{
public:
    compiler(node::Prog prog) : m_prog(std::move(prog)){}

    void comp_expr(const node::Expr& expr, std::string ExpectedType);


    void comp_stmt(const node::Stmt& stmt);


    std::stringstream compile();

private:

    static void push(const std::string& reg);

    static void pop(const std::string& reg);

    struct Var
    {
        size_t stack_loc;
        std::string Type;
    };
    

    const node::Prog m_prog;
    static std::stringstream m_output; 
    static size_t m_stack_size;
    static std::unordered_map<std::string, Var> m_vars;
};

