
#pragma once

#include <iostream>
#include <map>
#include "ast.h"

#include "errors.h"
extern Errors errors;

using namespace std;

class Rule {

    public:
    Rule(_ast_rule* rule, string* str) {
        rule_ptr = rule;
        rule_name = rule->nt;
        rule_str = str;
    }

    // no reason for this to be private
    string* rule_name;
    string* rule_str;
    _ast_rule* rule_ptr;
};

class AstFormat : public AstTraverse {

    public:
    AstFormat(_ast_node* node)
            : AstTraverse(node) {
        pad = 0;
        pad_size = 2;
    }

    void store_rule(Rule* rule) {
        output.push_back(rule);
        // this assumes that the rules are added in the order in which they
        // are encountered.
        index[*rule->rule_name] = output.size() - 1;
    }

    Rule* get_rule(int index) {
        return output[index];
    }

    Rule* get_rule(string key) {
        return output[index[key]];
    }

    vector<Rule*>::iterator begin() {
        return output.begin();
    }

    vector<Rule*>::iterator end() {
        return output.end();
    }

    vector<string> get_term_list() {
        return term_list;
    }

    private:
    // each item in the output is a rule
    vector<Rule*> output;
    map<string, int> index;
    vector<string> term_list;

    virtual void _traverse_grammar(_ast_grammar* node);
    virtual void _traverse_rule(_ast_rule* node);
    virtual void _traverse_primary(_ast_primary* node);
    virtual void _traverse_group(_ast_group* node);
    virtual void _traverse_select(_ast_select* node);
    virtual void _traverse_zero_or_one(_ast_zero_or_one* node);
    virtual void _traverse_zero_or_more(_ast_zero_or_more* node);
    virtual void _traverse_one_or_more(_ast_one_or_more* node);

    void emit(string str) {
        *crnt_line += str;
    }

    void indent() {
        for(int i = 0; i < pad * pad_size; i++)
            *crnt_line += " ";
    }

    int pad;
    int pad_size;
    string* crnt_line;
};
