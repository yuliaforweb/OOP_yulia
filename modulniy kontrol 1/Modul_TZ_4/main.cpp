#include <iostream>
using namespace std;

// Шаблон класу Pair
template <typename T1, typename T2>
class Pair {
private:
    T1 first;
    T2 second;

public:
    // Конструктор
    Pair(T1 a, T2 b) {
        first = a;
        second = b;
    }

    // Метод для виводу
    void print() {
        cout << "Пара: (" << first << ", " << second << ")" << endl;
    }

    // Методи для доступу (за бажанням)
    T1 getFirst() { return first; }
    T2 getSecond() { return second; }
};

int main() {
    // Приклади роботи з різними типами
    Pair<int, double> p1(5, 3.14);
    p1.print();

    Pair<float, int> p2(2.5f, 10);
    p2.print();

    Pair<double, double> p3(1.1, 2.2);
    p3.print();

    return 0;
}
