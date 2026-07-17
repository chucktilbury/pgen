
%token NON_TERMINAL TERMINAL QSTRG

%left '!'
%left '|'
%left '?' '*' '+'

%%

grammar
    : rule
    | grammar rule
    ;

rule
    : NON_TERMINAL grouping_expr
    ;

expression
    : primary_expr
    | expression primary_expr
    ;

primary_expr
    : QSTRG
    | TERMINAL
    | NON_TERMINAL
    | grouping_expr
    | select_expr
    | one_or_zero_expr
    | one_or_more_expr
    | zero_or_more_expr
    | not_expr
    ;

grouping_expr
    : '(' expression ')'
    ;

select_expr
    : '|' primary_expr
    ;

one_or_zero_expr
    : '?' primary_expr
    ;

one_or_more_expr
    : '+' primary_expr
    ;

zero_or_more_expr
    : '*' primary_expr
    ;

not_expr
    : '!' primary_expr
    ;

%%