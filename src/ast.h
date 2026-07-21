
#pragma once

#include <ostream>
#include <typeinfo>

#include "tokens.h"
#include "logger.h"
extern Logger logger;

class _ast_grammar;
class _ast_rule;
class _ast_primary;
class _ast_group;
class _ast_select;
class _ast_zero_or_one;
class _ast_zero_or_more;
class _ast_one_or_more;

class _ast_node {

    public:
    enum {
        AST_GRAMMAR,
        AST_RULE,
        AST_PRIMARY,
        AST_GROUP,
        AST_SELECT,
        AST_ZERO_OR_ONE,
        AST_ZERO_OR_MORE,
        AST_ONE_OR_MORE,
    };

    _ast_node(Token* tok, int type)
            : token(tok)
            , node_type(type) {
    }

    int get_line_no() {
        return token->file->line_no;
    }

    int get_col_no() {
        return token->file->col_no;
    }

    const string& get_file_name() {
        return token->file->fname;
    }

    int type() {
        return node_type;
    }

    const char* to_str() {
        return (node_type == AST_GRAMMAR)       ? "GRAMMAR" :
                (node_type == AST_RULE)         ? "RULE" :
                (node_type == AST_PRIMARY)      ? "PRIMARY" :
                (node_type == AST_GROUP)        ? "GROUP" :
                (node_type == AST_SELECT)       ? "SELECT" :
                (node_type == AST_ZERO_OR_ONE)  ? "ZERO_OR_ONE" :
                (node_type == AST_ZERO_OR_MORE) ? "ZERO_OR_MORE" :
                (node_type == AST_ONE_OR_MORE)  ? "ONE_OR_MORE" :
                                                  "UNKNOWN";
    }

    private:
    Token* token;
    int node_type;
};

/*
    grammar ( + rule )
 */
class _ast_grammar : public _ast_node {

    public:
    _ast_grammar(Token* tok)
            : _ast_node(tok, _ast_node::AST_GRAMMAR) {
    }

    void set_list(vector<_ast_rule*> lst) {
        items.assign(lst.begin(), lst.end());
    }

    vector<_ast_rule*> items;
};

/*
    rule ( NON_TERMINAL group_expr )
 */
class _ast_rule : public _ast_node {

    public:
    _ast_rule(Token* tok)
            : _ast_node(tok, _ast_node::AST_RULE) {
    }

    string* nt;
    _ast_group* group;
};

/*
    primary_expr (
        QSTRG | // creates a TERMINAL symbol
        TERMINAL |
        NON_TERMINAL |
        group_expr |
        zero_or_one_expr |
        zero_or_more_expr |
        one_or_more_expr |
        select_expr
    )
 */
class _ast_primary : public _ast_node {

    public:
    _ast_primary(Token* tok)
            : _ast_node(tok, _ast_node::AST_PRIMARY) {
    }

    string* terminal;
    string* non_terminal;
    _ast_node* node;
};

/*
    group_expr ( '(' + primary_expr ')' )
 */
class _ast_group : public _ast_node {

    public:
    _ast_group(Token* tok)
            : _ast_node(tok, _ast_node::AST_GROUP) {
    }

    void set_list(vector<_ast_primary*> lst) {
        list.assign(lst.begin(), lst.end());
    }

    vector<_ast_primary*> list;
};

/*
    select_expr ( '|' primary_expr )
 */
class _ast_select : public _ast_node {

    public:
    _ast_select(Token* tok)
            : _ast_node(tok, _ast_node::AST_SELECT) {
    }

    _ast_primary* item;
};

/*
    zero_or_one_expr ( '?' primary_expr )
 */
class _ast_zero_or_one : public _ast_node {

    public:
    _ast_zero_or_one(Token* tok)
            : _ast_node(tok, _ast_node::AST_ZERO_OR_ONE) {
    }

    _ast_primary* item;
};

/*
    zero_or_more_expr ( '*' primary_expr )
 */
class _ast_zero_or_more : public _ast_node {

    public:
    _ast_zero_or_more(Token* tok)
            : _ast_node(tok, _ast_node::AST_ZERO_OR_MORE) {
    }

    _ast_primary* item;
};

/*
    one_or_more_expr ( '+' primary_expr )
 */
class _ast_one_or_more : public _ast_node {

    public:
    _ast_one_or_more(Token* tok)
            : _ast_node(tok, _ast_node::AST_ONE_OR_MORE) {
    }

    _ast_primary* item;
};

class AstTraverse {

    public:
    AstTraverse(_ast_node* node)
            : root(node) {
    }

    void traverse() {
        ENTER;
        _traverse_grammar((_ast_grammar*)root);
        RETURN();
    }

    private:
    virtual void _traverse_grammar(_ast_grammar* node) = 0;
    virtual void _traverse_rule(_ast_rule* node) = 0;
    virtual void _traverse_primary(_ast_primary* node) = 0;
    virtual void _traverse_group(_ast_group* node) = 0;
    virtual void _traverse_select(_ast_select* node) = 0;
    virtual void _traverse_zero_or_one(_ast_zero_or_one* node) = 0;
    virtual void _traverse_zero_or_more(_ast_zero_or_more* node) = 0;
    virtual void _traverse_one_or_more(_ast_one_or_more* node) = 0;

    _ast_node* root;
};
