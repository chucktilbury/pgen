
#include <vector>
#include <string>
#include <iostream>
#include "scanner.h"

Scanner::Scanner(const string& fname) {

    File* file = new File(fname);
    file_queue.push_back(file);
    token_queue.push_back(new Token(crnt_file()));
    crnt_index = 0;
}

File* Scanner::open_file(const string& fname) {

    File* file = new File(fname);
    file_queue.push_back(file);
    return file;
}

void Scanner::close_file() {

    cerr << "scanner closing file\n";
    file_queue.pop_back();
}

Token* Scanner::token() {

    return crnt_token();
}

Token* Scanner::advance() {

    if(crnt_index+1 >= token_queue.size()) {
        Token* tok = new Token(crnt_file());
        token_queue.push_back(tok);
        crnt_index = end_index();
    }
    else
        crnt_index++;

    return crnt_token();
}

int Scanner::mark_queue() {

    return crnt_index;
}

void Scanner::reset_queue(int mark) {

    crnt_index = mark;
}

void Scanner::flush_queue() {

    while(token_queue.size() > crnt_index+1)
        token_queue.pop_back();
}

