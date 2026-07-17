
#pragma once

#include <ostream>

#include "ast.h"
#include "tokens.h"
#include "logger.h"
extern Logger logger;

class AstGrammar;
class AstRule;
class AstPrimary;
class AstGrouping;
class AstSelect;
class AstZeroOrOne;
class AstZeroOrMore;
class AstOneOrMore;
class AstNot;

class AstNode {

public:
    enum ast_type {
        AST_NON_TERMINAL,
        AST_TERMINAL,
        AST_GRAMMAR,
        AST_RULE,
        AST_PRIMARY,
        AST_GROUPING,
        AST_SELECT,
        AST_ZERO_OR_ONE,
        AST_ZERO_OR_MORE,
        AST_ONE_OR_MORE,
        AST_NOT,
    };

    AstNode(Token* tok, int ty) {
        token = tok;
        type = ty;
    }

    virtual void traverse() = 0;

    friend ostream& operator << (ostream &strm, const AstNode& a) {
        string str = (a.type == AST_GRAMMAR)? "GRAMMAR" :
            (a.type == AST_RULE)? "RULE" :
            (a.type == AST_NON_TERMINAL)? "NON_TERMINAL" :
            (a.type == AST_TERMINAL)? "TERMINAL" :
            (a.type == AST_PRIMARY)? "PRIMARY" :
            (a.type == AST_SELECT)? "SELECT" :
            (a.type == AST_GROUPING)? "GROUPING" :
            (a.type == AST_ZERO_OR_ONE)? "ZERO_OR_ONE" :
            (a.type == AST_ZERO_OR_MORE)? "ZERO_OR_MORE" :
            (a.type == AST_ONE_OR_MORE)? "ONE_OR_MORE" :
            (a.type == AST_NOT)? "NOT" : "UNKNOWN";
        return strm << str;
    }

    string type_to_str() {
        return string((type == AST_GRAMMAR)? "GRAMMAR" :
            (type == AST_RULE)? "RULE" :
            (type == AST_NON_TERMINAL)? "NON_TERMINAL" :
            (type == AST_TERMINAL)? "TERMINAL" :
            (type == AST_PRIMARY)? "PRIMARY" :
            (type == AST_SELECT)? "SELECT" :
            (type == AST_GROUPING)? "GROUPING" :
            (type == AST_ZERO_OR_ONE)? "ZERO_OR_ONE" :
            (type == AST_ZERO_OR_MORE)? "ZERO_OR_MORE" :
            (type == AST_ONE_OR_MORE)? "ONE_OR_MORE" :
            (type == AST_NOT)? "NOT" : "UNKNOWN");
    }

    string type_to_str(AstNode* node) {
        return string((node->type == AST_GRAMMAR)? "GRAMMAR" :
            (node->type == AST_RULE)? "RULE" :
            (node->type == AST_NON_TERMINAL)? "NON_TERMINAL" :
            (node->type == AST_TERMINAL)? "TERMINAL" :
            (node->type == AST_PRIMARY)? "PRIMARY" :
            (node->type == AST_SELECT)? "SELECT" :
            (node->type == AST_GROUPING)? "GROUPING" :
            (node->type == AST_ZERO_OR_ONE)? "ZERO_OR_ONE" :
            (node->type == AST_ZERO_OR_MORE)? "ZERO_OR_MORE" :
            (node->type == AST_ONE_OR_MORE)? "ONE_OR_MORE" :
            (node->type == AST_NOT)? "NOT" : "UNKNOWN");
    }

    int get_line_no() { return token->file->line_no; }
    int get_col_no() { return token->file->col_no; }
    const string& get_file_name() { return token->file->fname; }

private:
    int type;
    Token* token;
};

class AstGrammar: public AstNode {

public:
    AstGrammar(Token* tok): AstNode(tok, AstNode::AST_GRAMMAR) {}

    virtual void traverse() override {
        ENTER;
        START(" traverse ");
        for(auto node: *items) {
            ((AstNode*)node)->traverse();
        }
        END(" traverse ");
        RETURN();
    }

    virtual void action() {}

