#include <iostream>
#include <locale>
#include <string>
using namespace std;

int main() {
    setlocale(LC_ALL, ""); // підтримка кирилиці в Linux

    wstring text;
    wcout << L"Вкажіть текст для кодування (українською або англійською): ";
    getline(wcin, text);

    for (size_t i = 0; i + 1 < text.length(); i += 2) {
        swap(text[i], text[i + 1]);
    }

    wcout << L"Закодований текст: " << text << endl;
    return 0;
}