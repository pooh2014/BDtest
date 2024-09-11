#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <ctime>
#include <map>
#include <iomanip> 
#include <chrono>

#include <sqlite3.h>


struct Object {
    std::string name;
    double x;
    double y;
    std::string type;
    long long creation_time;

    Object(std::string n, double x_coord, double y_coord, std::string t, long long time)
        : name(n), x(x_coord), y(y_coord), type(t), creation_time(time) {}

    double getDistance() const {
        return sqrt(x * x + y * y);
    }
};


class ObjectList {
private:
    std::vector<Object> objects;

public:

    std::vector<Object> getObjects() {
        return objects;
    }

    //Читаем из файла данные
    void readFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Ошибка открытия файла: " << filename << std::endl;
            return;
        }

        std::string name, type;
        double x, y, creation_time;
        while (file >> name >> x >> y >> type >> creation_time) {
            objects.emplace_back(name, x, y, type, creation_time);
        }
        file.close();
    }

    void addObject(const Object& obj) {
        objects.push_back(obj);
    }

    //Печатаем список объетов
    void printingByObjects() {
        for (const auto obj : objects)
        {
            std::cout << "name = " << obj.name << " x = " << obj.x << " y = " << obj.y << " type = " << obj.type << " time = " << obj.creation_time << std::endl;
        }
    }

    // Сортировка по времени
    void sortByObjectsForTime(std::vector<Object> tmpObjects) {
        if (tmpObjects.size() > 1)
            for (const auto obj : tmpObjects)
                std::sort(tmpObjects.begin(), tmpObjects.end(), [](const Object& a, const Object& b) {
                return a.creation_time < b.creation_time;
            });
    }

    //Группируем по дистанции
    void groupByDistance() {
        std::sort(objects.begin(), objects.end(), [](const Object& a, const Object& b) {
            return a.getDistance() < b.getDistance();
        });

        std::vector<Object> group1, group2, group3, group4;
        for (const auto& obj : objects) {
            double distance = obj.getDistance();
            if (distance <= 100) {
                group1.push_back(obj);
            }
            else if (distance <= 1000) {
                group2.push_back(obj);
            }
            else if (distance <= 10000) {
                group3.push_back(obj);
            }
            else {
                group4.push_back(obj);
            }
        }

        // Сохраняем сгруппированные данные обратно в список объектов
        objects.clear();
        objects.insert(objects.end(), group1.begin(), group1.end());
        objects.insert(objects.end(), group2.begin(), group2.end());
        objects.insert(objects.end(), group3.begin(), group3.end());
        objects.insert(objects.end(), group4.begin(), group4.end());
    }

    //Группируем по имени
    void groupByName() {
        std::sort(objects.begin(), objects.end(), [](const Object& a, const Object& b) {
            return a.name < b.name;
        });

        std::vector<Object> group_hash;
        std::map<char, std::vector<Object>> groups;
        for (const auto& obj : objects)
        {
            char first_char = obj.name[0];
            if (isalpha(first_char)) {
                groups[toupper(first_char)].push_back(obj);
            }
            else {
                group_hash.push_back(obj);
            }
        }

        // Сохраняем сгруппированные данные обратно в список объектов
        objects.clear();
        for (auto& group : groups) {
            objects.insert(objects.end(), group.second.begin(), group.second.end());
        }
        objects.insert(objects.end(), group_hash.begin(), group_hash.end());
    }

    //Группируем по типу
    void groupByType(int N) {
        std::map<std::string, std::vector<Object>> type_groups;
        std::vector<Object> other_group;

        for (const auto& obj : objects) {
            type_groups[obj.type].push_back(obj);
        }

        objects.clear();
        for (auto& group : type_groups) {
            if (group.second.size() >= N) {
                std::sort(group.second.begin(), group.second.end(), [](const Object& a, const Object& b) {
                    return a.name < b.name;
                });
                objects.insert(objects.end(), group.second.begin(), group.second.end());
            }
            else {
                other_group.insert(other_group.end(), group.second.begin(), group.second.end());
            }
        }
        // Сохраняем сгруппированные данные обратно в список объектов
        objects.insert(objects.end(), other_group.begin(), other_group.end());
    }

    //Группируем по времени
    void groupByTime() {
        std::time_t now = std::time(nullptr);

        // Группы объектов
        std::vector<Object> today, tomorrow, this_week, this_month, this_year, earlier;

        // Получаем текущее время в виде структуры tm
        std::tm* now_tm = std::localtime(&now);

        // Определяем начало дня, недели, месяца и года
        std::tm start_of_day = *now_tm;
        start_of_day.tm_hour = 0;
        start_of_day.tm_min = 0;
        start_of_day.tm_sec = 0;
        std::time_t today_start = std::mktime(&start_of_day);

        std::time_t week_start = today_start + 86400 * 7;
        std::tm* tmp = std::localtime(&week_start);// для отладки

        std::tm start_of_month = start_of_day;
        start_of_month.tm_mon = now_tm->tm_mon + 1;
        std::time_t month_start = std::mktime(&start_of_month);

        std::tm start_of_year = start_of_day;
        start_of_year.tm_year = now_tm->tm_year + 1;
        std::time_t year_start = std::mktime(&start_of_year);

        for (const auto& obj : objects) {
            std::time_t obj_time = static_cast<std::time_t>(obj.creation_time);

            if (obj_time >= today_start && obj_time < today_start + 86400) {
                today.push_back(obj);
            }
            else if (obj_time >= today_start + 86400 && obj_time < today_start + 2 * 86400) {
                tomorrow.push_back(obj);
            }
            else if (obj_time >= today_start + 2 * 86400 && obj_time < week_start) {
                this_week.push_back(obj);
            }
            else if (obj_time >= week_start && obj_time < month_start) {
                this_month.push_back(obj);
            }
            else if (obj_time >= month_start && obj_time < year_start) {
                this_year.push_back(obj);
            }
            else {
                earlier.push_back(obj);
            }
        }
        //Сортируем внутри каждой временной группы 
        sortByObjectsForTime(today);
        sortByObjectsForTime(tomorrow);
        sortByObjectsForTime(this_week);
        sortByObjectsForTime(this_month);
        sortByObjectsForTime(this_year);
        sortByObjectsForTime(earlier);

        //Вы объектов по группам
        std::cout << "today" << std::endl;
        for (const auto obj : today)
            std::cout << "name = " << obj.name << " x = " << obj.x << " y = " << obj.y << " type = " << obj.type << " time = " << obj.creation_time << std::endl;
        std::cout << "tomorrow" << std::endl;
        for (const auto obj : tomorrow)
            std::cout << "name = " << obj.name << " x = " << obj.x << " y = " << obj.y << " type = " << obj.type << " time = " << obj.creation_time << std::endl;
        std::cout << "this_week" << std::endl;
        for (const auto obj : this_week)
            std::cout << "name = " << obj.name << " x = " << obj.x << " y = " << obj.y << " type = " << obj.type << " time = " << obj.creation_time << std::endl;
        std::cout << "this_month" << std::endl;
        for (const auto obj : this_month)
            std::cout << "name = " << obj.name << " x = " << obj.x << " y = " << obj.y << " type = " << obj.type << " time = " << obj.creation_time << std::endl;
        std::cout << "this_year" << std::endl;
        for (const auto obj : this_year)
            std::cout << "name = " << obj.name << " x = " << obj.x << " y = " << obj.y << " type = " << obj.type << " time = " << obj.creation_time << std::endl;
        std::cout << "earlier" << std::endl;
        for (const auto obj : earlier)
            std::cout << "name = " << obj.name << " x = " << obj.x << " y = " << obj.y << " type = " << obj.type << " time = " << obj.creation_time << std::endl;
        std::cout << std::endl;


        // Сохраняем сгруппированные данные обратно в список объектов
        objects.clear();
        objects.insert(objects.end(), today.begin(), today.end());
        objects.insert(objects.end(), tomorrow.begin(), tomorrow.end());
        objects.insert(objects.end(), this_week.begin(), this_week.end());
        objects.insert(objects.end(), this_month.begin(), this_month.end());
        objects.insert(objects.end(), this_year.begin(), this_year.end());
        objects.insert(objects.end(), earlier.begin(), earlier.end());
    }

    //Сохраняем в файл
    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "Ошибка открытия файла: " << filename << std::endl;
            return;
        }
        for (const auto& obj : objects) {
            file << obj.name << " " << obj.x << " " << obj.y << " " << obj.type << " " << std::fixed << std::setprecision(6) << obj.creation_time << std::endl;
        }
        file.close();
    }
};
    
