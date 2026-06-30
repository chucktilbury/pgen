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
    ~Token() {
        cout << "discard token: " << text << endl;
    }

    string& get_text() {
        return text;
    }

    token_type_t get_type() {
        return type;
    }

    const char* type_to_str();

    friend ostream& operator<<(ostream& os, Token* tok) {
        string fmt = format("{:>4}: {:>4}: ", tok->line, tok->col);
        os << fmt << "str: \"" << tok->get_text() << "\" type: " << tok->type_to_str();
        return os;
    }

private:
    string text;
    token_type_t type;
    File* file;
    int line;
    int col;

    void consume_multi_line_comment();
    void consume_single_line_comment();
    bool check_terminal(const string&);
    void read_symbol();
    void read_operator();
    void read_dquote();
    void read_squote();
};

