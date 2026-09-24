#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif

// Вспомогательная функция: обрезка пробелов
std::string trim_c(const char* str) {
    if (!str) return "";
    while (isspace(static_cast<unsigned char>(*str))) ++str;
    if (*str == '\0') return "";

    const char* end = str + strlen(str) - 1;
    while (end > str && isspace(static_cast<unsigned char>(*end))) --end;

    return std::string(str, end - str + 1);
}

// Безопасное чтение строки с проверкой переполнения и поддержкой "0" для выхода
std::string inputLineWithOverflowCheck() {
    char buf[102];  // 100 символов + '\n' + '\0'

    while (true) {
        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("Ошибка ввода.\n");
            return "";
        }

        size_t len = strlen(buf);
        if (len == 0) {
            // Пустая строка — вернём пустую строку, обработка будет выше
            return "";
        }

        // Обрезаем \n и \r
        if (buf[len - 1] == '\n') {
            buf[--len] = '\0';
            if (len > 0 && buf[len - 1] == '\r') {
                buf[--len] = '\0';
            }
        }
        else {
            // Переполнение: введено >=100 символов без \n
            printf("Слишком длинный ввод! Максимум 100 символов. Повторите.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }

        // Проверка: строго "0"
        if (buf[0] == '0' && len == 1) {
            return "0";
        }
        if (buf[0] == '0' && len == 2 && buf[1] == '\r') {
            return "0";
        }

        // Проверка на только пробелы/табы
        bool onlyWhitespace = true;//флаг для проверки пустого ввода
        for (size_t i = 0; i < len; ++i) {
            if (!isspace(static_cast<unsigned char>(buf[i]))) {
                onlyWhitespace = false;
                break;
            }
        }
        if (onlyWhitespace) {
            return "";  // Пустой ввод — обработка выше
        }

        return std::string(buf);
    }
}

// Ввод целого числа в диапазоне с поддержкой пустого ввода
int inputIntC(const char* prompt, int min, int max) {
    while (true) {
        printf("%s", prompt);
        fflush(stdout);

        char buf[32];
        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("Ошибка ввода.\n");
            continue;
        }

        size_t len = strlen(buf);
        if (len == 0) {
            printf("Пустой ввод. Повторите.\n");
            continue;
        }

        if (buf[len - 1] == '\n') {
            buf[--len] = '\0';
            if (len > 0 && buf[len - 1] == '\r') {
                buf[--len] = '\0';
            }
        }

        bool onlyWhitespace = true;//флаг для проверки пустого ввода
        for (size_t i = 0; i < len; ++i) {
            if (!isspace(static_cast<unsigned char>(buf[i]))) {
                onlyWhitespace = false;
                break;
            }
        }
        if (onlyWhitespace) {
            printf("Пустой ввод. Повторите.\n");
            continue;
        }

        char* endptr;
        long val = strtol(buf, &endptr, 10);
        while (*endptr && isspace(static_cast<unsigned char>(*endptr))) {
            ++endptr;
        }

        if (*endptr != '\0' || val < min || val > max) {
            printf("Введите целое число от %d до %d.\n", min, max);
            continue;
        }

        return static_cast<int>(val);
    }
}

// Ожидание '0'–'5'
char key() {
    char c;
    while (true) {
        c = _getch();
        if (c >= '0' && c <= '5') {
            putchar(c);
            putchar('\n');
            fflush(stdout);
            return c;
        }
    }
}

// Ожидание '0' или '1'
char key10() {
    char c;
    while (true) {
        c = _getch();
        if (c == '0' || c == '1') {
            putchar(c);
            putchar('\n');
            fflush(stdout);
            return c;
        }
    }
}

// Ожидание '1'
char key1() {
    char c;
    while (true) {
        c = _getch();
        if (c == '1') {
            putchar(c);
            putchar('\n');
            fflush(stdout);
            return c;
        }
    }
}

// Проверка: кириллическая буква в CP1251
inline bool isCyrillic(unsigned char c) {
    return (c >= 0xC0 && c <= 0xDF) || (c >= 0xE0 && c <= 0xFF);
}

