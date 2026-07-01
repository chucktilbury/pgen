#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include <filesystem>

#include "logger.h"

using namespace std;

typedef enum cmdline_flag_t {
    // place holder
    CMD_NONE = 0,

    // needed by getarg()
    CMD_ARGS = 0x01, // args required

    // data type helpers
    CMD_STR = 0x02,    // type is a string
    CMD_NUM = 0x04,    // type is a number
    CMD_BOOL = 0x08,   // type is bool, requires "true" or "false"
    CMD_LIST = 0x10,   // a list is accepted by the arg. implies CMD_ARGS.
    CMD_ANON = 0x20,   // a list with no command option
    CMD_SWITCH = 0x40, // a switch with no command option

    // arg attributes
    CMD_REQD = 0x80, // item is required
    CMD_DIV = 0x100, // item is a divider for the help screen
    CMD_HELP = 0x200, // show help and exit
    CMD_VERS = 0x400, // show version and exit

    // internal flags, do not use
    CMD_SEEN = 0x1000,
} cmdline_flag_t;

class CmdLineOpt {

public:
    CmdLineOpt() :
                short_opt(nullptr), long_opt(nullptr),
                name(nullptr), help(nullptr), def_val(nullptr),
                flags(0) {};
// TODO: Try to use non-pointers for string values
    CmdLineOpt(string* short_opt,
                string* long_opt,
                string* name,
                string* help,
                string* def_val,
                uint16_t flags) :
                short_opt(short_opt), long_opt(long_opt),
                name(name), help(help), def_val(def_val),
                flags(flags) {}

    string* get_short_opt() { return short_opt; }
    string* get_long_opt() { return long_opt; }
    string* get_name() { return name; }
    string* get_help() { return help; }
    string* get_def_val() { return def_val; }
    uint16_t get_flags() { return flags; }

    void set_name(string* s) {
        name = s;
    }

    void store_value(string* val) {
        value.push_back(val);
    }

    vector<string*>::iterator begin() noexcept {
        return value.begin();
    }

    vector<string*>::iterator end() noexcept {
        return value.end();
    }

    string* short_opt;
    string* long_opt;
    string* name;
    string* help;
    string* def_val;
    vector<string*> value;
    uint16_t flags;
};

class CmdLine {

public:

    CmdLine(const char* name, const char* pre, const char* vers, char* pname) :
            preamble(new string(pre)), version(new string(vers)),
            name(new string(name)), pname(new string(pname)), logger(DEBUG) {
    }

    // add command option. interface to the option class
    void add(int short_opt,
                const char* long_opt,
                const char* name,
                const char* help,
                const char* def_val,
                uint16_t type);

    // parse the command line input
    void parse(int argc, char** argv, char** env);
    void parse(int, char** argv);

    // return the value of the command option according to its type
    string* get_string_opt(const string& name);
    int get_int_opt(const string& name);
    bool get_bool_opt(const string& name);
    vector<string*>& get_opt_vector(const string& name);
    bool seen(const string& name);

    void add_help() {
        add('h', "help", NULL, "Print this helpful information", NULL, CMD_HELP);
    }

    void add_version() {
        add('V', "version", NULL, "Show the program version", NULL, CMD_VERS);
    }

    // Use a path variable to locate a file in the file system. Return the
    // fully qualified name.
    string find_file(string name);
    void add_path(vector<string*> name_lst);
    void add_path(vector<string> name_lst);
    void add_dir(const string& name);
    void add_dir(string* name);
    void add_dir(const filesystem::directory_entry& name);

    void dump_opts();

private:
    string* preamble;
    string* version;
    string* name;
    string* pname;
    vector<CmdLineOpt*> opts;
    vector<string> argv;
    vector<string>::iterator token;
    vector<string> search_path;
    Logger logger;
    int files;

    void show_version() { cout << "version: " << *version << endl; }
    void show_preamble() { cout << *name << ": " << *preamble << endl; }
    void show_help();
    vector<string> split_string(string s, string limiters);
    void store_environment(char** env);
    string get_token();
    string advance_token();
    void parse_long_option();
    void parse_short_option();
    CmdLineOpt* find_option(const string& name);

    //void dump_opts();

};


