#include <iostream>
#include "validation.h"
#include <string>
#include <vector>
#include <iomanip>
#include <conio.h>
#include <Windows.h>
#include "Database.h"
#include "User.h"
#include "Apartment.h"

using namespace std;

Database db;
User* currentUser = nullptr;

void mainMenu();
void registerUser();
void loginUser();
void viewProfile();
void checkBalance();
void topUpBalance();
void viewOwnedApartments();
void buyApartment();
void sellApartment();
void displayApartment(const Apartment& a);

void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void displayAuthMenu(int selected) {
    system("cls");
    string options[] = {
        "1. Регистрация",
        "2. Вход в аккаунт",
        "3. Выход"
    };

    for (int i = 0; i < 3; i++) {
        if (i == selected) {
            if (i == 0) {
                setColor(13); cout << "> "; setColor(215);
            }
            else if (i == 1) {
                setColor(2); cout << "> "; setColor(47);
            }
            else {
                setColor(12); cout << "> "; setColor(207);
            }
            cout << options[i];
            setColor(i == 0 ? 13 : (i == 1 ? 2 : 12));
            cout << " <" << endl;
            setColor(7);
        }
        else {
            cout << "  " << options[i] << endl;
        }
    }
}

void displayMainMenu(int selected) {
    system("cls");
    string options[] = {
        "1. Подобрать квартиру для покупки",
        "2. Выставить квартиру на продажу",
        "3. Профиль",
        "4. Выйти из аккаунта"
    };

    for (int i = 0; i < 4; i++) {
        if (i == selected) {
            setColor(9);
            cout << "> ";
            setColor(159);
            cout << options[i];
            setColor(9);
            cout << " <" << endl;
            setColor(7);
        }
        else {
            cout << "  " << options[i] << endl;
        }
    }
}

void mainMenu() {
    int selected = 0;
    char key;

    while (true) {
        displayMainMenu(selected);
        key = _getch();

        if (key == 72) {
            selected = (selected - 1 + 4) % 4;
        }
        else if (key == 80) {
            selected = (selected + 1) % 4;
        }
        else if (key == 13) {
            switch (selected) {
            case 0: buyApartment(); break;
            case 1: sellApartment(); break;
            case 2: viewProfile(); break;
            case 3:
                currentUser = nullptr;
                return;
            }
        }
    }
}

void displayProfileMenu(int selected) {
    system("cls");
    string options[] = {
        "1. Проверить баланс",
        "2. Пополнить баланс",
        "3. Просмотреть имеющиеся квартиры",
        "4. Назад"
    };

    for (int i = 0; i < 4; i++) {
        if (i == selected) {
            setColor(2);
            cout << "> ";
            setColor(47);
            cout << options[i];
            setColor(2);
            cout << " <" << endl;
            setColor(7);
        }
        else {
            cout << "  " << options[i] << endl;
        }
    }
}

void viewProfile() {
    int selected = 0;
    char key;

    while (true) {
        displayProfileMenu(selected);
        key = _getch();

        if (key == 72) {
            selected = (selected - 1 + 4) % 4;
        }
        else if (key == 80) {
            selected = (selected + 1) % 4;
        }
        else if (key == 13) {
            switch (selected) {
            case 0: checkBalance(); system("pause"); break;
            case 1: topUpBalance(); system("pause"); break;
            case 2: viewOwnedApartments(); system("pause"); break;
            case 3: return;
            }
        }
    }
}

void displayBuyMainChoice(int selected) {
    system("cls");
    string options[] = {
        "1. Показать все квартиры",
        "2. Подобрать по параметрам",
        "3. Назад"
    };

    for (int i = 0; i < 3; i++) {
        if (i == selected) {
            setColor(13);
            cout << "> ";
            setColor(215);
            cout << options[i];
            setColor(13);
            cout << " <" << endl;
            setColor(7);
        }
        else {
            cout << "  " << options[i] << endl;
        }
    }
}