// Безопасная проверка цифры
inline bool isDigit(unsigned char c) {
    return (c >= '0' && c <= '9');
}

// Имя спецсимвола для сообщений
const char* getSymbolName(unsigned char c) {
    switch (c) {
    case ',': return "запятая";
    case ';': return "точка с запятой";
    case ':': return "двоеточие";
    case '!': return "восклицательный знак";
    case '?': return "вопросительный знак";
    case '(': return "открывающая скобка";
    case ')': return "закрывающая скобка";
    case '[': return "открывающая квадратная скобка";
    case ']': return "закрывающая квадратная скобка";
    case '{': return "открывающая фигурная скобка";
    case '}': return "закрывающая фигурная скобка";
    case '#': return "решётка";
    case '@': return "собака";
    case '$': return "доллар";
    case '%': return "процент";
    case '^': return "крышка";
    case '&': return "амперсанд";
    case '*': return "звёздочка";
    case '_': return "нижнее подчёркивание";
    case '+': return "плюс";
    case '=': return "равно";
    case '<': return "знак меньше";
    case '>': return "знак больше";
    case '/': return "слэш";
    case '\\': return "обратный слэш";
    case '|': return "вертикальная черта";
    case '~': return "тильда";
    case '`': return "гравис";
    case '"': return "кавычки";
    default: return nullptr;
    }
}

