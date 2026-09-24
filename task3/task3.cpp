#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <cstdio>
#include <windows.h>
#include <conio.h>

using namespace std;

// Структура для публикации
struct Publication {
    string type = "отсутствует";
    string publisher = "отсутствует";
    string year = "отсутствует";
};

// Структура для редакции
struct Editor {
    string address = "отсутствует";
    string editor_surname = "отсутствует";
};

// Сгруппированная запись об издании
struct Grouped {
    string title = "";
    vector<Publication> pubs;
    vector<Editor> eds;
};

// Точка восстановления (бэкап)
struct BackupPoint {
    int id;
    string filename;
    string time_str;
    string comment;
    vector<Grouped> snapshot;
};

// Очистка строки от пробелов и переводов строк
string trim(string s) {
    while (!s.empty() && (s.back() == ' ' || s.back() == '\r' || s.back() == '\n' || s.back() == '\t')) s.pop_back();
    while (!s.empty() && (s.front() == ' ' || s.front() == '\r' || s.front() == '\n' || s.front() == '\t')) s.erase(0, 1);
    return s;
}

// Получение текущей даты и времени
string get_current_time() {
    time_t now = time(nullptr);
    tm* lt = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", lt);
    return string(buf);
}

// Настройка консоли
void init_console() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");
}

// Ввод строки
string input_line(string prompt) {
    cout << prompt;
    string s;
    getline(cin >> ws, s);
    return trim(s);
}

// Подтверждение y/n
bool confirm(string msg) {
    cout << "\n " << msg << " (y/n): ";
    while (true) {
        char c = _getch();
        if (c == 'y' || c == 'Y' || c == 'н' || c == 'Н') { cout << "y" << endl; return true; }
        if (c == 'n' || c == 'N' || c == 'т' || c == 'Т') { cout << "n" << endl; return false; }
    }
}

char get_key() {
    return _getch();
}

// Загрузка отчета в формате Задания 2
bool load_report(const string& filename, vector<Grouped>& data) {
    ifstream f(filename);
    if (!f.is_open()) return false;

    data.clear();
    string line;
    Grouped current;
    bool in_pubs = false;
    bool in_eds = false;
    bool has_entry = false;

    while (getline(f, line)) {
        string t = trim(line);
        if (t.empty()) continue;

        if (t.find("ИЗДАНИЕ:") != string::npos) {
            if (has_entry) {
                data.push_back(current);
                current = Grouped();
                in_pubs = false;
                in_eds = false;
            }
            int first = t.find("\"");
            int last = t.find_last_of("\"");
            if (first != string::npos && last != string::npos && last > first) {
                current.title = t.substr(first + 1, last - first - 1);
            }
            else {
                current.title = trim(t.substr(t.find(":") + 1));
            }
            has_entry = true;
            continue;
        }

        if (t.find(">> ПУБЛИКАЦИИ:") != string::npos) {
            in_pubs = true;
            in_eds = false;
            continue;
        }

        if (t.find(">> РЕДАКЦИИ:") != string::npos) {
            in_pubs = false;
            in_eds = true;
            continue;
        }

        if (in_pubs && t.find("Вид:") != string::npos) {
            Publication p;
            size_t p_vid = t.find("Вид:");
            size_t p_izd = t.find("Издатель:");
            size_t p_god = t.find("Год:");

            if (p_vid != string::npos && p_izd != string::npos && p_god != string::npos) {
                p.type = trim(t.substr(p_vid + 4, p_izd - (p_vid + 4)));
                if (!p.type.empty() && p.type.back() == '|') p.type.pop_back();
                p.type = trim(p.type);

                p.publisher = trim(t.substr(p_izd + 9, p_god - (p_izd + 9)));
                if (!p.publisher.empty() && p.publisher.back() == '|') p.publisher.pop_back();
                p.publisher = trim(p.publisher);

                p.year = trim(t.substr(p_god + 4));
                current.pubs.push_back(p);
            }
            continue;
        }

        if (in_eds && (t.find("Редактор:") != string::npos || t.find("Адрес:") != string::npos)) {
            Editor e;
            size_t p_red = t.find("Редактор:");
            size_t p_adr = t.find("Адрес:");

            if (p_red != string::npos && p_adr != string::npos) {
                e.editor_surname = trim(t.substr(p_red + 9, p_adr - (p_red + 9)));
                if (!e.editor_surname.empty() && e.editor_surname.back() == '|') e.editor_surname.pop_back();
                e.editor_surname = trim(e.editor_surname);

                e.address = trim(t.substr(p_adr + 6));
                current.eds.push_back(e);
            }
            continue;
        }
    }

    if (has_entry) {
        data.push_back(current);
    }

    f.close();
    return true;
}