void displayFilterMenu(int selected) {
    system("cls");
    string options[] = {
        "1. Тип жилья",
        "2. Цена за м2",
        "3. Общая цена",
        "4. Количество комнат",
        "5. Площадь",
        "6. Готово — показать результаты",
        "7. Отмена"
    };
    setColor(111);
    cout << "<-_-_-> Настройка фильтров <-_-_->\n\n";
    setColor(7);

    for (int i = 0; i < 7; i++) {
        if (i == selected) {
            setColor(11);
            cout << "> ";
            setColor(159);
            cout << options[i];
            setColor(11);
            cout << " <" << endl;
            setColor(7);
        }
        else {
            cout << "  " << options[i] << endl;
        }
    }
}

void buyApartment() {
    auto allApts = db.getAllApartments();
    if (allApts.empty()) {
        cout << "\nНет доступных квартир.\n";
        system("pause");
        return;
    }

    int mainChoice = 0;
    char key;

    while (true) {
        displayBuyMainChoice(mainChoice);
        key = _getch();

        if (key == 72) {
            mainChoice = (mainChoice - 1 + 3) % 3;
        }
        else if (key == 80) {
            mainChoice = (mainChoice + 1) % 3;
        }
        else if (key == 13) {
            if (mainChoice == 0) {
                system("cls");
                setColor(111);
                cout << "<-_-_-> Все доступные квартиры <-_-_->\n";
                setColor(7);
                for (const auto& a : allApts) {
                    displayApartment(a);
                }
                cout << "\nВведите ID квартиры для покупки (или 0 для отмены): ";
                int id;
                cin >> id;
                if (id != 0) {
                    Apartment* apt = db.getApartmentById(id);
                    if (!apt) {
                        cout << "Квартира не найдена!\n";
                    }
                    else if (currentUser->getBalance() < apt->getPrice()) {
                        cout << "Недостаточно средств! Баланс: $" << currentUser->getBalance() << "\n";
                    }
                    else if (db.buyApartment(id, currentUser->getId(), apt->getPrice())) {
                        currentUser->setBalance(currentUser->getBalance() - apt->getPrice());
                        db.updateUserBalance(currentUser->getId(), currentUser->getBalance());
                        currentUser->setOwnedApartmentIds(db.getUserApartmentIds(currentUser->getId()));
                        cout << "Квартира успешно куплена!\n";
                    }
                    else {
                        cout << "Ошибка при покупке!\n";
                    }
                }
                system("pause");
                return;
            }
            else if (mainChoice == 1) {
                break;
            }
            else {
                return;
            }
        }
    }

    struct Filters {
        bool useType = false;
        bool isNew = false;

        bool usePricePerSq = false;
        double minPPM = 0.0, maxPPM = 0.0;

        bool useTotalPrice = false;
        double minTotal = 0.0, maxTotal = 0.0;

        bool useRooms = false;
        int minRooms = 0, maxRooms = 0;

        bool useArea = false;
        double minArea = 0.0, maxArea = 0.0;
    } filters;

    int filterChoice = 0;
    bool done = false;

    while (!done) {
        system("cls");
        setColor(111);
        cout << "<-_-_-> Настройка фильтров <-_-_->\n";
        setColor(7);
        cout << "Активные фильтры:\n";
        if (filters.useType) {
            cout << "  • Тип: " << (filters.isNew ? "Новостройка" : "Вторичка") << "\n";
        }
        if (filters.usePricePerSq) {
            cout << "  • Цена за м2: $" << filters.minPPM << " – $" << filters.maxPPM << "\n";
        }
        if (filters.useTotalPrice) {
            cout << "  • Общая цена: $" << filters.minTotal << " – $" << filters.maxTotal << "\n";
        }
        if (filters.useRooms) {
            cout << "  • Комнат: " << filters.minRooms << " – " << filters.maxRooms << "\n";
        }
        if (filters.useArea) {
            cout << "  • Площадь: " << filters.minArea << " – " << filters.maxArea << " м2\n";
        }
        if (!filters.useType && !filters.usePricePerSq && !filters.useTotalPrice && !filters.useRooms && !filters.useArea) {
            cout << "  (пока не задано ни одного фильтра)\n";
        }
        cout << "\n";

        string options[] = {
            "1. Тип жилья",
            "2. Цена за м2",
            "3. Общая цена",
            "4. Количество комнат",
            "5. Площадь",
            "6. Готово — показать результаты",
            "7. Отмена"
        };

        for (int i = 0; i < 7; i++) {
            if (i == filterChoice) {
                setColor(11);
                cout << "> ";
                setColor(159);
                cout << options[i];
                setColor(11);
                cout << " <" << endl;
                setColor(7);
            }
            else {
                cout << "  " << options[i] << endl;
            }
        }

        char key = _getch();
        if (key == 72) {
            filterChoice = (filterChoice - 1 + 7) % 7;
        }
        else if (key == 80) {
            filterChoice = (filterChoice + 1) % 7;
        }
        else if (key == 13) {
            switch (filterChoice) {
            case 0: {
                system("cls");
                setColor(111);
                cout << "<-_-_-> Тип жилья <-_-_->\n";
                setColor(7);
                int type;
                cout << "1 — Новостройка\n";
                cout << "0 — Вторичка\n";
                cout << "Ваш выбор: ";
                cin >> type;
                filters.isNew = (type == 1);
                filters.useType = true;
                break;
            }
            case 1: {
                system("cls");
                setColor(111);
                cout << "<-_-_-> Цена за квадратный метр <-_-_->\n";
                setColor(7);
                cout << "Минимум ($): "; double minv = TolkoProstieChisla("");
                cout << "Максимум ($): "; double maxv = TolkoProstieChisla("");
                if (minv > maxv) {
                    cout << "\n Ошибка: минимум не может быть больше максимума!\n";
                    system("pause");
                }
                else {
                    filters.minPPM = minv;
                    filters.maxPPM = maxv;
                    filters.usePricePerSq = true;
                }
                break;
            }
            case 2: {
                system("cls");
                setColor(111);
                cout << "<-_-_-> Общая цена квартиры <-_-_->\n";
                setColor(7);
                cout << "Минимум ($): "; double minv = TolkoProstieChisla("");
                cout << "Максимум ($): "; double maxv = TolkoProstieChisla("");
                if (minv > maxv) {
                    cout << "\n Ошибка: минимум не может быть больше максимума!\n";
                    system("pause");
                }
                else {
                    filters.minTotal = minv;
                    filters.maxTotal = maxv;
                    filters.useTotalPrice = true;
                }
                break;
            }
            case 3: {
                system("cls");
                setColor(111);
                cout << "<-_-_-> Количество комнат <-_-_->\n";
                setColor(7);
                cout << "Минимум: "; int minv = TolkoProstieChisla("");
                cout << "Максимум: "; int maxv = TolkoProstieChisla("");
                if (minv > maxv) {
                    cout << "\n Ошибка: минимум не может быть больше максимума!\n";
                    system("pause");
                }
                else {
                    filters.minRooms = minv;
                    filters.maxRooms = maxv;
                    filters.useRooms = true;
                }
                break;
            }
            case 4: {
                system("cls");
                setColor(111);
                cout << "<-_-_-> Площадь квартиры <-_-_->\n";
                setColor(7);
                cout << "Минимум (м2): "; double minv = TolkoChisla("");
                cout << "Максимум (м2): "; double maxv = TolkoChisla("");
                if (minv > maxv) {
                    cout << "\n Ошибка: минимум не может быть больше максимума!\n";
                    system("pause");
                }
                else {
                    filters.minArea = minv;
                    filters.maxArea = maxv;
                    filters.useArea = true;
                }
                break;
            }
            case 5: {
                done = true;
                break;
            }
            case 6: {
                return;
            }
            }
        }
    }

    vector<Apartment> candidates;
    for (const auto& a : allApts) {
        bool match = true;

        if (filters.useType && a.getIsNewBuilding() != filters.isNew) match = false;
        if (filters.usePricePerSq) {
            double ppm = a.getPricePerSqMeter();
            if (ppm < filters.minPPM || ppm > filters.maxPPM) match = false;
        }
        if (filters.useTotalPrice) {
            if (a.getPrice() < filters.minTotal || a.getPrice() > filters.maxTotal) match = false;
        }
        if (filters.useRooms) {
            if (a.getRooms() < filters.minRooms || a.getRooms() > filters.maxRooms) match = false;
        }
        if (filters.useArea) {
            if (a.getArea() < filters.minArea || a.getArea() > filters.maxArea) match = false;
        }

        if (match) candidates.push_back(a);
    }

    system("cls");
    if (candidates.empty()) {
        cout << "По вашим фильтрам ничего не найдено.\n";
    }
    else {
        setColor(111);
        cout << "<-_-_-> Найдено " << candidates.size() << " подходящих квартир <-_-_->\n";
        setColor(7);
        for (const auto& a : candidates) {
            displayApartment(a);
        }
        cout << "\nВведите ID квартиры для покупки (или 0 для отмены): ";
        int id;
        cin >> id;
        if (id != 0) {
            Apartment* apt = db.getApartmentById(id);
            if (!apt) {
                cout << "Квартира не найдена!\n";
            }
            else if (currentUser->getBalance() < apt->getPrice()) {
                cout << "Недостаточно средств! Баланс: $" << currentUser->getBalance() << "\n";
            }
            else if (db.buyApartment(id, currentUser->getId(), apt->getPrice())) {
                currentUser->setBalance(currentUser->getBalance() - apt->getPrice());
                db.updateUserBalance(currentUser->getId(), currentUser->getBalance());
                currentUser->setOwnedApartmentIds(db.getUserApartmentIds(currentUser->getId()));
                cout << "Квартира успешно куплена!\n";
            }
            else {
                cout << "Ошибка при покупке!\n";
            }
        }
    }
    system("pause");
}

