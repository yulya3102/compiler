#include "gen.h"

#include <sem/l.h>

#include <utils/undefined.h>

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/TypeBuilder.h>
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

template <typename T>
llvm::Value * gen_rvalue(const codegen::frame & ctx, T expr)
{
    codegen::typed_value v = codegen::gen_expr(ctx, expr);
    if (v.second.first == codegen::value_type::LOAD)
        return get_builder().CreateLoad(v.second.second);
    return v.second.second;
}
}

namespace codegen
{

std::unique_ptr<llvm::Module> generate(const ast::Code & code, const char * name)
{
    std::unique_ptr<llvm::Module> result(new llvm::Module(name, llvm::getGlobalContext()));
    gen_static_data(result.get());

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
    ctx.declare({entry.type, {value_type::LOAD, var}}, entry.name);
}

void gen_entry(frame & ctx, const ast::FuncDefinition & entry)
{
    llvm::Function * f = gen_func_declaration(ctx, entry.declaration);

    llvm::BasicBlock * bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", f);
    get_builder().SetInsertPoint(bb);

    frame inner_scope(ctx.module, &ctx);
    {
        auto proto_it = entry.declaration.arguments.begin();
        for (auto arg_it = f->args().begin(); arg_it != f->args().end(); ++arg_it, ++proto_it)
        {
            arg_it->setName(proto_it->name);
            inner_scope.declare({proto_it->type, {value_type::NO_LOAD, &*arg_it}}, proto_it->name);
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
        typed_value found = ctx.get(entry.name);
        return llvm::cast<llvm::Function>(found.second.second);
    }

    std::vector <llvm::Type *> args;
    for (const auto & x : entry.arguments)
        args.push_back(gen_type(x.type));

    llvm::FunctionType * type = llvm::FunctionType::get(
            gen_type(entry.type), args, false);

    llvm::Function * f = llvm::Function::Create(type, llvm::Function::ExternalLinkage, entry.name, ctx.module);

    ctx.declare({ctx.get_type(entry), {value_type::LOAD, f}}, entry.name);
    return f;
}

void gen_entry(frame & ctx, const ast::FuncDeclaration & entry)
{
    gen_func_declaration(ctx, entry);
}

typed_value gen_expr(const frame & ctx, int64_t i)
{
    undefined;
}

typed_value gen_expr(const frame & ctx, bool b)
{
    undefined;
}

typed_value gen_expr(const frame & ctx, const ast::Const & v)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_expr(ctx, x); }, v.constant);
}

typed_value gen_expr(const frame & ctx, const std::string & v)
{
    return ctx.get(v);
}

typed_value gen_expr(const frame & ctx, const ast::Value & v)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_expr(ctx, x); }, v.value);
}

typed_value gen_expr(const frame & ctx, const ast::BinOperator & op)
{
    llvm::Value * lhs = gen_rvalue(ctx, *op.lhs);
    llvm::Value * rhs = gen_rvalue(ctx, *op.rhs);

    undefined;
    /*
    switch (op.oper.oper)
    {
        case ast::Oper::PLUS:
            return {value_type::NO_LOAD, get_builder().CreateAdd(lhs, rhs)};
        case ast::Oper::MINUS:
            return {value_type::NO_LOAD, get_builder().CreateSub(lhs, rhs)};
        case ast::Oper::MULT:
            return {value_type::NO_LOAD, get_builder().CreateMul(lhs, rhs)};
        case ast::Oper::DIV:
            return {value_type::NO_LOAD, get_builder().CreateSDiv(lhs, rhs)};
        case ast::Oper::MOD:
            return {value_type::NO_LOAD, get_builder().CreateSRem(lhs, rhs)};
        case ast::Oper::GT:
            return {value_type::NO_LOAD, get_builder().CreateICmpSGT(lhs, rhs)};
        case ast::Oper::LT:
            return {value_type::NO_LOAD, get_builder().CreateICmpSLT(lhs, rhs)};
        case ast::Oper::EQ:
            return {value_type::NO_LOAD, get_builder().CreateICmpEQ(lhs, rhs)};
        case ast::Oper::GE:
            return {value_type::NO_LOAD, get_builder().CreateICmpSGE(lhs, rhs)};
        case ast::Oper::LE:
            return {value_type::NO_LOAD, get_builder().CreateICmpSLE(lhs, rhs)};
        case ast::Oper::NE:
            return {value_type::NO_LOAD, get_builder().CreateICmpNE(lhs, rhs)};
        case ast::Oper::AND:
            return {value_type::NO_LOAD, get_builder().CreateAnd(lhs, rhs)};
        case ast::Oper::OR:
            return {value_type::NO_LOAD, get_builder().CreateOr(lhs, rhs)};
    }
    */

    throw std::runtime_error("unknown binary operator");
}

