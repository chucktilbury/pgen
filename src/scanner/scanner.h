#pragma once

#include <vector>
#include "tokens.h"

using namespace std;

class Scanner {

public:
    Scanner(const string& fname);
    File* open_file(const string& fname);
    void close_file();

    Token* token();
    Token* advance();
    int mark_queue();
    void reset_queue(int);
    void flush_queue();

private:
    vector<Token*> token_queue;
    vector<File*> file_queue;
    size_t crnt_index;

    File* crnt_file() {
        return (File*)file_queue[file_queue.size()-1];
    }

    Token* crnt_token() {
        return (Token*)token_queue[crnt_index];
    }

    int end_index() {
        return token_queue.size()-1;
    }
};

