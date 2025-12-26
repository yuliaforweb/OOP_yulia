#include <iostream>
using namespace std;

template <typename T>
void findInRange(T arr[], int n, T min, T max) {
    cout << "Елементи в діапазоні [" << min << "; " << max << "]: ";
    bool ok = false;

    for (int i = 0; i < n; i++) {
        if (arr[i] >= min && arr[i] <= max) {
            cout << arr[i] << " ";
            ok = true;
        }
    }

    if (!ok) cout << "немає";
    cout << endl;
}

int main() {
    int n;
    cout << "Вкажіть розмір масиву: ";
    cin >> n;

    int arr[100];
    cout << "Вкажіть елементи масиву ЧЕРЕЗ ПРОБІЛ:\n";
    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }

    int min, max;
    cout << "Вкажіть діапазон (min max): ";
    cin >> min >> max;

    findInRange(arr, n, min, max);

    return 0;
}
