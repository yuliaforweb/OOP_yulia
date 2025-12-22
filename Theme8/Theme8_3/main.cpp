#include <iostream>
#include <cmath>

using namespace std;

class Complex {
private:
    double re;
    double im;

public:
    Complex(double r = 0, double i = 0) : re(r), im(i) {}

    // Додавання
    Complex operator+(const Complex& other) const {
        return Complex(re + other.re, im + other.im);
    }

    // Віднімання
    Complex operator-(const Complex& other) const {
        return Complex(re - other.re, im - other.im);
    }

    // Множення
    Complex operator*(const Complex& other) const {
        return Complex(
            re * other.re - im * other.im,
            re * other.im + im * other.re
        );
    }

    // Ділення
    Complex operator/(const Complex& other) const {
        double denom = other.re * other.re + other.im * other.im;
        return Complex(
            (re * other.re + im * other.im) / denom,
            (im * other.re - re * other.im) / denom
        );
    }

    // Спряжене число
    Complex conjugate() const {
        return Complex(re, -im);
    }

    // Піднесення до цілого степеня
    Complex power(int n) const {
        double r = sqrt(re * re + im * im);
        double phi = atan2(im, re);

        double r_pow = pow(r, n);
        double phi_mul = phi * n;

        return Complex(
            r_pow * cos(phi_mul),
            r_pow * sin(phi_mul)
        );
    }

    // Квадратний корінь
    Complex sqrtComplex() const {
        double r = sqrt(re * re + im * im);
        return Complex(
            sqrt((r + re) / 2),
            (im >= 0 ? 1 : -1) * sqrt((r - re) / 2)
        );
    }

    void print() const {
        cout << re;
        if (im >= 0) cout << " + " << im << "i";
        else cout << " - " << -im << "i";
        cout << endl;
    }
};

int main() {
    Complex a(3, 4);
    Complex b(1, -2);

    cout << "a = "; a.print();
    cout << "b = "; b.print();

    cout << "a + b = "; (a + b).print();
    cout << "a - b = "; (a - b).print();
    cout << "a * b = "; (a * b).print();
    cout << "a / b = "; (a / b).print();

    cout << "Спряжене a = "; a.conjugate().print();
    cout << "a^3 = "; a.power(3).print();
    cout << "sqrt(a) = "; a.sqrtComplex().print();

    return 0;
}
