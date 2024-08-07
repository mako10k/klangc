# LANGUAGE SPECIFICATION

----

```
<prog> ::= <bare_closure>*;

<bare_closure> = <closure_entry>*;

<closure_entry> ::= ( <lambda> | <bind> ) ";";

<lambda> ::= "\" <pat> "->" <expr>;

<bind> ::= <pat> "=" <expr>;

<pat> ::= <pat_noapp> <pat_noapp>*;

<pat_noapp> ::= "(" <pat> ")"
              | <symbol>
              | <ref>
              | <pat_at>
              | <int>
              | <str>
              ;

<expr>

TOKENS

<symbol> ::= [A-Za-z_][A-Za-z_0-9]*;
<reference> ::= "~" <symbol>;
<int> ::= [0-9]+;
<str> ::= ["][^"]*["];

```