%{
#include <string>

int yylineno = 1;

#define YY_DECL \
    ast::generated_parser::token_type \
        lexer_t::lex( \
            ast::generated_parser::semantic_type * yylval, \
            ast::generated_parser::location_type * yylloc)

#include "lexer.h"

#define yyterminate()   return token::END

#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%option c++

string      [a-zA-Z]+
integer     -?[0-9]+

%%

%{
    // reset location
    yylloc->step();
%}

";"         { return token::SEMICOLON; }
"("         { return token::LPAREN; }
")"         { return token::RPAREN; }
","         { return token::COMMA; }
"{"         { return token::CLPAREN; }
"}"         { return token::CRPAREN; }
"="         { return token::ASSIGN; }
"if"        { return token::IF; }
"else"      { return token::ELSE; }
"while"     { return token::WHILE; }
"read"      { return token::READ; }
"write"     { return token::WRITE; }
"return"    { return token::RETURN; }
"+"         { return token::PLUS; }
"-"         { return token::MINUS; }
"*"         { return token::MULT; }
"/"         { return token::DIV; }
"%"         { return token::MOD; }
"<"         { return token::LT; }
">"         { return token::GT; }
"=="        { return token::EQ; }
"<="        { return token::LE; }
">="        { return token::GE; }
"!="        { return token::NE; }
"&&"        { return token::AND; }
"||"        { return token::OR; }
"int"       { return token::INTEGER_TYPE; }
"_Bool"     { return token::BOOLEAN_TYPE; }
"true"      { return token::TRUE; }
"false"     { return token::FALSE; }
{string}    { yylval->string = new std::string(yytext, yyleng); return token::NAME; }
{integer}   { yylval->integer = std::stoi(std::string(yytext, yyleng)); return token::INTEGER; }

[ \t]*      { yylloc->step(); }
[\n]        { yylloc->lines(yyleng); yylloc->step(); }

%%

// fuck you, yyFlexLexer interface
#ifdef yylex
#undef yylex
#endif
int yyFlexLexer::yylex()
{
    std::cerr << "error: yyFlexLexer::yylex()" << std::endl;
    return 0;
}

int yyFlexLexer::yywrap()
{
    return 1;
}
