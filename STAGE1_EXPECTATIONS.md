# Ожидаемые результаты Этапа 1 — Инфраструктура

**Этап:** 1 из 7  
**Название:** Базовая инфраструктура  
**Длительность:** 2-3 недели  
**Статус:** 📋 Готов к реализации

---

## 📋 Обзор

Этот документ описывает ожидаемые результаты от реализации Этапа 1 (Инфраструктура). После завершения проект должен иметь рабочую основу для всех последующих этапов.

---

## ✅ Ожидаемые результаты

### 1. Структура проекта создана

**Ожидаемое:**
```
h323p/
├── CMakeLists.txt              ✅ Основная сборка
├── README.md                   ✅ Описание проекта
├── LICENSE                     ✅ Лицензия MPL-2.0
├── .gitignore                  ✅ Правила игнорирования Git
├── .github/
│   └── workflows/
│       └── build.yml           ✅ CI/CD пайплайн
├── cmake/
│   ├── FindH323Plus.cmake      ✅ Модуль поиска H323Plus
│   └── FindPTLib.cmake         ✅ Модуль поиска PTLib
├── src/
│   ├── main.cpp                ✅ Точка входа
│   ├── cli/
│   │   ├── cli_parser.hpp      ✅ Заголовки CLI парсера
│   │   ├── cli_parser.cpp      ✅ Реализация CLI парсера
│   │   └── commands.hpp        ✅ Определения команд
│   ├── core/
│   │   ├── call_manager.hpp    ✅ Заготовка Call Manager
│   │   ├── call_manager.cpp
│   │   ├── call.hpp            ✅ Заготовка Call
│   │   ├── call.cpp
│   │   ├── endpoint.hpp        ✅ Заготовка Endpoint
│   │   └── endpoint.cpp
│   ├── h323/
│   │   └── (пусто для Этапа 2)
│   ├── media/
│   │   └── (пусто для Этапа 2)
│   ├── scenario/
│   │   └── (пусто для Этапа 4)
│   ├── load/
│   │   └── (пусто для Этапа 5)
│   ├── fuzz/
│   │   └── (пусто для Этапа 6)
│   ├── stats/
│   │   └── (пусто для Этапа 5)
│   └── utils/
│       ├── logging.hpp         ✅ Система логирования
│       ├── logging.cpp
│       ├── timer.hpp           ✅ Утилиты таймеров
│       ├── timer.cpp
│       ├── utils.hpp           ✅ Строковые/файловые утилиты
│       └── utils.cpp
├── tests/
│   ├── CMakeLists.txt          ✅ Сборка тестов
│   ├── test_cli_parser.cpp     ✅ Тесты CLI парсера
│   ├── test_logging.cpp        ✅ Тесты логирования
│   ├── test_timer.cpp          ✅ Тесты таймеров
│   └── test_utils.cpp          ✅ Тесты утилит
└── scenarios/
    └── (пусто для Этапа 4)
```

**Критерии приемки:**
- [ ] Все директории созданы
- [ ] Все файлы-заглушки созданы
- [ ] Структура файлов соответствует дизайну

---

### 2. Система сборки CMake работает

**Ожидаемое:**
```bash
$ mkdir build && cd build
$ cmake .. -DH323PLUS_DIR=/path/to/h323plus
-- Configuring done
-- Generating done
-- Build files have been written to: /path/to/build

$ make -j4
[ 10%] Building CXX object CMakeFiles/h323p.dir/src/main.cpp.o
[ 20%] Building CXX object CMakeFiles/h323p.dir/src/cli/cli_parser.cpp.o
...
[100%] Linking CXX executable h323p
[100%] Built target h323p

$ ./h323p --version
h323p version 0.1.0
H.323 Test Utility
Based on H323Plus + PTLib
```

**Критерии приемки:**
- [ ] Конфигурация CMake завершается без ошибок
- [ ] Проект успешно собирается через `make`
- [ ] Исполняемый файл `h323p` создан
- [ ] Команда `--version` работает
- [ ] Команда `--help` работает

---

### 3. CLI парсер функционален

**Ожидаемое:**
```bash
# Вывод помощи
$ ./h323p --help

H.323 Test Utility (h323p) v0.1.0

Использование:
  h323p <command> [options]

Команды:
  call       Инициирование вызова
  listen     Режим прослушивания входящих вызовов
  load       Нагрузочное тестирование
  fuzz       Fuzzing тестирование
  register   Регистрация на Gatekeeper
  info       Информация о системе

Примеры:
  h323p call 192.168.1.100
  h323p load 192.168.1.100 -c 100 -r 10
  h323p listen -m 50

# Неизвестная команда
$ ./h323p unknown
Ошибка: Неизвестная команда 'unknown'
Используйте './h323p --help' для получения информации.
```

