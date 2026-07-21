
#include <iostream>
#include <cstring>
#include <cerrno>
#include "fileio.h"

#include "logger.h"
extern Logger logger;
#include "errors.h"
extern Errors errors;


File::File(const string& fname)
        : fname(fname) {

    ENTER;
    TRACE(format("Opening file: {}", fname));
    file = fstream(fname, fstream::in);
    if(!file.is_open()) {
        cerr << "Error: Cannot open input file: \"" << fname << "\": " << strerror(errno) << endl;
        exit(1);
    }

    // this->fname = new string(fname);
    line_no = 1;
    col_no = 1;
    consume_char();
    RETURN();
}

File::~File() {

    ENTER;
    TRACE(format("Closing file: {}", fname));
    // cerr << "closing the file\n";
    if(file.is_open())
        file.close();
    RETURN();
}

int File::consume_char() {

    if(file.is_open()) {
        if(crnt_char == '\n') {
            line_no++;
            col_no = 1;
        }
        else
            col_no++;

        crnt_char = file.get();

        if(file.eof())
            crnt_char = EOF;
    }
    else {
        crnt_char = EOF;
        return EOF;
    }

    return crnt_char;
}