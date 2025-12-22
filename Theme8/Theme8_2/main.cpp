#include <iostream>
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

class Student {
public:
    string name;
    string group;
    int marks[5];

    double average() const {
        int sum = 0;
        for (int i = 0; i < 5; i++) {
            sum += marks[i];
        }
        return sum / 5.0;
    }

    bool onlyFourAndFive() const {
        for (int i = 0; i < 5; i++) {
            if (marks[i] != 4 && marks[i] != 5) {
                return false;
            }
        }
        return true;
    }
};

int main() {
    const int n = 3;
    Student students[n] = {
        {"Кучеренко І.І.", "КН-21", {5, 4, 5, 4, 5}},
        {"Петренко П.П.", "КН-22", {3, 4, 5, 4, 4}},
        {"Сидоренко С.С.", "КН-21", {4, 4, 4, 5, 4}}
    };

    sort(students, students + n, [](const Student& a, const Student& b) {
        return a.average() < b.average();
    });

    cout << "Відсортований список студентів:\n";
    for (int i = 0; i < n; i++) {
        cout << students[i].name
             << " | група: " << students[i].group
             << " | середній бал: " << students[i].average() << endl;
    }

    cout << "\nСтуденти з оцінками тільки 4 або 5:\n";
    for (int i = 0; i < n; i++) {
        if (students[i].onlyFourAndFive()) {
            cout << students[i].name
                 << " | група: " << students[i].group << endl;
        }
    }

    return 0;
}
