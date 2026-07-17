
#include <format>
#include <iostream>
#include <cctype>
#include "tokens.h"

#include "logger.h"
extern Logger logger;

#include "errors.h"
extern Errors errors;

using namespace std;

void Token::consume_multi_line_comment() {

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
            errors.error(file, "unexpected end of file in a comment");
            exit(1);
        }
    }
}

void Token::consume_single_line_comment() {

    bool finished = false;

    while(!finished) {
        int ch = file->consume_char();
        if(ch == '\n') {
            file->consume_char();
            finished = true;
        }
        else if(ch == EOF) {
            errors.error(file, "unexpected end of file in a comment");
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

void Token::read_symbol() {

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

void Token::read_operator() {

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
            errors.error(file, format("unexpected or unhandled operator {:c} ({:#02X})", ch, ch));
            exit(1);
    }
}

void Token::read_dquote() {

    file->consume_char(); // consume the leading '\"'
    bool finished = false;
    while(!finished) {
        int ch = file->get_char();
        if(ch == '\"') {
            file->consume_char();
            finished = true;
        }
        else if(ch == '\n') {
            errors.error(file, "unexpected newline in quoted string");
        }
        else if(ch == EOF) {
            errors.error(file, "unexpected end of file in quoted string");
            exit(1);
        }
        else {
            text += ch;
            file->consume_char();
        }
    }
    type = TOK_QSTR;
}

void Token::read_squote() {

    file->consume_char(); // consume the leading '\''
    bool finished = false;
    while(!finished) {
        int ch = file->get_char();
        if(ch == '\'') {
            file->consume_char();
            finished = true;
        }
        else if(ch == '\n') {
            errors.error(file, "unexpected newline in quoted string");
        }
        else if(ch == EOF) {
            errors.error(file, "unexpected end of file in quoted string");
            exit(1);
        }
        else {
            text += ch;
            file->consume_char();
        }
    }
    type = TOK_QSTR;
}

Token::Token(File* file) {

    this->file = file;
    bool finished = false;

    while(!finished) {
        int ch = file->get_char();
        if(ch == '/') {
            ch = file->consume_char();
            if(ch == '*')
                consume_multi_line_comment();
            else if(ch == '/')
                consume_single_line_comment();
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
            read_symbol();
            finished = true;
        }
        else if(ch == '\"') {
            read_dquote();
            finished = true;
        }
        else if(ch == '\'') {
            read_squote();
            finished = true;
        }
        else if(ispunct(ch)) {
            read_operator();
            finished = true;
        }
        else if(ch == EOF) {
            type = TOK_END_OF_FILE;
            text = "end of file";
            finished = true;
        }
        else {
            errors.warning(file, format("unexpected or unhandled character: {:c} ({:#02X})", ch, ch));
        }
    }
}

const char* Token::type_to_str() {

    return (type == TOK_END_OF_FILE)? "END_OF_FILE" :
        (type == TOK_QSTR)? "DSTR (TERMINAL)" :
        (type == TOK_OPAREN)? "OPAREN" :
        (type == TOK_CPAREN)? "CPAREN" :
        (type == TOK_STAR)? "STAR" :
        (type == TOK_PIPE)? "PIPE" :
        (type == TOK_QUESTION)? "QUESTION" :
        (type == TOK_PLUS)? "PLUS" :
        (type == TOK_BANG)? "BANG" :
        (type == TOK_TERMINAL)? "TERMINAL" :
        (type == TOK_NON_TERMINAL)? "NON_TERMINAL" : "UNKNOWN";
}

