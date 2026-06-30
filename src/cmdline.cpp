
#include <format>
#include "cmdline.h"

using namespace std;

void CmdLine::show_help() {

    show_preamble();
    show_version();

    cout << "\nuse: " << *pname << " [args]" <<
            ((files > 0)? (files > 1)? " [list of files]" : " [file]" : "") <<
            endl << endl;

    cout << "args:" << endl;
    cout << format("{:-<75}", ' ') << endl;
    for(auto x: opts) {
        if(x->flags & CMD_DIV) {
            cout << format("{:-<75}", ' ') << endl;
        }
        else if(x->flags & CMD_ANON) {
            cout << "          " << *x->help << ((x->flags & CMD_REQD)? " (reqd)": "") << endl;
        }
        else {
            if(x->short_opt != 0)
                cout <<  *x->short_opt << " ";
            else
                cout << "   ";

            if(x->long_opt != NULL)
                cout << format("{:12} ", *x->long_opt);
            else
                cout << "             ";

            if(x->flags & CMD_LIST) {
                if(x->flags & CMD_STR)
                    cout << format("{:<14s}", "(lst of str)");
                else if(x->flags & CMD_NUM)
                    cout << format("{:<14s}", "(list of num)");
                else if(x->flags & CMD_BOOL)
                    cout << format("{:<14s}", "(list of bool)");
            }
            else {
                if(x->flags & CMD_STR)
                    cout << format("{:<14s}", "(str)");
                else if(x->flags & CMD_NUM)
                    cout << format("{:<14s}", "(num)");
                else if(x->flags & CMD_BOOL)
                    cout << format("{:<14s}", "(bool)");
                else
                    cout << format("{:<14}", ' ');
            }
            cout << *x->help << ((x->flags & CMD_REQD)? " (reqd)": "") << endl;
        }
    }
    cout << format("{:-<75}", ' ') << endl << endl;
}

void CmdLine::add(int so,
            const char* lo,
            const char* na,
            const char* he,
            const char* dv,
            uint16_t fl) {

    string* short_arg = NULL;
    string* long_arg = NULL;
    string* name = NULL;
    string* help = NULL;
    string* def_val = NULL;

    if(so != 0)
        short_arg = new string(format("-{:c}", so));
    if(lo != NULL)
        long_arg = new string(format("--{}", lo));
    if(na != NULL)
        name = new string(na);
    if(he != NULL)
        help = new string(he);
    if(dv != NULL)
        def_val = new string(dv);

    CmdLineOpt* opt = new CmdLineOpt(short_arg, long_arg, name, help, def_val, fl);
    opts.push_back(opt);
}

// thank you stack overflow!
vector<string> CmdLine::split_string(string s, string limiters) {

    vector<string> tokens;
    size_t pos = 0;
    string token;

    while((pos = s.find(limiters)) != string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + limiters.length());
    }
    tokens.push_back(s);

    return tokens;
}

void CmdLine::store_environment(char** env) {

    for(int i = 0; env[i] != NULL; i++) {
        CmdLineOpt* ptr = new CmdLineOpt();

        vector<string> tokens = split_string(env[i], "=");
        ptr->set_name(new string(tokens[0]));
        //cout << tokens[0] << endl;

        tokens = split_string(tokens[1], ":");

        for(auto x: tokens) {
            ptr->store_value(new string(x));
            //cout << "\t" << x << endl;
        }

        opts.push_back(ptr);
    }
}

string CmdLine::get_token() {

    return *token;
}

string CmdLine::advance_token() {

    ++token;
    return *token;
}

/*
    Long options can have the format of
    --option space value
    --option=value
*/
void CmdLine::parse_long_option() {

    string tok = *token;
    bool found = false;
    for(auto x: opts) {
        if(x->long_opt) {
            if(!tok.compare(0, x->long_opt->size(), string(*x->long_opt))) {
                // cout << "long opt: " << *x->long_opt << " flags: " << format("{:04X}", x->flags) << endl;
                found = true;
                x->flags |= CMD_SEEN;
                if(x->flags & CMD_HELP) {
                    show_help();
                    exit(0);
                }
                else if(x->flags & CMD_VERS) {
                    show_version();
                    exit(0);
                }
                else if(x->flags & CMD_ARGS) {
                    vector<string> val = split_string(tok, "=");
                    if(val.size() > 1) {
                        if(string(val[1]).size() > 0) {
                            if(x->flags & CMD_LIST) {
                                vector<string> t = split_string(val[1], ":");
                                for(auto v: t) {
                                    x->value.push_back(new string(v));
                                    //cout << "\t" << x << endl;
                                }
                            }
                            else
                                x->value.push_back(new string(val[1]));
                        }
                        else {
                            cerr << "error: command argument \"" << tok << "\" requires a value" << endl;
                            show_help();
                            exit(1);
                        }
                    }
                    else {
                        token++;
                        if(token != this->argv.end()) {
                            tok = *token;
                            if(tok[0] != '-') {
                                if(x->flags & CMD_LIST) {
                                    vector<string> t = split_string(tok, ":");
                                    for(auto v: t) {
                                        x->value.push_back(new string(v));
                                        //cout << "\t" << x << endl;
                                    }
                                }
                                else {
                                    x->value.push_back(new string(tok));
                                }
                            }
                            else {
                                token--;
                                tok = *token;
                                cerr << "error: command argument \"" << tok << "\" requires a value" << endl;
                                show_help();
                                exit(1);
                            }
                        }
                        else {
                            cerr << "error: command argument \"" << tok << "\" requires a value" << endl;
                            show_help();
                            exit(1);
                        }
                    }
                }

                break;
            }
        }
    }

    if(!found) {
        cerr << "error: unknown command line argument: " << *token << endl;
        show_help();
        exit(1);
    }
}

