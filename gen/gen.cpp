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
    codegen::typed_value v = ctx.gen_expr(expr);
    if (v.second.first == codegen::value_type::LVALUE)
        return get_builder().CreateLoad(v.second.second);
    return v.second.second;
}
}

namespace codegen
{
void prepare(ast::Code & code)
{
    undefined;
}

std::unique_ptr<llvm::Module> generate(const ast::Code & code, const char * name)
{
    std::unique_ptr<llvm::Module> result(new llvm::Module(name, llvm::getGlobalContext()));
    gen_static_data(result.get());

    frame ctx(result.get());
    for (const auto & entry : code.entries)
        fmap([&ctx], x, gen_entry(ctx, x), entry.entry);

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
    auto dereferenced = gen_type(*type.type);
    return dereferenced->getPointerTo();
}

llvm::Type * gen_type(const ast::FuncType & type)
{
    llvm::Type * rettype = gen_type(*type.rettype);
    std::vector<llvm::Type *> argtypes;
    for (auto argtype : type.argtypes)
        argtypes.push_back(gen_type(argtype));
    return llvm::FunctionType::get(rettype, argtypes, false);
}

llvm::Type * gen_type(const ast::Type & type)
{
    return fmap([], x, gen_type(x), type.type);
}

llvm::Constant * gen_init(const ast::AtomType & type)
{
    switch (type.type)
    {
    case ast::AtomType::BOOL:
        return get_builder().getInt1(false);
    case ast::AtomType::INT:
        return get_builder().getInt64(0);
    }

    throw std::runtime_error("unknown atom type");
}

llvm::Constant * gen_init(const ast::PointerType & type)
{
    return nullptr;
}

llvm::Constant * gen_init(const ast::FuncType & type)
{
    undefined;
}

llvm::Constant * gen_init(const ast::Type & type)
{
    return fmap([], x, gen_init(x), type.type);
}

void gen_entry(frame & ctx, const ast::Declaration & entry)
{
    fmap([&ctx], x, gen_entry(ctx, x), entry.declaration);
}

void gen_entry(frame & ctx, const ast::Definition & entry)
{
    fmap([&ctx], x, gen_entry(ctx, x), entry.definition);
}

void gen_entry(frame & ctx, const ast::VarDeclaration & entry)
{
    llvm::Value * var = new llvm::GlobalVariable(
            *ctx.module, gen_type(entry.type), false,
            llvm::GlobalVariable::ExternalLinkage, gen_init(entry.type), entry.name);
    ctx.declare({entry.type, {value_type::LVALUE, var}}, entry.name);
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
            inner_scope.declare({proto_it->type, {value_type::RVALUE, &*arg_it}}, proto_it->name);
        }
    }
    inner_scope.gen_statement(entry.statements);
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

    ctx.declare({ctx.get_type(entry), {value_type::LVALUE, f}}, entry.name);
    return f;
}

void gen_entry(frame & ctx, const ast::FuncDeclaration & entry)
{
    gen_func_declaration(ctx, entry);
}

typed_value frame::gen_expr(int64_t i) const
{
    return {ast::int_type(), {value_type::RVALUE, get_builder().getInt64(i)}};
}

typed_value frame::gen_expr(bool b) const
{
    return {ast::bool_type(), {value_type::RVALUE, get_builder().getInt1(b)}};
}

typed_value frame::gen_expr(const ast::Const & v) const
{
    return fmap([this], x, this->gen_expr(x), v.constant);
}

typed_value frame::gen_expr(const std::string & v) const
{
    return this->get(v);
}

typed_value frame::gen_expr(const ast::Value & v) const
{
    return fmap([this], x, this->gen_expr(x), v.value);
}

