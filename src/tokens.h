#ifndef _TOKENS_H_
#define _TOKENS_H_

#include "fileio.h"

#include "logger.h"
extern Logger logger;

using namespace std;

typedef enum {
    TOK_END_OF_FILE,
    TOK_QSTR,
    TOK_OPAREN,
    TOK_CPAREN,
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
        TRACE(format("discard token: {}", text));
    }

    string& get_text() {
        return text;
    }

    token_type_t get_type() {
        return type;
    }

    const char* type_to_str();

    friend ostream& operator<<(ostream& os, Token* tok) {
        string fmt = format("{:>4}: {:>4}: ", tok->file->line_no, tok->file->col_no);
        os << fmt << "str: \"" << tok->get_text() << "\" type: " << tok->type_to_str();
        return os;
    }

    int line_no;
    int col_no;
    string* fname;
    File* file;

    private:
    string text;
    token_type_t type;

    void consume_multi_line_comment();
    void consume_single_line_comment();
    bool check_terminal(const string&);
    void read_symbol();
    void read_operator();
    void read_dquote();
    void read_squote();
};


#endif /* _TOKENS_H_ */
