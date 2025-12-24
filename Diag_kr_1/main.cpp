#include <iostream>
using namespace std;

class coord {
    int x, y;
public:
    coord() { x = 0; y = 0; }            // конструктор за замовчуванням
    coord(int i, int j) { x = i; y = j; } // конструктор з параметрами

    // inline-функція для отримання координат
    inline void get_xy(int &i, int &j) { i = x; j = y; }

    // перевантаження оператора ==
    bool operator==(const coord &c) const {
        return (x == c.x && y == c.y);
    }
};

int main() {
    coord a;
    coord b(5, 10);
    coord c(5, 10);

    if (b == c)
        cout << "b і c однакові" << endl;
    else
        cout << "b і c різні" << endl;

    int i, j;
    a.get_xy(i, j);
    cout << "Координати a: " << i << ", " << j << endl;

    return 0;
}
