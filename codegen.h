#pragma once

#include <ast/l.h>

#include <llvm/IR/Module.h>

#include <memory>
#include <unordered_set>
#include <unordered_map>

namespace codegen
{
std::unique_ptr<llvm::Module> generate(const ast::Code & code, const char * name);

struct context
{
    std::unordered_map<std::string, llvm::Value *> variables;
    std::unordered_map<std::string, llvm::Function *> functions;
};

llvm::Type * gen_type(const ast::AtomType & type);
llvm::Type * gen_type(const ast::PointerType & type);
llvm::Type * gen_type(const ast::Type & type);

llvm::Function * gen_func_declaration(llvm::Module * module, context & ctx, const ast::FuncDeclaration & entry);

void gen_entry(llvm::Module * module, context & ctx, const ast::Declaration & entry);
void gen_entry(llvm::Module * module, context & ctx, const ast::Definition & entry);
void gen_entry(llvm::Module * module, context & ctx, const ast::VarDeclaration & entry);
void gen_entry(llvm::Module * module, context & ctx, const ast::FuncDefinition & entry);
void gen_entry(llvm::Module * module, context & ctx, const ast::FuncDeclaration & entry);

llvm::Value * gen_expr(const context & ctx, int64_t i);
llvm::Value * gen_expr(const context & ctx, bool b);
llvm::Value * gen_expr(const context & ctx, const ast::Const & v);
llvm::Value * gen_expr(const context & ctx, const std::string & v);
llvm::Value * gen_expr(const context & ctx, const ast::Value & v);
llvm::Value * gen_expr(const context & ctx, const ast::BinOperator & op);
llvm::Value * gen_expr(const context & ctx, const ast::Dereference & deref);
llvm::Value * gen_expr(const context & ctx, const ast::Call & call);
llvm::Value * gen_expr(const context & ctx, const ast::Expression & expr);

context gen_statement(const context & ctx, const ast::Skip & st);
context gen_statement(const context & ctx, const ast::VarDeclaration & st);
context gen_statement(const context & ctx, const ast::Assignment & st);
context gen_statement(const context & ctx, const ast::Seq & st);
context gen_statement(const context & ctx, const ast::If & st);
context gen_statement(const context & ctx, const ast::While & st);
context gen_statement(const context & ctx, const ast::Read & st);
context gen_statement(const context & ctx, const ast::Write & st);
context gen_statement(const context & ctx, const ast::Return & ret);
context gen_statement(const context & ctx, const ast::Statement & st);
}
