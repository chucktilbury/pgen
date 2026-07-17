
#include <iostream>
#include <vector>

using namespace std;

class ast {

public:
    void set_vec(vector<int> vec) { list.assign(vec.begin(), vec.end()); }
    void show_vec() {
        for(auto x: list)
            cout << "value: " << x << endl;
    }

private:
    vector<int> list;
};

class parser {

public:
    ast* make_ast() {
        vector<int> lst = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        ast* a = new ast();
        a->set_vec(lst);
        return a;
    }
};

int main() {

    parser* par = new parser();
    ast* a = par->make_ast();
    a->show_vec();

    return 0;
}