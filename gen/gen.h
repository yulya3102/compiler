#pragma once

#include <parse/ast/l.h>
#include <sem/types.h>

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
using typed_value = std::pair<ast::Type, value>;
struct frame : sem::typed_ctx<value>
{
    frame(llvm::Module * module, frame * outer_scope = nullptr)
        : sem::typed_ctx<value>(outer_scope)
        , module(module)
    {}

    llvm::Module * module;
};

llvm::Type * gen_type(const ast::AtomType & type);
llvm::Type * gen_type(const ast::PointerType & type);
llvm::Type * gen_type(const ast::Type & type);

llvm::Function * gen_func_declaration(frame & ctx, const ast::FuncDeclaration & entry);

void gen_static_data(llvm::Module * module);
llvm::Value * gen_format_string(frame & ctx, const ast::Type & type);

void gen_entry(frame & ctx, const ast::Declaration & entry);
void gen_entry(frame & ctx, const ast::Definition & entry);
void gen_entry(frame & ctx, const ast::VarDeclaration & entry);
void gen_entry(frame & ctx, const ast::FuncDefinition & entry);
void gen_entry(frame & ctx, const ast::FuncDeclaration & entry);

typed_value gen_expr(const frame & ctx, int64_t i);
typed_value gen_expr(const frame & ctx, bool b);
typed_value gen_expr(const frame & ctx, const ast::Const & v);
typed_value gen_expr(const frame & ctx, const std::string & v);
typed_value gen_expr(const frame & ctx, const ast::Value & v);
typed_value gen_expr(const frame & ctx, const ast::BinOperator & op);
typed_value gen_expr(const frame & ctx, const ast::Dereference & deref);
typed_value gen_expr(const frame & ctx, const ast::Call & call);
typed_value gen_expr(const frame & ctx, const ast::Expression & expr);

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
