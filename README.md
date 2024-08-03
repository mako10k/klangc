# LANGUAGE SPECIFICATION

----

```
<prog> ::= <defs>*;

<defs> = <def>*;

<def> ::= <lambda>
        | <bind>
        ;

<lambda> ::= "\" <pat> "->" <expr>;

<bind> ::= <pat> "=" <expr>;

<pat> ::= <pat_data>
        | <pat_simp>
        ;

<pat_data> ::= <pat_constr>
             | <pat_data> <pat_simp>;

<pat_simp> ::= <pat_noappl>
             | <pat_simp> <pat>;

<pat_noappl> ::= <pat_var_or_at>
               | <pat_int>
               | <pat_str>
               ;

<pat_var_or_at> ::= <pat_var>
                  | <pat_var> "@" <pat>;

<pat_var> ::= [a-z_][a-zA-Z_]*;

<pat_int> ::= [0-9]+;

<pat_str> ::= ["][^"]*["];

```