#include <iostream>
#include <cmath>

using namespace std;

int main() {
    double S;
    double a;
    double P;

    cout << "Вкажіть площу квадрата: ";
    cin >> S;

    a = sqrt(S);      // сторона квадрата
    P = 4 * a;        // периметр

    cout << "Периметр квадрата: " << P << endl;

    return 0;
}