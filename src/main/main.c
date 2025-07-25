
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "cmdline.h"
#include "trace.h"

int find_dumper(const char* name) {

    return in_cmd_list("dump", name);
}

void cmdline(int argc, char** argv, char** env) {

    init_cmdline("pgen", "FSA parser generator", "0.1");
    add_cmdline('v', "verbosity", "verbosity", "From 0 to 10. Print more information", "0", NULL, CMD_NUM | CMD_ARGS);
    add_cmdline('p', "path", "path", "Add to the import path", "", NULL, CMD_STR | CMD_ARGS | CMD_LIST);
    add_cmdline('d', "dump", "dump", "Dump text as the parser is generated", "", NULL, CMD_STR | CMD_ARGS | CMD_LIST);
    add_cmdline('h', "help", NULL, "Print this helpful information", NULL, cmdline_help, CMD_NONE);
    add_cmdline('V', "version", NULL, "Show the program version", NULL, cmdline_vers, CMD_NONE);
    add_cmdline(0, NULL, NULL, NULL, NULL, NULL, CMD_DIV);
    add_cmdline(0, NULL, "files", "File name(s) to input", NULL, NULL, CMD_REQD | CMD_ANON);

    parse_cmdline(argc, argv, env);

    INIT_TRACE(NULL);
}

int main(int argc, char** argv, char** env) {

    cmdline(argc, argv, env);

    init_parser();
    parser();

    return 0;
}
