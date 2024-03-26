#include "compile.hpp"

bool CreateObjectFile(std::string Path)
{

    int res = system(("nasm -f win64 " + Path).c_str());
    if (res)
    {
        std::cerr << "Error Creating Object File";
        exit(EXIT_FAILURE);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 1;
}

bool LinkObjectFiles(std::string Path)
{

    int res = system(("golink /entry:_start /console kernel32.dll user32.dll " + Path).c_str());
    if (res)
    {
        std::cerr << "Error Linking File";
        exit(EXIT_FAILURE);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 1;
}

size_t compiler::m_stack_size;
std::stringstream compiler::m_output;
std::unordered_map<std::string, compiler::Var> compiler::m_vars;

void compiler::comp_expr(const node::Expr &expr)
{
    struct ExprVisitor
    {
        void operator()(const node::ExprIdent &expr_ident)
        {
            if (m_vars.count(expr_ident.ident.value))
            {
                const auto& var = m_vars[expr_ident.ident.value];
                push("QWORD [rsp + " + std::to_string((m_stack_size - var.stack_loc - 1) * 8) + "]");
            }
            else 
            {
                std::cerr << "ERR005 Undeclared Indentifier '" << expr_ident.ident.value << "'";
                exit(EXIT_FAILURE);
            }
        }
        void operator()(const node::ExprIntLit &expr_int)
        {
            m_output << "\tmov rdi, " << expr_int.int_lit.value << '\n';
            push("rdi");
        }
        void operator()(const node::ExprStrLit &expr_str)
        {
            m_output << "\tmov rdi, '" << expr_str.str_lit.value << '\'\n';
            push("rdi");
        }
    };

    ExprVisitor visitor;
    std::visit(visitor, expr.var);
}

void compiler::comp_stmt(const node::Stmt &stmt)
{
    struct StmtVisitor
    {
        compiler *comp;
        void operator()(const node::StmtReturn &stmt_ret)
        {
            comp->comp_expr(stmt_ret.Expr);
            pop("rcx");
            m_output << "\tcall ExitProcess" << "\n";
        }
        void operator()(const node::StmtIntLet &stmt_int_let)
        {
            if(m_vars.count(stmt_int_let.ident.value))
            {
                std::cerr << "ERR004 Identefier '" << stmt_int_let.ident.value << "' Is Already Defined";
                exit(EXIT_FAILURE);
            }
            else
            {
                m_vars.insert({stmt_int_let.ident.value, Var{m_stack_size}});
                comp->comp_expr(stmt_int_let.expr);
            }
        }
        void operator()(const node::StmtStrLet &stmt_str_let)
        {
            if(m_vars.count(stmt_str_let.ident.value))
            {
                std::cerr << "ERR004 Identefier '" << stmt_str_let.ident.value << "' Is Already Defined";
                exit(EXIT_FAILURE);
            }
        }
    };
    StmtVisitor visitor;
    std::visit(visitor, stmt.var);
}

std::stringstream compiler::compile()
{
    m_output << "section .text\nglobal _start\nextern ExitProcess\n_start:\n";

    for (const node::Stmt &stmt : m_prog.statements)
    {   
        comp_stmt(stmt);
    }

    std::stringstream output = std::move(m_output);
    return output;
}

void compiler::push(const std::string &reg)
{
    m_output << "\tpush " << reg << "\n";
    ++m_stack_size;
}

void compiler::pop(const std::string &reg)
{
    m_output << "\tpop " << reg << "\n";
    --m_stack_size;
}