// Запись отчета строго без изменения формата
bool save_report(const string& filename, const vector<Grouped>& data) {
    ofstream out(filename);
    if (!out.is_open()) return false;

    for (size_t i = 0; i < data.size(); i++) {
        out << "====================================================" << endl;
        out << "ИЗДАНИЕ: \"" << data[i].title << "\"" << endl;
        out << "----------------------------------------------------" << endl;
        out << "  >> ПУБЛИКАЦИИ:" << endl;
        if (data[i].pubs.empty()) {
            out << "     - Вид: отсутствует | Издатель: отсутствует | Год: отсутствует" << endl;
        }
        else {
            for (const auto& p : data[i].pubs) {
                out << "     - Вид: " << p.type << " | Издатель: " << p.publisher << " | Год: " << p.year << endl;
            }
        }
        out << "  >> РЕДАКЦИИ:" << endl;
        if (data[i].eds.empty()) {
            out << "     - Редактор: отсутствует | Адрес: отсутствует" << endl;
        }
        else {
            for (const auto& e : data[i].eds) {
                out << "     - Редактор: " << e.editor_surname << " | Адрес: " << e.address << endl;
            }
        }
        out << "====================================================" << endl << endl;
    }

    out.close();
    return true;
}

// Перезапись индексного файла бэкапов на диске
void sync_backups_index(const vector<BackupPoint>& history) {
    ofstream idx("backups_index.txt");
    if (!idx.is_open()) return;

    for (const auto& bp : history) {
        idx << bp.id << "|" << bp.filename << "|" << bp.time_str << "|" << bp.comment << endl;
    }
    idx.close();
}

// Сканирование и загрузка существующих бэкапов при старте программы
void load_existing_backups(vector<BackupPoint>& history) {
    history.clear();

    // 1. Пытаемся прочесть из backups_index.txt
    ifstream idx("backups_index.txt");
    if (idx.is_open()) {
        string line;
        while (getline(idx, line)) {
            line = trim(line);
            if (line.empty()) continue;

            // Разбор строки: id|filename|time_str|comment
            size_t p1 = line.find("|");
            size_t p2 = (p1 != string::npos) ? line.find("|", p1 + 1) : string::npos;
            size_t p3 = (p2 != string::npos) ? line.find("|", p2 + 1) : string::npos;

            if (p1 != string::npos && p2 != string::npos && p3 != string::npos) {
                BackupPoint bp;
                bp.id = stoi(line.substr(0, p1));
                bp.filename = line.substr(p1 + 1, p2 - (p1 + 1));
                bp.time_str = line.substr(p2 + 1, p3 - (p2 + 1));
                bp.comment = line.substr(p3 + 1);

                // Загружаем данные снимка из файла бэкапа
                if (load_report(bp.filename, bp.snapshot)) {
                    history.push_back(bp);
                }
            }
        }
        idx.close();
    }

    // 2. Если индекс был пуст, но файлы backup_1.txt, backup_2.txt уже лежали в папке
    if (history.empty()) {
        int id = 1;
        while (true) {
            string fname = "backup_" + to_string(id) + ".txt";
            ifstream test(fname);
            if (!test.is_open()) break;
            test.close();

            BackupPoint bp;
            bp.id = id;
            bp.filename = fname;
            bp.time_str = get_current_time();
            bp.comment = "Найденный архивный снимок #" + to_string(id);
            if (load_report(fname, bp.snapshot)) {
                history.push_back(bp);
            }
            id++;
        }
        if (!history.empty()) {
            sync_backups_index(history);
        }
    }
}

