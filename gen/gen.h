#pragma once

#include <parse/ast/l.h>
#include <sem/types.h>

#include <llvm/IR/Module.h>

#include <memory>
#include <unordered_set>
#include <unordered_map>

namespace codegen
{
/*
 * After typechecking, there are some things left to do:
 * for example, declaring local variables inside while loop
 * is totally valid, but since I do that with alloca(), it
 * can cause various crashes. I can avoid that with moving
 * local variables declarations outside of the while loop.
 * This won't break scope, since all semantic analysis has
 * been done already, and scope violations was already banned.
 *
 * Some dirty hacks like described above should be done in
 * prepare() function.
 */
void prepare(ast::Code & code);
std::unique_ptr<llvm::Module> generate(const ast::Code & code, const char * name);

enum value_type
{
    LVALUE,
    RVALUE,
};

using value = std::pair<value_type, llvm::Value *>;
using typed_value = std::pair<ast::Type, value>;
struct frame : sem::typed_ctx<value>
{
    frame(llvm::Module * module, frame * outer_scope = nullptr)
        : sem::typed_ctx<value>(outer_scope)
        , module(module)
    {}

    typed_value gen_expr(int64_t i) const;
    typed_value gen_expr(bool b) const;
    typed_value gen_expr(const ast::Const & v) const;
    typed_value gen_expr(const std::string & v) const;
    typed_value gen_expr(const ast::Value & v) const;
    typed_value gen_expr(const ast::BinOperator & op) const;
    typed_value gen_expr(const ast::Dereference & deref) const;
    typed_value gen_expr(const ast::Address & addr) const;
    typed_value gen_expr(const ast::Call & call) const;
    typed_value gen_expr(const ast::Expression & expr) const;

    void gen_statement(const ast::VarDeclaration & st);
    void gen_statement(const ast::Assignment & st);
    void gen_statement(const ast::If & st);
    void gen_statement(const ast::While & st);
    void gen_statement(const ast::Read & st);
    void gen_statement(const ast::Write & st);
    void gen_statement(const ast::Return & ret);
    void gen_statement(const ast::Block & block);
    void gen_statement(const ast::Statement & st);

    llvm::Module * module;
};

llvm::Type * gen_type(const ast::AtomType & type);
llvm::Type * gen_type(const ast::PointerType & type);
llvm::Type * gen_type(const ast::FuncType & type);
llvm::Type * gen_type(const ast::Type & type);

llvm::Function * gen_func_declaration(frame & ctx, const ast::FuncDeclaration & entry);

void gen_static_data(llvm::Module * module);
llvm::Value * gen_format_string(frame & ctx, const ast::Type & type);

void gen_entry(frame & ctx, const ast::Declaration & entry);
void gen_entry(frame & ctx, const ast::Definition & entry);
void gen_entry(frame & ctx, const ast::VarDeclaration & entry);
void gen_entry(frame & ctx, const ast::FuncDefinition & entry);
void gen_entry(frame & ctx, const ast::FuncDeclaration & entry);
}