typed_value frame::gen_expr(const ast::BinOperator & op) const
{
    llvm::Value * lhs = gen_rvalue(*this, *op.lhs);
    llvm::Value * rhs = gen_rvalue(*this, *op.rhs);

    switch (op.oper.oper)
    {
        case ast::Oper::PLUS:
            return {ast::int_type(), {value_type::RVALUE, get_builder().CreateAdd(lhs, rhs)}};
        case ast::Oper::MINUS:
            return {ast::int_type(), {value_type::RVALUE, get_builder().CreateSub(lhs, rhs)}};
        case ast::Oper::MULT:
            return {ast::int_type(), {value_type::RVALUE, get_builder().CreateMul(lhs, rhs)}};
        case ast::Oper::DIV:
            return {ast::int_type(), {value_type::RVALUE, get_builder().CreateSDiv(lhs, rhs)}};
        case ast::Oper::MOD:
            return {ast::int_type(), {value_type::RVALUE, get_builder().CreateSRem(lhs, rhs)}};
        case ast::Oper::GT:
            return {ast::bool_type(), {value_type::RVALUE, get_builder().CreateICmpSGT(lhs, rhs)}};
        case ast::Oper::LT:
            return {ast::bool_type(), {value_type::RVALUE, get_builder().CreateICmpSLT(lhs, rhs)}};
        case ast::Oper::EQ:
            return {ast::bool_type(), {value_type::RVALUE, get_builder().CreateICmpEQ(lhs, rhs)}};
        case ast::Oper::GE:
            return {ast::bool_type(), {value_type::RVALUE, get_builder().CreateICmpSGE(lhs, rhs)}};
        case ast::Oper::LE:
            return {ast::bool_type(), {value_type::RVALUE, get_builder().CreateICmpSLE(lhs, rhs)}};
        case ast::Oper::NE:
            return {ast::bool_type(), {value_type::RVALUE, get_builder().CreateICmpNE(lhs, rhs)}};
        case ast::Oper::AND:
            return {ast::bool_type(), {value_type::RVALUE, get_builder().CreateAnd(lhs, rhs)}};
        case ast::Oper::OR:
            return {ast::bool_type(), {value_type::RVALUE, get_builder().CreateOr(lhs, rhs)}};
    }

    throw std::runtime_error("unknown binary operator");
}

typed_value frame::gen_expr(const ast::Dereference & deref) const
{
    llvm::Value * v = gen_rvalue(*this, *deref.expr);
    return {this->get_type(deref), {value_type::LVALUE, v}};
}

typed_value frame::gen_expr(const ast::Call & call) const
{
    typed_value f = this->gen_expr(*call.function);

    std::vector<llvm::Value *> args;
    for (const auto & arg : call.arguments)
        args.push_back(gen_rvalue(*this, arg));

    return {this->get_type(call), {value_type::RVALUE, get_builder().CreateCall(f.second.second, args)}};
}

typed_value frame::gen_expr(const ast::Read & st) const
{
    llvm::Value * f = this->module->getNamedValue("scanf");
    llvm::Value * v = this->get(st.varname).second.second;
    llvm::Value * format_string = gen_format_string(*this, this->get_type(st.varname));
    std::vector<llvm::Value *> args = { format_string, v };
    llvm::Value * scanf_result = get_builder().CreateCall(f, args);
    llvm::Value * eof_const = get_builder().getInt32(EOF);
    llvm::Value * is_eof = get_builder().CreateICmpNE(scanf_result, eof_const, "is_eof");

    return {this->get_type(st), {value_type::RVALUE, is_eof}};
}

typed_value frame::gen_expr(const ast::Expression & expr) const
{
    return fmap([this], x, this->gen_expr(x), expr.expression);
}

void frame::gen_statement(const ast::VarDeclaration & v)
{
    llvm::Value * val = get_builder().CreateAlloca(gen_type(v.type), nullptr, v.name);
    this->declare({v.type, {value_type::LVALUE, val}}, v.name);
}

void frame::gen_statement(const ast::Assignment & st)
{
    llvm::Value * lval = this->gen_expr(st.lvalue).second.second;
    llvm::Value * rval = gen_rvalue(*this, st.rvalue);
    get_builder().CreateStore(rval, lval);
}

void frame::gen_statement(const ast::If & st)
{
    /* Generate condition */
    llvm::Value * cond = gen_rvalue(*this, st.condition);
    llvm::Function * f = get_builder().GetInsertBlock()->getParent();
    llvm::BasicBlock * then_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "then", f);
    llvm::BasicBlock * else_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "else");
    llvm::BasicBlock * cont_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "cont");
    get_builder().CreateCondBr(cond, then_block, else_block);

    /* Generate 'then' branch */
    get_builder().SetInsertPoint(then_block);
    this->gen_statement(*st.thenBody);
    get_builder().CreateBr(cont_block);
    then_block = get_builder().GetInsertBlock();

    /* Generate 'else' branch */
    f->getBasicBlockList().push_back(else_block);
    get_builder().SetInsertPoint(else_block);
    this->gen_statement(*st.elseBody);
    get_builder().CreateBr(cont_block);
    else_block = get_builder().GetInsertBlock();

    /* Continue */
    f->getBasicBlockList().push_back(cont_block);
    get_builder().SetInsertPoint(cont_block);
}