// Создание таблицы
bool createBD(sqlite3* db, char* errMessage) {
    int exit = 0;
    const char* createTableSQL = "CREATE TABLE IF NOT EXISTS Object("
                                 "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "Name TEXT NOT NULL, "
                                 "X REAL NOT NULL, "
                                 "Y REAL NOT NULL, "
                                 "Type TEXT NOT NULL, "
                                 "Time REAL NOT NULL);";
    exit = sqlite3_exec(db, createTableSQL, 0, 0, &errMessage);
    if (exit != SQLITE_OK) {
        std::cerr << "Ошибка создания таблицы: " << errMessage << std::endl;
        sqlite3_free(errMessage);
    }
    else {
        std::cout << "Таблица успешно создана!" << std::endl;
    }
    return exit;
};

// Вставка данных из Objects
void insertProduct(sqlite3* db, std::vector<Object> objects) {
    for (auto& objectItem : objects) {
        std::string sql = "INSERT INTO Object (Name, X, Y, Type, Time) VALUES (?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt;
        int b = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_text(stmt, 1, objectItem.name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 2, objectItem.x);
            sqlite3_bind_double(stmt, 3, objectItem.y);
            sqlite3_bind_text(stmt, 4, objectItem.type.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_double(stmt, 5, objectItem.creation_time);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Ошибка вставки данных: " << sqlite3_errmsg(db) << std::endl;
            }
            else {
                std::cout << "Объект \"" << objectItem.name << "\" успешно добавлен!" << std::endl;
            }
        }
        else {
            std::cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << std::endl;
        }
        sqlite3_finalize(stmt);
    }
};

