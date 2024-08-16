%{
#include "klangc.h"
%}

%union {
    const klangc_symbol_t* kyy_symbol;
    klangc_expr_t* kyy_expr;
    klangc_pat_t* kyy_pat;
    const klangc_str_t* kyy_str;
    int kyy_int;
    double kyy_float;
}

%define api.pure full
%define api.prefix {klangc_parser_}
%header "parser.h"
%output "parser.c"

%token KLANGC_SYMBOL
%token KLANGC_INT
%token KLANGC_FLOAT
%token KLANGC_STRING
%token '{' '}' '[' ']' '(' ')' ';' ':' '=' '@' ','
%token KLANGC_ARROW

%start prog


%%

prog
    : /* empty */
    | expr
    ;

expr
    : expr0
    | expr0 '@' binding
    ;

expr0
    : expr9
    ;

expr9
    : reference
    | lambda
    | expr_list
    | expr_tuple
    | primitive
    ;

expr_list
    : '[' ']'
    | '[' expr_list1 ']'
    ;

expr_list1
    : expr
    | expr_list1 ',' expr
    ;

expr_tuple
    : '(' ')'
    | '(' expr_tuple1 ')'
    ;

expr_tuple1
    : expr
    | expr_tuple1 ',' expr
    ;

lambda
    : '\\' pattern KLANGC_ARROW expr0
    | '{' lambda_list '}'
    ;

lambda_list
    : /* empty */
    | lambda_list lambda ';'
    ;

binding
    : pattern '=' expr
    | '{' binding_list '}'
    ;

binding_list
    : /* empty */
    | binding_list binding ';'
    ;

pattern
    : pattern0
    | pattern0 ':' pattern
    ;

pattern0
    : pattern8
    ;

pattern8
    : pattern9
    | pattern8 pattern9
    ;

pattern9
    : reference '@' pattern9
    | reference
    | pattern_list
    | pattern_tuple
    | primitive
    ;

pattern_list
    : '[' ']'
    | '[' pattern_list1 ']'
    ;

pattern_list1
    : pattern
    | pattern_list1 ',' pattern
    ;

pattern_tuple
    : '(' ')'
    | '(' pattern_tuple1 ')'
    ;

pattern_tuple1
    : pattern
    | pattern_tuple1 ',' pattern
    ;

reference
    : '~' primitive
    ;

primitive
    : KLANGC_SYMBOL
    | KLANGC_INT
    | KLANGC_FLOAT
    | KLANGC_STRING
    ;