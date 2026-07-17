
#pragma once

#include <iostream>
#include "scanner.h"
#include "ast.h"

class ParserState {

public:
    ParserState(Scanner* scn) : scanner(scn) {}
    int get_line_no() { return scanner->file()->line_no; }
    int get_col_no() { return scanner->file()->col_no; }
    const string& get_file_name() { return scanner->file()->fname; }

    Scanner* scanner;
};

AstGrammar* ParseGrammar(ParserState* state);
AstRule* ParseRule(ParserState* state);
AstPrimary* ParsePrimary(ParserState* state);
AstSelect* ParseSelect(ParserState* state);
AstGrouping* ParseGroup(ParserState* state);
AstZeroOrOne* ParseZeroOrOne(ParserState* state);
AstZeroOrMore* ParseZeroOrMore(ParserState* state);
AstOneOrMore* ParseOneOrMore(ParserState* state);
AstNot* ParseNot(ParserState* state);

