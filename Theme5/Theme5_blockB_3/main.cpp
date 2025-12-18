#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    srand(time(0)); // генератор випадкових чисел

    int arr[7][4];

    cout << "Масив:" << endl;
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 4; j++) {
            arr[i][j] = rand() % 11 - 5; // числа від -5 до 5
            cout << arr[i][j] << "\t";
        }
        cout << endl;
    }

    int maxDobutok = -1;
    int indexRow = 0;

    for (int i = 0; i < 7; i++) {
        int currentDobutok = 1;

        for (int j = 0; j < 4; j++) {
            currentDobutok *= arr[i][j];
        }

        if (currentDobutok < 0) {
            currentDobutok = -currentDobutok; // беремо модуль
        }

        if (currentDobutok > maxDobutok) {
            maxDobutok = currentDobutok;
            indexRow = i;
        }
    }

    cout << "\n Результат:" << endl;
    cout << "Індекс рядка: " << indexRow << endl;
    cout << "Найбільший добуток (по модулю): " << maxDobutok << endl;

    return 0;
}