// Создание нового бэкапа из текущих данных в памяти
void create_backup(vector<BackupPoint>& history, const vector<Grouped>& current_data, string default_comment = "") {
    if (history.size() >= 10) {
        cout << "\n -----------------------------------------------------------";
        cout << "\n [!] ВНИМАНИЕ: Накопилось уже " << history.size() << " бэкапов!";
        cout << "\n Рекомендуется удалить старые снимки, чтобы не тратить память.";
        cout << "\n -----------------------------------------------------------\n";
    }

    BackupPoint bp;
    bp.id = (int)history.size() + 1;
    bp.filename = "backup_" + to_string(bp.id) + ".txt";
    bp.time_str = get_current_time();
    bp.snapshot = current_data;

    if (default_comment.empty()) {
        cout << "\n [GIT-БЭКАП] Введите комментарий к снимку: ";
        getline(cin >> ws, bp.comment);
        bp.comment = trim(bp.comment);
        if (bp.comment.empty()) bp.comment = "Снимок без описания";
    }
    else {
        bp.comment = default_comment;
    }

    // Сохраняем физический файл бэкапа на диск
    save_report(bp.filename, bp.snapshot);

    history.push_back(bp);
    sync_backups_index(history);

    cout << " [OK] Точка бэкапа #" << bp.id << " сохранена в файл '" << bp.filename << "'!\n";
}

// Просмотр всех строк
void show_data(const vector<Grouped>& data) {
    if (data.empty()) {
        cout << "\n [!] Список данных пуст.\n";
        return;
    }
    cout << "\n================ ТЕКУЩИЕ ИНФОРМАЦИОННЫЕ СТРОКИ ================\n";
    for (size_t i = 0; i < data.size(); i++) {
        cout << " [" << (i + 1) << "] ИЗДАНИЕ: \"" << data[i].title << "\"" << endl;
        cout << "     Публикации (" << data[i].pubs.size() << " шт.):" << endl;
        for (size_t j = 0; j < data[i].pubs.size(); j++) {
            cout << "       - Вид: " << data[i].pubs[j].type
                << " | Издатель: " << data[i].pubs[j].publisher
                << " | Год: " << data[i].pubs[j].year << endl;
        }
        cout << "     Редакции (" << data[i].eds.size() << " шт.):" << endl;
        for (size_t j = 0; j < data[i].eds.size(); j++) {
            cout << "       - Редактор: " << data[i].eds[j].editor_surname
                << " | Адрес: " << data[i].eds[j].address << endl;
        }
        cout << " ------------------------------------------------------------\n";
    }
}

// Просмотр конкретной строки
void view_single_record(const vector<Grouped>& data) {
    if (data.empty()) {
        cout << "\n [!] Список пуст.\n";
        return;
    }
    cout << "\n Введите номер издания (1 - " << data.size() << "): ";
    int idx;
    if (!(cin >> idx) || idx < 1 || idx >(int)data.size()) {
        cin.clear(); cin.ignore(1000, '\n');
        cout << " [!] Некорректный номер.\n";
        return;
    }
    idx--;

    system("cls");
    cout << "================ ДЕТАЛЬНЫЙ ПРОСМОТР СТРОКИ ================\n";
    cout << " ИЗДАНИЕ #" << (idx + 1) << ": \"" << data[idx].title << "\"\n";
    cout << " >> ПУБЛИКАЦИИ:\n";
    if (data[idx].pubs.empty()) cout << "    (нет публикаций)\n";
    for (size_t j = 0; j < data[idx].pubs.size(); j++) {
        cout << "    [" << (j + 1) << "] Вид: " << data[idx].pubs[j].type
            << " | Издатель: " << data[idx].pubs[j].publisher
            << " | Год: " << data[idx].pubs[j].year << endl;
    }
    cout << " >> РЕДАКЦИИ:\n";
    if (data[idx].eds.empty()) cout << "    (нет редакций)\n";
    for (size_t j = 0; j < data[idx].eds.size(); j++) {
        cout << "    [" << (j + 1) << "] Редактор: " << data[idx].eds[j].editor_surname
            << " | Адрес: " << data[idx].eds[j].address << endl;
    }
    cout << "===========================================================\n";
}