void displaySellMenu(int selected) {
    system("cls");
    string options[] = {
        "1. Выставить свою квартиру",
        "2. Создать новое объявление",
        "3. Назад"
    };

    for (int i = 0; i < 3; i++) {
        if (i == selected) {
            setColor(13);
            cout << "> ";
            setColor(215);
            cout << options[i];
            setColor(13);
            cout << " <" << endl;
            setColor(7);
        }
        else {
            cout << "  " << options[i] << endl;
        }
    }
}

void sellApartment() {
    int selected = 0;
    char key;

    while (true) {
        displaySellMenu(selected);
        key = _getch();

        if (key == 72) {
            selected = (selected - 1 + 3) % 3;
        }
        else if (key == 80) {
            selected = (selected + 1) % 3;
        }
        else if (key == 13) {
            if (selected == 0) {
                auto myApts = db.getApartmentsByOwner(currentUser->getId());
                system("cls");
                if (myApts.empty()) {
                    cout << "У вас нет квартир для продажи.\n";
                }
                else {
                    setColor(111);
                    cout << ">>>>> Ваши квартиры <<<<<\n";
                    setColor(7);
                    for (const auto& a : myApts) {
                        displayApartment(a);
                    }
                    cout << "\nВсе ваши квартиры уже публично доступны для покупки!\n";
                }
                system("pause");
                return;
            }
            else if (selected == 1) {
                Apartment a;
                a.setOwnerId(currentUser->getId());

                system("cls");
                setColor(111);
                cout << "<-_-_-> Новое объявление о продаже <-_-_->\n";
                setColor(7);
                cout << "Цена ($): "; a.setPrice(TolkoChisla(""));
                cout << "Площадь (м2): "; a.setArea(TolkoChisla(""));
                cout << "Комнат: "; a.setRooms(TolkoProstieChisla(""));
                cout << "Новостройка (1) или вторичка (0)? ";
                int type; cin >> type; a.setIsNewBuilding(type == 1);
                cout << "Год постройки: "; cin >> type; a.setYearBuilt(type);
                cout << "Этаж: "; cin >> type; a.setFloor(type);
                cin.ignore();
                cout << "Адрес: ";
                string addr; getline(cin, addr);
                a.setAddress(addr);

                if (db.addApartment(a)) {
                    cout << "\n Объявление успешно создано!\n";
                }
                else {
                    cout << "\n Ошибка создания объявления!\n";
                }
                system("pause");
                return;
            }
            else {
                return;
            }
        }
    }
}

