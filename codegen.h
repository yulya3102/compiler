#pragma once

#include <ast/l.h>

#include <llvm/IR/Module.h>

#include <memory>
#include <unordered_set>
#include <unordered_map>

namespace codegen
{
std::unique_ptr<llvm::Module> generate(const ast::Code & code, const char * name);

struct frame
{
    frame(llvm::Module * module, frame * outer_scope = nullptr)
        : outer_scope(outer_scope)
        , module(module)
    {}

    void declare_int(const std::string & name);
    void declare_bool(const std::string & name);
    void declare_var(const ast::VarDeclaration & v);
    void declare_func(llvm::Function * f, const std::string & name);

    llvm::Value * & get_var(const std::string & name) const;
    llvm::Function * get_function(const std::string & name) const;

    template <typename T>
    using map = std::unordered_map<std::string, T>;

    map<llvm::Value *> locals;
    map<llvm::Function *> functions;
    frame * outer_scope;
    llvm::Module * module;
};

llvm::Type * gen_type(const ast::AtomType & type);
llvm::Type * gen_type(const ast::PointerType & type);
llvm::Type * gen_type(const ast::Type & type);

llvm::Function * gen_func_declaration(llvm::Module * module, frame & ctx, const ast::FuncDeclaration & entry);

void gen_entry(llvm::Module * module, frame & ctx, const ast::Declaration & entry);
void gen_entry(llvm::Module * module, frame & ctx, const ast::Definition & entry);
void gen_entry(llvm::Module * module, frame & ctx, const ast::VarDeclaration & entry);
void gen_entry(llvm::Module * module, frame & ctx, const ast::FuncDefinition & entry);
void gen_entry(llvm::Module * module, frame & ctx, const ast::FuncDeclaration & entry);

llvm::Value * gen_expr(const frame & ctx, int64_t i);
llvm::Value * gen_expr(const frame & ctx, bool b);
llvm::Value * gen_expr(const frame & ctx, const ast::Const & v);
llvm::Value * gen_expr(const frame & ctx, const std::string & v);
llvm::Value * gen_expr(const frame & ctx, const ast::Value & v);
llvm::Value * gen_expr(const frame & ctx, const ast::BinOperator & op);
llvm::Value * gen_expr(const frame & ctx, const ast::Dereference & deref);
llvm::Value * gen_expr(const frame & ctx, const ast::Call & call);
llvm::Value * gen_expr(const frame & ctx, const ast::Expression & expr);

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