// Редактирование поля
bool edit_field(vector<Grouped>& data) {
    if (data.empty()) {
        cout << "\n [!] Список пуст.\n";
        return false;
    }
    show_data(data);
    cout << "\n Выберите номер издания (1 - " << data.size() << "): ";
    int idx;
    if (!(cin >> idx) || idx < 1 || idx >(int)data.size()) {
        cin.clear(); cin.ignore(1000, '\n');
        cout << " [!] Неверный номер.\n";
        return false;
    }
    idx--;

    cout << "\n Что редактировать?\n";
    cout << " [1] Название издания\n";
    cout << " [2] Поле в публикации\n";
    cout << " [3] Поле в редакции\n";
    cout << " [0] Отмена\n >> ";
    char ch = _getch();
    cout << ch << endl;

    if (ch == '1') {
        string nt = input_line(" Новое название издания: ");
        if (!nt.empty()) {
            data[idx].title = nt;
            cout << " [OK] Название обновлено.\n";
            return true;
        }
    }
    else if (ch == '2') {
        if (data[idx].pubs.empty()) { cout << " [!] Нет публикаций.\n"; return false; }
        cout << " Номер публикации (1 - " << data[idx].pubs.size() << "): ";
        int p_idx;
        if (!(cin >> p_idx) || p_idx < 1 || p_idx >(int)data[idx].pubs.size()) {
            cin.clear(); cin.ignore(1000, '\n'); return false;
        }
        p_idx--;
        cout << " Поле: 1-Вид, 2-Издатель, 3-Год >> ";
        char fch = _getch(); cout << fch << endl;
        if (fch == '1') data[idx].pubs[p_idx].type = input_line(" Новый вид: ");
        else if (fch == '2') data[idx].pubs[p_idx].publisher = input_line(" Новый издатель: ");
        else if (fch == '3') data[idx].pubs[p_idx].year = input_line(" Новый год: ");
        else return false;
        cout << " [OK] Поле публикации обновлено.\n";
        return true;
    }
    else if (ch == '3') {
        if (data[idx].eds.empty()) { cout << " [!] Нет редакций.\n"; return false; }
        cout << " Номер редакции (1 - " << data[idx].eds.size() << "): ";
        int e_idx;
        if (!(cin >> e_idx) || e_idx < 1 || e_idx >(int)data[idx].eds.size()) {
            cin.clear(); cin.ignore(1000, '\n'); return false;
        }
        e_idx--;
        cout << " Поле: 1-Фамилия, 2-Адрес >> ";
        char fch = _getch(); cout << fch << endl;
        if (fch == '1') data[idx].eds[e_idx].editor_surname = input_line(" Новая фамилия: ");
        else if (fch == '2') data[idx].eds[e_idx].address = input_line(" Новый адрес: ");
        else return false;
        cout << " [OK] Поле редакции обновлено.\n";
        return true;
    }
    return false;
}

// Добавление строки
bool add_info_row(vector<Grouped>& data) {
    cout << "\n Добавление:\n";
    cout << " [1] Новое издание целиком\n";
    cout << " [2] Новая публикация к существующему изданию\n";
    cout << " [3] Новая редакция к существующему изданию\n";
    cout << " [0] Отмена\n >> ";
    char ch = _getch();
    cout << ch << endl;

    if (ch == '1') {
        Grouped g;
        g.title = input_line(" Название издания: ");
        if (g.title.empty()) return false;

        Publication p;
        p.type = input_line(" Вид (Enter = отсутствует): ");
        if (p.type.empty()) p.type = "отсутствует";
        p.publisher = input_line(" Издатель (Enter = отсутствует): ");
        if (p.publisher.empty()) p.publisher = "отсутствует";
        p.year = input_line(" Год (Enter = отсутствует): ");
        if (p.year.empty()) p.year = "отсутствует";
        g.pubs.push_back(p);

        Editor e;
        e.editor_surname = input_line(" Фамилия редактора (Enter = отсутствует): ");
        if (e.editor_surname.empty()) e.editor_surname = "отсутствует";
        e.address = input_line(" Адрес редакции (Enter = отсутствует): ");
        if (e.address.empty()) e.address = "отсутствует";
        g.eds.push_back(e);

        data.push_back(g);
        cout << " [OK] Издание добавлено.\n";
        return true;
    }
    else if (ch == '2') {
        if (data.empty()) { cout << " [!] База пуста.\n"; return false; }
        show_data(data);
        cout << " К какому изданию добавить (1 - " << data.size() << "): ";
        int idx;
        if (!(cin >> idx) || idx < 1 || idx >(int)data.size()) {
            cin.clear(); cin.ignore(1000, '\n'); return false;
        }
        idx--;
        Publication p;
        p.type = input_line(" Вид: ");
        p.publisher = input_line(" Издатель: ");
        p.year = input_line(" Год: ");
        data[idx].pubs.push_back(p);
        cout << " [OK] Публикация добавлена.\n";
        return true;
    }
    else if (ch == '3') {
        if (data.empty()) { cout << " [!] База пуста.\n"; return false; }
        show_data(data);
        cout << " К какому изданию добавить (1 - " << data.size() << "): ";
        int idx;
        if (!(cin >> idx) || idx < 1 || idx >(int)data.size()) {
            cin.clear(); cin.ignore(1000, '\n'); return false;
        }
        idx--;
        Editor e;
        e.editor_surname = input_line(" Фамилия редактора: ");
        e.address = input_line(" Адрес: ");
        data[idx].eds.push_back(e);
        cout << " [OK] Редакция добавлена.\n";
        return true;
    }
    return false;
}

