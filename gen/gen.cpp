#include "gen.h"

#include <utils/undefined.h>

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

#include <boost/variant.hpp>

#include <memory>
#include <map>

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

    if (llvm::verifyModule(*result, &llvm::errs()))
    {
        result->dump();
        throw std::runtime_error("internal compiler error: module verification failed");
    }

    return std::move(result);
}

llvm::Type * gen_type(const ast::AtomType & type)
{
    switch (type.type)
    {
        case ast::AtomType::BOOL:
            return llvm::Type::getInt1Ty(llvm::getGlobalContext());
        case ast::AtomType::INT:
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
            llvm::GlobalVariable::ExternalLinkage, nullptr, entry.name);
    ctx.declare(var, entry.name);
}

void gen_entry(frame & ctx, const ast::FuncDefinition & entry)
{
    llvm::Function * f = gen_func_declaration(ctx, entry.declaration);

    llvm::BasicBlock * bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", f);
    get_builder().SetInsertPoint(bb);

    frame inner_scope(ctx.module, &ctx);
    {
        auto name_it = entry.declaration.arguments.begin();
        for (auto arg_it = f->args().begin(); arg_it != f->args().end(); ++arg_it, ++name_it)
        {
            arg_it->setName(name_it->name);
            inner_scope.declare(&*arg_it, name_it->name);
        }
    }
    gen_statement(inner_scope, entry.statement);
    get_builder().CreateUnreachable();

    if (llvm::verifyFunction(*f, &llvm::errs()))
    {
        ctx.module->dump();
        throw std::runtime_error("internal compiler error: function verification failed");
    }
}

llvm::Function * gen_func_declaration(frame & ctx, const ast::FuncDeclaration & entry)
{
    if (ctx.is_declared(entry.name))
    {
        llvm::Value * found = ctx.get(entry.name);

        if (found->getType()->isFunctionTy())
            return llvm::cast<llvm::Function>(found);

        throw std::runtime_error(entry.name + " redeclared as a different kind of symbol");
    }

    std::vector <llvm::Type *> args;
    for (const auto & x : entry.arguments)
        args.push_back(gen_type(x.type));

    llvm::FunctionType * type = llvm::FunctionType::get(
            gen_type(entry.type), args, false);

    llvm::Function * f = llvm::Function::Create(type, llvm::Function::ExternalLinkage, entry.name, ctx.module);

    ctx.declare(f, entry.name);
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
    return ctx.get(v);
}

llvm::Value * gen_expr(const frame & ctx, const ast::Value & v)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_expr(ctx, x); }, v.value);
}

llvm::Value * gen_expr(const frame & ctx, const ast::BinOperator & op)
{
    llvm::Value * lhs = gen_expr(ctx, *op.lhs);
    llvm::Value * rhs = gen_expr(ctx, *op.rhs);

    switch (op.oper.oper)
    {
        case ast::Oper::PLUS:
            return get_builder().CreateAdd(lhs, rhs);
        case ast::Oper::MINUS:
            return get_builder().CreateSub(lhs, rhs);
        case ast::Oper::MULT:
            return get_builder().CreateMul(lhs, rhs);
        case ast::Oper::DIV:
            return get_builder().CreateSDiv(lhs, rhs);
        case ast::Oper::MOD:
            return get_builder().CreateSRem(lhs, rhs);
        case ast::Oper::GT:
            return get_builder().CreateICmpSGT(lhs, rhs);
        case ast::Oper::LT:
            return get_builder().CreateICmpSLT(lhs, rhs);
        case ast::Oper::EQ:
            return get_builder().CreateICmpEQ(lhs, rhs);
        case ast::Oper::GE:
            return get_builder().CreateICmpSGE(lhs, rhs);
        case ast::Oper::LE:
            return get_builder().CreateICmpSLE(lhs, rhs);
        case ast::Oper::NE:
            return get_builder().CreateICmpNE(lhs, rhs);
        case ast::Oper::AND:
            return get_builder().CreateAnd(lhs, rhs);
        case ast::Oper::OR:
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
    llvm::Value * f = ctx.get(call.function);

    std::vector<llvm::Value *> args;
    for (const auto & arg : call.arguments)
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
    ctx.declare(val, v.name);
}

void gen_statement(frame & ctx, const ast::Assignment & st)
{
    llvm::Value * val = gen_expr(ctx, st.value);
    get_builder().CreateStore(val, ctx.get(st.varname));
}

void gen_statement(frame & ctx, const ast::Seq & st)
{
    gen_statement(ctx, *st.first);
    gen_statement(ctx, *st.second);
}

void gen_statement(frame & ctx, const ast::If & st)
{
    /* Generate condition */
    llvm::Value * cond = gen_expr(ctx, st.condition);
    llvm::Function * f = get_builder().GetInsertBlock()->getParent();
    llvm::BasicBlock * then_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "then", f);
    llvm::BasicBlock * else_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "else");
    llvm::BasicBlock * cont_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "cont");
    get_builder().CreateCondBr(cond, then_block, else_block);

    /* Generate 'then' branch */
    get_builder().SetInsertPoint(then_block);
    gen_statement(ctx, *st.thenBody);
    get_builder().CreateBr(cont_block);
    then_block = get_builder().GetInsertBlock();

    /* Generate 'else' branch */
    f->getBasicBlockList().push_back(else_block);
    get_builder().SetInsertPoint(else_block);
    gen_statement(ctx, *st.elseBody);
    get_builder().CreateBr(cont_block);
    else_block = get_builder().GetInsertBlock();

    /* Continue */
    f->getBasicBlockList().push_back(cont_block);
    get_builder().SetInsertPoint(cont_block);
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

    llvm::Function * f = get_builder().GetInsertBlock()->getParent();
    llvm::BasicBlock * unreachable = llvm::BasicBlock::Create(llvm::getGlobalContext(), "unreachable", f);
    get_builder().SetInsertPoint(unreachable);
}

void gen_statement(frame & ctx, const ast::Statement & st)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_statement(ctx, x); }, st.statement);
}

void frame::declare(llvm::Value * v, const std::string & name)
{
    locals[name] = v;
}

bool frame::is_declared(const std::string & name)
{
    auto it = locals.find(name);

    if (it == locals.end())
        return false;

    return true;
}

llvm::Value * frame::get(const std::string & name) const
{
    auto it = locals.find(name);
    if (it != locals.end())
        return const_cast<llvm::Value *>(it->second);

    if (outer_scope)
        return outer_scope->get(name);

    throw std::runtime_error("undefined symbol: " + name);
}

}
