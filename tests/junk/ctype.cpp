
#include <cctype>
#include <iostream>

using namespace std;

int main() {

    if(isalnum('_'))
        cout << "the character '_' is alpha" << endl;
    else
        cout << "the character '_' is not alpha" << endl;
}