
#include <iostream>
#include <vector>

using namespace std;

class ast {

public:
    virtual void traverse() = 0;

};

class ast_one: public ast {

public:
    virtual void traverse() override {
        cout << __PRETTY_FUNCTION__ << endl;
        for(auto x: list) {
            x->traverse();
        }
    }

    void set_vec(vector<ast*> vec) {
        list.assign(vec.begin(), vec.end());
    }

private:
    vector<ast*> list;
};

class ast_two: public ast {

public:
    ast_two(int val): value(val) {}

    virtual void traverse() override {
        cout << __PRETTY_FUNCTION__ << ": value: " << value << endl;
    }

    void set_val(int val) {
        value = val;
    }

private:
    int value;
};

class ast_three: public ast {

public:
    ast_three(int val): value(val) {}

    virtual void traverse() override {
        cout << __PRETTY_FUNCTION__ << ": value: " << value << endl;
    }

    void set_val(int val) {
        value = val;
    }

private:
    int value;
};

class ast_four: public ast {

public:
    ast_four(int val): value(val) {}

    virtual void traverse() override {
        cout << __PRETTY_FUNCTION__ << ": value: " << value << endl;
    }

    void set_val(int val) {
        value = val;
    }

private:
    int value;
};

class ast_five: public ast {

public:
    ast_five(int val): value(val) {}

    // virtual void traverse() override {
    //     cout << __PRETTY_FUNCTION__ << ": value: " << value << endl;
    // }

    void set_val(int val) {
        value = val;
    }

private:
    int value;
};


class parser {

public:
    ast* make_ast() {
        vector<ast*> lst;
        ast_one* a = new ast_one();
        ast* x = new ast_two(10);
        lst.push_back(x);
        x = new ast_three(20);
        lst.push_back(x);
        x = new ast_four(30);
        lst.push_back(x);
        a->set_vec(lst);
        // x = new ast_five(100);
        return a;
    }
};

int main() {

    parser* par = new parser();
    ast* a = par->make_ast();
    a->traverse();

    return 0;
}