
#pragma once

#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstring>
#include "ast.h"

#include "errors.h"
extern Errors errors;

using namespace std;

class AstRegurge : public AstTraverse {

    public:
    AstRegurge(_ast_node* node)
            : AstTraverse(node) {
        pad = 0;
        pad_size = 2;
    }
    // each string in the output is a rule
    vector<string*> output;

    private:
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