// Перенос в файл из SQLite
void fetchObjects(sqlite3* db) {
    const char* selectSQL = "SELECT * FROM Object;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, 0) == SQLITE_OK) {
        std::ofstream outFile("objects1.txt");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* name = sqlite3_column_text(stmt, 1);
            double x = sqlite3_column_double(stmt, 2);
            double y = sqlite3_column_double(stmt, 3);
            const unsigned char* type = sqlite3_column_text(stmt, 4);
            double creation_time = sqlite3_column_double(stmt, 5);

            // Запись в файл
            outFile << name << " " << x << " " << y << " " << type << " " << creation_time << std::endl;
            std::cout << "ID: " << id
                << ", Name: " << name
                << ", x: " << x
                << ", y: " << y
                << ", type: " << type
                << ", time: " << creation_time << std::endl; // Для вывода на экран
        }
        outFile.close();
        std::cout << "Данные продуктов записаны в файл objects1.txt." << std::endl;
    }
    else {
        std::cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
};

//Проверка существования ранее созданной таблицы в базе данных, т.к. при открытии если нет бд он ее создаст
bool tableExists(sqlite3* db, const std::string& tableName) {
    std::string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + tableName + "';";
    sqlite3_stmt* stmt;  
    // Подготовка SQL запроса
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка подготовки SQL запроса: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    // Выполнение запроса и проверка результата
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    // Освобождение ресурсов
    sqlite3_finalize(stmt);

    return exists;
}


int main() {
    setlocale(LC_ALL, "Russian");

    static const std::string filePath = "objects1.txt";
    ObjectList objList;
    objList.readFromFile(filePath);

    sqlite3* db;
    char* errMessage = 0;
    static const std::string nameDB = "testBDForSort.db";
       
    // Открытие базы данных
    int exit = sqlite3_open(nameDB.c_str(), &db);
    if (exit) {
        std::cerr << "Ошибка открытия базы данных: " << sqlite3_errmsg(db) << std::endl;
        return exit;
    }
    else {
        std::cout << "База данных успешно открыта!" << std::endl;
    }
    std::string nameTable = "Objects";

    //Предлагаем создать таблицу объектов
    if (tableExists(db, nameTable))
    {
        char otvet;
        std::cout << "Создать новую таблицу объектов?(y/n) : ";
        std::cin >> otvet;
        if (otvet == 'y' || otvet == 'Y')
            createBD(db, errMessage);
    }
        
    while (true) {
        int action;
        std::cout << "Выберите действие:\n";
        std::cout << "1.  Добавить объект\n";
        std::cout << "2.  Группировать по расстоянию\n";
        std::cout << "3.  Группировать по имени\n";
        std::cout << "4.  Группировать по типу\n";
        std::cout << "5.  Группировать по времени\n";
        std::cout << "6.  Распечатать содержимое\n";
        std::cout << "7.  Сохранить в файл\n";
        std::cout << "8.  Сохранить в SQLite BD\n";
        std::cout << "9.  Копировать в файл из BD SQLite\n";
        std::cout << "10: Загрузить из BD SQLite\n";
        std::cout << "0.  Выйти\n";
        std::cin >> action;

        switch (action) {
        case 1: {
            std::string name, type;
            double x, y, creation_time;
            std::cout << "Введите имя: ";
            std::cin >> name;
            std::cout << "\nВведите координату x: ";
            std::cin >> x;
            std::cout << "\nВведите координату y: ";
            std::cin >> y;
            std::cout << "\nУкажите тип: ";
            std::cin >> type;
            std::cout << "\nВведите время создания: ";
            std::cin >> creation_time;

            objList.addObject(Object(name, x, y, type, creation_time));
            break;
        }
        case 2:
            objList.groupByDistance();
            break;
        case 3:
            objList.groupByName();
            break;
        case 4:
            int N;
            std::cout << "Введите минимальное количество объектов одного типа для группировки: ";
            std::cin >> N;
            objList.groupByType(N);
            break;
        case 5:
            objList.groupByTime();
            break;
        case 6:
            std::cout << "\nСписок обектов: \n";
            objList.printingByObjects();
            break;
        case 7:
            objList.saveToFile(filePath);
            break;
        case 8:
            insertProduct(db, objList.getObjects());
            break;
        case 9: 
            fetchObjects(db);
            break;
        case 10:
            fetchObjects(db);
            objList.readFromFile(filePath);
            break;
        case 0:
            return 0;
        default:
            std::cout << "Неверный выбор. Повторите попытку.\n";
        }
    }
    // Закрытие базы данных
    sqlite3_close(db);
}