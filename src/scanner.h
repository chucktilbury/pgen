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
    void mark_queue();
    void reset_queue();
    void flush_queue();

private:
    vector<Token*> token_queue;
    vector<File*> file_queue;
    vector<int> tracker;
    int crnt_index;

};

