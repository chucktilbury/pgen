
/*
 * Text template rendering engine.
 */

#ifndef _RENDER_H_
#define _RENDER_H_


#include <format>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cerrno>
#include <ctime>
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
    Render() {
        time_t ct = time(NULL);
        add("date", string(ctime(&ct)));
    }

    // register the replacement text
    void add(string name, RenderInstance* rend) { table[name] = rend; }
    void add(string name, string str) { add(name, new RenderInstance(str)); }

    // render a block of text to the output file
    string render(string text) { return render_text(text); }

    private:
    map<string, RenderInstance*> table;

    // render a block of text
    string render_text(string text);
};



#endif /* _RENDER_H_ */
