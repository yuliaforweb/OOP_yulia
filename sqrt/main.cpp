#include <iostream>
#include "root.h"

using namespace std;

int printName();
int printAge();
int main() {

    double x(2.0);

    cout << "give your value for x: ";
    cin >> x;

    double result = root(x);

    cout << "Square root is: " << result << endl;

    printName();

    printAge();

    return 0;
}