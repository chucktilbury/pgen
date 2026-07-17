
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <errno.h>
#include <iostream>
#include <format>

#include "ast.h"
#include "parser.h"
#include "scanner.h"
#include "cmdline.h"

#include "logger.h"
Logger logger = Logger(Logger::WARNING);

#include "errors.h"
Errors errors = Errors();

CmdLine* cmdline(int argc, char** argv, char** env) {

    CmdLine* cmd = new CmdLine("PGEN", "PGEN Parser Generator", "0.0.1", argv[0]);

    // init_cmdline("template", "template project", "0.1");
    cmd->add('v', "verbosity", "verbosity", "use: \"debug\", \"info\", \"warn\", \"silent\".", "warn", CMD_STR | CMD_ARGS);
    cmd->add('p', "path", "path", "Add to the import path", "", CMD_STR | CMD_ARGS | CMD_LIST);
    cmd->add('d', "dump", "dump", "Dump text as the parser is generated", "", CMD_STR | CMD_ARGS | CMD_LIST);
    cmd->add_help();    // name "help" is reserved
    cmd->add_version(); // name "version" is reserved
    cmd->add(0, NULL, NULL, NULL, NULL, CMD_DIV);
    cmd->add(0, NULL, "files", "File name(s) to input", NULL, CMD_REQD | CMD_ANON);

    cmd->parse(argc, argv, env);

    if(cmd->seen("verbosity")) {
        string lev = *cmd->get_string_opt("verbosity");
        //cout << "here! " << lev << endl;;
        if(!lev.compare("debug"))
            logger.set_level(Logger::DEBUG);
        else if(!lev.compare("info"))
            logger.set_level(Logger::INFO);
        else if(!lev.compare("warn"))
            logger.set_level(Logger::WARNING);
        else if(!lev.compare("silent"))
            logger.set_level(Logger::SILENT);
        else
            logger.warning(format("invalid verbosity argument: {}", lev));
    }

    if(cmd->seen("path"))
        cmd->add_path(cmd->get_opt_vector("path"));

    if(cmd->seen("PATH"))
        cmd->add_path(cmd->get_opt_vector("PATH"));

    //cmd->dump_opts();
    // INIT_TRACE(NULL);
    return(cmd);
}

int main(int argc, char** argv, char** env) {

    CmdLine* cmd = cmdline(argc, argv, env);

    ENTER; // after verbosity is init
    string filename = cmd->find_file(*cmd->get_string_opt("files"));
    ParserState* state = new ParserState(new Scanner(filename));

    AstGrammar* root = ParseGrammar(state);

    if(root != nullptr && errors.get_errors() == 0)
        root->traverse();

    RETURN(0);
}
