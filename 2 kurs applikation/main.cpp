#include <iostream>
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
void viewFavoriteApartments();
void viewTransactionHistory();
void buyApartment();
void sellApartment();
void displayApartment(const Apartment& a);
void addToFavorites(int apartmentId);
void removeFromFavorites(int apartmentId);

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
        "3. Мои квартиры",
        "4. Избранные квартиры",
        "5. История сделок",
        "6. Назад"
    };

    for (int i = 0; i < 6; i++) {
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
            selected = (selected - 1 + 6) % 6;
        }
        else if (key == 80) {
            selected = (selected + 1) % 6;
        }
        else if (key == 13) {
            switch (selected) {
            case 0: checkBalance(); system("pause"); break;
            case 1: topUpBalance(); system("pause"); break;
            case 2: viewOwnedApartments(); system("pause"); break;
            case 3: viewFavoriteApartments(); system("pause"); break;
            case 4: viewTransactionHistory(); system("pause"); break;
            case 5: return;
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
                cout << "=!!= Все доступные квартиры =!!=\n";
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
                        cout << " Квартира успешно куплена!\n";
                    }
                    else {
                        cout << " Ошибка при покупке!\n";
                    }
                }
                system("pause");
                return;
            }
            else if (mainChoice == 1) {

                system("cls");
                cout << "=--= Подбор по параметрам =--=\n";
                cout << "Показаны все квартиры. Для расширенной фильтрации используйте отдельную функцию.\n\n";
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
                        cout << "Квартира успешно куплена!\n";
                    }
                    else {
                        cout << "Ошибка при покупке!\n";
                    }
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
                    cout << "=xx= Ваши квартиры =xx=\n";
                    for (const auto& a : myApts) {
                        displayApartment(a);
                    }
                    cout << "\nВсе ваши квартиры уже доступны для покупки!\n";
                }
                system("pause");
                return;
            }
            else if (selected == 1) {
                Apartment a;
                a.setOwnerId(currentUser->getId());

                system("cls");
                cout << "=xx= Новое объявление =xx=\n";
                double price, area;
                int rooms, year, floor;
                bool isNew;
                string address;

                cout << "Цена ($): "; cin >> price;
                cout << "Площадь (м²): "; cin >> area;
                cout << "Комнат: "; cin >> rooms;
                cout << "Новостройка (1) или вторичка (0)? "; cin >> isNew;
                cout << "Год постройки: "; cin >> year;
                cout << "Этаж: "; cin >> floor;
                cin.ignore();
                cout << "Адрес: "; getline(cin, address);

                a.setPrice(price);
                a.setArea(area);
                a.setRooms(rooms);
                a.setIsNewBuilding(isNew);
                a.setYearBuilt(year);
                a.setFloor(floor);
                a.setAddress(address);

                if (db.addApartment(a)) {
                    cout << "\n Объявление создано!\n";
                }
                else {
                    cout << "\n Ошибка создания!\n";
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
        << " | Цена/м²: $" << a.getPricePerSqMeter()
        << " | Площадь: " << a.getArea() << " м²"
        << " | Комнат: " << a.getRooms()
        << " | " << (a.getIsNewBuilding() ? "Новостройка" : "Вторичка")
        << " | Этаж: " << a.getFloor()
        << " | Год: " << a.getYearBuilt() << "\n";
}

void checkBalance() {
    system("cls");
    cout << "=$$= Баланс =$$=\n";
    cout << "Ваш баланс: $" << fixed << setprecision(2) << currentUser->getBalance() << "\n";
}

void topUpBalance() {
    system("cls");
    cout << "=$$= Пополнение баланса =$$=\n";
    double amount;
    string password;
    cout << "Сумма: $"; cin >> amount;
    if (amount <= 0) {
        cout << "Сумма должна быть положительной!\n";
        return;
    }
    cout << "Пароль для подтверждения: "; cin >> password;
    if (password != currentUser->getPassword()) {
        cout << "Неверный пароль!\n";
        return;
    }
    currentUser->setBalance(currentUser->getBalance() + amount);
    if (db.updateUserBalance(currentUser->getId(), currentUser->getBalance())) {
        cout << " Баланс пополнен!\n";
        cout << "Новый баланс: $" << fixed << setprecision(2) << currentUser->getBalance() << "\n";
    }
    else {
        cout << "Ошибка обновления!\n";
    }
}

void viewOwnedApartments() {
    system("cls");
    auto apts = db.getApartmentsByOwner(currentUser->getId());
    cout << "=!!= Мои квартиры =!!=\n";
    if (apts.empty()) {
        cout << "У вас нет квартир.\n";
        return;
    }
    for (const auto& a : apts) {
        displayApartment(a);
    }
}

void viewFavoriteApartments() {
    system("cls");
    auto apts = db.getFavoriteApartments(currentUser->getId());
    cout << "=<3= Избранные квартиры =<3=\n";
    if (apts.empty()) {
        cout << "Список избранных пуст.\n";
        cout << "Чтобы добавить — найдите квартиру и выберите опцию 'В избранное'.\n";
        return;
    }
    for (const auto& a : apts) {
        displayApartment(a);
    }
    cout << "\n1. Удалить из избранного\n2. Назад\nВыбор: ";
    int choice; cin >> choice;
    if (choice == 1) {
        cout << "ID квартиры для удаления: "; int id; cin >> id;
        if (db.removeFromFavorites(currentUser->getId(), id)) {
            cout << " Удалено из избранного.\n";
        }
        else {
            cout << " Не найдено в избранном.\n";
        }
        system("pause");
    }
}

void viewTransactionHistory() {
    system("cls");
    auto txs = db.getTransactionsByUser(currentUser->getId());
    cout << "=== История сделок ===\n";
    if (txs.empty()) {
        cout << "У вас ещё нет сделок.\n";
        return;
    }
    for (const auto& tx : txs) {
        cout << "Сумма: $" << tx[0]
            << " | Дата: " << tx[1]
            << " | Адрес: " << tx[2]
            << " | Продавец: " << tx[3] << "\n";
    }
}

void addToFavorites(int apartmentId) {
    if (db.addToFavorites(currentUser->getId(), apartmentId)) {
        cout << " Квартира добавлена в избранное!\n";
    }
    else {
        cout << " Ошибка добавления!\n";
    }
}


void registerUser() {
    system("cls");
    cout << "=>>= Регистрация =<<=\n";
    string username, password;
    cout << "Логин: "; cin >> username;
    cout << "Пароль: "; cin >> password;
    if (db.registerUser(username, password)) {
        cout << "\n Регистрация успешна!\n";
    }
    else {
        cout << "\n Логин занят!\n";
    }
}

void loginUser() {
    system("cls");
    cout << "=>>= Вход =<<=\n";
    string username, password;
    cout << "Логин: "; cin >> username;
    cout << "Пароль: "; cin >> password;
    currentUser = db.loginUser(username, password);
    if (currentUser) {
        cout << "\n Вход выполнен!\n";
    }
    else {
        cout << "\n Неверный логин или пароль!\n";
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (!db.open("real_estate.db")) {
        cerr << "Не удалось открыть БД!\n";
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
                return 0;
            }
        }
    }
}