/*
    Long options can have the format of
    -o space value
    -o=value
    -ovalue
*/
void CmdLine::parse_short_option() {

    string tok = *token;
    bool found = false;

    for(auto x: opts) {
        if(x->short_opt) {
            if(!tok.compare(0, 2, string(*x->short_opt))) {
                // cout << "short opt: " << *x->short_opt << " flags: " << format("{:04X}", x->flags) << endl;
                found = true;
                x->flags |= CMD_SEEN;
                if(x->flags & CMD_HELP) {
                    show_help();
                    exit(0);
                }
                else if(x->flags & CMD_VERS) {
                    show_version();
                    exit(0);
                }
                else if(x->flags & CMD_ARGS) {
                    vector<string> val = split_string(tok, "=");
                    if(val.size() > 1) {
                        if(string(val[1]).size() > 0) {
                            if(x->flags & CMD_LIST) {
                                vector<string> t = split_string(val[1], ":");
                                for(auto v: t) {
                                    x->value.push_back(new string(v));
                                    //cout << "\t" << x << endl;
                                }
                            }
                            else {
                                x->value.push_back(new string(val[1]));
                            }
                        }
                        else {
                            cerr << "error: command argument \"" << tok << "\" requires a value" << endl;
                            show_help();
                            exit(1);
                        }
                    }
                    else if(tok.size() > 2) {
                        string val = tok.substr(2);
                        x->value.push_back(new string(val));
                    }
                    else {
                        token++;
                        if(token != this->argv.end()) {
                            tok = *token;
                            if(tok[0] != '-') {
                                if(x->flags & CMD_LIST) {
                                    vector<string> t = split_string(tok, ":");
                                    for(auto v: t) {
                                        x->value.push_back(new string(v));
                                        //cout << "\t" << x << endl;
                                    }
                                }
                                else {
                                    x->value.push_back(new string(tok));
                                }
                            }
                            else {
                                token--;
                                tok = *token;
                                cerr << "error: command argument \"" << tok << "\" requires a value" << endl;
                                show_help();
                                exit(1);
                            }
                        }
                        else {
                            cerr << "error: command argument \"" << tok << "\" requires a value" << endl;
                            show_help();
                            exit(1);
                        }
                    }
                }

                break;
            }
        }
    }


    if(!found) {
        cerr << "error: unknown command line argument: " << *token << endl;
        show_help();
        exit(1);
    }
}

// parse the command line input
void CmdLine::parse(int argc, char** argv, char** env) {

    parse(argc, argv);

    // store the environment
    store_environment(env);
    //dump_opts();
}

void CmdLine::parse(int argc, char** argv) {

    for(int i = 1; i < argc; i++)
        this->argv.push_back(string(argv[i]));
    token = this->argv.begin();

    while(token != this->argv.end()) {
        // parse the command options
        if(!token->compare(0, 2, "--")) {
            parse_long_option();
        }
        else if(!token->compare(0, 1, "-")) {
            parse_short_option();
        }
        else {
            // see if there is an anonymous option and add this to it if there is.
            bool found = false;
            for(auto x: opts) {
                if(x->flags & CMD_ANON) {
                    x->value.push_back(&(*token));
                    found = true;
                    break;
                }
            }

            if(!found) {
                cerr << "error: unknown command line argument: " << *token << endl;
                show_help();
                exit(1);
            }
        }
        token++;
    }


    // check that required options have a value
    for(auto x: opts) {
        if(x->flags & CMD_REQD && x->value.size() == 0) {
            if(x->short_opt)
                cerr << "error: command argment \"" << *x->short_opt << "\" requires a value." << endl;
            else if(x->long_opt)
                cerr << "error: command argment \"" << *x->long_opt << "\" requires a value." << endl;
            else if(x->name)
                cerr << "error: command argment \"" << *x->name << "\" requires a value." << endl;
            else
                cerr << "internal command line parser error" << endl;

            show_help();
            exit(1);
        }
    }
}

CmdLineOpt* CmdLine::find_option(const string& name) {

    for(auto x: opts) {
        if(x->name) {
            cout << "compare: " << *x->name << endl;
            if(!name.compare(string(*x->name))) {
                cout << "found: " << *x->name << endl;
                return x;
            }
        }
    }
    return nullptr;
}

// return the value of the command option according to its type
string* CmdLine::get_opt(const string& name) {

    CmdLineOpt* opt = find_option(name);
    return *opt->value.begin();
}

void CmdLine::dump_opts() {

    for(auto x: opts) {
        cout << ((x->name)? *x->name: "none") << ": ";
        cout << ((x->short_opt)? *x->short_opt: "none") << ": ";
        cout << ((x->long_opt)? *x->long_opt: "none") << ": ";
        cout << ((x->help)? *x->help: "none");

        for(auto y: x->value) {
            cout << endl << "\t" << *y;
        }

        cout << endl;
    }
}
