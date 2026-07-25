
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

void Render::render_file(string infile, string outfile) {

    ofstream outf(outfile);
    if(outf.is_open()) {
        outf << *render_file(infile) << endl;
        outf.close();
    }
    else {
        cerr << format("fatal error: {}: cannot open output file: {}: {}\n",
                __PRETTY_FUNCTION__, outfile, strerror(errno));
        exit(1);
    }
}

string* Render::render_text(string text) {

    bool finished = false;
    int state;
    string outs = "";
    string key;
    int change = 1;
    string scopy = text;

    while(!finished) {
        state = 0;
        key = "";
        if(change) {
            change = 0;
            for(auto const& c: scopy) {
                switch(state) {
                    case 0:
                        if(c == '{')
                            state = 1;
                        else
                            outs += c;
                        break;

                    case 1:
                        if(c == '{')
                            state = 2;
                        else {
                            outs += '{';
                            outs += c;
                            state = 0;
                        }
                        break;

                    case 2:
                        if(c == '}')
                            state = 3;
                        else
                            key += c;
                        break;

                    case 3:
                        if(c == '}')
                            state = 4;
                        else {
                            cerr << "expected a '}' for a key: " << key << "but got a " << c << endl;
                            exit(1);
                        }
                        break;

                    case 4: {
                        RenderInstance* ri = table[key];
                        if(ri) {
                            outs += ri->rfunc();
                            //cout << outs << endl;
                            change++;
                        }
                        else {
                            outs += "{{";
                            outs += key;
                            outs += "}}";
                        }
                        outs += c;
                        key = "";
                        state = 0;
                    } break;
                }
            }
            scopy = outs;
            if(change)
                outs = "";
        }
        else
            finished = true;

    }

    return new string(outs);
}


class inst_one: public RenderInstance {

    public:
    virtual string rfunc() override { return string("this is inst_one class"); }
};

class inst_two: public RenderInstance {

    public:
    virtual string rfunc() override { return string("this is inst_two class"); }
};

class inst_three: public RenderInstance {

    public:
    virtual string rfunc() override { return string("this is inst_three class"); }
};

// class nested: public RenderInstance {

//     public:
//     virtual string rfunc() override {
//         Render rend;
//         rend.add("nothing", "aint nothing if its something");
//     }
// };

int main() {

    Render rend;

    rend.add("first", new inst_one());
    rend.add("third", new inst_three());
    rend.add("second", new inst_two());
    rend.add("nothing", "aint nothing if its {{blart}} something");
    rend.add("blart", format("\"this is a {} string\"", "good"));

    string text("The first one: {{first}}\n"
                    "but no match here {{nothing}}\n"
                    "The second one: {{second}}\n"
                    "The third one: {{third}}\n");

    string* s(rend.render_text(text));

    cout << *rend.render_file("flarp.txt") << endl;

    cout << endl << *s << endl;

    rend.render_file("flarp.txt", "flarp-out.txt");
}

