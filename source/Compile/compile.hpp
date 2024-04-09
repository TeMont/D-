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


    std::stringstream compile();

private:

    static void push(const std::string& reg);

    static void pop(const std::string& reg);

    static void comp_expr(const node::Expr& expr, std::string ExpectedType);

    static void comp_bin_expr(const node::BinExpr& expr, std::string ExpectedType);

    static void comp_val_expr(const node::ValExpr& expr, std::string ExpectedType);

    static void comp_stmt(const node::Stmt& stmt);
    
    
    struct Var
    {
        size_t stack_loc;
        std::string Type;
    };
    

    const node::Prog m_prog;
    static std::stringstream m_output; 
    static size_t m_stack_size;
    static std::unordered_map<std::string, Var> m_vars;

    static uint64_t m_end_label_count;
    static uint64_t m_false_label_count;
    static uint64_t m_true_label_count;
};