// ФУНКЦИЯ ВВОДА — со всеми проверками
std::string inputValidatedStringC(const char* field_name) {
    while (true) {
        printf("%s: ", field_name);
        fflush(stdout);

        std::string raw = inputLineWithOverflowCheck();
        if (raw.empty()) {
            // Пустой ввод или только пробелы
            printf("Пустой ввод. Повторите.\n");
            continue;
        }

        // Проверка: строго "0"
        if (raw == "0") {
            return "0";
        }

        std::string s = trim_c(raw.c_str());
        if (s.empty()) {
            printf("Поле не может быть пустым.\n");
            continue;
        }

        // Запрет опасных символов (для файловой системы и безопасности)
        const char* forbidden = "<>:\"/\\|?*";
        for (const char* p = forbidden; *p; ++p) {
            if (s.find(*p) != std::string::npos) {
                printf("Недопустимый символ: '%c' — запрещён (опасен для файлов).\n", *p);
                continue;
            }
        }

        size_t n = s.length();
        unsigned char first = static_cast<unsigned char>(s[0]);
        unsigned char last = static_cast<unsigned char>(s[n - 1]);

        // Проверка начала/конца
        if (first == ' ' || first == '-' || first == '\'' || first == '"' || first == '.') {
            printf("Строка не должна начинаться с пробела, дефиса, апострофа, кавычек или точки.\n");
            continue;
        }
        if (last == ' ' || last == '-' || last == '\'' || last == '"') {
            printf("Строка не должна заканчиваться пробелом, дефисом, апострофом или кавычками.\n");
            continue;
        }

        // Состояния
        bool valid = true;//флаг на пропуск строки(найден не найден плохой симол)
        bool hasLetter = false; //флаг на буквы кириллицы
        int space_run = 0, hyphen_run = 0, apostrophe_run = 0, quote_run = 0;
        int apostrophe_count = 0, quote_count = 0, dot_count = 0;

        for (size_t i = 0; i < n; ++i) {
            unsigned char c = static_cast<unsigned char>(s[i]);

            if (c == ' ') {
                space_run++;
                hyphen_run = apostrophe_run = quote_run = 0;
                if (space_run > 1) {
                    printf("Нельзя использовать более одного пробела подряд.\n");
                    valid = false;
                    break;
                }
            }
            else if (c == '-') {
                space_run = apostrophe_run = quote_run = 0;
                hyphen_run++;
                if (hyphen_run > 1) {
                    printf("Нельзя использовать два дефиса подряд.\n");
                    valid = false;
                    break;
                }
                if (i == 0 || i == n - 1 ||
                    !isCyrillic(static_cast<unsigned char>(s[i - 1])) ||
                    !isCyrillic(static_cast<unsigned char>(s[i + 1]))) {
                    printf("Дефис должен находиться между двумя кириллическими буквами.\n");
                    valid = false;
                    break;
                }
            }
            else if (c == '\'') {
                space_run = hyphen_run = quote_run = 0;
                apostrophe_run++;
                if (apostrophe_run > 1) {
                    printf("Нельзя использовать два апострофа подряд.\n");
                    valid = false;
                    break;
                }
                apostrophe_count++;
                if (apostrophe_count > 1) {
                    printf("Разрешён только один апостроф.\n");
                    valid = false;
                    break;
                }
                if (i == 0 || i == n - 1 ||
                    !isCyrillic(static_cast<unsigned char>(s[i - 1])) ||
                    !isCyrillic(static_cast<unsigned char>(s[i + 1]))) {
                    printf("Апостроф должен находиться между двумя кириллическими буквами.\n");
                    valid = false;
                    break;
                }
            }
            else if (c == '"') {
                space_run = hyphen_run = apostrophe_run = 0;
                quote_run++;
                if (quote_run > 1) {
                    printf("Нельзя использовать две кавычки подряд.\n");
                    valid = false;
                    break;
                }
                quote_count++;
                if (quote_count > 2) {
                    printf("Разрешено не более двух кавычек.\n");
                    valid = false;
                    break;
                }
                bool afterStartOrSpace = (i == 0) || (s[i - 1] == ' ');
                bool beforeEndOrSpace = (i == n - 1) || (s[i + 1] == ' ');
                if (!afterStartOrSpace && !beforeEndOrSpace) {
                    printf("Кавычки должны окружать слово, а не стоять внутри него.\n");
                    valid = false;
                    break;
                }
            }
            else if (c == '.') {
                space_run = hyphen_run = apostrophe_run = quote_run = 0;
                dot_count++;
                if (dot_count > 1) {
                    printf("Разрешена только одна точка.\n");
                    valid = false;
                    break;
                }
                if (i != n - 1) {
                    printf("Точка разрешена только в конце строки.\n");
                    valid = false;
                    break;
                }
            }
            else if (isCyrillic(c)) {
                space_run = hyphen_run = apostrophe_run = quote_run = 0;
                hasLetter = true;
            }
            else if (isDigit(c)) {
                printf("Цифры запрещены. Введите '0' отдельно для выхода.\n");
                valid = false;
                break;
            }
            else {
                const char* name = getSymbolName(c);
                if (name) {
                    printf("Недопустимый символ: '%c' — %s.\n", c, name);
                }
                else if (c < 32 || c == 127) {
                    printf("Управляющий символ (код %d) недопустим.\n", c);
                }
                else {
                    printf("Недопустимый символ: '%c' (код %d).\n", c, c);
                }
                valid = false;
                break;
            }
        }

        if (!valid) continue;
        if (!hasLetter) {
            printf("Строка должна содержать хотя бы одну кириллическую букву.\n");
            continue;
        }
        if (quote_count == 1) {
            printf("Кавычки должны быть парными: либо нет, либо \"так\".\n");
            continue;
        }

        return s;
    }
}

// Структуры данных
struct Publication {
    std::string title;
    std::string type;
    std::string publisher;
    int year;
};

struct Editor {
    std::string title;
    std::string address;
    std::string editor_surname;
};

// Вспомогательные функции
int getCurrentYear() {
    std::time_t t = std::time(nullptr);
    std::tm lt{};
#ifdef _WIN32
    localtime_s(&lt, &t);
#else
    localtime_r(&t, &lt);
#endif
    return 1900 + lt.tm_year;
}

std::string getCurrentDateTimeStr() {
    std::time_t t = std::time(nullptr);
    std::tm lt{};
#ifdef _WIN32
    localtime_s(&lt, &t);
#else
    localtime_r(&t, &lt);
#endif
    std::ostringstream oss;
    oss << std::put_time(&lt, "%d.%m.%Y %H:%M");
    return oss.str();
}

