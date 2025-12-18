#include <iostream>
using namespace std;

int main()
{
    int a;
    double b;

    // Введення значень користувачем
    cout << "Вкажіть ціле число a: ";
    cin >> a;

    cout << "Вкажіть дійсне число b: ";
    cin >> b;

    int* pInt = &a;
    double* pDouble = &b;

    cout << "\n До обміну:" << endl;
    cout << "a = " << a << endl;
    cout << "b = " << b << endl;

    // Обмін значень через вказівники
    double temp = *pInt;        // int -> double
    *pInt = (int)(*pDouble);    // double -> int
    *pDouble = temp;

    cout << "\n Після обміну:" << endl;
    cout << "a = " << a << endl;
    cout << "b = " << b << endl;

    return 0;
}
