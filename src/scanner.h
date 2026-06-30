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
    File* file();
    int mark_queue();
    void reset_queue(size_t);
    void flush_queue(size_t);

private:
    vector<Token*> token_queue;
    vector<File*> file_queue;
    size_t crnt_index;

    int end_index() {
        return token_queue.size()-1;
    }
};

