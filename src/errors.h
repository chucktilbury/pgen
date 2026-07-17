
#pragma once

#include <iostream>
#include <format>
#include <string>

#include "fileio.h"

using namespace std;

class Errors {

public:
    Errors() {
        errors = 0;
        warnings = 0;
    }

    void error(const string& msg) {
        cerr << "error: " << msg << endl;
        errors++;
    }

    void warning(const string& msg) {
        cerr << "warning: " << msg << endl;
        warnings++;
    }

    void error(File* file, const string& msg) {
        cerr << format("error: {}:{}:{}: {}", file->fname,
            file->line_no, file->col_no, msg) << endl;
        errors++;
    }

    void warning(File* file, const string& msg) {
        cerr << format("warning: {}:{}:{}: {}", file->fname,
            file->line_no, file->col_no, msg) << endl;
        warnings++;
    }

    int get_errors() { return errors; }
    int get_warnings() { return warnings; }

    void fatal(const string& msg) {
        cerr << "fatal error: " << msg << endl;
        exit(1);
    }

private:
    int errors;
    int warnings;

};

