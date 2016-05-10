#include "l.h"

#include <sem/types.h>

#include <utils/undefined.h>
#include <utils/fmap.h>
#include <utils/top.h>

namespace codegen
{

namespace
{
void insert_entry(const ast::Declaration & decl, std::list<CodeEntry> & entries)
{ }

Variable construct_entry(const ast::VarDeclaration & var)
{
    return var;
}

std::string annotated_variable(const sem::typed_ctx<top> & frame, const std::string & name)
{
    if (frame.is_declared(name))
        return name + "_" + ast::to_string(frame.get_type(name));

    return name;
}

ast::VarDeclaration annotate_variable(sem::typed_ctx<top> & frame, const ast::VarDeclaration & var)
{
    frame.declare({var.type, top()}, var.name);
    return {var.loc, var.type, annotated_variable(frame, var.name)};
}

ast::FuncDeclaration annotate_variables(sem::typed_ctx<top> & frame, const ast::FuncDeclaration & func)
{
    std::list<ast::VarDeclaration> arguments;
    for (auto var : func.arguments)
    {
        arguments.push_back(annotate_variable(frame, var));
    }
    return {func.loc, func.type, func.name, arguments};
}

ast::Const annotate_variables(sem::typed_ctx<top> & frame, const ast::Const & expr)
{
    return expr;
}

std::string annotate_variables(sem::typed_ctx<top> & frame, const std::string & expr)
{
    return annotated_variable(frame, expr);
}

ast::Value annotate_variables(sem::typed_ctx<top> & frame, const ast::Value & expr)
{
    return fmap([&frame], x, ast::Value(annotate_variables(frame, x)), expr.value);
}

ast::Expression annotate_variables(sem::typed_ctx<top> & frame, const ast::Expression & expr);

ast::BinOperator annotate_variables(sem::typed_ctx<top> & frame, const ast::BinOperator & expr)
{
    auto lhs = annotate_variables(frame, *expr.lhs);
    auto rhs = annotate_variables(frame, *expr.rhs);

    return {expr.loc,
                std::shared_ptr<ast::Expression>(new ast::Expression(lhs)),
                std::shared_ptr<ast::Expression>(new ast::Expression(rhs)),
                expr.oper};
}

ast::Dereference annotate_variables(sem::typed_ctx<top> & frame, const ast::Dereference & expr)
{
    auto deref = annotate_variables(frame, *expr.expr);

    return {expr.loc,
                std::shared_ptr<ast::Expression>(new ast::Expression(deref))};
}

ast::Address annotate_variables(sem::typed_ctx<top> & frame, const ast::Address & expr)
{
    auto addr = annotate_variables(frame, *expr.expr);

    return {expr.loc,
                std::shared_ptr<ast::Expression>(new ast::Expression(addr))};
}

ast::Call annotate_variables(sem::typed_ctx<top> & frame, const ast::Call & expr)
{
    auto func = annotate_variables(frame, *expr.function);

    std::list<ast::Expression> arguments;
    for (auto arg : expr.arguments)
        arguments.push_back(annotate_variables(frame, arg));

    return {expr.loc, std::shared_ptr<ast::Expression>(new ast::Expression(func)), arguments};
}

ast::Read annotate_variables(sem::typed_ctx<top> & frame, const ast::Read & expr)
{
    return {expr.loc, annotated_variable(frame, expr.varname)};
}

ast::Expression annotate_variables(sem::typed_ctx<top> & frame, const ast::Expression & expr)
{
    return fmap([&frame], x, ast::Expression(annotate_variables(frame, x)), expr.expression);
}

ast::VarDeclaration annotate_variables(sem::typed_ctx<top> & frame, const ast::VarDeclaration & st)
{
    return annotate_variable(frame, st);
}

ast::Assignment annotate_variables(sem::typed_ctx<top> & frame, const ast::Assignment & st)
{
    auto lvalue = annotate_variables(frame, st.lvalue);
    auto rvalue = annotate_variables(frame, st.rvalue);
    return {st.loc, lvalue, rvalue};
}

ast::Block annotate_variables(sem::typed_ctx<top> & frame, const ast::Block & st);

ast::If annotate_variables(sem::typed_ctx<top> & frame, const ast::If & st)
{
    sem::typed_ctx<top> inner_frame(&frame);

    auto cond = annotate_variables(inner_frame, st.condition);
    auto thenBody = annotate_variables(inner_frame, *st.thenBody);
    auto elseBody = annotate_variables(inner_frame, *st.elseBody);

    return {st.loc, cond,
                std::shared_ptr<ast::Block>(new ast::Block(thenBody)),
                std::shared_ptr<ast::Block>(new ast::Block(elseBody))};
}

ast::While annotate_variables(sem::typed_ctx<top> & frame, const ast::While & st)
{
    sem::typed_ctx<top> inner_frame(&frame);

    auto cond = annotate_variables(inner_frame, st.condition);
    auto body = annotate_variables(inner_frame, *st.body);

    return {st.loc, cond,
                std::shared_ptr<ast::Block>(new ast::Block(body))};
}

ast::Write annotate_variables(sem::typed_ctx<top> & frame, const ast::Write & st)
{
    auto expr = annotate_variables(frame, st.expr);

    return {st.loc, ast::Expression(expr)};
}

ast::Return annotate_variables(sem::typed_ctx<top> & frame, const ast::Return & st)
{
    return {st.loc, ast::Expression(annotate_variables(frame, st.expr))};
}

ast::Statement annotate_variables(sem::typed_ctx<top> & frame, const ast::Statement & st)
{
    return fmap([&frame], x, ast::Statement(annotate_variables(frame, x)), st.statement);
}

ast::Block annotate_variables(sem::typed_ctx<top> & frame, const ast::Block & st)
{
    sem::typed_ctx<top> inner_frame(&frame);
    std::list<ast::Statement> statements;

    for (auto statement : st.statements)
        statements.push_back(annotate_variables(inner_frame, statement));

    return {st.loc, statements};
}

ast::FuncDefinition annotate_variables(const ast::FuncDefinition & func)
{
    sem::typed_ctx<top> frame;
    auto decl = annotate_variables(frame, func.declaration);
    auto body = annotate_variables(frame, func.statements);
    return {func.loc, decl, body};
}

Function construct_entry(const ast::FuncDefinition & func)
{
    return annotate_variables(func);
}

void insert_entry(const ast::Definition & decl, std::list<CodeEntry> & entries)
{
    CodeEntry entry = fmap([&decl], x,
                           CodeEntry(construct_entry(x)),
                           decl.definition);
    entries.push_back(entry);
}
}

std::list<codegen::CodeEntry> construct_entries(const std::list<ast::CodeEntry> & entries)
{
    std::list<codegen::CodeEntry> gen_entries;

    for (auto entry : entries)
        fmap([&gen_entries], x, insert_entry(x, gen_entries), entry.entry);

    return gen_entries;
}

namespace
{
void insert_variable(const ast::Assignment & st, std::list<Variable> & entries) { }
void insert_variable(const ast::Write & st, std::list<Variable> & entries) { }
void insert_variable(const ast::Return & st, std::list<Variable> & entries) { }

void insert_variable(const ast::VarDeclaration & st, std::list<Variable> & entries)
{
    entries.push_back(st);
}

void insert_variable(const ast::Block & st, std::list<Variable> & entries);

void insert_variable(const ast::If & st, std::list<Variable> & entries)
{
    insert_variable(*st.thenBody, entries);
    insert_variable(*st.elseBody, entries);
}

void insert_variable(const ast::While & st, std::list<Variable> & entries)
{
    insert_variable(*st.body, entries);
}

void insert_variable(const ast::Block & st, std::list<Variable> & entries)
{
    for (auto statement : st.statements)
        fmap([&entries], x, insert_variable(x, entries), statement.statement);
}
}

std::list<Variable> function_variables(const ast::FuncDefinition & func)
{
    std::list<Variable> variables;

    insert_variable(func.statements, variables);

    /*
     * Variable can't be declared more than once in one scope,
     * semantic checker made sure of that.
     * Also variables with different types can't have same names,
     * as we annotated variables with their types.
     * If any variable gets redeclared here, their locations
     * can be safely merged into one location.
     */
    variables.sort([] (const Variable & lhs, const Variable & rhs)
    {
        return lhs.name < rhs.name;
    });
    auto it = std::unique(variables.begin(), variables.end(),
                [] (const Variable & lhs, const Variable & rhs)
    {
        return lhs.name == rhs.name;
    });
    variables.erase(it, variables.end());

    return variables;
}

namespace
{
void insert_statements(const ast::VarDeclaration & st, std::list<Statement> & entries) { }

void insert_statements(const ast::Assignment & st, std::list<Statement> & entries)
{
    entries.push_back(Statement(st));
}

void insert_statements(const ast::Write & st, std::list<Statement> & entries)
{
    entries.push_back(Statement(st));
}

void insert_statements(const ast::Return & st, std::list<Statement> & entries)
{
    entries.push_back(Statement(st));
}

void insert_statements(const ast::Block & st, std::list<Statement> & entries);

void insert_statements(const ast::If & st, std::list<Statement> & entries)
{
    std::list<Statement> thenBody, elseBody;
    insert_statements(*st.thenBody, thenBody);
    insert_statements(*st.elseBody, elseBody);
    If gen_st{st.loc, st.condition, thenBody, elseBody};
    entries.push_back(Statement(gen_st));
}

void insert_statements(const ast::While & st, std::list<Statement> & entries)
{
    std::list<Statement> body;
    insert_statements(*st.body, body);
    static std::size_t label_idx = 0;
    std::string label = "label_" + label_idx;
    label_idx += 1;
    While gen_st{st.loc, label, st.condition, body};
    entries.push_back(Statement(gen_st));
}

void insert_statements(const ast::Block & st, std::list<Statement> & entries)
{
    for (auto statement : st.statements)
        fmap([&entries], x, insert_statements(x, entries), statement.statement);
}
}

std::list<Statement> function_statements(const ast::FuncDefinition & func)
{
    std::list<Statement> statements;

    insert_statements(func.statements, statements);

    return statements;
}

std::string to_string(const Code & code)
{
    undefined;
}

std::string to_string(const CodeEntry & code)
{
    undefined;
}

std::string to_string(const Function & code)
{
    undefined;
}

std::string to_string(const Statement & code)
{
    undefined;
}

std::string to_string(const Continue & code)
{
    undefined;
}

std::string to_string(const While & code)
{
    undefined;
}

std::string to_string(const If & code)
{
    undefined;
}

}
