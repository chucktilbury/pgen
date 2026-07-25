
#include <iostream>
#include <map>

using namespace std;

class root {

    public:
    root(string n): name(n) {}
    virtual void entry_point() = 0;

    string get_name() { return name; }

    private:
    string name;
};

class class1: public root {

    public:
    class1(string name): root(name) {}

    virtual void entry_point() override {
        cout << "this is class1" << endl;
    }
};

class class2: public root {

    public:
    class2(string name): root(name) {}

    virtual void entry_point() override {
        cout << "this is class2" << endl;
    }
};

int main() {

    map<string, root*> list;

    list["first"] = new class1("first");
    list["second"] = new class2("second");

    list["first"]->entry_point();
    list["second"]->entry_point();
    list["nope"]->entry_point();

}

