#include "validation.h"


int TolkoProstieChisla(const string& prompt, int min, int max) {
    string line;
    int value;

    while (true) {
        cout << prompt;

        if (!getline(cin, line)) {
            if (cin.eof()) {
                cout << "Обнаружен конец файла (EOF)! Программа завершена.\n";
                exit(1);
            }
            if (cin.bad()) {
                cout << "Критическая ошибка потока! Невозможно восстановить.\n";
                exit(1);
            }
            cout << "Ошибка ввода! Попробуйте снова.\n";
            continue;
        }

        stringstream ss(line);
        if (!(ss >> value) || !(ss.eof())) {
            cout << "Ошибка ввода! Введите целое число.\n";
            continue;
        }

        if (value < min || value > max) {
            cout << "Ошибка: значение должно быть от " << min << " до " << max << "!\n";
            continue;
        }

        return value;
    }
}

double TolkoChisla(const string& prompt, double min) {
    string line;
    double value;

    while (true) {
        cout << prompt;

        if (!getline(cin, line)) {
            if (cin.eof()) {
                cout << "Обнаружен конец файла (EOF)! Программа завершена.\n";
                exit(1);
            }
            if (cin.bad()) {
                cout << "Критическая ошибка потока! Невозможно восстановить.\n";
                exit(1);
            }
            cout << "Ошибка ввода! Попробуйте снова.\n";
            continue;
        }

        stringstream ss(line);
        if (!(ss >> value) || !(ss.eof())) {
            cout << "Ошибка ввода! Введите число.\n";
            continue;
        }

        if (value < min) {
            cout << "Ошибка: значение должно быть не меньше " << min << "!\n";
            continue;
        }

        return value;
    }
}

bool askYesNo(const string& question) {
    string line;
    while (true) {
        cout << question << " (да->1/нет->0): ";

        if (!getline(cin, line)) {
            if (cin.eof()) {
                cout << "Обнаружен конец файла (EOF)! Программа завершена.\n";
                exit(1);
            }
            if (cin.bad()) {
                cout << "Критическая ошибка потока! Невозможно восстановить.\n";
                exit(1);
            }
            cout << "Ошибка ввода! Попробуйте снова.\n";
            continue;
        }

        if (line.empty()) {
            cout << "Пожалуйста, введите '1' или '0'.\n";
            continue;
        }

        char answer = tolower(line[0]);
        if (answer == '1') {
            cout << "Выбрано: Да\n";
            return true;
        }
        if (answer == '0') {
            cout << "Выбрано: Нет\n";
            return false;
        }

        cout << "Пожалуйста, введите '1' или '0'.\n";
    }
}

