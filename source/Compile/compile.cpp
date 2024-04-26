#include "compile.hpp"

bool CreateObjectFile(std::string Path)
{
    int res = system(("nasm -fwin64 " + Path + ".asm").c_str());
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

    int res = system(("gcc.exe " + Path + ".obj -g -o" + Path + ".exe -m64").c_str());
    if (res)
    {
        std::cerr << "Error Linking File";
        exit(EXIT_FAILURE);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 1;
}

std::string compiler::create_label()
{
    std::stringstream ss;
    ss << "label" << label_count;
    label_count++;
    return ss.str();
}

std::string compiler::create_SC_label()
{
    std::stringstream ss;
    ss << "SC" << SC_count;
    SC_count++;
    return ss.str();
}

size_t compiler::m_stack_size;
std::unordered_map<std::string, compiler::Var> compiler::m_vars;
uint64_t compiler::label_count = 0;
uint64_t compiler::SC_count = 0;

std::stringstream compiler::m_output;
std::stringstream compiler::m_SC;
std::stringstream compiler::m_bss_SC;

bool compiler::comp_val_expr(const node::ValExpr &expr, std::string ExpectedType)
{
    struct ExprVisitor
    {
        std::string ExpectedType;

        ExprVisitor(std::string expectedType) : ExpectedType(std::move(expectedType)) {}

        bool operator()(const node::ExprIdent &expr_ident)
        {
            if (m_vars.count(expr_ident.ident.value.value()))
            {
                const auto &var = m_vars[expr_ident.ident.value.value()];
                if (ExpectedType == ANY_TYPE || ExpectedType == var.Type)
                {
                    push("QWORD [rsp + " + std::to_string((m_stack_size - var.stack_loc - 1) * 8) + "]");
                    return 1;
                }
                else if (ExpectedType == BOOL_TYPE && var.Type != STR_TYPE)
                {
                    comp_bool_expr({"QWORD [rsp + " + std::to_string((m_stack_size - var.stack_loc - 1) * 8) + "]"});
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                std::cerr << "[Compile Error] ERR005 Undeclared Indentifier '" << expr_ident.ident.value.value() << "'";
                exit(EXIT_FAILURE);
            }
        }
        bool operator()(const node::ExprIntLit &expr_int)
        {
            if (ExpectedType == ANY_TYPE || ExpectedType == INT_TYPE)
            {
                if ((expr_int.int_lit.value.has_value()))
                {
                    m_output << "\tmov rdx, " << expr_int.int_lit.value.value() << '\n';
                }
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                return 1;
            }
            else if (ExpectedType == BOOL_TYPE)
            {
                comp_bool_expr(expr_int.int_lit.value);
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::ExprCharLit &expr_char)
        {
            if (ExpectedType == ANY_TYPE || ExpectedType == CHAR_TYPE)
            {
                if ((expr_char.char_lit.value.has_value()))
                {
                    m_output << "\tmov rdx, '" << expr_char.char_lit.value.value() << "'\n";
                }
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                return 1;
            }
            else if (ExpectedType == BOOL_TYPE)
            {
                comp_bool_expr(expr_char.char_lit.value);
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::ExprStrLit &expr_str)
        {
            if (ExpectedType == ANY_TYPE || ExpectedType == STR_TYPE)
            {
                if ((expr_str.str_lit.value.has_value()))
                {
                    std::string SC = create_SC_label();
                    m_SC << SC << ": db '" << expr_str.str_lit.value.value() << "',00H\n";
                    m_output << "\tmov rdx, " << SC << '\n';
                }
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::ExprBoolLit &expr_bool)
        {
            if (ExpectedType == ANY_TYPE || ExpectedType == BOOL_TYPE)
            {
                if ((expr_bool.bool_lit.value.has_value()))
                {
                    if (expr_bool.bool_lit.value.value() == "false")
                    {
                        m_output << "\tmov rdx, 0\n";
                    }
                    else if (expr_bool.bool_lit.value.value() == "true")
                    {
                        m_output << "\tmov rdx, 1\n";
                    }
                }
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
    };

    ExprVisitor visitor(ExpectedType);
    return std::visit(visitor, expr.var);
}

void compiler::comp_bool_expr(const std::optional<std::string> &literal)
{
    std::string endLabel = create_label();
    std::string falseLabel = create_label();
    if ((literal.has_value()))
    {
        m_output << "\tmov rdx, " << literal.value() << '\n';
    }
    m_output << "\tcmp rdx, 0\n";
    m_output << "\tjle " << falseLabel << "\n";
    m_output << "\tmov rdx, 1\n";
    m_output << "\tjmp " << endLabel << "\n";
    m_output << "\t" << falseLabel << ":\n";
    m_output << "\tmov rdx, 0\n";
    m_output << "\t" << endLabel << ":\n";
    push("rdx");
    m_output << "\txor rdx, rdx\n";
}

bool compiler::comp_bin_expr(const node::BinExpr &expr, std::string ExpectedType)
{
    struct ExprVisitor
    {
        std::string ExpectedType;

        ExprVisitor(std::string expectedType) : ExpectedType(std::move(expectedType)) {}

        bool operator()(const node::BinExprAdd *bin_expr_add)
        {
            if (comp_expr(*bin_expr_add->fvl, STR_TYPE) || comp_expr(*bin_expr_add->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            else if (comp_expr(*bin_expr_add->fvl, CHAR_TYPE) || comp_expr(*bin_expr_add->svl, CHAR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Arithmetic Operations Cannot Be Used With Char Constants";
                exit(EXIT_FAILURE);
            }
            if (comp_expr(*bin_expr_add->fvl, ExpectedType) && comp_expr(*bin_expr_add->svl, ExpectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tadd rdx, rdi\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::BinExprSub *bin_expr_sub)
        {
            if (comp_expr(*bin_expr_sub->fvl, STR_TYPE) || comp_expr(*bin_expr_sub->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            else if (comp_expr(*bin_expr_sub->fvl, CHAR_TYPE) || comp_expr(*bin_expr_sub->svl, CHAR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Arithmetic Operations Cannot Be Used With Char Constants";
                exit(EXIT_FAILURE);
            }
            if (comp_expr(*bin_expr_sub->fvl, ExpectedType) && comp_expr(*bin_expr_sub->svl, ExpectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tsub rdx, rdi\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::BinExprMul *bin_expr_mul)
        {
            if (comp_expr(*bin_expr_mul->fvl, STR_TYPE) || comp_expr(*bin_expr_mul->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            else if (comp_expr(*bin_expr_mul->fvl, CHAR_TYPE) || comp_expr(*bin_expr_mul->svl, CHAR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Arithmetic Operations Cannot Be Used With Char Constants";
                exit(EXIT_FAILURE);
            }
            if (comp_expr(*bin_expr_mul->fvl, ExpectedType) && comp_expr(*bin_expr_mul->svl, ExpectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\timul rdx, rdi\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::BinExprDiv *bin_expr_div)
        {
            if (comp_expr(*bin_expr_div->fvl, STR_TYPE) || comp_expr(*bin_expr_div->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            else if (comp_expr(*bin_expr_div->fvl, CHAR_TYPE) || comp_expr(*bin_expr_div->svl, CHAR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Arithmetic Operations Cannot Be Used With Char Constants";
                exit(EXIT_FAILURE);
            }
            if (comp_expr(*bin_expr_div->fvl, ExpectedType) && comp_expr(*bin_expr_div->svl, ExpectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tmov rax, rdx\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tidiv rdi\n";
                m_output << "\tmov rdx, rax\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::EQCondition *bin_eq_cond)
        {
            if (comp_expr(*bin_eq_cond->fvl, STR_TYPE) || comp_expr(*bin_eq_cond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string true_label = create_label();
            std::string end_label = create_label();
            if (comp_expr(*bin_eq_cond->fvl, ExpectedType) && comp_expr(*bin_eq_cond->svl, ExpectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, rdi\n";
                m_output << "\tje " << true_label << "\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tjmp " << end_label << "\n";
                m_output << "\t" << true_label << ":\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\t" << end_label << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::NotEQCondition *bin_not_eq_cond)
        {
            if (comp_expr(*bin_not_eq_cond->fvl, STR_TYPE) || comp_expr(*bin_not_eq_cond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string true_label = create_label();
            std::string end_label = create_label();
            if (comp_expr(*bin_not_eq_cond->fvl, ExpectedType) && comp_expr(*bin_not_eq_cond->svl, ExpectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, rdi\n";
                m_output << "\tjne " << true_label << "\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tjmp " << end_label << "\n";
                m_output << "\t" << true_label << ":\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\t" << end_label << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::LessCondition *bin_less_cond)
        {
            if (comp_expr(*bin_less_cond->fvl, STR_TYPE) || comp_expr(*bin_less_cond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string true_label = create_label();
            std::string end_label = create_label();
            if (comp_expr(*bin_less_cond->fvl, ExpectedType) && comp_expr(*bin_less_cond->svl, ExpectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, rdi\n";
                m_output << "\tjl " << true_label << "\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tjmp " << end_label << "\n";
                m_output << "\t" << true_label << ":\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\t" << end_label << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::GreaterCondition *bin_greater_cond)
        {
            if (comp_expr(*bin_greater_cond->fvl, STR_TYPE) || comp_expr(*bin_greater_cond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string true_label = create_label();
            std::string end_label = create_label();
            if (comp_expr(*bin_greater_cond->fvl, ExpectedType) && comp_expr(*bin_greater_cond->svl, ExpectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, rdi\n";
                m_output << "\tjg " << true_label << "\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tjmp " << end_label << "\n";
                m_output << "\t" << true_label << ":\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\t" << end_label << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::EQLessCondition *bin_less_eq_cond)
        {
            if (comp_expr(*bin_less_eq_cond->fvl, STR_TYPE) || comp_expr(*bin_less_eq_cond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string true_label = create_label();
            std::string end_label = create_label();
            if (comp_expr(*bin_less_eq_cond->fvl, ExpectedType) && comp_expr(*bin_less_eq_cond->svl, ExpectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, rdi\n";
                m_output << "\tjle " << true_label << "\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tjmp " << end_label << "\n";
                m_output << "\t" << true_label << ":\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\t" << end_label << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::EQGreaterCondition *bin_great_eq_cond)
        {
            if (comp_expr(*bin_great_eq_cond->fvl, STR_TYPE) || comp_expr(*bin_great_eq_cond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string true_label = create_label();
            std::string end_label = create_label();
            if (comp_expr(*bin_great_eq_cond->fvl, ExpectedType) && comp_expr(*bin_great_eq_cond->svl, ExpectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, rdi\n";
                m_output << "\tjge " << true_label << "\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tjmp " << end_label << "\n";
                m_output << "\t" << true_label << ":\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\t" << end_label << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::AndCondition *bin_and_cond)
        {
            if (comp_expr(*bin_and_cond->fvl, STR_TYPE) || comp_expr(*bin_and_cond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string false_label = create_label();
            std::string end_label = create_label();
            if (comp_expr(*bin_and_cond->fvl, ExpectedType) && comp_expr(*bin_and_cond->svl, ExpectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, 0\n";
                m_output << "\tjle " << false_label << "\n";
                m_output << "\tcmp rdi, 0\n";
                m_output << "\tjle " << false_label << "\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\tjmp " << end_label << "\n";
                m_output << "\t" << false_label << ":\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\t" << end_label << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
        bool operator()(const node::OrCondition *bin_or_cond)
        {
            if (comp_expr(*bin_or_cond->fvl, STR_TYPE) || comp_expr(*bin_or_cond->svl, STR_TYPE))
            {
                std::cerr << "[Compile Error] ERR009 Binary Operations Cannot Be Used With Strings";
                exit(EXIT_FAILURE);
            }
            std::string true_label = create_label();
            std::string end_label = create_label();
            if (comp_expr(*bin_or_cond->fvl, ExpectedType) && comp_expr(*bin_or_cond->svl, ExpectedType))
            {
                pop("rdi");
                pop("rdx");
                m_output << "\tcmp rdx, 0\n";
                m_output << "\tjg " << true_label << "\n";
                m_output << "\tcmp rdi, 0\n";
                m_output << "\tjg " << true_label << "\n";
                m_output << "\tmov rdx, 0\n";
                m_output << "\tjmp " << end_label << "\n";
                m_output << "\t" << true_label << ":\n";
                m_output << "\tmov rdx, 1\n";
                m_output << "\t" << end_label << ":\n";
                push("rdx");
                m_output << "\txor rdx, rdx\n";
                m_output << "\txor rdi, rdi\n";
                return 1;
            }
            else
            {
                return 0;
            }
        }
    };

    ExprVisitor visitor(ExpectedType);
    return std::visit(visitor, expr.var);
}

bool compiler::comp_expr(const node::Expr &expr, std::string ExpectedType)
{
    struct ExprVisitor
    {
        std::string ExpectedType;

        ExprVisitor(std::string expectedType) : ExpectedType(std::move(expectedType)) {}

        bool operator()(const node::BinExpr *BinExpr)
        {
            return comp_bin_expr(*BinExpr, ExpectedType);
        }
        bool operator()(const node::ValExpr *ValExpr)
        {
            return comp_val_expr(*ValExpr, ExpectedType);
        }
        bool operator()(const node::StmtInput *InputExpr)
        {
            m_output << ";;\tInput\n";
            comp_input(*InputExpr);
            if (ExpectedType == STR_TYPE || ExpectedType == ANY_TYPE)
            {
                std::string false_label1 = create_label();
                std::string true_label1 = create_label();
                std::string end_label1 = create_label();
                std::string false_label2 = create_label();
                std::string true_label2 = create_label();
                std::string end_label2 = create_label();
                std::string SC = create_SC_label();
                m_bss_SC << "\t" << SC << " resb 256\n";
                m_output << "\tmov rdi, " << SC << "\n";
                m_output << "\tmov rcx, 256\n";
                m_output << "\trep movsb\n";
                m_output << "\tmov rdx, " << SC << "\n";
                m_output << "\tcall _countStrLen\n";
                m_output << "\tcmp byte [rdx+rcx-1], 10\n";
                m_output << "\tje " << true_label1 << "\n";
                m_output << "\tjmp " << end_label1 << "\n";
                m_output << "\t" << true_label1 << ":\n";
                m_output << "\tmov byte [rdx+rcx-1], 00H\n";
                m_output << "\t" << end_label1 << ":\n";
                m_output << "\tcmp byte [rdx+rcx-2], 13\n";
                m_output << "\tje " << true_label2 << "\n";
                m_output << "\tjmp " << end_label2 << "\n";
                m_output << "\t" << true_label2 << ":\n";
                m_output << "\tmov byte [rdx+rcx-2], 00H\n";
                m_output << "\t" << end_label2 << ":\n";
                push("rdx");
            }
            else if (ExpectedType == CHAR_TYPE)
            {
                m_output << "\tmovzx rdx, byte [rsi]\n";
                push("rdx");
            }
            else if (ExpectedType == INT_TYPE)
            {
                m_output << "\tcall _stoi\n";
                push("rdi");
            }
            else if (ExpectedType == BOOL_TYPE)
            {
                m_output << "\tmovzx rdx, byte [rsi]\n";
                comp_bool_expr("rdx");
            }

            m_output << "\tmov rsi, OutputBuffer\n";
            m_output << "\tmov rdx, 20\n";
            m_output << "\tcall _clearBuffer\n";
            m_output << "\tmov rsi, InputBuffer\n";
            m_output << "\tmov rdx, 256\n";
            m_output << "\tcall _clearBuffer\n";
            m_output << ";;\t/Input\n";
            return 1;
        }
    };

    ExprVisitor visitor(ExpectedType);
    return std::visit(visitor, expr.var);
}

void compiler::comp_input(const node::StmtInput stmt_input)
{
    if (comp_expr(*stmt_input.msg, STR_TYPE))
    {
        pop("rdx");
        m_output << "\tmov rsi, InputBuffer\n";
        m_output << "\tmov rax, 256\n";
        m_output << "\tcall _scanf\n";
    }
    else if (comp_expr(*stmt_input.msg, CHAR_TYPE))
    {
        pop("rdx");
        m_output << "\tmov rsi, OutputBuffer\n";
        m_output << "\tmov [rsi], dx\n";
        m_output << "\tmov rdx, rsi\n";
        m_output << "\tmov rsi, InputBuffer\n";
        m_output << "\tmov rax, 256\n";
        m_output << "\tcall _scanf\n";
    }
    else if (comp_expr(*stmt_input.msg, ANY_TYPE))
    {
        pop("rdx");
        m_output << "\tmov rax, rdx\n";
        m_output << "\tmov rsi, OutputBuffer\n";
        m_output << "\tcall _itoa\n";
        m_output << "\tmov rdx, rsi\n";
        m_output << "\tmov rsi, InputBuffer\n";
        m_output << "\tmov rax, 256\n";
        m_output << "\tcall _scanf\n";
    }
}

void compiler::comp_if_pred(const node::IfPred &pred, std::string end_label)
{
    struct PredVisitor
    {
        std::string end_label;

        PredVisitor(std::string end_label) : end_label(std::move(end_label)) {}

        void operator()(const node::StmtElIf *ElIf)
        {
            m_output << ";;\telif\n";
            std::string false_label = create_label();
            comp_expr(*ElIf->Cond, ANY_TYPE);
            pop("rdx");
            m_output << "\tcmp rdx, 0\n";
            m_output << "\tjle " << false_label << "\n";
            for (int i = 0; i < ElIf->statements.size(); ++i)
            {
                comp_stmt(ElIf->statements[i]);
            }
            m_output << ";;\t/elif\n";
            if (ElIf->pred.has_value())
            {
                m_output << false_label << ":\n";
                comp_if_pred(*ElIf->pred.value(), end_label);
            }
            else
            {
                m_output << false_label << ":\n";
            }
            m_output << "\txor rdx, rdx\n";
        }
        void operator()(const node::StmtElse *Else)
        {
            m_output << ";;\telse\n";
            for (int i = 0; i < Else->statements.size(); ++i)
            {
                comp_stmt(Else->statements[i]);
            }
            m_output << ";;\t/else\n";
        }
    };

    PredVisitor visitor(end_label);
    std::visit(visitor, pred.var);
}

void compiler::comp_var(Token ident, node::Expr *Expr, std::string ExpectedType)
{
    if (m_vars.count(ident.value.value()))
    {
        const auto &var = m_vars[ident.value.value()];
        if (var.Type == BOOL_TYPE)
        {
            std::string false_label = create_label();
            std::string end_label = create_label();
            comp_expr(*Expr, ANY_TYPE);
            pop("rdx");
            m_output << "\tcmp rdx, 0\n";
            m_output << "\tjle " << false_label << "\n";
            m_output << "\tmov rdx, 1\n";
            m_output << "\tjmp " << end_label << "\n";
            m_output << "\t" << false_label << ":\n";
            m_output << "\tmov rdx, 0\n";
            m_output << "\t" << end_label << ":\n";
            m_output << "\tmov [rsp + " + std::to_string((m_stack_size - var.stack_loc - 1) * 8) + "], rdx\n";
            m_output << "\txor rdx, rdx\n";
        }
        else if (var.Type == ExpectedType)
        {
            if (comp_expr(*Expr, ExpectedType))
            {
                pop("rdx");
                m_output << "\tmov [rsp + " + std::to_string((m_stack_size - var.stack_loc - 1) * 8) + "], rdx\n";
                m_output << "\txor rdx, rdx\n";
            }
            else
            {
                std::cerr << "[Compile Error] ERR006 Value Doesnt Mathces Type";
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            std::cerr << "[Compile Error] ERR006 Value Doesnt Mathces Type";
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "[Compile Error] ERR004 Identefier '" << ident.value.value() << "' Was Not Declared";
        exit(EXIT_FAILURE);
    }
}

void compiler:: comp_let(Token ident, node::Expr *Expr, std::string ExpectedType)
{
    if (m_vars.count(ident.value.value()))
    {
        std::cerr << "[Compile Error] ERR004 Identefier '" << ident.value.value() << "' Is Already Declared";
        exit(EXIT_FAILURE);
    }
    else
    {
        m_vars.insert({ident.value.value(), Var{m_stack_size, ExpectedType}});
        if (ExpectedType != BOOL_TYPE)
        {
            if (&Expr->var != nullptr)
            {
                if (comp_expr(*Expr, ExpectedType))
                {
                }
                else
                {
                    std::cerr << "[Compile Error] ERR006 Value Doesnt Mathces Type";
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                push("rdx");
            }
        }
        else
        {
            if (&Expr->var != nullptr)
            {
                comp_expr(*Expr, ANY_TYPE);
            }
            else
            {
                push("rdx");
            }
            pop("rdx");
            comp_bool_expr("rdx");
        }
    }
}

void compiler::comp_stmt(const node::Stmt &stmt)
{
    struct StmtVisitor
    {
        void operator()(const node::StmtReturn &stmt_ret)
        {
            m_output << ";;\treturn\n";
            if (comp_expr(*stmt_ret.Expr, INT_TYPE) || comp_expr(*stmt_ret.Expr, BOOL_TYPE))
            {
                pop("rcx");
                m_output << "\tcall ExitProcess"
                         << "\n";
                m_output << ";;\t/return\n";
            }
            else
            {
                std::cerr << "[Compile Error] ERR006 Value Doesnt Mathces Type";
                exit(EXIT_FAILURE);
            }
        }
        void operator()(const node::StmtIf &stmt_if)
        {
            m_output << ";;\tif\n";
            std::string false_label = create_label();
            comp_expr(*stmt_if.Cond, ANY_TYPE);
            pop("rdx");
            m_output << "\tcmp rdx, 0\n";
            m_output << "\tjle " << false_label << "\n";
            for (int i = 0; i < stmt_if.statements.size(); ++i)
            {
                comp_stmt(stmt_if.statements[i]);
            }
            m_output << ";;\t/if\n";
            if (stmt_if.pred.has_value())
            {
                std::string end_label = create_label();
                m_output << "\tjmp " << end_label << "\n";
                m_output << "\t" << false_label << ":\n";
                comp_if_pred(*stmt_if.pred.value(), end_label);
                m_output << "\t" << end_label << ":\n";
            }
            else
            {
                m_output << "\t" << false_label << ":\n";
            }
            m_output << "\txor rdx, rdx\n";
        }
        void operator()(const node::StmtOutput &stmt_output)
        {
            m_output << ";;\tOutput\n";
            if (comp_expr(*stmt_output.Expr, STR_TYPE))
            {
                pop("rdx");
                m_output << "\tcall _printf\n";
            }
            else if (comp_expr(*stmt_output.Expr, CHAR_TYPE))
            {
                pop("rdx");
                m_output << "\tmov rsi, OutputBuffer\n";
                m_output << "\tmov [rsi], dx\n";
                m_output << "\tmov rdx, rsi\n";
                m_output << "\tcall _printf\n";
                m_output << "\tmov rsi, OutputBuffer\n";
                m_output << "\tmov rdx, 20\n";
                m_output << "\tcall _clearBuffer\n";
            }
            else if (comp_expr(*stmt_output.Expr, ANY_TYPE))
            {
                pop("rdx");
                m_output << "\tmov rax, rdx\n";
                m_output << "\tmov rsi, OutputBuffer\n";
                m_output << "\tcall _itoa\n";
                m_output << "\tmov rdx, rsi\n";
                m_output << "\tcall _printf\n";
                m_output << "\tmov rsi, OutputBuffer\n";
                m_output << "\tmov rdx, 20\n";
                m_output << "\tcall _clearBuffer\n";
            }
            m_output << ";;\t/Output\n";
        }
        void operator()(const node::StmtInput &stmt_input)
        {
            m_output << ";;\tInput\n";
            comp_input(stmt_input);
            m_output << "\tmov rsi, OutputBuffer\n";
            m_output << "\tmov rdx, 20\n";
            m_output << "\tcall _clearBuffer\n";
            m_output << "\tmov rsi, InputBuffer\n";
            m_output << "\tmov rdx, 256\n";
            m_output << "\tcall _clearBuffer\n";
            m_output << ";;\t/Input\n";
        }
        void operator()(const node::StmtIntLet &stmt_int_let)
        {
            m_output << ";;\tint let\n";
            comp_let(stmt_int_let.ident, stmt_int_let.Expr, INT_TYPE);
            m_output << ";;\t/int let\n";
        }
        void operator()(const node::StmtStrLet &stmt_str_let)
        {
            m_output << ";;\tstr let\n";
            comp_let(stmt_str_let.ident, stmt_str_let.Expr, STR_TYPE);
            m_output << ";;\t/str let\n";
        }
        void operator()(const node::StmtBoolLet &stmt_bool_let)
        {
            m_output << ";;\tbool let\n";
            comp_let(stmt_bool_let.ident, stmt_bool_let.Expr, BOOL_TYPE);
            m_output << ";;\t/bool let\n";
        }
        void operator()(const node::StmtCharLet &stmt_char_let)
        {
            m_output << ";;\tchar let\n";
            comp_let(stmt_char_let.ident, stmt_char_let.Expr, CHAR_TYPE);
            m_output << ";;\t/char let\n";
        }
        void operator()(const node::StmtIntVar &stmt_int_var)
        {
            comp_var(stmt_int_var.ident, stmt_int_var.Expr, INT_TYPE);
        }
        void operator()(const node::StmtStrVar &stmt_str_var)
        {
            comp_var(stmt_str_var.ident, stmt_str_var.Expr, STR_TYPE);
        }
        void operator()(const node::StmtBoolVar &stmt_bool_var)
        {
            comp_var(stmt_bool_var.ident, stmt_bool_var.Expr, BOOL_TYPE);
        }
        void operator()(const node::StmtCharVar &stmt_char_var)
        {
            comp_var(stmt_char_var.ident, stmt_char_var.Expr, CHAR_TYPE);
        }
    };
    StmtVisitor visitor;
    std::visit(visitor, stmt.var);
}

std::stringstream compiler::compile()
{
    m_output << "extern GetStdHandle, WriteConsoleA, ReadConsoleA, ExitProcess\n\n"
                "stdout_query equ -11\n"
                "stdin_query equ -10\n"
                "section .data\n"
                "\tstdout dw 0\n"
                "\tstdin dw 0\n"
                "\tbytesWritten dw 0\n"
                "\tbytesRead dw 0\n\n"
                "section .bss\n"
                "\tOutputBuffer resb 20\n"
                "\tInputBuffer resb 256\n\n"
                "section .text\n"
                "global main\n"
                "main:\n";

    for (const node::Stmt &stmt : m_prog.statements)
    {
        comp_stmt(stmt);
    }

    m_output << "\n"
                "_printf:\n"
                "\t; INPUT:\n"
                "\t; RDX - string\n"
                "\tcall _countStrLen\n"
                "\tmov r8, rcx\n"
                "\tmov rcx, stdout_query\n"
                "\tcall GetStdHandle\n"
                "\tmov [rel stdout], rax\n"
                "\tmov rcx, [rel stdout]\n"
                "\tmov r9, bytesWritten\n"
                "\txor r10, r10\n"
                "\tcall WriteConsoleA\n"
                "\tret\n"
                "\n"
                "_scanf:\n"
                "\t; INPUT:\n"
                "\t; RDX - message\n"
                "\t; RSI - buffer for input\n"
                "\t; RAX - buffer size\n"
                "\t; OUTPUT:\n"
                "\t; RSI - buffer with user input\n"
                "\tpush rax\n"
                "\tpush rsi\n"
                "\tpush rdx\n"
                "\tmov rdx, rax\n"
                "\tcall _clearBuffer\n"
                "\tpop rdx\n"
                "\tcall _printf\n"
                "\tmov rcx, stdin_query\n"
                "\tcall GetStdHandle\n"
                "\tmov [rel stdin], rax\n"
                "\tmov rcx, [rel stdin]\n"
                "\tpop rdx\n"
                "\tpop r8\n"
                "\tmov r9, bytesRead\n"
                "\tcall ReadConsoleA\n"
                "\tret\n"
                "\n"
                "_countStrLen:\n"
                "\t; INPUT:\n"
                "\t; RDX - string\n"
                "\t; OUTPUT:\n"
                "\t; RCX - string length\n"
                "\txor rcx, rcx\n"
                "\tcontinue_count:\n"
                "\tmov al, byte [rdx + rcx]\n"
                "\tcmp al, 0\n"
                "\tje end_len_count\n"
                "\tinc rcx\n"
                "\tjmp continue_count\n"
                "\tend_len_count:\n"
                "\tret\n"
                "\n"
                "_itoa:\n"
                "\t; INPUT:\n"
                "\t; RSI - output string\n"
                "\t; RAX - integer\n"
                "\t; OUTPUT:\n"
                "\t; RSI - string\n"
                "\tpush rsi\n"
                "\tpush rax\n"
                "\tmov rdi, 1\n"
                "\tmov rcx, 1\n"
                "\tmov rbx, 10\n"
                "\t.get_divisor:\n"
                "\txor rdx, rdx\n"
                "\tdiv rbx\n"
                "\tcmp rax, 0\n"
                "\tje ._after\n"
                "\timul rcx, 10\n"
                "\tinc rdi\n"
                "\tjmp .get_divisor\n"
                "\t._after:\n"
                "\tpop rax\n"
                "\tpush rdi\n"
                "\t.to_string:\n"
                "\txor rdx, rdx\n"
                "\tdiv rcx\n"
                "\tadd al, '0'\n"
                "\tmov [rsi], al\n"
                "\tinc rsi\n"
                "\tpush rdx\n"
                "\txor rdx, rdx\n"
                "\tmov rax, rcx\n"
                "\tmov rbx, 10\n"
                "\tdiv rbx\n"
                "\tmov rcx, rax\n"
                "\tpop rax\n"
                "\tcmp rcx, 0\n"
                "\tjg .to_string\n"
                "\tpop rdx\n"
                "\tpop rsi\n"
                "\tret\n"
                "\n"
                "_stoi:\n"
                "\t; INPUT:\n"
                "\t; RSI - buffer to convert\n"
                "\t; OUTPUT:\n"
                "\t; RDI - integer\n"
                "\txor rdi, rdi\n"
                "\tmov rbx, 10\n"
                "\txor rax, rax\n"
                "\tnext_digit:\n"
                "\tmovzx rdx, byte[rsi]\n"
                "\ttest rdx, rdx\n"
                "\tjz done\n"
                "\tcmp rdx, 13\n"
                "\tje done\n"
                "\tcmp rdx, '0'\n"
                "\tjl error\n"
                "\tcmp rdx, '9'\n"
                "\tjg error\n"
                "\timul rdi, rbx\n"
                "\tsub rdx, '0'\n"
                "\tadd rdi, rdx\n"
                "\tinc rsi\n"
                "\tjmp next_digit\n"
                "\terror:\n"
                "\tmov rdx, ERR1\n"
                "\tcall _printf\n"
                "\tcall ExitProcess\n"
                "\tdone:\n"
                "\tmov rsi, rdx\n"
                "\tret\n"
                "\n"
                "_clearBuffer:\n"
                "\t; INPUT:\n"
                "\t; RSI - buffer to clear\n"
                "\t; RDX - buffer size\n"
                "\tclear:\n"
                "\tcmp rdx, 0\n"
                "\tje end\n"
                "\tcmp BYTE [rsi], 00H\n"
                "\tje end\n"
                "\tmov al, 00H\n"
                "\tmov [rsi], al\n"
                "\tinc rsi\n"
                "\tdec rdx\n"
                "\tjmp clear\n"
                "\tend:\n"
                "\tret\n"
                "\n"
                "ERR1: db 'Runtime Error. Cannot Convert String To Integer',7,00H\n";

    std::stringstream output;
    output << m_SC.str();
    output << "section .bss\n";
    output << m_bss_SC.str();
    output << m_output.str();

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