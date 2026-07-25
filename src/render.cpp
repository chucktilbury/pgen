
#include "render.h"

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
                            cerr << format("template error: {} expected a '}' for the key: {} but got a {}\n",
                                        __PRETTY_FUNCTION__, key, c) << endl;
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