typed_value gen_expr(const frame & ctx, const ast::Dereference & deref)
{
    throw std::runtime_error("dereference: not implemented");
}

typed_value gen_expr(const frame & ctx, const ast::Call & call)
{
    typed_value f = ctx.get(call.function);

    std::vector<llvm::Value *> args;
    for (const auto & arg : call.arguments)
        args.push_back(gen_rvalue(ctx, arg));

    return {ctx.get_type(call), {value_type::NO_LOAD, get_builder().CreateCall(f.second.second, args)}};
}

typed_value gen_expr(const frame & ctx, const ast::Expression & expr)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_expr(ctx, x); }, expr.expression);
}

void gen_statement(frame & ctx, const ast::Skip &)
{ }

void gen_statement(frame & ctx, const ast::VarDeclaration & v)
{
    llvm::Value * val = get_builder().CreateAlloca(gen_type(v.type), nullptr, v.name);
    ctx.declare({v.type, {value_type::LOAD, val}}, v.name);
}

void gen_statement(frame & ctx, const ast::Assignment & st)
{
    llvm::Value * val = gen_rvalue(ctx, st.value);
    get_builder().CreateStore(val, ctx.get(st.varname).second.second);
}

void gen_statement(frame & ctx, const ast::Seq & st)
{
    gen_statement(ctx, *st.first);
    gen_statement(ctx, *st.second);
}

void gen_statement(frame & ctx, const ast::If & st)
{
    /* Generate condition */
    llvm::Value * cond = gen_rvalue(ctx, st.condition);
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
    llvm::Function * f = get_builder().GetInsertBlock()->getParent();
    llvm::BasicBlock * while_body = llvm::BasicBlock::Create(llvm::getGlobalContext(), "while_body", f);
    llvm::BasicBlock * cond_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "while_cond");
    llvm::BasicBlock * cont_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "while_cont");
    get_builder().CreateBr(cond_block);

    /* Generate condition */
    get_builder().SetInsertPoint(cond_block);
    llvm::Value * cond = gen_rvalue(ctx, st.condition);
    get_builder().CreateCondBr(cond, while_body, cont_block);

    /* Generate body branch */
    get_builder().SetInsertPoint(while_body);
    gen_statement(ctx, *st.body);
    get_builder().CreateBr(cond_block);
    while_body = get_builder().GetInsertBlock();

    /* Continue */
    f->getBasicBlockList().push_back(cond_block);
    f->getBasicBlockList().push_back(cont_block);
    get_builder().SetInsertPoint(cont_block);
}

void gen_statement(frame & ctx, const ast::Read & st)
{
    undefined;
}

llvm::Value * gen_format_string(const ast::Type & type)
{
    undefined;
}

void gen_statement(frame & ctx, const ast::Write & st)
{
    llvm::Value * f = ctx.module->getNamedValue("printf");
    llvm::Value * v = gen_rvalue(ctx, *st.expr);
    llvm::Value * format_string = gen_format_string(ctx.get_type(*st.expr));
    std::vector<llvm::Value *> args = { format_string, v };

    get_builder().CreateCall(f, args);
}

void gen_statement(frame & ctx, const ast::Return & ret)
{
    get_builder().CreateRet(gen_rvalue(ctx, *ret.expr));

    llvm::Function * f = get_builder().GetInsertBlock()->getParent();
    llvm::BasicBlock * unreachable = llvm::BasicBlock::Create(llvm::getGlobalContext(), "unreachable", f);
    get_builder().SetInsertPoint(unreachable);
}

void gen_statement(frame & ctx, const ast::Statement & st)
{
    return boost::apply_visitor([&ctx] (const auto & x) { return gen_statement(ctx, x); }, st.statement);
}

void gen_static_data(llvm::Module * module)
{
    llvm::FunctionType * type = llvm::TypeBuilder<int(char *, ...), false>::get(llvm::getGlobalContext());
    llvm::Function::Create(type, llvm::Function::ExternalLinkage, "printf", module);

    // TODO: declaration of scanf
}

}
