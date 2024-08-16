%{
#include "klangc.h"
#include "str.h"
#include "symbol.h"
#include "parser.h"
#define YYSTYPE         KLANGC_PARSER_STYPE
void klangc_parser_error(const char *s);
%}
%option noyywrap
%option yylineno
%option warn
%option reentrant bison-bridge
%option prefix="klangc_parser_"
%option outfile="lexer.c"
%option header-file="lexer.h"

%x COMMENT
%%
[_A-Za-z][_A-Za-z0-9]*	{
    const klangc_str_t* str = klangc_str_new(yytext, strlen(yytext));
    yylval->kyy_symbol = klangc_symbol_new(str);
    return KLANGC_SYMBOL;
}
[1-9][0-9]*	{
    yylval->kyy_int = atoi(yytext);
    return KLANGC_INT;
}
0[0-7]*	{
    yylval->kyy_int = strtol(yytext, NULL, 8);
    return KLANGC_INT;
}
0[xX][0-9a-fA-F]+	{
    yylval->kyy_int = strtol(yytext, NULL, 16);
    return KLANGC_INT;
}
[0-9]+[eE][+-]?[0-9]+	{
    yylval->kyy_float = atof(yytext);
    return KLANGC_FLOAT;
}
[0-9]+\.[0-9]*([eE][+-]?[0-9]+)?	{
    yylval->kyy_float = atof(yytext);
    return KLANGC_FLOAT;
}
\"([^\\"]|\\.)*\"	{
    yylval->kyy_str = klangc_str_parse_str(yytext, strlen(yytext));
    return KLANGC_STRING;
}
[{}\[\]();:=\\]	{ return yytext[0]; }
[ \t\n]	{ /* ignore whitespace */ }
#.*	{ /* ignore comments */ }
. { klangc_parser_error("unexpected character"); }
\{- { BEGIN COMMENT; }

<COMMENT>-\} { BEGIN INITIAL; }
<COMMENT>.	{ /* ignore comments */ }

%%

void klangc_parser_error(const char *s) {
    fprintf(stderr, "error: %s\n", s);
}