#include <iostream>
#include <cmath>

using namespace std;

int main() {
    double a, b, c, P;

    cout << "Вкажіть перший катет: ";
    cin >> a;
    cout << "Вкажіть другий катет: ";
    cin >> b;

    // Аналіз існування трикутника
    if (a <= 0 || b <= 0) {
        cout << "Трикутник не існує (катети мають бути додатними)." << endl;
        return 0;
    }

    // Гіпотенуза
    c = sqrt(a * a + b * b);

    // Периметр
    P = a + b + c;

    cout << "Периметр прямокутного трикутника: " << P << endl;

    return 0;
}
