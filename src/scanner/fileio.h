#pragma once

#include <fstream>
#include <string>

using namespace std;

class File {

public:
    File(const string& name);
    ~File();
    int consume_char();

    int get_char() {
        if(file.is_open())
            return crnt_char;
        else
            return EOF;
    }

    const string& get_file_name() {
        return fname;
    }

    int get_line_no() {
        return line_no;
    }

    int get_col_no() {
        return col_no;
    }

    bool is_open() {
        return file.is_open();
    }

private:
    const string& fname;
    fstream file;
    int line_no;
    int col_no;
    int crnt_char;
};

