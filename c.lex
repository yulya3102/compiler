%{
#include <string>

int yylineno = 1;

#define YY_DECL \
    yy::parser::token_type \
        lexer_t::lex( \
            yy::parser::semantic_type * yylval, \
            yy::parser::location_type * yylloc)

#include "lexer.h"

#define yyterminate()   return token::END
%}

%option c++

string      [a-zA-Z]+
integer     -?[1-9][0-9]*

%%

";"         { return token::SEMICOLON; }
"("         { return token::LPAREN; }
")"         { return token::RPAREN; }
","         { return token::COMMA; }
"{"         { return token::CLPAREN; }
"}"         { return token::CRPAREN; }
"="         { return token::ASSIGN; }
"extern"    { return token::EXTERN; }
"if"        { return token::IF; }
"else"      { return token::ELSE; }
"while"     { return token::WHILE; }
"read"      { return token::READ; }
"write"     { return token::WRITE; }
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

[ \t]*      {}
[\n]        { yylineno++; }


