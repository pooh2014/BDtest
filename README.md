# BDtest
Описание

Данная программа реализует управление списком объектов с возможностью их сортировки и группировки по различным критериям (расстояние, имя, тип, время создания). Объекты могут храниться в текстовом файле и базе данных SQLite, предоставляя различные операции по добавлению, сортировке и извлечению данных.

Основные функции программы:
1. Добавление объекта: Ввод информации о новом объекте (имя, координаты, тип, время создания) и его добавление в список.
2. Группировка объектов:
  - По расстоянию (на основе координат объекта).
  - По имени (в алфавитном порядке).
  - По типу (с возможностью указать минимальное количество объектов одного типа для группировки).
  - По времени создания.
3. Сохранение в текстовый файл: Список объектов может быть сохранён в файл для дальнейшего использования.
4. Работа с базой данных SQLite:
  - Создание таблицы для хранения объектов.
  - Сохранение объектов в базу данных.
  - Извлечение данных из базы данных и сохранение в файл.
5. Чтение данных из файла: Программа может загружать объекты из файла для дальнейшей работы.
6. objects1.txt - файл с объектами.
7. testBDForSort.db - база данных для QLite.

Установка и запуск:
  1. Склонируйте репозиторий:
    git clone https://github.com/pooh2014/BDtest.git

  2. Убедитесь, что у вас установлена библиотека SQLite3:
    На Linux:
      sudo apt-get install sqlite3 libsqlite3-dev
    На Windows:
      Скачайте SQLite и настройте окружение.
      Для Visual Studio сделайте следующие настройки:
     
        a) Свойства проекта - Свойства конфигурации - С/С++ - Общие - Доп. каталоги включения - Добавляем папку из проекта SQLite и звтем Ok.
     
        b) Свойства проекта - Компоновщик -Ввод - Доп. зависимости - Пишем sqlite3.lib и затем Ok.
     
        c) Свойства проекта - Общие - Доп каталог библиотек - Добавляем папку из проекта SQLite и звтем Ok.
  
  4. Соберите проект с помощью компилятора, например g++:
    g++ main.cpp -o program -lsqlite3
  
  5. Запустите программу:
    ./program

Требования:
  - Компилятор C++ (например, g++).
  - SQLite3.

#Лицензия:
Этот проект распространяется под лицензией MIT.
