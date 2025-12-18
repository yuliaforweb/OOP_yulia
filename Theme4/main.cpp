#include <iostream>

using namespace std;

int main() {
    int n = 11;
    int* array = new int[n]{0, 1, 3, 4, 5, 2, 1, -4, -1, 10, 55};

    cout << "Всі елементи: ";
    for (int i = 0; i < n; i++) cout << array[i] << " ";
    cout << endl;

    cout << "Зворотний порядок: ";
    for (int i = n - 1; i >= 0; i--) cout << array[i] << " ";
    cout << endl;

    cout << "Парні: ";
    for (int i = 0; i < n; i++) if (array[i] % 2 == 0) cout << array[i] << " ";
    cout << endl;

    cout << "Через один: ";
    for (int i = 0; i < n; i += 2) cout << array[i] << " ";
    cout << endl;

    cout << "До числа -1: ";
    for (int i = 0; i < n; i++) {
        if (array[i] == -1) break;
        cout << array[i] << " ";
    }
    cout << endl << endl;

    // Додаємо в кінець
    int valueToAdd = 10;
    int* newArray = new int[n + 1];
    for (int i = 0; i < n; i++) newArray[i] = array[i];
    newArray[n] = valueToAdd;
    delete[] array;
    array = newArray;
    n++;
    cout << "Додали в кінець: ";
    for (int i = 0; i < n; i++) cout << array[i] << " ";
    cout << endl;

    // Додаємо на початок
    valueToAdd = 99;
    newArray = new int[n + 1];
    newArray[0] = valueToAdd;
    for (int i = 0; i < n; i++) newArray[i + 1] = array[i];
    delete[] array;
    array = newArray;
    n++;
    cout << "Додали на початок: ";
    for (int i = 0; i < n; i++) cout << array[i] << " ";
    cout << endl;

    // Додаємо на 4-ту позицію
    valueToAdd = 777;
    int pos = 4;
    newArray = new int[n + 1];
    for (int i = 0; i < pos; i++) newArray[i] = array[i];
    newArray[pos] = valueToAdd;
    for (int i = pos; i < n; i++) newArray[i + 1] = array[i];
    delete[] array;
    array = newArray;
    n++;
    cout << "Додали на 4-ту позицію: ";
    for (int i = 0; i < n; i++) cout << array[i] << " ";
    cout << endl;

    // Видаляємо з кінця
    newArray = new int[n - 1];
    for (int i = 0; i < n - 1; i++) newArray[i] = array[i];
    delete[] array;
    array = newArray;
    n--;
    cout << "Видалили з кінця: ";
    for (int i = 0; i < n; i++) cout << array[i] << " ";
    cout << endl;

    // Видаляємо з початку
    newArray = new int[n - 1];
    for (int i = 1; i < n; i++) newArray[i - 1] = array[i];
    delete[] array;
    array = newArray;
    n--;
    cout << "Видалили з початку: ";
    for (int i = 0; i < n; i++) cout << array[i] << " ";
    cout << endl;

    // Видаляємо з 5-ї позиції
    pos = 5;
    newArray = new int[n - 1];
    int j = 0;
    for (int i = 0; i < n; i++) {
        if (i == pos) continue;
        newArray[j++] = array[i];
    }
    delete[] array;
    array = newArray;
    n--;
    cout << "Видалили з 5-ї позиції: ";
    for (int i = 0; i < n; i++) cout << array[i] << " ";
    cout << endl;

    // Об'єднання масивів
    int arr1[] = {1, 2, 3};
    int arr2[] = {4, 5, 6};
    int totalLen = 6;
    int* result = new int[totalLen];
    for (int i = 0; i < 3; i++) result[i] = arr1[i];
    for (int i = 0; i < 3; i++) result[i + 3] = arr2[i];
    cout << "Об'єднання масивів: ";
    for (int i = 0; i < totalLen; i++) cout << result[i] << " ";
    cout << endl;
    delete[] result;

    delete[] array;
    return 0;
}
