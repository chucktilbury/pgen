
/*
 * Text template rendering engine.
 */

#pragma once

#include <format>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cerrno>
#include <map>
//#include <regex>

using namespace std;

class RenderInstance {

    public:
    RenderInstance(string s): str(s) {}
    RenderInstance(): str("") {}

    virtual string rfunc() {
        return str;
    }

    private:
    string str;

};

class Render {

    public:
    // register the replacement text
    void add(string name, RenderInstance* rend) { table[name] = rend; }
    void add(string name, string str) { add(name, new RenderInstance(str)); }

    // render a block of text
    string* render_text(string text);

    // open a file with the text to render and output a string
    string* render_file(string fname) { return render_text(slurp(fname)); }

    // render one file to another file
    void render_file(string infile, string outfile);

    // emit a C++ comment header
    string header() {

    }

    private:
    map<string, RenderInstance*> table;
    string slurp(string fname) {
        ifstream inf(fname);
        string str;
        if(inf.is_open()) {
            ostringstream ss;
            ss << inf.rdbuf();
            str = ss.str();
        }
        else {
            cerr << format("fatal error: {}: cannot open template file: {}: {}\n",
                    __PRETTY_FUNCTION__, fname, strerror(errno));
            exit(1);
        }

        return str;
    }
};