// Удаление строки
bool delete_info_row(vector<Grouped>& data) {
    if (data.empty()) {
        cout << "\n [!] Список пуст.\n";
        return false;
    }
    show_data(data);
    cout << "\n Удаление:\n";
    cout << " [1] Издание целиком\n";
    cout << " [2] Конкретная публикация\n";
    cout << " [3] Конкретная редакция\n";
    cout << " [0] Отмена\n >> ";
    char ch = _getch();
    cout << ch << endl;

    if (ch == '1') {
        cout << " Номер издания для удаления (1 - " << data.size() << "): ";
        int idx;
        if (!(cin >> idx) || idx < 1 || idx >(int)data.size()) {
            cin.clear(); cin.ignore(1000, '\n'); return false;
        }
        idx--;
        if (confirm("Удалить издание \"" + data[idx].title + "\"?")) {
            data.erase(data.begin() + idx);
            cout << " [OK] Издание удалено.\n";
            return true;
        }
    }
    else if (ch == '2') {
        cout << " Номер издания (1 - " << data.size() << "): ";
        int idx;
        if (!(cin >> idx) || idx < 1 || idx >(int)data.size()) {
            cin.clear(); cin.ignore(1000, '\n'); return false;
        }
        idx--;
        if (data[idx].pubs.empty()) { cout << " [!] Нет публикаций.\n"; return false; }
        cout << " Номер публикации для удаления (1 - " << data[idx].pubs.size() << "): ";
        int p_idx;
        if (!(cin >> p_idx) || p_idx < 1 || p_idx >(int)data[idx].pubs.size()) {
            cin.clear(); cin.ignore(1000, '\n'); return false;
        }
        p_idx--;
        data[idx].pubs.erase(data[idx].pubs.begin() + p_idx);
        cout << " [OK] Публикация удалена.\n";
        return true;
    }
    else if (ch == '3') {
        cout << " Номер издания (1 - " << data.size() << "): ";
        int idx;
        if (!(cin >> idx) || idx < 1 || idx >(int)data.size()) {
            cin.clear(); cin.ignore(1000, '\n'); return false;
        }
        idx--;
        if (data[idx].eds.empty()) { cout << " [!] Нет редакций.\n"; return false; }
        cout << " Номер редакции для удаления (1 - " << data[idx].eds.size() << "): ";
        int e_idx;
        if (!(cin >> e_idx) || e_idx < 1 || e_idx >(int)data[idx].eds.size()) {
            cin.clear(); cin.ignore(1000, '\n'); return false;
        }
        e_idx--;
        data[idx].eds.erase(data[idx].eds.begin() + e_idx);
        cout << " [OK] Редакция удалена.\n";
        return true;
    }
    return false;
}

