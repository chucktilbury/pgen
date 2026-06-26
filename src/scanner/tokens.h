#pragma once

#include "fileio.h"

using namespace std;

typedef enum {
    TOK_END_OF_FILE,
    TOK_DSTR,
    TOK_SSTR,
    TOK_OPAREN,
    TOK_CPAREN,
    TOK_OCURLY,
    TOK_CCURLY,
    TOK_STAR,
    TOK_PIPE,
    TOK_QUESTION,
    TOK_PLUS,
    TOK_BANG,
    TOK_TERMINAL,
    TOK_NON_TERMINAL,
} token_type_t;

class Token {

public:
    Token(File*);

    string& get_text() {
        return text;
    }

    token_type_t get_type() {
        return type;
    }

    const char* type_to_str();
    friend ostream& operator<<(ostream& os, Token* tok) {
        os << "str: \"" << tok->text << "\" type: " << tok->type_to_str();
        return os;
    }

private:
    string text;
    token_type_t type;

    void consume_multi_line_comment(File*);
    void consume_single_line_comment(File*);
    bool check_terminal(const string&);
    void read_symbol(File*);
    void read_operator(File*);
    void read_dquote(File*);
    void read_squote(File*);
};

