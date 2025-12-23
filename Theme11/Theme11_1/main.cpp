#include <iostream>
#include <string>

using namespace std;

void printValue(int x) {
    cout << "Ціле число: " << x << endl;
}

void printValue(double x) {
    cout << "Дробове число: " << x << endl;
}

void printValue(string x) {
    cout << "Рядок: " << x << endl;
}

int main() {
    printValue(10);
    printValue(3.14);
    printValue("Тестова робота");

    return 0;
}