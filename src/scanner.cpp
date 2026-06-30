
#include <vector>
#include <string>
#include <iostream>
#include "scanner.h"

Scanner::Scanner(const string& fname) {

    File* fp = new File(fname);
    file_queue.push_back(fp);
    Token* tok = new Token(file());
    token_queue.push_back(tok);
    crnt_index = 0;
}

File* Scanner::open_file(const string& fname) {

    File* fp = new File(fname);
    file_queue.push_back(fp);
    return fp;
}

void Scanner::close_file() {

    cerr << "scanner closing file\n";
    file_queue.pop_back();
}

// Token* Scanner::token() {

//     return crnt_token();
// }

Token* Scanner::advance() {

    if(crnt_index+1 >= token_queue.size()) {
        Token* tok = new Token(file());
        token_queue.push_back(tok);
        crnt_index = end_index();
    }
    else
        crnt_index++;

    return token();
}

int Scanner::mark_queue() {

    cout << "mark current token: " << token() << endl;
    return crnt_index;
}

void Scanner::reset_queue(size_t mark) {

    cout << "reset queue: mark: " << token_queue[mark] << endl;
    crnt_index = mark;
}

// discard matched tokens
void Scanner::flush_queue(size_t mark) {

    //cout << "flush queue: size: " << token_queue.size() << endl;
    while(token_queue.size() > mark) {
        delete token_queue[token_queue.size()-1];
        token_queue.pop_back();
    }
    //cout << "new queue size: " << token_queue.size() << endl;

    crnt_index = token_queue.size()-1;
}

File* Scanner::file() {
    return (File*)file_queue[file_queue.size()-1];
}

Token* Scanner::token() {
    return (Token*)token_queue[crnt_index];
}
