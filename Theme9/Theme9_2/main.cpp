#include <iostream>
#include <cmath>

using namespace std;

int main() {
    double R, r;
    double S;

    cout << "Вкажіть зовнішній радіус R: ";
    cin >> R;
    cout << "Вкажіть внутрішній радіус r: ";
    cin >> r;

    S = M_PI * (R * R - r * r);

    cout << "Площа кільця: " << S << endl;

    return 0;
}