void displayApartment(const Apartment& a) {
    cout << "ID: " << a.getId()
        << " | Адрес: " << a.getAddress()
        << " | Цена: $" << fixed << setprecision(2) << a.getPrice()
        << " | Цена/м2: $" << a.getPricePerSqMeter()
        << " | Площадь: " << a.getArea() << " м2"
        << " | Комнат: " << a.getRooms()
        << " | " << (a.getIsNewBuilding() ? "Новостройка" : "Вторичка")
        << " | Этаж: " << a.getFloor()
        << " | Год: " << a.getYearBuilt() << "\n";
}

void checkBalance() {
    system("cls");
    setColor(111);
    cout << "<$$$ Баланс $$$>\n";
    setColor(7);
    cout << "Ваш баланс: $" << fixed << setprecision(2) << currentUser->getBalance() << "\n";
}

void topUpBalance() {
    system("cls");
    setColor(111);
    cout << "<-_-_-> Пополнение баланса <-_-_->\n";
    setColor(7);
    double amount = 0.0;
    string password;
    cout << "Введите сумму для пополнения: $";
    cin >> amount;
    if (amount <= 0) {
        cout << "Сумма должна быть положительной!\n";
        return;
    }
    cout << "Введите пароль для подтверждения: ";
    cin >> password;
    if (password != currentUser->getPassword()) {
        cout << "Неверный пароль! Операция отменена.\n";
        return;
    }
    currentUser->setBalance(currentUser->getBalance() + amount);
    if (db.updateUserBalance(currentUser->getId(), currentUser->getBalance())) {
        cout << "Баланс успешно пополнен!\n";
        cout << "Новый баланс: $" << fixed << setprecision(2) << currentUser->getBalance() << "\n";
    }
    else {
        cout << " Ошибка при обновлении баланса!\n";
    }
}