// === MAIN ===
int main() {
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif
    setlocale(LC_ALL, "");
    int case3 = 0;   //счетчик для перезаписи изданий
    int case4 = 0;  //счетчик для перезаписи редакций
    std::vector<Publication> publications;
    std::vector<Editor> editors;

    bool pub_header_written = false;
    bool ed_header_written = false;
    while (true) {
        printf("==============================\n");
        printf("Здравствуйте, вы в меню программы для учёта изданий и редакций.\n");
        printf("1 - добавить издание в publications.txt\n");
        printf("2 - добавить редакцию в editors.txt\n");
        printf("3 - очистить publications.txt и записать заново\n");
        printf("4 - очистить editors.txt и записать заново\n");
        printf("5 - Инструкция по работе с программой\n");
        printf("0 - выйти\n");
        printf("==============================\n");
        printf("\nМеню: 1, 2, 3, 4, 5, 0 -> ");

        while (true) {
            char choice = key();

            switch (choice) {
            case '1': {
                printf("\n-> Добавление изданий. Введите '0' в любом поле для выхода (с подтверждением).\n");
                int cur_year = getCurrentYear();

                while (true) {
                    Publication p;
                    bool exitConfirmed = false;
                    bool shouldSaveWithDefaults = false; //флаги для ввода в файл

                    // --- Название ---
                    while (true) {
                        p.title = inputValidatedStringC("Название издания");
                        if (p.title == "0") {
                            printf("Вы точно хотите завершить ввод? (1 - да, 0 - нет): ");
                            if (key10() == '1') {
                                exitConfirmed = true;
                                break;
                            }
                            else {
                                printf("Повторите ввод названия.\n");
                            }
                        }
                        else {
                            break;
                        }
                    }
                    if (exitConfirmed) break;

                    // --- Вид издания ---
                    while (true) {
                        p.type = inputValidatedStringC("Вид издания");
                        if (p.type == "0") {
                            printf("Вы точно хотите завершить ввод? (1 - да, 0 - нет): ");
                            if (key10() == '1') {
                                p.type = "Отсутствует";
                                p.publisher = "Отсутствует";
                                p.year = 1;
                                shouldSaveWithDefaults = true;
                                break;
                            }
                            else {
                                printf("Повторите ввод вида издания.\n");
                            }
                        }
                        else {
                            break;
                        }
                    }
                    if (shouldSaveWithDefaults) {
                        publications.push_back(p);
                    }
                    else {
                        // --- Издающая организация ---
                        while (true) {
                            p.publisher = inputValidatedStringC("Издающая организация");
                            if (p.publisher == "0") {
                                printf("Вы точно хотите завершить ввод? (1 - да, 0 - нет): ");
                                if (key10() == '1') {
                                    p.publisher = "Отсутствует";
                                    p.year = 1;
                                    shouldSaveWithDefaults = true;
                                    break;
                                }
                                else {
                                    printf("Повторите ввод издающей организации.\n");
                                }
                            }
                            else {
                                break;
                            }
                        }
                        if (shouldSaveWithDefaults) {
                            publications.push_back(p);
                        }
                        else {
                            p.year = inputIntC(("Год выпуска (1900-" + std::to_string(cur_year) + "): ").c_str(), 1900, cur_year);
                            publications.push_back(p);
                            case3++;
                        }
                    }

                    // Сохранение
                    Publication& entry = publications.back();
                    int entry_num = static_cast<int>(publications.size());

                    FILE* file = fopen("publications.txt", "a");
                    if (!file) {
                        printf("Ошибка открытия файла publications.txt\n");
                        break;
                    }

                    fprintf(file, "[%d] \"%s\"\n", entry_num, entry.title.c_str());
                    fprintf(file, "    Вид: %s\n", entry.type.c_str());
                    fprintf(file, "    Издатель: %s\n", entry.publisher.c_str());
                    fprintf(file, "    Год: %d\n", entry.year);
                    fprintf(file, "----------------------------------------\n");
                    fclose(file);

                    printf("Издание \"%s\" добавлено как [%d].\n", entry.title.c_str(), entry_num);

                    if (shouldSaveWithDefaults || exitConfirmed) {
                        break;
                    }
                }
                break;
            }

            case '2': {
                printf("\n-> Добавление редакций. Введите '0' в любом поле для выхода (с подтверждением).\n");

                while (true) {
                    Editor e;
                    bool exitConfirmed = false;
                    bool shouldSaveWithDefaults = false;//флаги для ввода в файл

                    // --- Название ---
                    while (true) {
                        e.title = inputValidatedStringC("Название издания");
                        if (e.title == "0") {
                            printf("Вы точно хотите завершить ввод? (1 - да, 0 - нет): ");
                            if (key10() == '1') {
                                exitConfirmed = true;
                                break;
                            }
                            else {
                                printf("Повторите ввод названия.\n");
                            }
                        }
                        else {
                            break;
                        }
                    }
                    if (exitConfirmed) break;

                    // --- Адрес ---
                    while (true) {
                        e.address = inputValidatedStringC("Адрес редакции");
                        if (e.address == "0") {
                            printf("Вы точно хотите завершить ввод? (1 - да, 0 - нет): ");
                            if (key10() == '1') {
                                e.address = "Отсутствует";
                                e.editor_surname = "Отсутствует";
                                shouldSaveWithDefaults = true;
                                break;
                            }
                            else {
                                printf("Повторите ввод адреса.\n");
                            }
                        }
                        else {
                            break;
                        }
                    }
                    if (shouldSaveWithDefaults) {
                        editors.push_back(e);
                    }
                    else {
                        // --- Фамилия ---
                        while (true) {
                            e.editor_surname = inputValidatedStringC("Фамилия редактора");
                            if (e.editor_surname == "0") {
                                printf("Вы точно хотите завершить ввод? (1 - да, 0 - нет): ");
                                if (key10() == '1') {
                                    e.editor_surname = "Отсутствует";
                                    shouldSaveWithDefaults = true;
                                    break;
                                }
                                else {
                                    printf("Повторите ввод фамилии редактора.\n");
                                }
                            }
                            else {
                                break;
                            }
                        }
                        if (shouldSaveWithDefaults) {
                            editors.push_back(e);
                        }
                        else {
                            editors.push_back(e);
                            case4++;
                        }
                    }

                    //  Сохранение
                    Editor& entry = editors.back();
                    int entry_num = static_cast<int>(editors.size());

                    FILE* file = fopen("editors.txt", "a");
                    if (!file) {
                        printf("Ошибка открытия файла editors.txt\n");
                        break;
                    }

                    fprintf(file, "[%d] Издание: \"%s\"\n", entry_num, entry.title.c_str());
                    fprintf(file, "    Адрес: %s\n", entry.address.c_str());
                    fprintf(file, "    Редактор: %s\n", entry.editor_surname.c_str());
                    fprintf(file, "----------------------------------------\n");
                    fclose(file);

                    printf("Редакция для \"%s\" добавлена как [%d].\n", entry.title.c_str(), entry_num);

                    if (shouldSaveWithDefaults || exitConfirmed) {
                        break;
                    }
                }
                break;
            }

            case '3': {
                if (case3 == 0) {
                    printf("Нельзя очистить publications.txt — в него ещё ничего не записано.\n");
                    printf("\nМеню: 1, 2, 3, 4, 0 -> ");
                    continue;
                }
                printf("Очистить publications.txt и начать заново? (1 - да, 0 - нет): \n");
                if (key10() != '1') break;

                FILE* f = fopen("publications.txt", "w");
                if (f) fclose(f);
                else { printf("Не удалось очистить файл.\n"); break; }
                printf("Файл очищен и готов к работе\n");
                publications.clear();
                pub_header_written = false;
                case3 = 0;

                printf("\n-> Ввод новых изданий. '0' в любом поле — выход с подтверждением.\n");
                int cur_year = getCurrentYear();

                while (true) {
                    Publication p;
                    bool exitConfirmed = false;
                    bool shouldSaveWithDefaults = false; //так же флаги для записи в файл при миссклике

                    // --- Название ---
                    while (true) {
                        p.title = inputValidatedStringC("Название издания");
                        if (p.title == "0") {
                            printf("Вы точно хотите завершить ввод? (1 - да, 0 - нет): ");
                            if (key10() == '1') {
                                exitConfirmed = true;
                                break;
                            }
                            else {
                                printf("Повторите ввод названия.\n");
                            }
                        }
                        else {
                            break;
                        }
                    }
                    if (exitConfirmed) break;

                    // --- Вид ---
                    while (true) {
                        p.type = inputValidatedStringC("Вид издания");
                        if (p.type == "0") {
                            printf("Вы точно хотите завершить ввод? (1 - да, 0 - нет): ");
                            if (key10() == '1') {
                                p.type = "Отсутствует";
                                p.publisher = "Отсутствует";
                                p.year = 1;
                                shouldSaveWithDefaults = true;
                                break;
                            }
                            else {
                                printf("Повторите ввод вида издания.\n");
                            }
                        }
                        else {
                            break;
                        }
                    }
                    if (shouldSaveWithDefaults) {
                        publications.push_back(p);
                    }
                    else {
                        // --- Издатель ---
                        while (true) {
                            p.publisher = inputValidatedStringC("Издающая организация");
                            if (p.publisher == "0") {
                                printf("Вы точно хотите завершить ввод? (1 - да, 0 - нет): ");
                                if (key10() == '1') {
                                    p.publisher = "Отсутствует";
                                    p.year = 1;
                                    shouldSaveWithDefaults = true;
                                    break;
                                }
                                else {
                                    printf("Повторите ввод издающей организации.\n");
                                }
                            }
                            else {
                                break;
                            }
                        }
                        if (shouldSaveWithDefaults) {
                            publications.push_back(p);
                        }
                        else {
                            p.year = inputIntC(("Год (1900-" + std::to_string(cur_year) + "): ").c_str(), 1900, cur_year);
                            publications.push_back(p);
                            case3++;
                        }
                    }

                    //  Сохранение
                    Publication& entry = publications.back();
                    int entry_num = static_cast<int>(publications.size());

                    FILE* file = fopen("publications.txt", "a");
                    if (!file) break;

                    fprintf(file, "[%d] \"%s\"\n", entry_num, entry.title.c_str());
                    fprintf(file, "    Вид: %s\n", entry.type.c_str());
                    fprintf(file, "    Издатель: %s\n", entry.publisher.c_str());
                    fprintf(file, "    Год: %d\n", entry.year);
                    fprintf(file, "----------------------------------------\n");
                    fclose(file);

                    printf("Записано [%d] \"%s\".\n", entry_num, entry.title.c_str());

                    if (shouldSaveWithDefaults || exitConfirmed) {
                        break;
                    }
                }
                break;
            }

            case '4': {
                if (case4 == 0) {
                    printf("Нельзя очистить editors.txt — в него ещё ничего не записано.\n");
                    printf("\nМеню: 1, 2, 3, 4, 0 -> ");
                    continue;
                }
                printf("Очистить editors.txt и начать заново? (1 - да, 0 - нет): ");
                if (key10() != '1') break;

                FILE* f = fopen("editors.txt", "w");
                if (f) fclose(f);
                else { printf("Не удалось очистить файл.\n"); break; }
                printf("Файл очищен и готов к работе\n");
                editors.clear();
                ed_header_written = false;
                case4 = 0;

                printf("\n-> Ввод новых редакций. '0' в любом поле — выход с подтверждением.\n");

                while (true) {
                    Editor e;
                    bool exitConfirmed = false;
                    bool shouldSaveWithDefaults = false;    //флаги для записи в файл при миссклике

                    // --- Название ---
                    while (true) {
                        e.title = inputValidatedStringC("Название издания");
                        if (e.title == "0") {
                            printf("Вы точно хотите завершить ввод? (1 - да, 0 - нет): ");
                            if (key10() == '1') {
                                exitConfirmed = true;
                                break;
                            }
                            else {
                                printf("Повторите ввод названия.\n");
                            }
                        }
                        else {
                            break;
                        }
                    }
                    if (exitConfirmed) break;

                    // --- Адрес ---
                    while (true) {
                        e.address = inputValidatedStringC("Адрес редакции");
                        if (e.address == "0") {
                            printf("Вы точно хотите завершить ввод? (1 - да, 0 - нет): ");
                            if (key10() == '1') {
                                e.address = "Отсутствует";
                                e.editor_surname = "Отсутствует";
                                shouldSaveWithDefaults = true;
                                break;
                            }
                            else {
                                printf("Повторите ввод адреса.\n");
                            }
                        }
                        else {
                            break;
                        }
                    }
                    if (shouldSaveWithDefaults) {
                        editors.push_back(e);
                    }
                    else {
                        // --- Фамилия ---
                        while (true) {
                            e.editor_surname = inputValidatedStringC("Фамилия редактора");
                            if (e.editor_surname == "0") {
                                printf("Вы точно хотите завершить ввод? (1 - да, 0 - нет): ");
                                if (key10() == '1') {
                                    e.editor_surname = "Отсутствует";
                                    shouldSaveWithDefaults = true;
                                    break;
                                }
                                else {
                                    printf("Повторите ввод фамилии редактора.\n");
                                }
                            }
                            else {
                                break;
                            }
                        }
                        if (shouldSaveWithDefaults) {
                            editors.push_back(e);
                        }
                        else {
                            editors.push_back(e);
                            case4++;
                        }
                    }

                    //  Сохранение
                    Editor& entry = editors.back();
                    int entry_num = static_cast<int>(editors.size());

                    FILE* file = fopen("editors.txt", "a");
                    if (!file) break;

                    fprintf(file, "[%d] Издание: \"%s\"\n", entry_num, entry.title.c_str());
                    fprintf(file, "    Адрес: %s\n", entry.address.c_str());
                    fprintf(file, "    Редактор: %s\n", entry.editor_surname.c_str());
                    fprintf(file, "----------------------------------------\n");
                    fclose(file);

                    printf("Записано [%d] \"%s\".\n", entry_num, entry.title.c_str());

                    if (shouldSaveWithDefaults || exitConfirmed) {
                        break;
                    }
                }
                break;
            }
            case '5': {
                printf("Инструкция:\n");
                printf("Правила ввода:\n");
                printf("- Вводить можно только кириллицу\n");
                printf("- Вводить можно не более 100 символов\n");
                printf("- Запрещены цифры(кроме 0) и символы <>:\"/\\|?*\n");
                printf("- Дефис и апостроф можно вводить только между буквами\n");
                printf("- Точку можно вводить только в конце строки\n");
                printf("- Пробелы и дефисы можно вводить только между словами не подрят\n");
                printf("- Пустой ввод (только Enter) нельзя, программа попросит повторить ввод\n");
                printf("\n");
                printf("Примечание:\n");
                printf("- При вводе 0(защита от missclick) программа уведомит вас точно ли вы хотите завершить ввод\n");
                printf("- Если вы завершите ввод на поле(кроме названия издания — остальные поля заполнятся записью Отсутствует,а год цифрой 1\n");
                printf("\n");
                printf("Готовы? Нажмите 1 для возврата в меню: ");
                key1();
                break;
            }
            case '0': {
                printf("Вы уверены, что хотите выйти? (1 - да, 0 - нет): ");
                if (key10() == '1') {
                    printf("До свидания.\n");
                    return 0;
                }
                break;
            }

            default:
                printf("Неверный выбор.\n");//впринципе его срабатывание не возможно но в case он нужен(неотьемлемая часть)
                break;
            }

            break;
        }
    }

    return 0;
}