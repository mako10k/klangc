# LANGUAGE SPECIFICATION

----

```
<program> ::= <closure>*;
<closure> ::= <lambda> ( ";" <bind> )? ";";
<lambda> ::= "\" <pat> "->" <expr> ( ";" <lambda> )?;
<bind> ::= <pat> "=" <expr> ( ";" <bind> )?;
<pat> ::= <pat_noalge> | <pat_alge>;
<pat_noalge> ::=
    "(" <pat> ")";
  | <ref> ("@" <pat_noalge>)?
  | <int>
  | <string>
  ;
<pat_alge> ::= <symbol> | <pat|except pat_alge>*; 
<expr> ::= <expr_noappl> | <expr_appl>;
<expr_noappl> ::=
    "(" <expr> ")"
  | <lambda>
  | <ref>
  | <expr_closure>
  | <int>
  | <string>
  ;
<expr_appl> ::= <expr_alge> | <expr_noappl>+;
<expr_alge> ::= <symbol> <expr_noappl>*;
<expr_closure> ::= "{" <closure> "}";

## TOKENS

<symbol> ::= [A-Za-z_][A-Za-z_0-9]*;
<ref> ::= "~" <symbol>;
<int> ::= [0-9]+;
<str> ::= ["][^"]*["];

```