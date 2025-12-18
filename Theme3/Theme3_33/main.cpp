#include <iostream>
using namespace std;

int main() {
    int n;
    cout << "Вкажіть скільки буде елементів масиву? (максимум 15) ";
    cin >> n;

    if (n <= 0 || n > 15) {
        cout << "Помилка: число елементів має бути від 1 до 15!" << endl;
        return 1;
    }

    double arr[n];

    for (int i = 0; i < n; i++) {
        if (i == 0) {
            cout << "Вкажіть перше число масиву:[" << i << "] ";
        } else if (i == 1) {
            cout << "Вкажіть друге число масиву:[" << i << "] ";
        } else {
            cout << "Вкажіть наступне число масиву[" << i << "] = ";
        }
        cin >> arr[i];
    }

    cout << "Масив: ";
    for (int i = 0; i < n; i++) cout << arr[i] << " ";
    cout << endl;

    return 0;
}