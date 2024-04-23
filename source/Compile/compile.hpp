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
    static bool comp_expr(const node::Expr& expr, std::string ExpectedType);
    static bool comp_bin_expr(const node::BinExpr& expr, std::string ExpectedType);
    static bool comp_val_expr(const node::ValExpr& expr, std::string ExpectedType);
    static void comp_if_pred(const node::IfPred &pred, std::string end_label);
    static void comp_var(Token ident, node::Expr* Expr, std::string ExpectedType);
    static void comp_let(Token ident, node::Expr* Expr, std::string ExpectedType);
    static void comp_stmt(const node::Stmt& stmt);
    static std::string create_label();
    static std::string create_SC_label();

    
    struct Var
    {
        size_t stack_loc;
        std::string Type;
    };
    
    const node::Prog m_prog;
    static size_t m_stack_size;
    static std::unordered_map<std::string, Var> m_vars;

    static std::stringstream m_output; 
    static std::stringstream m_SC; 

    static uint64_t label_count;
    static uint64_t SC_count;
};

