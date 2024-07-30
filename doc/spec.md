# 遅延評価

```
pat = PConstr pat*
    | PVar ( "@" pat )*
    ;

val = VConstr val*
    | VVar val*
    ;

expr = EConstr expr*
     | EVar expr*
     | "\" pat "->" expr
     ;

```