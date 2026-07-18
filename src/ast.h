
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
    enum ast_node_type {
        AST_GRAMMAR,
        AST_RULE,
        AST_PRIMARY,
        AST_GROUP,
        AST_SELECT,
        AST_ZERO_OR_ONE,
        AST_ZERO_OR_MORE,
        AST_ONE_OR_MORE,
    };

    _ast_node(Token* tok, int node_type) {
        token = tok;
        type = node_type;
    }

    virtual void traverse() = 0;

    const char* to_str() {
        return (type == AST_GRAMMAR)       ? "GRAMMAR" :
                (type == AST_RULE)         ? "RULE" :
                (type == AST_PRIMARY)      ? "PRIMARY" :
                (type == AST_GROUP)        ? "GROUP" :
                (type == AST_SELECT)       ? "SELECT" :
                (type == AST_ZERO_OR_ONE)  ? "ZERO_OR_ONE" :
                (type == AST_ZERO_OR_MORE) ? "ZERO_OR_MORE" :
                (type == AST_ONE_OR_MORE)  ? "ONE_OR_MORE" :
                                             "UNKNOWN";
    }

    int get_type() {
        return type;
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

    private:
    Token* token;
    int type;
};

/*
    grammar ( + rule )
 */
class _ast_grammar : public _ast_node {

    public:
    _ast_grammar(Token* tok)
    : _ast_node(tok, _ast_node::AST_GRAMMAR) {
    }

    virtual void traverse() override {
        ENTER;
        START(" traverse ");
        pre_action();
        for(auto node : items) {
            TRACE(format("node type: {}", node->to_str()));
            node->traverse();
        }
        post_action();
        END(" traverse ");
        RETURN();
    }

    virtual void pre_action() {
    }
    virtual void post_action() {
    }

    void set_list(vector<_ast_node*> lst) {
        items.assign(lst.begin(), lst.end());
    }

    void add_item(_ast_node* node) {
        items.push_back(node);
    }

    private:
    vector<_ast_node*> items;
};

/*
    rule ( NON_TERMINAL group_expr )
 */
class _ast_rule : public _ast_node {

    public:
    _ast_rule(Token* tok)
    : _ast_node(tok, _ast_node::AST_RULE) {
    }

    virtual void traverse() override {
        ENTER;
        pre_action();
        TRACE(format("define non-terminal symbol: {}", *nt));
        ((_ast_node*)group)->traverse();
        post_action();
        RETURN();
    }

    virtual void pre_action() {
    }
    virtual void post_action() {
    }

    void set_non_terminal(string* node) {
        nt = node;
    }

    void set_group(_ast_group* node) {
        group = node;
    }

    private:
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

    virtual void traverse() override {
        ENTER;
        pre_action();
        // TRACE(format("this: {}: {}", (void*)this, typeid(node).name()));
        if(non_terminal)
            TRACE(format("non-terminal symbol: \"{}\"", *non_terminal));
        else if(terminal)
            TRACE(format("terminal symbol: \"{}\"", *terminal));
        else if(node) {
            TRACE(format("traversing node type: {}", node->to_str()));
            node->traverse();
        }
        else
            TRACE("LAME VALUE");
        post_action();
        RETURN();
    }

    virtual void pre_action() {
    }
    virtual void post_action() {
    }

    void set_term(string* str) {
        terminal = str;
    }

    void set_nterm(string* str) {
        non_terminal = str;
    }

    void set_item(_ast_node* val) {
        node = val;
    }

    private:
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

    virtual void traverse() override {
        ENTER;
        pre_action();
        TRACE(format("list size is {}", list.size()));
        for(auto node : list) {
            TRACE(format("this: {}: {}", (void*)this, typeid(*node).name()));
            TRACE(format("traversing node type: {}", node->to_str()));
            ((_ast_node*)node)->traverse();
        }
        post_action();
        RETURN();
    }

    virtual void pre_action() {
    }
    virtual void post_action() {
    }

    void set_list(vector<_ast_primary*> lst) {
        list.assign(lst.begin(), lst.end());
    }

    void add_node(_ast_primary* node) {
        list.push_back(node);
    }

    private:
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

    virtual void traverse() override {
        ENTER;
        pre_action();
        TRACE(format("traversing node type: {}", item->to_str()));
        ((_ast_node*)item)->traverse();
        post_action();
        RETURN();
    }

    virtual void pre_action() {
    }
    virtual void post_action() {
    }

    void set_item(_ast_primary* node) {
        item = node;
    }

    private:
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

    virtual void traverse() override {
        ENTER;
        pre_action();
        TRACE(format("traversing node type: {}", item->to_str()));
        ((_ast_node*)item)->traverse();
        post_action();
        RETURN();
    }

    virtual void pre_action() {
    }
    virtual void post_action() {
    }

    void set_item(_ast_primary* node) {
        item = node;
    }

    private:
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

    virtual void traverse() override {
        pre_action();
        ENTER;
        TRACE(format("traversing node type: {}", item->to_str()));
        ((_ast_node*)item)->traverse();
        post_action();
        RETURN();
    }

    virtual void pre_action() {
    }
    virtual void post_action() {
    }

    void set_item(_ast_primary* node) {
        item = node;
    }

    private:
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

    virtual void traverse() override {
        ENTER;
        pre_action();
        TRACE(format("traversing node type: {}", item->to_str()));
        ((_ast_node*)item)->traverse();
        post_action();
        RETURN();
    }

    virtual void pre_action() {
    }
    virtual void post_action() {
    }

    void set_item(_ast_primary* node) {
        item = node;
    }

    private:
    _ast_primary* item;
};
