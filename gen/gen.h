#pragma once

#include <parse/ast/l.h>

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

struct frame
{
    frame(llvm::Module * module, frame * outer_scope = nullptr)
        : outer_scope(outer_scope)
        , module(module)
    {}

    void declare(value_type type, llvm::Value * v, const std::string & name);
    bool is_declared(const std::string & name);

    using value = std::pair<value_type, llvm::Value *>;
    value get(const std::string & name) const;

    template <typename T>
    using map = std::unordered_map<std::string, T>;

    map<value> locals;
    frame * outer_scope;
    llvm::Module * module;
};

llvm::Type * gen_type(const ast::AtomType & type);
llvm::Type * gen_type(const ast::PointerType & type);
llvm::Type * gen_type(const ast::Type & type);

llvm::Function * gen_func_declaration(frame & ctx, const ast::FuncDeclaration & entry);

void gen_entry(frame & ctx, const ast::Declaration & entry);
void gen_entry(frame & ctx, const ast::Definition & entry);
void gen_entry(frame & ctx, const ast::VarDeclaration & entry);
void gen_entry(frame & ctx, const ast::FuncDefinition & entry);
void gen_entry(frame & ctx, const ast::FuncDeclaration & entry);

frame::value gen_expr(const frame & ctx, int64_t i);
frame::value gen_expr(const frame & ctx, bool b);
frame::value gen_expr(const frame & ctx, const ast::Const & v);
frame::value gen_expr(const frame & ctx, const std::string & v);
frame::value gen_expr(const frame & ctx, const ast::Value & v);
frame::value gen_expr(const frame & ctx, const ast::BinOperator & op);
frame::value gen_expr(const frame & ctx, const ast::Dereference & deref);
frame::value gen_expr(const frame & ctx, const ast::Call & call);
frame::value gen_expr(const frame & ctx, const ast::Expression & expr);

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
