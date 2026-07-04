
#pragma once

#include "parser.h"
#include "logger.h"
extern Logger logger;

class AstGrammar;
class AstRule;
class AstNonTerminal;
class AstTerminal;
class AstExpression;
class AstSelect;
class AstSequence;
class AstZeroOrOne;
class AstZeroOrMore;
class AstOneOrMore;
class AstNot;

class AstNode {

public:
    enum ast_type {
        AST_GRAMMAR,
        AST_RULE,
        AST_NON_TERMINAL,
        AST_TERMINAL,
        AST_EXPRESSION,
        AST_SELECT,
        AST_SEQUENCE,
        AST_ZERO_OR_ONE,
        AST_ZERO_OR_MORE,
        AST_ONE_OR_MORE,
        AST_NOT,
    };

    AstNode(ParserState* ps, int ty) {
        type = ty;
        line = ps->get_line_no();
        col = ps->get_col_no();
        fname = ps->get_file_name();
    }

    virtual void traverse() = 0;

private:
    int type;
    int line;
    int col;
    string fname;
};

class AstGrammar: public AstNode {

public:
    AstGrammar(ParserState* ps): AstNode(ps, AstNode::AST_GRAMMAR) {}

    virtual void traverse() override {
        for(auto node: items) {
            node->traverse();
        }
    }

    virtual void action() = 0;

    void add_item(AstNode* node) {
        items.push_back(node);
    }

private:
    vector<AstNode*> items;
};

class AstRule: public AstNode {

public:
    AstRule(ParserState* ps): AstNode(ps, AstNode::AST_RULE) {}

    virtual void traverse() override {
        ((AstNode*)(nt))->traverse();
        ((AstNode*)(seq))->traverse();
    }

    virtual void action() = 0;

    void set_non_terminal(AstNonTerminal* node) {
        nt = node;
    }

    void set_sequence(AstSequence* node) {
        seq = node;
    }

private:
    AstNonTerminal* nt;
    AstSequence* seq;
};

class AstNonTerminal: public AstNode {

public:
    AstNonTerminal(ParserState* ps): AstNode(ps, AstNode::AST_NON_TERMINAL) {}

    virtual void traverse() override {
        TRACE(format("non-terminal symbol: {}", symbol->get_text()));
    }

    virtual void action() = 0;

    void set_symbol(Token* tok) {
        symbol = tok;
    }

private:
    Token* symbol;
};

class AstSequence: public AstNode {

public:
    AstSequence(ParserState* ps): AstNode(ps, AstNode::AST_SEQUENCE) {}

    virtual void traverse() override {
        for(auto node: items)
            node->traverse();
    }

    virtual void action() = 0;

    void add_item(AstNode* node) {
        items.push_back(node);
    }

private:
    vector<AstNode*> items;
};

class AstTerminal: public AstNode {

public:
    AstTerminal(ParserState* ps): AstNode(ps, AstNode::AST_TERMINAL) {}

    virtual void traverse() override {
        TRACE(format("terminal symbol: {}", symbol->get_text()));
    }

    virtual void action() = 0;

    void set_symbol(Token* tok) {
        symbol = tok;
    }

private:
    Token* symbol;
};

class AstExpression: public AstNode {

public:
    AstExpression(ParserState* ps): AstNode(ps, AstNode::AST_EXPRESSION) {}

    virtual void traverse() override {
        ((AstNode*)expr)->traverse();
    }

    virtual void action() = 0;

    void set_expr(AstExpression* node) {
        expr = node;
    }

private:
    AstExpression* expr;
};

class AstSelect: public AstNode {

public:
    AstSelect(ParserState* ps): AstNode(ps, AstNode::AST_SELECT) {}

    virtual void traverse() override {
        for(auto node: items) {
            node->traverse();
        }
    }

    virtual void action() = 0;

    void add_item(AstNode* node) {
        items.push_back(node);
    }

private:
    vector<AstNode*> items;
};

class AstZeroOrOne: public AstNode {

public:
    AstZeroOrOne(ParserState* ps): AstNode(ps, AstNode::AST_ZERO_OR_ONE) {}

    virtual void traverse() override {
        for(auto node: items) {
            node->traverse();
        }
    }

    virtual void action() = 0;

    void add_item(AstNode* node) {
        items.push_back(node);
    }

private:
    vector<AstNode*> items;
};

class AstZeroOrMore: public AstNode {

public:
    AstZeroOrMore(ParserState* ps): AstNode(ps, AstNode::AST_ZERO_OR_MORE) {}

    virtual void traverse() override {
        for(auto node: items) {
            node->traverse();
        }
    }

    virtual void action() = 0;

    void add_item(AstNode* node) {
        items.push_back(node);
    }

private:
    vector<AstNode*> items;
};

class AstOneOrMore: public AstNode {

public:
    AstOneOrMore(ParserState* ps): AstNode(ps, AstNode::AST_ONE_OR_MORE) {}

    virtual void traverse() override {
        for(auto node: items) {
            node->traverse();
        }
    }

    virtual void action() = 0;

    void add_item(AstNode* node) {
        items.push_back(node);
    }

private:
    vector<AstNode*> items;
};

class AstNot: public AstNode {

public:
    AstNot(ParserState* ps): AstNode(ps, AstNode::AST_NOT) {}

    virtual void traverse() override {
        node->traverse();
    }

    virtual void action() = 0;

    void set_node(AstNot* no) {
        node = no;
    }

private:
    AstNode* node;
};

