#include "codegen.h"

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/BasicBlock.h>

#include <boost/variant.hpp>

#include <memory>
#include <map>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define __AT__ __FILE__ ":" TOSTRING(__LINE__)

#define undefined throw std::runtime_error(__AT__ ": not implemented")

namespace
{
llvm::IRBuilder<> & get_builder()
{
    static llvm::IRBuilder<> builder(llvm::getGlobalContext());
    return builder;
}
}

namespace codegen
{
std::unique_ptr<llvm::Module> generate(const ast::Code & code, const char * name)
{
    std::unique_ptr<llvm::Module> result(new llvm::Module(name, llvm::getGlobalContext()));

    frame ctx(result.get());
    for (const auto & entry : code.entries)
        boost::apply_visitor([&ctx] (const auto & x) { gen_entry(ctx, x); }, entry.entry);

    return std::move(result);
}

llvm::Type * gen_type(const ast::AtomType & type)
{
    switch (type)
    {
        case ast::BOOL:
            return llvm::Type::getInt1Ty(llvm::getGlobalContext());
        case ast::INT:
            return llvm::Type::getInt64Ty(llvm::getGlobalContext());
    }

    throw std::runtime_error("unknown type");
}

llvm::Type * gen_type(const ast::PointerType & type)
{
    undefined;
}

llvm::Type * gen_type(const ast::Type & type)
{
    return boost::apply_visitor([] (const auto & x) { return gen_type(x); }, type.type);
}

void gen_entry(frame & ctx, const ast::Declaration & entry)
{
    boost::apply_visitor([&ctx] (const auto & x) { gen_entry(ctx, x); }, entry.declaration);
}

void gen_entry(frame & ctx, const ast::Definition & entry)
{
    boost::apply_visitor([&ctx] (const auto & x) { gen_entry(ctx, x); }, entry.definition);
}

void gen_entry(frame & ctx, const ast::VarDeclaration & entry)
{
    llvm::Value * var = new llvm::GlobalVariable(
            *ctx.module, gen_type(entry.type), false,
            llvm::GlobalVariable::InternalLinkage, nullptr, entry.name);
    ctx.declare_var(var, entry.name);
}

void gen_entry(frame & ctx, const ast::FuncDefinition & entry)
{
    llvm::Function * f = gen_func_declaration(ctx, entry.declaration);

    llvm::BasicBlock * bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", f);
    get_builder().SetInsertPoint(bb);

    frame inner_scope(ctx.module, &ctx);
    gen_statement(inner_scope, entry.statement);
}

llvm::Function * gen_func_declaration(frame & ctx, const ast::FuncDeclaration & entry)
{
    if (ctx.is_declared_func(entry.name))
        return ctx.get_function(entry.name);

    std::vector <llvm::Type *> args;
    for (const auto & x : entry.arguments)
        args.push_back(gen_type(x.type));

    llvm::FunctionType * type = llvm::FunctionType::get(
            gen_type(entry.type), args, false);

    llvm::Function * f = llvm::Function::Create(type, llvm::Function::ExternalLinkage, entry.name, ctx.module);

    ctx.declare_func(f, entry.name);
    return f;
}

void gen_entry(frame & ctx, const ast::FuncDeclaration & entry)
{
    gen_func_declaration(ctx, entry);
}

llvm::Value * gen_expr(const frame & ctx, int64_t i)
{
    return get_builder().getInt64(i);
}

llvm::Value * gen_expr(const frame & ctx, bool b)
{
    return get_builder().getInt1(b);
}

llvm::Value * gen_expr(const frame & ctx, const ast::Const & v)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_expr(ctx, x); }, v.constant);
}

llvm::Value * gen_expr(const frame & ctx, const std::string & v)
{
    return ctx.get_var(v);
}

llvm::Value * gen_expr(const frame & ctx, const ast::Value & v)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_expr(ctx, x); }, v.value);
}

