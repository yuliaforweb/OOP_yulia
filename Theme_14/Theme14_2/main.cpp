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
            if (!jobs[i]) {
                jobs[i] = true;
                return;
            }
        }
    }

    void fire(const size_t i) {
        if (i >= jobs.size()) {
            cout << "Неправильний номер місця" << endl;
            return;
        }

        if (jobs[i]) {
            jobs[i] = false;
            cout << "Робоче місце " << i << " звільнено" << endl;
        } else {
            cout << "Місце вже вільне" << endl;
        }
    }
};

int main() {
    Employer e(2);

    e.hire();
    e.hire();
    e.fire(1);
    e.fire(1);
    e.fire(5);

    return 0;
}
