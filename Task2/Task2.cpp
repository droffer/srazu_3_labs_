#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <windows.h>
#include <conio.h>

using namespace std;

struct Publication {
    string title = "";
    string type = "";
    string publisher = "";
    string year = "";
};

struct Editor {
    string title = "";
    string address = "";
    string editor_surname = "";
};

struct Grouped {
    string title = "";
    vector<Publication> pubs;
    vector<Editor> eds;
};

// Очистка строк от лишнего мусора
string trim(string s) {
    while (!s.empty() && (s.back() == ' ' || s.back() == '\r' || s.back() == '\n' || s.back() == '\t')) s.pop_back();
    while (!s.empty() && (s.front() == ' ' || s.front() == '\r' || s.front() == '\n' || s.front() == '\t')) s.erase(0, 1);
    return s;
}

// Функция подтверждения (y/n)
bool confirm(string msg) {
    cout << "\n " << msg << " (y/n): ";
    while (true) {
        char c = _getch();
        if (c == 'y' || c == 'Y' || c == 'н' || c == 'Н') { cout << "y" << endl; return true; }
        if (c == 'n' || c == 'N' || c == 'т' || c == 'Т') { cout << "n" << endl; return false; }
    }
}

// Ожидание выбора в меню
char get_choice() {
    while (true) {
        char c = _getch();
        if (c == '0' || c == '1' || c == '2') return c;
    }
}

void draw_menu() {
    cout << "  ________________________________________" << endl;
    cout << " /                                        \\" << endl;
    cout << " |       СИСТЕМА ОБРАБОТКИ ДАННЫХ         |" << endl;
    cout << " |________________________________________|" << endl;
    cout << " |                                        |" << endl;
    cout << " |  [1] Поиск (ПОЛНОЕ СОВПАДЕНИЕ)         |" << endl;
    cout << " |  [2] Записать отчет в файл             |" << endl;
    cout << " |  [0] Выход                             |" << endl;
    cout << " |________________________________________|" << endl;
    cout << "\n >> Выберите действие: ";
}

// Инициализация консоли
void init_console() {
    system("color 0E");
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
}

// Загрузка баз данных из файлов
bool load_databases(vector<Publication>& all_p, vector<Editor>& all_e) {
    ifstream f1("publications.txt");
    ifstream f2("editors.txt");

    bool f1_ok = f1.is_open();//флаги для проверки открытия файлов
    bool f2_ok = f2.is_open();

    if (!f1_ok || !f2_ok) {
        cout << " [!] КРИТИЧЕСКАЯ ОШИБКА: Ошибка доступа к базе данных!" << endl;
        if (!f1_ok) cout << "     - Файл 'publications.txt' не найден." << endl;
        if (!f2_ok) cout << "     - Файл 'editors.txt' не найден." << endl;
        cout << " Для работы программы требуются ОБА файла." << endl;
        if (f1_ok) f1.close();
        if (f2_ok) f2.close();
        return false;
    }

    cout << " [OK] Оба файла базы данных успешно открыты!" << endl;

    string line;
    Publication temp_p;
    while (getline(f1, line)) {
        if (line.find("\"") != string::npos) {
            int first = line.find("\"");
            int last = line.find_last_of("\"");
            temp_p.title = trim(line.substr(first + 1, last - first - 1));
        }
        if (line.find("Вид:") != string::npos) temp_p.type = trim(line.substr(line.find(":") + 2));
        if (line.find("Издатель:") != string::npos) temp_p.publisher = trim(line.substr(line.find(":") + 2));
        if (line.find("Год:") != string::npos) {
            temp_p.year = trim(line.substr(line.find(":") + 2));
            all_p.push_back(temp_p);
        }
    }
    f1.close();

    Editor temp_e;
    while (getline(f2, line)) {
        if (line.find("Издание:") != string::npos) {
            int first = line.find("\"");
            int last = line.find_last_of("\"");
            temp_e.title = trim(line.substr(first + 1, last - first - 1));
        }
        if (line.find("Адрес:") != string::npos) temp_e.address = trim(line.substr(line.find(":") + 2));
        if (line.find("Редактор:") != string::npos) {
            temp_e.editor_surname = trim(line.substr(line.find(":") + 2));
            all_e.push_back(temp_e);
        }
    }
    f2.close();
    return true;
}

// Группировка публикаций и редакций по названию издания
vector<Grouped> group_data(const vector<Publication>& all_p, const vector<Editor>& all_e) {
    vector<Grouped> groups;
    for (int i = 0; i < all_p.size(); i++) {
        bool found = false;// флаг для проверки существования группы (true - если группа найдена, false - если не найдена)
        for (int j = 0; j < groups.size(); j++) {
            if (groups[j].title == all_p[i].title) {
                groups[j].pubs.push_back(all_p[i]);
                found = true; break;
            }
        }
        if (!found) {
            Grouped g; g.title = all_p[i].title;
            g.pubs.push_back(all_p[i]); groups.push_back(g);
        }
    }
    for (int i = 0; i < all_e.size(); i++) {
        bool found = false;
        for (int j = 0; j < groups.size(); j++) {
            if (groups[j].title == all_e[i].title) {
                groups[j].eds.push_back(all_e[i]);
                found = true; break;
            }
        }
        if (!found) {
            Grouped g; g.title = all_e[i].title;
            g.eds.push_back(all_e[i]); groups.push_back(g);
        }
    }
    return groups;
}

