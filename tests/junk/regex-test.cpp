
#include <iostream>
#include <regex>
#include <string>

using namespace std;

int main() {

    string templ("the is {{the_template}} string {{another}}");
    cerr << "here\n";
    regex reg("\\{\\{([_a-zA-Z][a-zA-Z0-9_]+)\\}\\}");
    smatch match;

    cerr << "here\n";
    cout << regex_search(templ, match, reg) << endl;
    cout << match[1] << endl;
    for(auto const& x : match)
        cout << "\t" << x << endl;
    cerr << "here\n";

    string s = regex_replace(templ, reg, "the something unreasonable",
        regex_constants::format_first_only);
    cout << s << endl;

    cout << "plart" << endl;
    string junk("there is nothing here that matches");
    cout << regex_search(junk, match, reg) << endl;

}
