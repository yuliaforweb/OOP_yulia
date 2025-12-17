#include <iostream>
using namespace std;

int printAge()
{
    int age_years;
    cout << "Введіть ваш вік у роках: ";
    cin >> age_years;

    int age_days = age_years * 365;
    cout << "Ваш вік: " << age_years << " років " << age_days << " днів." << endl;

    return 0;
}