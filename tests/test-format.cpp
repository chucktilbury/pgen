
#include <format>
#include <iostream>

using namespace std;

int main () {
    string* fmt = new string("asdf");
    cout << format("string: \"{:-^10}\"", "break") << endl;
}