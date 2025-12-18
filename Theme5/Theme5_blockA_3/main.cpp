#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    srand(time(0)); // генератор випадкових чисел

    int arr[15];
    int count = 0;

    cout << "Масив: ";
    for (int i = 0; i < 15; i++) {
        arr[i] = rand() % 10; // числа від 0 до 9
        cout << arr[i] << " ";
    }
    cout << endl;

    count = 0;
    for (int i = 0; i < 15; i++) {
        if (arr[i] % 2 == 0) {
            count++;
        }
    }

    cout << "Кількість парних елементів: " << count << endl;

    return 0;
}