// Поиск по точному значению поля
void handle_search(const vector<Grouped>& groups) {
    cout << "\n [ПОИСК] Введите точное значение поля: ";
    string query;
    getline(cin >> ws, query);

    bool found_any = false;//флаг для проверки наличия совпадений во всем списке(true - если найдено хотя бы одно совпадение, false - если не найдено)
    for (int i = 0; i < groups.size(); i++) {
        bool match = false;// флаг для проверки совпадения текущего элемента с запросом(true - если текущий элемент подошел, false - если не подошел)

        if (groups[i].title == query) match = true;

        if (!match) {
            for (auto& p : groups[i].pubs) {
                if (p.type == query || p.publisher == query || p.year == query) {
                    match = true; break;
                }
            }
        }

        if (!match) {
            for (auto& e : groups[i].eds) {
                if (e.address == query || e.editor_surname == query) {
                    match = true; break;
                }
            }
        }

        if (match) {
            cout << "  ____________________________________________________" << endl;
            cout << "  ИЗДАНИЕ: \"" << groups[i].title << "\"" << endl;
            cout << "  ----------------------------------------------------" << endl;
            cout << "    >> ПУБЛИКАЦИИ:" << endl;
            if (groups[i].pubs.empty()) {
                cout << "       - Вид: отсутствует | Издатель: отсутствует | Год: отсутствует" << endl;
            }
            else {
                for (auto& p : groups[i].pubs)
                    cout << "       - Вид: " << p.type << " | Издатель: " << p.publisher << " | Год: " << p.year << endl;
            }
            cout << "    >> РЕДАКЦИИ:" << endl;
            if (groups[i].eds.empty()) {
                cout << "       - Редактор: отсутствует | Адрес: отсутствует" << endl;
            }
            else {
                for (auto& e : groups[i].eds)
                    cout << "       - Редактор: " << e.editor_surname << " | Адрес: " << e.address << endl;
            }
            cout << "  ____________________________________________________" << endl << endl;
            found_any = true;
        }
    }
    if (!found_any) cout << "  [!] Совпадений не найдено (требуется полное соответствие)." << endl;
    system("pause");
}

// Запись отчета в файл
void handle_write_report(const vector<Grouped>& groups) {
    // Проверяем существование файла перед предупреждением
    bool file_exists = false;// флаг для проверки существования файла(true - если сущетвует, false - если не существует)
    {
        ifstream check("report.txt");
        if (check.is_open()) file_exists = true;
    }

    if (file_exists) {
        if (!confirm("Файл 'report.txt' уже существует и будет перезаписан. Продолжить?")) {
            system("cls");
            return;
        }
    }

    ofstream out("report.txt");
    if (!out.is_open()) {
        cout << "\n [!] ОШИБКА: Не удалось создать файл 'report.txt'!" << endl;
    }
    else {
        for (int i = 0; i < groups.size(); i++) {
            out << "====================================================" << endl;
            out << "ИЗДАНИЕ: \"" << groups[i].title << "\"" << endl;
            out << "----------------------------------------------------" << endl;
            out << "  >> ПУБЛИКАЦИИ:" << endl;
            if (groups[i].pubs.empty()) {
                out << "     - Вид: отсутствует | Издатель: отсутствует | Год: отсутствует" << endl;
            }
            else {
                for (auto& p : groups[i].pubs)
                    out << "     - Вид: " << p.type << " | Издатель: " << p.publisher << " | Год: " << p.year << endl;
            }
            out << "  >> РЕДАКЦИИ:" << endl;
            if (groups[i].eds.empty()) {
                out << "     - Редактор: отсутствует | Адрес: отсутствует" << endl;
            }
            else {
                for (auto& e : groups[i].eds)
                    out << "     - Редактор: " << e.editor_surname << " | Адрес: " << e.address << endl;
            }
            out << "====================================================" << endl << endl;
        }
        out.close();
        cout << "\n [УСПЕХ] Отчет записан в report.txt" << endl;
    }
    system("pause");
}

int main() {
    init_console();

    vector<Publication> all_p;
    vector<Editor> all_e;

    if (!load_databases(all_p, all_e)) {
        system("pause");
        return 0;
    }

    vector<Grouped> groups = group_data(all_p, all_e);

    system("pause");
    system("cls");

    while (true) {
        draw_menu();
        char choice = get_choice();
        cout << choice << endl;

        if (choice == '0') {
            if (confirm("Вы точно хотите выйти?")) break;
            else { system("cls"); continue; }
        }

        if (choice == '1') {
            handle_search(groups);
        }

        if (choice == '2') {
            handle_write_report(groups);
        }
        system("cls");
    }
    return 0;
}