**Критерии приемки:**
- [ ] `--help` отображает информацию об использовании
- [ ] `--version` отображает версию
- [ ] Неизвестные команды показывают сообщение об ошибке
- [ ] Все 6 команд распознаются (call, listen, load, fuzz, register, info)
- [ ] Глобальные опции парсятся корректно (--log-file, --log-level, --quiet)

---

### 4. Система логирования работает

**Ожидаемое:**
```bash
# Консольное логирование (по умолчанию)
$ ./h323p call 192.168.1.100
2026-03-16 10:00:00.123 [INFO] Запуск h323p v0.1.0
2026-03-16 10:00:00.124 [INFO] Команда: call
2026-03-16 10:00:00.125 [INFO] Назначение: 192.168.1.100

# Логирование в файл
$ ./h323p call 192.168.1.100 --log-file h323p.log --log-level debug
$ cat h323p.log
2026-03-16 10:00:00.123 [DEBUG] Инициализация логгера
2026-03-16 10:00:00.124 [INFO] Запуск h323p v0.1.0
2026-03-16 10:00:00.125 [DEBUG] CLI успешно распарсен

# Тихий режим
$ ./h323p call 192.168.1.100 --quiet
# (нет вывода)

# Уровни логирования
$ ./h323p call 192.168.1.100 --log-level debug
2026-03-16 10:00:00.123 [DEBUG] Отладочное сообщение
2026-03-16 10:00:00.124 [INFO] Информационное сообщение
2026-03-16 10:00:00.125 [WARN] Предупреждение
2026-03-16 10:00:00.126 [ERROR] Ошибка
```

**Критерии приемки:**
- [ ] Сообщения лога включают метку времени, уровень и сообщение
- [ ] Вывод в консоль работает (stdout/stderr)
- [ ] Вывод в файл работает (--log-file)
- [ ] Уровни логирования работают (debug, info, warn, error)
- [ ] Тихий режим подавляет вывод (--quiet)
- [ ] Асинхронное логирование реализовано (неблокирующее)

---

### 5. Утилиты таймеров работают

**Ожидаемое:**
```cpp
// Тест ElapsedTimer
ElapsedTimer timer;
std::this_thread::sleep_for(std::chrono::milliseconds(100));
auto elapsed = timer.elapsed();
assert(elapsed.count() >= 90);  // С допуском

// Тест CountdownTimer
CountdownTimer timer;
bool callbackCalled = false;
timer.start(std::chrono::milliseconds(100), [&callbackCalled]() {
    callbackCalled = true;
});
std::this_thread::sleep_for(std::chrono::milliseconds(150));
assert(callbackCalled == true);
```

**Критерии приемки:**
- [ ] `ElapsedTimer` измеряет прошедшее время
- [ ] `CountdownTimer` вызывает callback после истечения времени
- [ ] Функции остановки/сброса таймера работают
- [ ] Потокобезопасные операции таймера

---

### 6. Утилиты реализованы

**Ожидаемое:**
```cpp
// Строковые утилиты
utils::trim("  hello  ") == "hello"
utils::split("a,b,c", ',') == ["a", "b", "c"]
utils::join(["a", "b", "c"], ",") == "a,b,c"
utils::toLower("HELLO") == "hello"
utils::toUpper("hello") == "HELLO"
utils::startsWith("hello world", "hello") == true
utils::endsWith("hello world", "world") == true

// Парсинг адресов
utils::parseAddress("192.168.1.1:1720") == Address{"192.168.1.1", 1720, "tcp"}
utils::parseAddress("invalid") == nullopt

// Файловые утилиты
utils::fileExists("/etc/passwd") == true
utils::fileExists("/nonexistent") == false
utils::writeFile("/tmp/test.txt", "content") == true
utils::readFile("/tmp/test.txt") == "content"
```

**Критерии приемки:**
- [ ] Все строковые утилиты реализованы
- [ ] Парсинг адресов работает (формат host:port)
- [ ] Файловые утилиты работают (exists, read, write)
- [ ] Все утилиты протестированы

---

### 7. Модульные тесты проходят

