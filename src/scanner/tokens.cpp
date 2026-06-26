
#include <format>
#include <iostream>
#include <cctype>
#include "tokens.h"

using namespace std;

void Token::consume_multi_line_comment(File* file) {

    bool finished = false;

    while(!finished) {
        int ch = file->consume_char();
        if(ch == '*') {
            ch = file->consume_char();
            if(ch == '/') {
                file->consume_char();
                finished = true;
            }
        }
        else if(ch == EOF) {
            cerr << "error: " << file->get_file_name() <<
                    ": " << file->get_line_no() <<
                    ": " << file->get_col_no() <<
                    ": " << "unexpected end of file in a comment" <<
                    endl;
            exit(1);
        }
    }
}

void Token::consume_single_line_comment(File* file) {

    bool finished = false;

    while(!finished) {
        int ch = file->consume_char();
        if(ch == '\n') {
            file->consume_char();
            finished = true;
        }
        else if(ch == EOF) {
            cerr << "error: " << file->get_file_name() <<
                    ": " << file->get_line_no() <<
                    ": " << file->get_col_no() <<
                    ": " << "unexpected end of file in a comment" <<
                    endl;
            exit(1);
        }
    }
}

bool Token::check_terminal(const string& text) {

    for(auto &ch : text)
        if(islower(ch))
            return false;

    return true;
}

void Token::read_symbol(File* file) {

    bool finished = false;

    while(!finished) {
        int ch = file->get_char();
        if(isalpha(ch) || ch == '_') {
            file->consume_char();
            text += ch;
        }
        else
            finished = true;
    }

    if(text.length() > 0) {
        if(check_terminal(text))
            type = TOK_TERMINAL;
        else
            type = TOK_NON_TERMINAL;
    }
    else
        type = TOK_END_OF_FILE;
}

void Token::read_operator(File* file) {

    int ch = file->get_char();
    switch(ch) {
        case '(':
            text += ch;
            file->consume_char();
            type = TOK_OPAREN;
            break;
        case ')':
            text += ch;
            file->consume_char();
            type = TOK_CPAREN;
            break;
        case '{':
            text += ch;
            file->consume_char();
            type = TOK_OCURLY;
            break;
        case '}':
            text += ch;
            file->consume_char();
            type = TOK_CCURLY;
            break;
        case '?':
            text += ch;
            file->consume_char();
            type = TOK_QUESTION;
            break;
        case '|':
            text += ch;
            file->consume_char();
            type = TOK_PIPE;
            break;
        case '+':
            text += ch;
            file->consume_char();
            type = TOK_PLUS;
            break;
        case '*':
            text += ch;
            file->consume_char();
            type = TOK_STAR;
            break;
        case '!':
            text += ch;
            file->consume_char();
            type = TOK_BANG;
            break;
        default:
            cerr << "error: " << file->get_file_name() <<
                    ": " << file->get_line_no() <<
                    ": " << file->get_col_no() <<
                    ": " << "unexpected or unhandled operator" <<
                    ": " << format("{:c} ({:#02X})", ch, ch) <<
                    endl;
            exit(1);
    }
}

void Token::read_dquote(File* file) {

    file->consume_char(); // consume the leading '\"'
    bool finished = false;
    while(!finished) {
        int ch = file->get_char();
        if(ch == '\"') {
            file->consume_char();
            finished = true;
        }
        else if(ch == '\n') {
            cerr << "error: " << file->get_file_name() <<
                    ": " << file->get_line_no() <<
                    ": " << file->get_col_no() <<
                    ": " << "unexpected newline in quoted string" <<
                    endl;
            exit(1);
        }
        else if(ch == EOF) {
            cerr << "error: " << file->get_file_name() <<
                    ": " << file->get_line_no() <<
                    ": " << file->get_col_no() <<
                    ": " << "unexpected end of file in quoted string" <<
                    endl;
            exit(1);
        }
        else {
            text += ch;
            file->consume_char();
        }
    }
    type = TOK_DSTR;
}

void Token::read_squote(File* file) {

    file->consume_char(); // consume the leading '\''
    bool finished = false;
    while(!finished) {
        int ch = file->get_char();
        if(ch == '\'') {
            file->consume_char();
            finished = true;
        }
        else if(ch == '\n') {
            cerr << "error: " << file->get_file_name() <<
                    ": " << file->get_line_no() <<
                    ": " << file->get_col_no() <<
                    ": " << "unexpected newline in quoted string" <<
                    endl;
            exit(1);
        }
        else if(ch == EOF) {
            cerr << "error: " << file->get_file_name() <<
                    ": " << file->get_line_no() <<
                    ": " << file->get_col_no() <<
                    ": " << "unexpected end of file in quoted string" <<
                    endl;
            exit(1);
        }
        else {
            text += ch;
            file->consume_char();
        }
    }
    type = TOK_SSTR;
}

Token::Token(File* file) {

    bool finished = false;

    while(!finished) {
        int ch = file->get_char();
        if(ch == '/') {
            ch = file->consume_char();
            if(ch == '*')
                consume_multi_line_comment(file);
            else if(ch == '/')
                consume_single_line_comment(file);
            else {
                text += '/';
                text += ch;
                // note that this is probably a syntax error
            }
        }
        else if(isspace(ch)) {
            file->consume_char();
        }
        // pgen grammar does not have numbers.
        // else if(isdigit(ch)) {
        // }
        else if(isalpha(ch) || ch == '_') {
            read_symbol(file);
            finished = true;
        }
        else if(ch == '\"') {
            read_dquote(file);
            finished = true;
        }
        else if(ch == '\'') {
            read_squote(file);
            finished = true;
        }
        else if(ispunct(ch)) {
            read_operator(file);
            finished = true;
        }
        else if(ch == EOF) {
            type = TOK_END_OF_FILE;
            finished = true;
        }
        else {
            cerr << "warning: " << file->get_file_name() <<
                    ": " << file->get_line_no() <<
                    ": " << file->get_col_no() <<
                    ": " << "unexpected or unhandled character" <<
                    ": " << format("{:c} ({:#02X})", ch, ch) <<
                    endl;
        }
    }
}

const char* Token::type_to_str() {

    return (type == TOK_END_OF_FILE)? "END_OF_FILE" :
        (type == TOK_DSTR)? "DSTR (TERMINAL)" :
        (type == TOK_SSTR)? "SSTR (TERMINAL)" :
        (type == TOK_OPAREN)? "OPAREN" :
        (type == TOK_CPAREN)? "CPAREN" :
        (type == TOK_OCURLY)? "OCURLY" :
        (type == TOK_CCURLY)? "CCURLY" :
        (type == TOK_STAR)? "STAR" :
        (type == TOK_PIPE)? "PIPE" :
        (type == TOK_QUESTION)? "QUESTION" :
        (type == TOK_PLUS)? "PLUS" :
        (type == TOK_BANG)? "BANG" :
        (type == TOK_TERMINAL)? "TERMINAL" :
        (type == TOK_NON_TERMINAL)? "NON_TERMINAL" : "UNKNOWN";
}

