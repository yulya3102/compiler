#pragma once

#include <parse/ast/l.h>
#include <sem/ctx.h>

#include <llvm/IR/Module.h>

#include <memory>
#include <unordered_set>
#include <unordered_map>

namespace codegen
{
std::unique_ptr<llvm::Module> generate(const ast::Code & code, const char * name);

enum value_type
{
    LOAD,
    NO_LOAD,
};

using value = std::pair<value_type, llvm::Value *>;
struct frame : sem::context<value>
{
    frame(llvm::Module * module, frame * outer_scope = nullptr)
        : sem::context<value>(outer_scope)
        , module(module)
    {}

    llvm::Module * module;
};

llvm::Type * gen_type(const ast::AtomType & type);
llvm::Type * gen_type(const ast::PointerType & type);
llvm::Type * gen_type(const ast::Type & type);

llvm::Function * gen_func_declaration(frame & ctx, const ast::FuncDeclaration & entry);

void gen_static_data(llvm::Module * module);
llvm::Value * gen_format_string(const ast::Type & type);

void gen_entry(frame & ctx, const ast::Declaration & entry);
void gen_entry(frame & ctx, const ast::Definition & entry);
void gen_entry(frame & ctx, const ast::VarDeclaration & entry);
void gen_entry(frame & ctx, const ast::FuncDefinition & entry);
void gen_entry(frame & ctx, const ast::FuncDeclaration & entry);

value gen_expr(const frame & ctx, int64_t i);
value gen_expr(const frame & ctx, bool b);
value gen_expr(const frame & ctx, const ast::Const & v);
value gen_expr(const frame & ctx, const std::string & v);
value gen_expr(const frame & ctx, const ast::Value & v);
value gen_expr(const frame & ctx, const ast::BinOperator & op);
value gen_expr(const frame & ctx, const ast::Dereference & deref);
value gen_expr(const frame & ctx, const ast::Call & call);
value gen_expr(const frame & ctx, const ast::Expression & expr);

void gen_statement(frame & ctx, const ast::Skip & st);
void gen_statement(frame & ctx, const ast::VarDeclaration & st);
void gen_statement(frame & ctx, const ast::Assignment & st);
void gen_statement(frame & ctx, const ast::Seq & st);
void gen_statement(frame & ctx, const ast::If & st);
void gen_statement(frame & ctx, const ast::While & st);
void gen_statement(frame & ctx, const ast::Read & st);
void gen_statement(frame & ctx, const ast::Write & st);
void gen_statement(frame & ctx, const ast::Return & ret);
void gen_statement(frame & ctx, const ast::Statement & st);
}