    void set_list(vector<AstRule*>* lst) {
        items = lst;
    }

    void add_item(AstRule* node) {
        items->push_back(node);
    }

private:
    vector<AstRule*>* items;
};

class AstRule: public AstNode {

public:
    AstRule(Token* tok): AstNode(tok, AstNode::AST_RULE) {}

    virtual void traverse() override {
        ENTER;
        TRACE(format("define non-terminal symbol: {}", *nt));
        ((AstNode*)(expr))->traverse();
        RETURN();
    }

    virtual void action() {}

    void set_non_terminal(string* node) {
        nt = node;
    }

    void set_expr(AstGrouping* node) {
        expr = node;
    }

private:
    string* nt;
    AstGrouping* expr;
};

class AstPrimary: public AstNode {

public:
    AstPrimary(Token* tok): AstNode(tok, AstNode::AST_PRIMARY) {}

    virtual void traverse() override {
        ENTER;
        TRACE(format("this: {}: {}", (void*)this, type_to_str(node)));
        if(non_terminal)
            TRACE(format("non-terminal symbol: {}", *non_terminal));
        else if(terminal)
            TRACE(format("terminal symbol: {}", *terminal));
        else if(node)
            node->traverse();
        RETURN();
    }

    virtual void action() {}

    void set_terminal(string* str) {
        terminal = str;
        non_terminal = nullptr;
        node = nullptr;
    }

    void set_non_terminal(string* str) {
        non_terminal = str;
        terminal = nullptr;
        node = nullptr;
    }

    void set_node(AstNode* val) {
        node = val;
        terminal = nullptr;
        non_terminal = nullptr;
    }

private:
    string* terminal;
    string* non_terminal;
    AstNode* node;
};

class AstGrouping: public AstNode {

public:
    AstGrouping(Token* tok): AstNode(tok, AstNode::AST_GROUPING) {}

    virtual void traverse() override {
        ENTER;
        for(auto node: list) {
            node->traverse();
        }
        RETURN();
    }

    virtual void action() {}

    void set_list(vector<AstPrimary*> lst) {
        list.assign(lst.begin(), lst.end());
    }

    void add_node(AstPrimary* node) {
        list.push_back(node);
    }

private:
    vector<AstPrimary*> list;
};

class AstSelect: public AstNode {

public:
    AstSelect(Token* tok): AstNode(tok, AstNode::AST_SELECT) {}

    virtual void traverse() override {
        ((AstNode*)item)->traverse();
    }

    virtual void action() {}

    void set_item(AstPrimary* node) {
        item = node;
    }

private:
    AstPrimary* item;
};

class AstZeroOrOne: public AstNode {

public:
    AstZeroOrOne(Token* tok): AstNode(tok, AstNode::AST_ZERO_OR_ONE) {}

    virtual void traverse() override {
        ((AstNode*)item)->traverse();
    }

    virtual void action() {}

    void set_item(AstPrimary* node) {
        item = node;
    }

private:
    AstPrimary* item;
};

class AstZeroOrMore: public AstNode {

public:
    AstZeroOrMore(Token* tok): AstNode(tok, AstNode::AST_ZERO_OR_MORE) {}

    virtual void traverse() override {
        ((AstNode*)item)->traverse();
    }

    virtual void action() {}

    void set_item(AstPrimary* node) {
        item = node;
    }

private:
    AstPrimary* item;
};

class AstOneOrMore: public AstNode {

public:
    AstOneOrMore(Token* tok): AstNode(tok, AstNode::AST_ONE_OR_MORE) {}

    virtual void traverse() override {
        ((AstNode*)item)->traverse();
    }

    virtual void action() {}

    void set_item(AstPrimary* node) {
        item = node;
    }

private:
    AstPrimary* item;
};

class AstNot: public AstNode {

public:
    AstNot(Token* tok): AstNode(tok, AstNode::AST_NOT) {}

    virtual void traverse() override {
        ((AstNode*)item)->traverse();
    }

    virtual void action() {}

    void set_item(AstPrimary* node) {
        item = node;
    }

private:
    AstPrimary* item;
};