// Меню управления бэкапами (с предупреждением о 10+ бэкапах и удалением с диска)
void handle_backups_menu(vector<BackupPoint>& history, vector<Grouped>& current_data, bool& modified) {
    while (true) {
        system("cls");
        cout << "  ______________________________________________________\n";
        cout << " /                                                      \\\n";
        cout << " |            УПРАВЛЕНИЕ БЭКАПАМИ (GIT-ИСТОРИЯ)          |\n";
        cout << " |______________________________________________________|\n";

        // Предупреждение о лимите в 10 бэкапов
        if (history.size() >= 10) {
            cout << "  ******************************************************\n";
            cout << "  [!] ВНИМАНИЕ: Накопилось " << history.size() << " бэкапов (рекомендуемый лимит 10)!\n";
            cout << "      Рекомендуется удалить старые точки (пункт [3] или [4]),\n";
            cout << "      чтобы освободить память и место на диске.\n";
            cout << "  ******************************************************\n";
        }

        if (history.empty()) {
            cout << "  История бэкапов пуста (точек сохранения нет).\n";
        }
        else {
            cout << "  ДОСТУПНЫЕ ТОЧКИ ВОССТАНОВЛЕНИЯ:\n";
            for (size_t i = 0; i < history.size(); i++) {
                cout << "  [" << history[i].id << "] " << history[i].time_str
                    << " | " << history[i].filename
                    << " | Записей: " << history[i].snapshot.size()
                    << " | \"" << history[i].comment << "\"\n";
            }
        }
        cout << "  ------------------------------------------------------\n";
        cout << "  [1] Создать новый бэкап из ТЕКУЩЕГО состояния\n";
        cout << "  [2] ПЕРЕЙТИ (откатиться) к выбранному бэкапу\n";
        cout << "  [3] Удалить конкретный бэкап (удалит и файл с диска)\n";
        cout << "  [4] Очистить ВСЕ бэкапы (освободить память и диск)\n";
        cout << "  [0] Назад в главное меню\n";
        cout << "  ______________________________________________________\n";
        cout << "\n >> Выберите действие: ";

        char c = get_key();
        cout << c << endl;

        if (c == '0') break;

        if (c == '1') {
            create_backup(history, current_data);
            system("pause");
        }
        else if (c == '2') {
            if (history.empty()) {
                cout << "\n [!] Нет созданных бэкапов.\n";
                system("pause");
                continue;
            }
            cout << "\n Введите номер бэкапа для перехода (1 - " << history.size() << "): ";
            int bid;
            if (cin >> bid && bid >= 1 && bid <= (int)history.size()) {
                if (confirm("Заменить текущие данные в памяти снимком #" + to_string(bid) + "?")) {
                    current_data = history[bid - 1].snapshot;
                    modified = true;
                    cout << " [УСПЕХ] Данные в памяти заменены на бэкап #" << bid
                        << " (\"" << history[bid - 1].comment << "\")!\n";
                    cout << " Чтобы записать их в файл report.txt, нажмите [6] в главном меню.\n";
                }
            }
            else {
                cin.clear(); cin.ignore(1000, '\n');
                cout << " [!] Неверный номер бэкапа.\n";
            }
            system("pause");
        }
        else if (c == '3') {
            if (history.empty()) {
                cout << "\n [!] Нечего удалять.\n";
                system("pause");
                continue;
            }
            cout << "\n Введите номер бэкапа для удаления: ";
            int bid;
            if (cin >> bid && bid >= 1 && bid <= (int)history.size()) {
                string target_file = history[bid - 1].filename;
                if (confirm("Удалить бэкап #" + to_string(bid) + " (" + target_file + ")?")) {
                    // 1. Физическое удаление файла с диска
                    remove(target_file.c_str());

                    // 2. Удаление из памяти
                    history.erase(history.begin() + (bid - 1));

                    // 3. Переиндексация оставшихся бэкапов
                    for (size_t i = 0; i < history.size(); i++) {
                        history[i].id = (int)i + 1;
                    }
                    sync_backups_index(history);

                    cout << " [OK] Бэкап удален из памяти и файл " << target_file << " стерт с диска.\n";
                }
            }
            else {
                cin.clear(); cin.ignore(1000, '\n');
                cout << " [!] Неверный номер.\n";
            }
            system("pause");
        }
        else if (c == '4') {
            if (history.empty()) {
                cout << "\n [!] История уже пуста.\n";
                system("pause");
                continue;
            }
            if (confirm("Вы точно хотите стереть ВСЕ бэкапы с диска?")) {
                // Удаляем все файлы backup_N.txt
                for (const auto& bp : history) {
                    remove(bp.filename.c_str());
                }
                remove("backups_index.txt");
                history.clear();
                cout << " [OK] Все файлы бэкапов удалены с диска, память очищена.\n";
            }
            system("pause");
        }
    }
}

