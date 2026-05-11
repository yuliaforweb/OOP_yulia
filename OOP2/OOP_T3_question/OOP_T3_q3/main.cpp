#include <iostream>
#include <Windows.h>
using namespace std;

struct Distance {
    int meters;
    float centimeters;
};

// Функція порівнює дві відстані та повертає найбільшу
Distance maxDistance(Distance d1, Distance d2) {
    float total1 = d1.meters * 100 + d1.centimeters;
    float total2 = d2.meters * 100 + d2.centimeters;

    return (total1 > total2) ? d1 : d2;
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    Distance dist1 = {5, 45.5};
    Distance dist2 = {10, 12.0};

    Distance result = maxDistance(dist1, dist2);

    cout << "Найбільша відстань: " << result.meters << " м, "
         << result.centimeters << " см" << endl;

    return 0;
}