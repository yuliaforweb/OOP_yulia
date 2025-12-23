#include <iostream>
#include <vector>
using namespace std;

class Employer {
    vector<bool> jobs;

public:
    Employer(int n) {
        jobs.resize(n, false);
    }

    void hire() {
        for (int i = 0; i < jobs.size(); i++) {
            if (jobs[i] == false) {
                jobs[i] = true;
                cout << "Працівника прийнято на місце " << i << endl;
                return;
            }
        }
        cout << "Вакантних місць немає" << endl;
    }
};

int main() {
    Employer e(2);

    e.hire();
    e.hire();
    e.hire(); // вакансій немає

    return 0;
}
