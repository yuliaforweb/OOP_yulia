#include <iostream>
#include <string>
using namespace std;

class PasswordCheck {
private:
    string valid_pass;

public:
    PasswordCheck() {
        valid_pass = "qwerty123";
    }

    void check(string password) {
        if (password == valid_pass) {
            cout << "Доступ дозволено" << endl;
        } else {
            cout << "Невірний пароль!" << endl;
        }
    }
};

int main() {
    string user_pass;

    cout << "Введіть пароль: ";
    getline(cin, user_pass);

    PasswordCheck obj;
    obj.check(user_pass);

    return 0;
}
