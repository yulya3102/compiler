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
std::unique_ptr<llvm::Module> generate(const ast::Code & code)
{
    std::unique_ptr<llvm::Module> result(new llvm::Module("abc", llvm::getGlobalContext()));

    context ctx;
    for (const auto & entry : code.entries)
        boost::apply_visitor([&result, &ctx] (const auto & x) { gen_entry(result.get(), ctx, x); }, entry.entry);

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

void gen_entry(llvm::Module * module, context & ctx, const ast::Declaration & entry)
{
    boost::apply_visitor([module, &ctx] (const auto & x) { gen_entry(module, ctx, x); }, entry.declaration);
}

void gen_entry(llvm::Module * module, context & ctx, const ast::Definition & entry)
{
    boost::apply_visitor([module, &ctx] (const auto & x) { gen_entry(module, ctx, x); }, entry.definition);
}

void gen_entry(llvm::Module * module, context & ctx, const ast::VarDeclaration & entry)
{
    llvm::Value * var = new llvm::GlobalVariable(
            *module, gen_type(entry.type), false,
            llvm::GlobalVariable::InternalLinkage, nullptr, entry.name);
    ctx.variables[entry.name] = var;
}

void gen_entry(llvm::Module * module, context & ctx, const ast::FuncDefinition & entry)
{
    llvm::Function * f = gen_func_declaration(module, ctx, entry.declaration);

    llvm::BasicBlock * bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", f);
    get_builder().SetInsertPoint(bb);

    gen_statement(ctx, entry.statement);
}

llvm::Function * gen_func_declaration(llvm::Module * module, context & ctx, const ast::FuncDeclaration & entry)
{
    // TODO: do not create new entry if it was already declared

    std::vector <llvm::Type *> args;
    for (const auto & x : entry.arguments)
        args.push_back(gen_type(x.type));

    llvm::FunctionType * type = llvm::FunctionType::get(
            gen_type(entry.type), args, false);

    llvm::Function * f = llvm::Function::Create(type, llvm::Function::ExternalLinkage, entry.name, module);

    ctx.functions[entry.name] = f;
}

void gen_entry(llvm::Module * module, context & ctx, const ast::FuncDeclaration & entry)
{
    gen_func_declaration(module, ctx, entry);
}

llvm::Value * gen_expr(const context & ctx, int64_t i)
{
    return get_builder().getInt64(i);
}

llvm::Value * gen_expr(const context & ctx, bool b)
{
    return get_builder().getInt1(b);
}

llvm::Value * gen_expr(const context & ctx, const ast::Const & v)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_expr(ctx, x); }, v.constant);
}

llvm::Value * gen_expr(const context & ctx, const std::string & v)
{
    return ctx.variables.at(v);
}

llvm::Value * gen_expr(const context & ctx, const ast::Value & v)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_expr(ctx, x); }, v.value);
}

llvm::Value * gen_expr(const context & ctx, const ast::BinOperator & op)
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

llvm::Value * gen_expr(const context & ctx, const ast::Dereference & deref)
{
    throw std::runtime_error("dereference: not implemented");
}

llvm::Value * gen_expr(const context & ctx, const ast::Call & call)
{
    llvm::Function * f = ctx.functions.at(call.function);

    std::vector<llvm::Value *> args;
    for (const auto & arg : args)
        args.push_back(gen_expr(ctx, arg));

    return get_builder().CreateCall(f, args);
}

llvm::Value * gen_expr(const context & ctx, const ast::Expression & expr)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_expr(ctx, x); }, expr.expression);
}

llvm::Value * gen_init_value(const ast::Type & type)
{
    undefined;
}

context gen_statement(const context & ctx, const ast::Skip &)
{
    return ctx;
}

context gen_statement(const context & ctx, const ast::VarDeclaration & v)
{
    llvm::Value * var = gen_init_value(v.type);
    context res(ctx);
    res.variables[v.name] = var;
    return res;
}

context gen_statement(const context & ctx, const ast::Statement & st)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_statement(ctx, x); }, st.statement);
}

}
