
#include <iostream>
#include "scanner.h"

#include "logger.h"
extern Logger logger;

class ParserState {

public:
    ParserState(Scanner* scn) : scanner(scn) {}
    int get_line_no() { return scanner->file()->get_line_no(); }
    int get_col_no() { return scanner->file()->get_col_no(); }
    const string& get_file_name() { return scanner->file()->get_file_name(); }

private:
    Scanner* scanner;
};