void frame::gen_statement(const ast::While & st)
{
    llvm::Function * f = get_builder().GetInsertBlock()->getParent();
    llvm::BasicBlock * while_body = llvm::BasicBlock::Create(llvm::getGlobalContext(), "while_body", f);
    llvm::BasicBlock * cond_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "while_cond");
    llvm::BasicBlock * cont_block = llvm::BasicBlock::Create(llvm::getGlobalContext(), "while_cont");
    get_builder().CreateBr(cond_block);

    /* Generate condition */
    get_builder().SetInsertPoint(cond_block);
    llvm::Value * cond = gen_rvalue(*this, st.condition);
    get_builder().CreateCondBr(cond, while_body, cont_block);

    /* Generate body branch */
    get_builder().SetInsertPoint(while_body);
    this->gen_statement(*st.body);
    get_builder().CreateBr(cond_block);
    while_body = get_builder().GetInsertBlock();

    /* Continue */
    f->getBasicBlockList().push_back(cond_block);
    f->getBasicBlockList().push_back(cont_block);
    get_builder().SetInsertPoint(cont_block);
}

llvm::Value * gen_format_string(const frame & ctx, const ast::Type & type)
{
    // TODO: check output type
    llvm::Value * format_ptr = ctx.module->getNamedValue("printf_int");
    return format_ptr;
}

void frame::gen_statement(const ast::Write & st)
{
    llvm::Value * f = this->module->getNamedValue("printf");
    llvm::Value * v = gen_rvalue(*this, *st.expr);
    llvm::Value * format_string = gen_format_string(*this, this->get_type(*st.expr));
    std::vector<llvm::Value *> args = { format_string, v };

    get_builder().CreateCall(f, args);
}

void frame::gen_statement(const ast::Return & ret)
{
    get_builder().CreateRet(gen_rvalue(*this, *ret.expr));

    llvm::Function * f = get_builder().GetInsertBlock()->getParent();
    llvm::BasicBlock * unreachable = llvm::BasicBlock::Create(llvm::getGlobalContext(), "unreachable", f);
    get_builder().SetInsertPoint(unreachable);
}

void frame::gen_statement(const ast::Block & block)
{
    frame inner_scope(this->module, this);
    for (auto st : block.statements)
        inner_scope.gen_statement(st);
}

void frame::gen_statement(const ast::Statement & st)
{
    return fmap([this], x, this->gen_statement(x), st.statement);
}

void gen_static_data(llvm::Module * module)
{
    llvm::FunctionType * type = llvm::TypeBuilder<int(...), false>::get(llvm::getGlobalContext());
    llvm::Function::Create(type, llvm::Function::ExternalLinkage, "printf", module);

    llvm::Constant * printf_int_init
            = llvm::ConstantDataArray::getString(llvm::getGlobalContext(), "%d\n");
    llvm::Value * printf_int = new llvm::GlobalVariable(
            *module, llvm::TypeBuilder<char[4], false>::get(llvm::getGlobalContext()), true,
            llvm::GlobalVariable::InternalLinkage, printf_int_init, "printf_int");

    llvm::Function::Create(type, llvm::Function::ExternalLinkage, "scanf", module);
}

typed_value frame::gen_expr(const ast::Address & addr) const
{
    typed_value v = this->gen_expr(*addr.expr);
    if (v.second.first != value_type::LVALUE)
        throw sem::semantic_error(addr.loc, "trying to take address of rvalue");
    llvm::ArrayRef<llvm::Value *> idxList = { get_builder().getInt32(0) };
    llvm::Value * res = get_builder().CreateGEP(v.second.second, idxList, "address");
    return {this->get_type(addr), {value_type::RVALUE, res}};
}

}
