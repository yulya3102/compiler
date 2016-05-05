#include "l.h"

#include <utils/undefined.h>
#include <utils/fmap.h>

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

Function construct_entry(const ast::FuncDefinition & func)
{
    return func;
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
    insert_statements(*st.thenBody, entries);
    insert_statements(*st.elseBody, entries);
}

void insert_statements(const ast::While & st, std::list<Statement> & entries)
{
    insert_statements(*st.body, entries);
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

}