// Главное меню
void draw_menu(bool modified, int backups_count, int records_count) {
    cout << "  ______________________________________________________\n";
    cout << " /                                                      \\\n";
    cout << " |     МОДИФИКАЦИЯ ДАННЫХ ИЗ ФАЙЛА (ЗАДАНИЕ 1 ПО ПЯВУ)  |\n";
    cout << " |______________________________________________________|\n";
    cout << " |  [1] Просмотр всех строк на экране                   |\n";
    cout << " |  [2] Просмотр конкретной строки (по номеру)          |\n";
    cout << " |  [3] Изменить содержимое любого поля                 |\n";
    cout << " |  [4] Добавить информационную строку                  |\n";
    cout << " |  [5] Удалить информационную строку                   |\n";
    cout << " |  [6] СОХРАНИТЬ текущие данные в главный report.txt   |\n";
    cout << " |  [7] Меню бэкапов (Git-история, прыжки, удаление)    |\n";
    cout << " |  [0] Выход из программы                              |\n";
    cout << " |______________________________________________________|\n";
    cout << "  Статус: Записей в памяти: " << records_count
        << " | Доступно бэкапов: " << backups_count << "\n";
    if (backups_count >= 10) {
        cout << "  * Внимание: накопилось " << backups_count << " бэкапов! Зайдите в [7] для очистки.\n";
    }
    if (modified) {
        cout << "  * Есть несохраненные изменения в памяти!\n";
    }
    cout << "\n >> Выберите действие: ";
}

int main() {
    init_console();
    system("cls");

    const string report_file = "report.txt";
    vector<Grouped> current_data;
    vector<BackupPoint> backups_history;
    bool modified = false;

    // 1. Поиск и загрузка сохраненных ранее бэкапов при повторном запуске программы
    load_existing_backups(backups_history);

    // 2. Первичная загрузка рабочего отчета
    if (!load_report(report_file, current_data)) {
        cout << "=========================================================\n";
        cout << " [!] Внимание: Файл '" << report_file << "' не найден.\n";
        cout << " Скопируйте файл report.txt из Задания 2 в папку программы,\n";
        cout << " либо создайте чистую базу прямо сейчас.\n";
        cout << "=========================================================\n";
        if (confirm("Создать чистый report.txt?")) {
            save_report(report_file, current_data);
        }
        else {
            return 0;
        }
    }
    else {
        cout << " [OK] Файл '" << report_file << "' успешно загружен ("
            << current_data.size() << " записей).\n";
    }

    if (!backups_history.empty()) {
        cout << " [OK] Обнаружено ранее созданных бэкапов: " << backups_history.size() << " шт.\n";
    }
    else {
        // Если бэкапов еще не было, фиксируем стартовый бэкап
        create_backup(backups_history, current_data, "Исходное состояние файла при запуске программы");
    }

    system("pause");
    system("cls");

    // Главный цикл
    while (true) {
        system("cls");
        draw_menu(modified, (int)backups_history.size(), (int)current_data.size());

        char choice = get_key();
        cout << choice << endl;

        if (choice == '0') {
            if (modified) {
                if (confirm("Есть несохраненные правки. Записать в " + report_file + " перед выходом?")) {
                    save_report(report_file, current_data);
                    cout << " [OK] Данные сохранены в " << report_file << endl;
                }
            }
            if (confirm("Вы точно хотите выйти из программы?")) break;
        }
        else if (choice == '1') {
            system("cls");
            show_data(current_data);
            system("pause");
        }
        else if (choice == '2') {
            system("cls");
            view_single_record(current_data);
            system("pause");
        }
        else if (choice == '3') {
            system("cls");
            if (edit_field(current_data)) modified = true;
            system("pause");
        }
        else if (choice == '4') {
            system("cls");
            if (add_info_row(current_data)) modified = true;
            system("pause");
        }
        else if (choice == '5') {
            system("cls");
            if (delete_info_row(current_data)) modified = true;
            system("pause");
        }
        else if (choice == '6') {
            system("cls");
            if (save_report(report_file, current_data)) {
                cout << "\n [УСПЕХ] Все изменения записаны в '" << report_file
                    << "' СТРОГО БЕЗ ИЗМЕНЕНИЯ ФОРМАТА!\n";
                modified = false;
            }
            else {
                cout << "\n [ОШИБКА] Не удалось записать в файл!\n";
            }
            system("pause");
        }
        else if (choice == '7') {
            handle_backups_menu(backups_history, current_data, modified);
        }
    }

    system("cls");
    cout << "Работа завершена. До свидания!\n";
    return 0;
}