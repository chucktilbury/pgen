
#include <iostream>
using namespace std;

class base {

    public:
    void the_func() {
        cout << "this is the base" << endl;
    }

    private:
    int num;
};

class inherit_1: public base {

    public:
    inherit_1(): base() {}
    void first();
    void second();
};

class inherit_2: public inherit_1 {

    public:
    void the_func() {
        base::the_func();
        cout << "enter the func" << endl;
        first();
        second();
        cout << "leave the func" << endl;
    }

    void first() {
        cout << "first one!" << endl;
    }

    void second() {
        cout << "second one!" << endl;
    }

};

int main() {

    inherit_2 obj = inherit_2();

    obj.the_func();

    return 0;
}