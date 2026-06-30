
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <errno.h>
#include <iostream>
#include <format>

#include "scanner.h"
#include "cmdline.h"
// #include "trace.h"

// extern FILE* yyin; // would be defined in a FLEX scanner.

CmdLine* cmdline(int argc, char** argv, char** env) {

    CmdLine* cmd = new CmdLine("PGEN", "PGEN Parser Generator", "0.0.1", argv[0]);

    // init_cmdline("template", "template project", "0.1");
    cmd->add('v', "verbosity", "verbosity", "From 0 to 10. Print more information", "0", CMD_NUM | CMD_ARGS);
    cmd->add('p', "path", "path", "Add to the import path", "", CMD_STR | CMD_ARGS | CMD_LIST);
    cmd->add('d', "dump", "dump", "Dump text as the parser is generated", "", CMD_STR | CMD_ARGS | CMD_LIST);
    cmd->add_help();    // name "help" is reserved
    cmd->add_version(); // name "version" is reserved
    cmd->add(0, NULL, NULL, NULL, NULL, CMD_DIV);
    cmd->add(0, NULL, "files", "File name(s) to input", NULL, CMD_REQD | CMD_ANON);

    cmd->parse(argc, argv, env);

    // INIT_TRACE(NULL);
    return cmd;
}

int main(int argc, char** argv, char** env) {

    CmdLine* cmd = cmdline(argc, argv, env);
    string* file = cmd->get_opt("files");

    cout << "file: " << *file << endl;

    Scanner scn{"../tests/pgen-grammar.txt"};
    int line = 1;
    //Token* tok = scn.token();

    cout << "read 5 tokens" << endl;
    for(int i = 0; i < 5; i++) {
        // string fmt = format("{:>4}:{:>4}:{:>4}: ", line, scn.file()->get_line_no(), scn.file()->get_col_no());
        // cout << fmt << scn.token() << endl;
        // tok = scn.advance();
        // line++;
        cout << scn.token() << endl;
        scn.advance();
    }

    int mark = scn.mark_queue();
    cout << "mark the queue: " << mark << endl;
    cout << "read 5 more tokens" << endl;
    for(int i = 0; i < 5; i++) {
        // string fmt = format("{:>4}:{:>4}:{:>4}: ", line, scn.file()->get_line_no(), scn.file()->get_col_no());
        // cout << fmt << scn.token() << endl;
        // tok = scn.advance();
        // line++;
        cout << scn.token() << endl;
        scn.advance();
    }
    cout << "reset the queue" << endl;
    scn.reset_queue(mark);
    cout << "read 10 more tokens" << endl;
    for(int i = 0; i < 10; i++) {
        // string fmt = format("{:>4}:{:>4}:{:>4}: ", line, scn.file()->get_line_no(), scn.file()->get_col_no());
        // cout << fmt << scn.token() << endl;
        // tok = scn.advance();
        // line++;
        cout << scn.token() << endl;
        scn.advance();
    }

    cout << "flush the queue" << endl;
    scn.flush_queue(mark);
    cout << "read 5 more tokens" << endl;
    for(int i = 0; i < 5; i++) {
        // string fmt = format("{:>4}:{:>4}:{:>4}: ", line, scn.file()->get_line_no(), scn.file()->get_col_no());
        // cout << fmt << scn.token() << endl;
        // tok = scn.advance();
        // line++;
        cout << scn.token() << endl;
        scn.advance();
    }

    return 0;

    while(scn.token()->get_type() != TOK_END_OF_FILE) {
        string fmt = format("{:>4}:{:>4}:{:>4}: ", line, scn.file()->get_line_no(), scn.file()->get_col_no());
        cout << fmt << scn.token() << endl;
        // tok = scn.advance();
        line++;
        scn.advance();
    }


    // cmdline(argc, argv, env);

    // const char* fname = raw_string(get_cmd_opt("files"));
    // if(fname != NULL) {
    //     yyin = fopen(fname, "r");
    //     if(yyin == NULL) {
    //         fprintf(stderr, "cannot open input file \"%s\": %s\n", fname, strerror(errno));
    //         cmdline_help();
    //     }
    // }
    // else
    //     FATAL("internal error in %s: command line failed", __func__);

    // printf("Hello Template: %s\n", argv[1]);

    return 0;
}