llvm::Value * gen_expr(const frame & ctx, const ast::BinOperator & op)
{
    llvm::Value * lhs = gen_expr(ctx, *op.lhs);
    llvm::Value * rhs = gen_expr(ctx, *op.rhs);

    switch (op.oper)
    {
        case ast::PLUS:
            return get_builder().CreateAdd(lhs, rhs);
        case ast::MINUS:
            return get_builder().CreateSub(lhs, rhs);
        case ast::MULT:
            return get_builder().CreateMul(lhs, rhs);
        case ast::DIV:
            return get_builder().CreateSDiv(lhs, rhs);
        case ast::MOD:
            return get_builder().CreateSRem(lhs, rhs);
        case ast::GT:
            return get_builder().CreateICmpSGT(lhs, rhs);
        case ast::LT:
            return get_builder().CreateICmpSLT(lhs, rhs);
        case ast::EQ:
            return get_builder().CreateICmpEQ(lhs, rhs);
        case ast::GE:
            return get_builder().CreateICmpSGE(lhs, rhs);
        case ast::LE:
            return get_builder().CreateICmpSLE(lhs, rhs);
        case ast::NE:
            return get_builder().CreateICmpNE(lhs, rhs);
        case ast::AND:
            return get_builder().CreateAnd(lhs, rhs);
        case ast::OR:
            return get_builder().CreateOr(lhs, rhs);
    }

    throw std::runtime_error("unknown binary operator");
}

llvm::Value * gen_expr(const frame & ctx, const ast::Dereference & deref)
{
    throw std::runtime_error("dereference: not implemented");
}

llvm::Value * gen_expr(const frame & ctx, const ast::Call & call)
{
    llvm::Function * f = ctx.functions.at(call.function);

    std::vector<llvm::Value *> args;
    for (const auto & arg : args)
        args.push_back(gen_expr(ctx, arg));

    return get_builder().CreateCall(f, args);
}

llvm::Value * gen_expr(const frame & ctx, const ast::Expression & expr)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_expr(ctx, x); }, expr.expression);
}

llvm::Value * gen_init_value(const ast::Type & type)
{
    undefined;
}

void gen_statement(frame & ctx, const ast::Skip &)
{ }

void gen_statement(frame & ctx, const ast::VarDeclaration & v)
{
    llvm::Value * val = get_builder().CreateAlloca(gen_type(v.type), nullptr, v.name);
    ctx.declare_var(val, v.name);
}

void gen_statement(frame & ctx, const ast::Assignment & st)
{
    llvm::Value * val = gen_expr(ctx, st.value);
    get_builder().CreateStore(val, ctx.get_var(st.varname));
}

void gen_statement(frame & ctx, const ast::Seq & st)
{
    gen_statement(ctx, *st.first);
    gen_statement(ctx, *st.second);
}

void gen_statement(frame & ctx, const ast::If & st)
{
    undefined;
}

void gen_statement(frame & ctx, const ast::While & st)
{
    undefined;
}

void gen_statement(frame & ctx, const ast::Read & st)
{
    undefined;
}

void gen_statement(frame & ctx, const ast::Write & st)
{
    undefined;
}

void gen_statement(frame & ctx, const ast::Return & ret)
{
    get_builder().CreateRet(gen_expr(ctx, *ret.expr));
}

void gen_statement(frame & ctx, const ast::Statement & st)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_statement(ctx, x); }, st.statement);
}

void frame::declare_var(llvm::Value * v, const std::string & name)
{
    locals[name] = v;
}

void frame::declare_func(llvm::Function * f, const std::string & name)
{
    functions[name] = f;
}

bool frame::is_declared_func(const std::string & name)
{
    auto it = functions.find(name);

    if (it == functions.end())
        return false;

    return true;
}

llvm::Value * & frame::get_var(const std::string & name) const
{
    auto it = locals.find(name);
    if (it != locals.end())
        return const_cast<llvm::Value * &>(it->second);

    if (outer_scope)
        return outer_scope->get_var(name);

    throw std::runtime_error("undefined variable: " + name);
}

llvm::Function * frame::get_function(const std::string & name) const
{
    auto it = functions.find(name);
    if (it != functions.end())
        return const_cast<llvm::Function *>(it->second);

    throw std::runtime_error("undefined function: " + name);
}

}
