
#pragma once

#include "scanner.h"
#include "ast.h"

class Parser {

    public:
    Parser(Scanner scn)
            : scanner(scn) {
        errors = 0;
        warnings = 0;
    }
    // int get_line_no() { return scanner->file()->line_no; }
    // int get_col_no() { return scanner->file()->col_no; }
    // const string& get_file_name() { return scanner->file()->fname; }
    // Scanner* get_scanner();
    _ast_node* parse();
    int get_errors() {
        return errors;
    }
    int get_warnings() {
        return warnings;
    }

    private:
    _ast_grammar* _parse_grammar();
    _ast_rule* _parse_rule();
    _ast_primary* _parse_primary();
    _ast_select* _parse_select();
    _ast_group* _parse_group();
    _ast_zero_or_one* _parse_zero_or_one();
    _ast_zero_or_more* _parse_zero_or_more();
    _ast_one_or_more* _parse_one_or_more();

    void _error(string msg);
    void _warning(string msg);
    void _error(Token* tok, string msg);
    void _warning(Token* tok, string msg);
    void _fatal(string msg);
    string* _convert_str(string& str);

    Scanner scanner;
    int errors;
    int warnings;
};