void viewOwnedApartments() {
    system("cls");
    auto apartments = db.getApartmentsByOwner(currentUser->getId());
    setColor(111);
    cout << "<-_-_-> Ваши квартиры <-_-_->\n";
    setColor(7);
    if (apartments.empty()) {
        cout << "У вас нет квартир.\n";
        return;
    }
    for (const auto& a : apartments) {
        displayApartment(a);
    }
}

void registerUser() {
    system("cls");
    setColor(111);
    cout << "<-_=_-> Регистрация <-_=_->\n";
    setColor(7);
    string username, password;
    cout << "Логин: "; cin >> username;
    cout << "Пароль: "; cin >> password;
    if (db.registerUser(username, password)) {
        cout << "\n Регистрация успешна!\n";
    }
    else {
        cout << "\n Ошибка: логин уже занят или ошибка БД.\n";
    }
}

void loginUser() {
    system("cls");
    setColor(111);
    cout << "<-_-_-> Вход в аккаунт <-_-_->\n";
    setColor(7);
    string username, password;
    cout << "Логин: "; cin >> username;
    cout << "Пароль: "; cin >> password;
    currentUser = db.loginUser(username, password);
    if (currentUser) {
        cout << "\n Вход выполнен успешно!\n";
    }
    else {
        cout << "\n Неверный логин или пароль!\n";
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    if (!db.open("real_estate.db")) {
        cerr << " Не удалось открыть базу данных!\n";
        return 1;
    }

    if (db.getAllApartments().empty()) {
        db.registerUser("admin", "admin");
        Apartment demo;
        demo.setId(1);
        demo.setOwnerId(1);
        demo.setPrice(60000);
        demo.setArea(60);
        demo.setRooms(2);
        demo.setIsNewBuilding(false);
        demo.setYearBuilt(2010);
        demo.setFloor(4);
        demo.setAddress("ул. Пушкина, д.10");
        db.addApartment(demo);
    }

    int selected = 0;
    char key;

    while (true) {
        displayAuthMenu(selected);
        key = _getch();

        if (key == 72) {
            selected = (selected - 1 + 3) % 3;
        }
        else if (key == 80) {
            selected = (selected + 1) % 3;
        }
        else if (key == 13) {
            if (selected == 0) {
                registerUser();
                system("pause");
            }
            else if (selected == 1) {
                loginUser();
                system("pause");
                if (currentUser) {
                    mainMenu();
                }
            }
            else if (selected == 2) {
                db.close();
                exit(0);
            }
        }
    }
    return 0;
}