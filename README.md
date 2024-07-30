# LANGUAGE SPECIFICATION

----

```
<INCLUDE> ::= <MODULE> ("@" <INCLUDE>)*;
<MODULE> ::= "{" <BINDS> "}";
<BINDS> ::= <BIND>*;
<BIND> ::= <NAME> "=" <EXPR_N> ";";
<EXPR> ::= <EXPR'> ("." <NAME>)*; 
<EXPR'> ::= <NAME> /*VAR or CONSTR*/
         | <LITERAL>
         | <APPL>
         | <LAMBDAS>
         | "(" <EXPR> ")";
<LITERAL> ::= "0" | "1";
<APPL> ::= <EXPR> <EXPR>+ ;
<LAMBDAS> ::= <LAMBDA> ("|" <LAMBDA>)*;
<LAMBDA> ::= "\" <PATTERN> "->" <EXPR> ;
<PATTERN> ::= <WILDCARD>
            | <PAPPL>

```