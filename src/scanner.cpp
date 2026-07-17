
#include <vector>
#include <string>
#include <iostream>
#include "scanner.h"

#include "logger.h"
extern Logger logger;
#include "errors.h"
extern Errors errors;

Scanner::Scanner(const string& fname) {

    ENTER;
    //File* fp = new File(fname);
    // file_queue.push_back(fp);
    open_file(fname);

    // Token* tok = new Token(file());
    // slurp the whole file
    int count = 0;
    //Token* tok = new Token(file());
    //while(tok->get_type() != TOK_END_OF_FILE) {

    Token* tok;
    do {
        tok = new Token(file());
        token_queue.push_back(tok);
        count++;
    } while(tok->get_type() != TOK_END_OF_FILE);

    TRACE(format("read {} tokens", count));

    //close_file();
    crnt_index = 0;

    RETURN();
}

File* Scanner::open_file(const string& fname) {

    ENTER;
    File* fp = new File(fname);
    file_queue.push_back(fp);
    RETURN(fp);
}

void Scanner::close_file() {

    ENTER;
    delete file_queue.back();
    file_queue.pop_back();
    RETURN();
}

Token* Scanner::advance() {

    ENTER;
    TRACE(format("old token: {} ({})", token()->get_text(), token()->type_to_str()));
    // if(crnt_index+1 >= token_queue.size()) {
    //     Token* tok = new Token(file());
    //     token_queue.push_back(tok);
    //     crnt_index = end_index();
    // }
    // else
    //     crnt_index++;
    if(token()->get_type() != TOK_END_OF_FILE)
        crnt_index++;

    TRACE(format("new token: {} ({})", token()->get_text(), token()->type_to_str()));
    RETURN(token());
}

void Scanner::mark_queue() {

    ENTER;
    TRACE(format("mark current token: {} ({})", token()->get_text(), crnt_index));
    tracker.push_back(crnt_index);
    RETURN();
}

void Scanner::reset_queue() {

    ENTER;
    crnt_index = tracker.back();
    TRACE(format("reset queue: index: {} ({})", token()->get_text(), crnt_index));
    tracker.pop_back();
    RETURN();
}

void Scanner::flush_queue() {
    ENTER;
    tracker.pop_back();
    RETURN();
}

// discard matched tokens
// void Scanner::flush_queue() {

//     ENTER;
//     if(mark > 0) {
//         while(token_queue.size() > mark) {
//             delete token_queue[token_queue.back()];
//             token_queue.pop_back();
//         }

//         crnt_index = token_queue.back();
//         advance();
//     }
//     RETURN();
// }

File* Scanner::file() {
    return file_queue.back();
}

Token* Scanner::token() {
    return (Token*)token_queue[crnt_index];
}