**Ожидаемое:**
```bash
$ ctest --output-on-failure
Test project /path/to/build
    Start 1: UtilsTest.Trim
1/4 Test #1: UtilsTest.Trim ...................   Passed    0.00 sec
    Start 2: TimerTest.ElapsedTimer
2/4 Test #2: TimerTest.ElapsedTimer ...........   Passed    0.15 sec
    Start 3: LoggingTest.LogLevelConversion
3/4 Test #3: LoggingTest.LogLevelConversion ...   Passed    0.00 sec
    Start 4: CliParserTest.HelpOutput
4/4 Test #4: CliParserTest.HelpOutput .........   Passed    0.00 sec

100% tests passed!
```

**Критерии приемки:**
- [ ] Все модульные тесты написаны (CppUTest)
- [ ] Все тесты проходят (100%)
- [ ] Покрытие тестами > 80% для кода Этапа 1
- [ ] Тесты запускаются через `ctest`

---

### 8. Интеграционные тесты работают

**Ожидаемое:**
```bash
# Базовый интеграционный тест
$ ./h323p_tests -g BasicIntegrationTest
Tests running in group BasicIntegrationTest:
....
OK (4 tests, 0 failures)

# CLI интеграция
$ ./h323p info --version | grep "h323p version"
h323p version 0.1.0
```

**Критерии приемки:**
- [ ] Интеграционные тесты созданы
- [ ] CLI команды протестированы end-to-end
- [ ] Все интеграционные тесты проходят

---

### 9. CI/CD пайплайн настроен

**Ожидаемое:**
```yaml
# .github/workflows/build.yml
name: Сборка

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Установка зависимостей
        run: sudo apt-get install -y cmake libssl-dev libcpputest-dev
      - name: Настройка CMake
        run: cmake -B build -DCMAKE_BUILD_TYPE=Release
      - name: Сборка
        run: cmake --build build
      - name: Тесты
        run: ctest --test-dir build --output-on-failure
```

**Критерии приемки:**
- [ ] GitHub Actions workflow создан
- [ ] Сборка проходит на GitHub Actions
- [ ] Тесты запускаются при каждом push
- [ ] Бейдж сборки в README.md

---

### 10. Корректное завершение работы реализовано

**Ожидаемое:**
```bash
# Запуск вызова (зависнет без медиа)
$ ./h323p call 192.168.1.100
2026-03-16 10:00:00.123 [INFO] Запуск h323p v0.1.0
2026-03-16 10:00:00.124 [INFO] Команда: call
^C2026-03-16 10:00:05.456 [INFO] Запрошено завершение работы...
2026-03-16 10:00:05.457 [INFO] Очистка ресурсов
2026-03-16 10:00:05.458 [INFO] До свидания!
```

**Критерии приемки:**
- [ ] SIGINT (Ctrl+C) обрабатывается корректно
- [ ] SIGTERM обрабатывается корректно
- [ ] Ресурсы очищаются при выходе
- [ ] Нет утечек памяти (valgrind clean)

---

## 📊 Итоговый чек-лист

### Результаты по коду

- [ ] `CMakeLists.txt` — Конфигурация сборки
- [ ] `src/main.cpp` — Точка входа с корректным завершением
- [ ] `src/cli/cli_parser.*` — Реализация CLI парсера
- [ ] `src/utils/logging.*` — Асинхронная система логирования
- [ ] `src/utils/timer.*` — Утилиты таймеров
- [ ] `src/utils/utils.*` — Строковые/файловые утилиты
- [ ] `src/core/*` — Файлы-заглушки для Этапа 2
- [ ] `tests/*` — Модульные и интеграционные тесты

### Результаты по документации

- [ ] `README.md` — Описание проекта
- [ ] `INSTALL.md` — Инструкция по установке
- [ ] Комментарии к коду — Все публичные API задокументированы
- [ ] `CHANGELOG.md` — Начальная запись создана

### Метрики качества

- [ ] Сборка проходит без предупреждений (`-Wall -Wextra -Wpedantic`)
- [ ] Все тесты проходят (100%)
- [ ] Покрытие тестами > 80%
- [ ] Нет утечек памяти (valgrind)
- [ ] CI/CD пайплайн зелёный

---

## 🚀 Следующие шаги (Этап 2)

После успешного завершения Этапа 1, перейти к Этапу 2:

**Этап 2: Стек H.323 (H.225)**
- Реализовать обёртку H323Plus
- Реализовать H.225 RAS (регистрация на Gatekeeper)
- Реализовать H.225 Q.931 (сигнализация вызовов)
- Реализовать Call Manager
- Реализовать команды `call` и `listen`

---

*Версия документа: 1.0*  
*Создан: 2026-03-16*  
*Репозиторий: https://github.com/Ardameon/h323p*
