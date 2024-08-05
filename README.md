# LANGUAGE SPECIFICATION

----

```
<prog> ::= <bare_closure>*;

<bare_closure> = <closure_entry>*;

<closure_entry> ::= <lambda> | <bind>;

<lambda> ::= "\" <pat> "->" <expr>;

<bind> ::= <pat> "=" <expr>;

<pat> ::= <pat_noapp> <pat>*
        | <pat_data>;

<pat_noapp> ::= "(" <pat> ")"
              | <symbol> "@" <pat>
              | <int>
              | <str>
              ;

<pat_data> ::= ":" <pat>*;


TOKENS

<symbol> ::= [A-Za-z_][A-Za-z_0-9]*;
<reference> ::= "~" <symbol>;
<int> ::= [0-9]+;
<str> ::= ["][^"]*["];

```