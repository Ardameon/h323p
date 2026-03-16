# Этап 1: Базовая инфраструктура

**Длительность:** 2-3 недели  
**Статус:** 📋 Готов к реализации

---

## 📋 Содержание

1. [Обзор этапа](#-обзор-этапа)
2. [Задачи этапа](#-задачи-этапа)
3. [Структура проекта](#-структура-проекта)
4. [Детальная реализация](#-детальная-реализация)
5. [Критерии завершения](#-критерии-завершения)
6. [Проверка результатов](#-проверка-результатов)

---

## 📌 Обзор этапа

Цель Этапа 1 — создать каркас проекта и базовые утилиты, необходимые для дальнейшей разработки H.323 тестовой утилиты.

### Результаты этапа

```
✅ Проект собирается через CMake
✅ Работают команды --version и --help
✅ Настроена система логирования
✅ Реализован CLI парсер
✅ Система таймеров для пауз и таймаутов
✅ Базовые утилиты (строки, файлы, сеть)
✅ Unit тесты проходят
```

---

## 📊 Задачи этапа

| № | Задача | Длительность | Статус |
|---|--------|--------------|--------|
| 1.1 | Настройка сборки (CMakeLists.txt) | 2 дня | ⏳ |
| 1.2 | CLI парсер | 3 дня | ⏳ |
| 1.3 | Логирование | 2 дня | ⏳ |
| 1.4 | Система таймеров | 2 дня | ⏳ |
| 1.5 | Базовые утилиты | 2 дня | ⏳ |
| 1.6 | Unit тесты | 2 дня | ⏳ |
| **Итого** | | **13 дней** | |

---

## 🏗️ Структура проекта

```
h323p/
├── CMakeLists.txt                    # Основная сборка
├── README.md                         # Быстрый старт
├── LICENSE                           # Лицензия (MPL)
├── .gitignore                        # Git ignore
├── .github/
│   └── workflows/
│       └── build.yml                 # CI/CD сборка
├── cmake/
│   ├── FindH323Plus.cmake            # Поиск H323Plus
│   └── FindPTLib.cmake               # Поиск PTLib
├── src/
│   ├── main.cpp                      # Точка входа
│   ├── cli/
│   │   ├── cli_parser.hpp            # CLI парсер (заголовки)
│   │   ├── cli_parser.cpp            # CLI парсер (реализация)
│   │   └── commands.hpp              # CLI команды
│   ├── core/
│   │   ├── call_manager.hpp          # Управление вызовами (пустой)
│   │   ├── call_manager.cpp
│   │   ├── call.hpp                  # Класс вызова (пустой)
│   │   ├── call.cpp
│   │   ├── endpoint.hpp              # H.323 endpoint (пустой)
│   │   └── endpoint.cpp
│   ├── scenario/
│   │   ├── scenario_parser.hpp       # XML парсер (пустой)
│   │   ├── scenario_parser.cpp
│   │   ├── scenario.hpp
│   │   ├── scenario.cpp
│   │   ├── actions.hpp
│   │   └── actions.cpp
│   ├── h323/
│   │   ├── h323_wrapper.hpp          # Обёртка H323Plus (пустой)
│   │   ├── h323_wrapper.cpp
│   │   ├── ras.hpp
│   │   ├── ras.cpp
│   │   ├── q931.hpp
│   │   ├── q931.cpp
│   │   ├── h245.hpp
│   │   └── h245.cpp
│   ├── media/
│   │   ├── rtp_session.hpp           # RTP сессия (пустой)
│   │   ├── rtp_session.cpp
│   │   ├── codec_manager.hpp
│   │   ├── codec_manager.cpp
│   │   ├── media_generator.hpp
│   │   └── media_generator.cpp
│   ├── stats/
│   │   ├── statistics.hpp            # Статистика (пустой)
│   │   ├── statistics.cpp
│   │   ├── cdr.hpp
│   │   └── cdr.cpp
│   └── utils/
│       ├── logging.hpp               # Логирование ✅
│       ├── logging.cpp               # Логирование ✅
│       ├── timer.hpp                 # Таймеры ✅
│       ├── timer.cpp                 # Таймеры ✅
│       ├── utils.hpp                 # Утилиты ✅
│       └── utils.cpp                 # Утилиты ✅
├── scenarios/
│   ├── basic_call.xml                # Базовый вызов
│   ├── gatekeeper_register.xml       # Регистрация на GK
│   ├── load_test.xml                 # Нагрузочный тест
│   └── fuzzing.xml                   # Fuzzing тест
├── tests/
│   ├── CMakeLists.txt                # Сборка тестов
│   ├── test_cli_parser.cpp           # Тесты CLI
│   ├── test_logging.cpp              # Тесты логирования
│   ├── test_timer.cpp                # Тесты таймеров
│   └── test_utils.cpp                # Тесты утилит
└── docs/
    ├── architecture.md               # Архитектура
    ├── cli_reference.md              # CLI справочник
    └── development.md                # Разработка
```

---

## 🔧 Детальная реализация

### 1.1 CMakeLists.txt

**Файл: `CMakeLists.txt`**

```cmake
cmake_minimum_required(VERSION 3.16)
project(h323p 
    VERSION 0.1.0 
    DESCRIPTION "H.323 Test Utility"
    LANGUAGES CXX
)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции сборки
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(USE_H323PLUS "Use H323Plus library" ON)

# Путь для поиска модулей
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# ============================================================================
# Зависимости
# ============================================================================

# H323Plus (обязательная)
if(USE_H323PLUS)
    find_package(H323Plus REQUIRED)
    find_package(PTLib REQUIRED)
endif()

# Опциональные зависимости
find_package(OpenSSL REQUIRED)
find_package(CLI11 CONFIG)
find_package(spdlog CONFIG)
find_package(pugixml CONFIG)

# Проверка наличия обязательных зависимостей
if(NOT H323Plus_FOUND)
    message(FATAL_ERROR "H323Plus not found. Set H323PLUS_DIR.")
endif()

# ============================================================================
# Исходные файлы
# ============================================================================

set(H323P_SOURCES
    src/main.cpp
    src/cli/cli_parser.cpp
    src/core/call_manager.cpp
    src/core/call.cpp
    src/core/endpoint.cpp
    src/scenario/scenario_parser.cpp
    src/scenario/scenario.cpp
    src/scenario/actions.cpp
    src/h323/h323_wrapper.cpp
    src/h323/ras.cpp
    src/h323/q931.cpp
    src/h323/h245.cpp
    src/media/rtp_session.cpp
    src/media/codec_manager.cpp
    src/media/media_generator.cpp
    src/stats/statistics.cpp
    src/stats/cdr.cpp
    src/utils/logging.cpp
    src/utils/timer.cpp
    src/utils/utils.cpp
)

set(H323P_HEADERS
    src/cli/cli_parser.hpp
    src/cli/commands.hpp
    src/core/call_manager.hpp
    src/core/call.hpp
    src/core/endpoint.hpp
    src/scenario/scenario_parser.hpp
    src/scenario/scenario.hpp
    src/scenario/actions.hpp
    src/h323/h323_wrapper.hpp
    src/h323/ras.hpp
    src/h323/q931.hpp
    src/h323/h245.hpp
    src/media/rtp_session.hpp
    src/media/codec_manager.hpp
    src/media/media_generator.hpp
    src/stats/statistics.hpp
    src/stats/cdr.hpp
    src/utils/logging.hpp
    src/utils/timer.hpp
    src/utils/utils.hpp
)

# ============================================================================
# Целевой исполняемый файл
# ============================================================================

add_executable(h323p ${H323P_SOURCES} ${H323P_HEADERS})

# Include directories
target_include_directories(h323p PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${H323PLUS_INCLUDE_DIRS}
    ${PTLIB_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(h323p PRIVATE
    ${H323PLUS_LIBRARIES}
    ${PTLIB_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Опциональные линковки
if(CLI11_FOUND)
    target_link_libraries(h323p PRIVATE CLI11::CLI11)
else()
    message(WARNING "CLI11 not found, using internal parser")
endif()

if(spdlog_FOUND)
    target_link_libraries(h323p PRIVATE spdlog::spdlog)
endif()

if(pugixml_FOUND)
    target_link_libraries(h323p PRIVATE pugixml::pugixml)
endif()

# Компиляционные флаги
target_compile_options(h323p PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3>
)

# ============================================================================
# Тесты
# ============================================================================

if(BUILD_TESTS)
    enable_testing()
    find_package(CppUTest CONFIG)

    if(CppUTest_FOUND)
        add_subdirectory(tests)
    else()
        message(WARNING "CppUTest not found, tests disabled")
    endif()
endif()

# ============================================================================
# Установка
# ============================================================================

install(TARGETS h323p DESTINATION bin)
install(DIRECTORY scenarios/ DESTINATION share/h323p/scenarios)

# ============================================================================
# Документация (опционально)
# ============================================================================

if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
        doxygen_add_docs(docs ${CMAKE_SOURCE_DIR}/src)
    endif()
endif()
```

---

### 1.2 CLI парсер

**Файл: `src/cli/commands.hpp`**

```cpp
#pragma once

namespace h323p {

// Типы команд
enum class CommandType {
    CALL,
    LISTEN,
    LOAD,
    FUZZ,
    REGISTER,
    INFO,
    UNKNOWN
};

// Структура конфигурации команды
struct CommandConfig {
    CommandType type = CommandType::UNKNOWN;
    std::string destination;
    std::string scenario;
    std::string gatekeeper;
    std::string username;
    std::string password;
    
    // Настройки вызовов
    size_t maxCalls = 10;
    size_t repeatCount = 1;
    bool cycleMode = false;
    
    // Медиа настройки
    bool video = false;
    bool h239 = false;
    std::string audioCodec = "G.711";
    std::string videoCodec = "H.264";
    int mediaEncBits = 0;  // 0 = отключено, 128/192/256
    
    // Тайминги
    int tminCall = 10;
    int tmaxCall = 30;
    
    // Трассировка
    int traceLevel = 0;
    std::string outputFile;
    std::string cdrFile;
    
    // TLS
    bool tls = false;
    std::string tlsCert;
    std::string tlsKey;
    std::string tlsCaFile;
    
    // Порты
    int tcpBase = 0;
    int udpBase = 0;
    int rtpBase = 0;
    
    // Fuzzing
    std::string fuzzType;
    int fuzzPercentage = 0;
    
    // Глобальные
    std::string configFile;
    std::string logFile;
    std::string logLevel = "info";
    bool quiet = false;
    bool verbose = false;
};

} // namespace h323p
```

**Файл: `src/cli/cli_parser.hpp`**

```cpp
#pragma once

#include "commands.hpp"
#include <string>

namespace h323p {

// Парсер командной строки
class CliParser {
public:
    CliParser();
    ~CliParser();
    
    // Парсинг аргументов
    CommandConfig parse(int argc, char* argv[]);
    
    // Проверка успешности парсинга
    bool isValid() const { return valid_; }
    
    // Сообщение об ошибке
    std::string getError() const { return error_; }
    
    // Помощь
    std::string getHelp() const;
    
private:
    bool valid_ = false;
    std::string error_;
    
    // Парсинг подкоманд
    CommandType parseCommand(const std::string& cmd);
    
    // Валидация конфигурации
    bool validateConfig(CommandConfig& config);
};

} // namespace h323p
```

**Файл: `src/cli/cli_parser.cpp`**

```cpp
#include "cli_parser.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

namespace h323p {

CliParser::CliParser() = default;
CliParser::~CliParser() = default;

CommandType CliParser::parseCommand(const std::string& cmd) {
    if (cmd == "call") return CommandType::CALL;
    if (cmd == "listen") return CommandType::LISTEN;
    if (cmd == "load") return CommandType::LOAD;
    if (cmd == "fuzz") return CommandType::FUZZ;
    if (cmd == "register") return CommandType::REGISTER;
    if (cmd == "info") return CommandType::INFO;
    return CommandType::UNKNOWN;
}

CommandConfig CliParser::parse(int argc, char* argv[]) {
    CommandConfig config;
    
    if (argc < 2) {
        error_ = "Требуется указать команду";
        return config;
    }
    
    // Глобальные опции
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--log-file") == 0 && i + 1 < argc) {
            config.logFile = argv[++i];
        } else if (strcmp(argv[i], "--log-level") == 0 && i + 1 < argc) {
            config.logLevel = argv[++i];
        } else if (strcmp(argv[i], "--quiet") == 0 || strcmp(argv[i], "-q") == 0) {
            config.quiet = true;
        } else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            config.verbose = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            valid_ = false;
            return config;
        } else if (argv[i][0] != '-') {
            config.type = parseCommand(argv[i]);
            // Парсинг опций команды
            for (int j = i + 1; j < argc; j++) {
                if (strcmp(argv[j], "--scenario") == 0 || strcmp(argv[j], "-s") == 0) {
                    config.scenario = argv[++j];
                } else if (strcmp(argv[j], "--gatekeeper") == 0 || strcmp(argv[j], "-g") == 0) {
                    config.gatekeeper = argv[++j];
                } else if (strcmp(argv[j], "--max-calls") == 0 || strcmp(argv[j], "-m") == 0) {
                    config.maxCalls = std::stoul(argv[++j]);
                } else if (strcmp(argv[j], "--video") == 0 || strcmp(argv[j], "-V") == 0) {
                    config.video = true;
                } else if (strcmp(argv[j], "--trace") == 0 || strcmp(argv[j], "-t") == 0) {
                    config.traceLevel++;
                } else if (argv[j][0] != '-' && config.type == CommandType::CALL) {
                    config.destination = argv[j];
                }
            }
            break;
        }
    }
    
    valid_ = (config.type != CommandType::UNKNOWN);
    return config;
}

std::string CliParser::getHelp() const {
    std::ostringstream oss;
    oss << "H.323 Test Utility (h323p) v0.1.0\n\n"
        << "Использование:\n"
        << "  h323p <command> [options]\n\n"
        << "Команды:\n"
        << "  call       Запуск вызова\n"
        << "  listen     Режим прослушивания\n"
        << "  load       Нагрузочное тестирование\n"
        << "  fuzz       Fuzzing тестирование\n"
        << "  register   Регистрация на Gatekeeper\n"
        << "  info       Информация\n\n"
        << "Примеры:\n"
        << "  h323p call 192.168.1.100\n"
        << "  h323p load 192.168.1.100 -c 100 -r 10\n"
        << "  h323p listen -m 50\n"
        << "  h323p info --version\n";
    return oss.str();
}

bool CliParser::validateConfig(CommandConfig& config) {
    // Проверка обязательных параметров
    if (config.type == CommandType::CALL && config.destination.empty()) {
        error_ = "Требуется указать адрес назначения";
        return false;
    }
    
    // Проверка диапазонов
    if (config.maxCalls == 0) {
        error_ = "maxCalls должен быть > 0";
        return false;
    }
    
    return true;
}

} // namespace h323p
```

---

### 1.3 Логирование

**Файл: `src/utils/logging.hpp`**

```cpp
#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace h323p {

// Уровни логирования
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    NONE = 4
};

// Конвертация строки в уровень
inline LogLevel stringToLogLevel(const std::string& level) {
    if (level == "debug") return LogLevel::DEBUG;
    if (level == "info") return LogLevel::INFO;
    if (level == "warn") return LogLevel::WARN;
    if (level == "error") return LogLevel::ERROR;
    return LogLevel::INFO;
}

inline std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

// Класс логгера
class Logger {
public:
    static Logger& instance();
    
    // Инициализация
    void init(LogLevel level = LogLevel::INFO, 
              const std::string& file = "",
              bool quiet = false);
    
    // Методы логирования
    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);
    
    // Установка уровня
    void setLevel(LogLevel level);
    LogLevel getLevel() const { return level_; }
    
private:
    Logger() = default;
    ~Logger();
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void log(LogLevel level, const std::string& msg);
    
    LogLevel level_ = LogLevel::INFO;
    bool quiet_ = false;
    FILE* file_ = nullptr;
};

// Макросы для удобства
#define LOG_DEBUG(msg) h323p::Logger::instance().debug(msg)
#define LOG_INFO(msg) h323p::Logger::instance().info(msg)
#define LOG_WARN(msg) h323p::Logger::instance().warn(msg)
#define LOG_ERROR(msg) h323p::Logger::instance().error(msg)

} // namespace h323p
```

**Файл: `src/utils/logging.cpp`**

```cpp
#include "logging.hpp"
#include <iostream>

namespace h323p {

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

Logger::~Logger() {
    if (file_) {
        fclose(file_);
    }
}

void Logger::init(LogLevel level, const std::string& file, bool quiet) {
    level_ = level;
    quiet_ = quiet;
    
    if (!file.empty()) {
        file_ = fopen(file.c_str(), "a");
    }
}

void Logger::log(LogLevel level, const std::string& msg) {
    if (level < level_ || quiet_) {
        return;
    }
    
    // Получение текущего времени
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    oss << " [" << logLevelToString(level) << "] " << msg;
    
    std::string line = oss.str();
    
    // Вывод в консоль
    if (!quiet_) {
        if (level >= LogLevel::WARN) {
            std::cerr << line << std::endl;
        } else {
            std::cout << line << std::endl;
        }
    }
    
    // Вывод в файл
    if (file_) {
        fprintf(file_, "%s\n", line.c_str());
        fflush(file_);
    }
}

void Logger::debug(const std::string& msg) { log(LogLevel::DEBUG, msg); }
void Logger::info(const std::string& msg) { log(LogLevel::INFO, msg); }
void Logger::warn(const std::string& msg) { log(LogLevel::WARN, msg); }
void Logger::error(const std::string& msg) { log(LogLevel::ERROR, msg); }

void Logger::setLevel(LogLevel level) { level_ = level; }

} // namespace h323p
```

---

### 1.4 Система таймеров

**Файл: `src/utils/timer.hpp`**

```cpp
#pragma once

#include <chrono>
#include <functional>
#include <thread>
#include <atomic>
#include <map>
#include <memory>

namespace h323p {

// Таймер для измерения длительности
class ElapsedTimer {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::milliseconds;
    
    ElapsedTimer() { start(); }
    
    void start() { startTime_ = Clock::now(); }
    
    Duration elapsed() const {
        return std::chrono::duration_cast<Duration>(Clock::now() - startTime_);
    }
    
    double elapsedSeconds() const {
        return std::chrono::duration<double>(elapsed()).count();
    }
    
    void reset() { start(); }
    
private:
    TimePoint startTime_;
};

// Таймер с обратным отсчётом
class CountdownTimer {
public:
    using Callback = std::function<void()>;
    
    CountdownTimer() = default;
    
    // Запуск таймера
    void start(ElapsedTimer::Duration duration, Callback callback = nullptr);
    
    // Остановка
    void stop();
    
    // Сброс
    void reset();
    
    // Проверка истечения
    bool isExpired() const;
    
    // Оставшееся время
    ElapsedTimer::Duration remaining() const;
    
    // Проверка запуска
    bool isRunning() const { return running_; }
    
private:
    ElapsedTimer::TimePoint startTime_;
    ElapsedTimer::Duration duration_;
    Callback callback_;
    std::atomic<bool> running_{false};
    std::thread thread_;
};

} // namespace h323p
```

**Файл: `src/utils/timer.cpp`**

```cpp
#include "timer.hpp"

namespace h323p {

void CountdownTimer::start(ElapsedTimer::Duration duration, Callback callback) {
    duration_ = duration;
    callback_ = callback;
    startTime_ = ElapsedTimer::Clock::now();
    running_ = true;
    
    if (callback_) {
        thread_ = std::thread([this]() {
            std::this_thread::sleep_for(duration_);
            if (running_) {
                callback_();
            }
        });
    }
}

void CountdownTimer::stop() {
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

void CountdownTimer::reset() {
    stop();
    running_ = false;
}

bool CountdownTimer::isExpired() const {
    if (!running_) return false;
    auto elapsed = std::chrono::duration_cast<ElapsedTimer::Duration>(
        ElapsedTimer::Clock::now() - startTime_);
    return elapsed >= duration_;
}

ElapsedTimer::Duration CountdownTimer::remaining() const {
    if (!running_) return ElapsedTimer::Duration(0);
    auto elapsed = std::chrono::duration_cast<ElapsedTimer::Duration>(
        ElapsedTimer::Clock::now() - startTime_);
    auto rem = duration_ - elapsed;
    return rem.count() > 0 ? rem : ElapsedTimer::Duration(0);
}

} // namespace h323p
```

---

### 1.5 Базовые утилиты

**Файл: `src/utils/utils.hpp`**

```cpp
#pragma once

#include <string>
#include <vector>
#include <optional>

namespace h323p {
namespace utils {

// Строковые утилиты
std::string trim(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
std::string join(const std::vector<std::string>& parts, const std::string& delimiter);
std::string toLower(const std::string& str);
std::string toUpper(const std::string& str);
bool startsWith(const std::string& str, const std::string& prefix);
bool endsWith(const std::string& str, const std::string& suffix);

// Парсинг адресов
struct Address {
    std::string host;
    int port = 0;
    std::string protocol = "tcp";
    
    bool isValid() const { return !host.empty() && port > 0; }
    std::string toString() const;
};

std::optional<Address> parseAddress(const std::string& addr);

// Файловые утилиты
bool fileExists(const std::string& path);
std::string readFile(const std::string& path);
bool writeFile(const std::string& path, const std::string& content);

} // namespace utils
} // namespace h323p
```

**Файл: `src/utils/utils.cpp`**

```cpp
#include "utils.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace h323p {
namespace utils {

std::string trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

std::string join(const std::vector<std::string>& parts, const std::string& delimiter) {
    std::string result;
    for (size_t i = 0; i < parts.size(); i++) {
        if (i > 0) result += delimiter;
        result += parts[i];
    }
    return result;
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string Address::toString() const {
    return host + ":" + std::to_string(port);
}

std::optional<Address> parseAddress(const std::string& addr) {
    Address result;
    
    // Формат: host:port или host:port/protocol
    auto colonPos = addr.find(':');
    if (colonPos == std::string::npos) {
        return std::nullopt;
    }
    
    result.host = addr.substr(0, colonPos);
    
    auto slashPos = addr.find('/', colonPos);
    std::string portStr;
    if (slashPos != std::string::npos) {
        portStr = addr.substr(colonPos + 1, slashPos - colonPos - 1);
        result.protocol = addr.substr(slashPos + 1);
    } else {
        portStr = addr.substr(colonPos + 1);
    }
    
    try {
        result.port = std::stoi(portStr);
    } catch (...) {
        return std::nullopt;
    }
    
    return result.isValid() ? std::optional(result) : std::nullopt;
}

bool fileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) return "";
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file) return false;
    
    file << content;
    return file.good();
}

} // namespace utils
} // namespace h323p
```

---

### 1.6 Точка входа (main.cpp)

**Файл: `src/main.cpp`**

```cpp
#include <iostream>
#include <string>
#include "cli/cli_parser.hpp"
#include "utils/logging.hpp"
#include "utils/utils.hpp"

#define H323P_VERSION "0.1.0"

void printVersion() {
    std::cout << "h323p version " << H323P_VERSION << std::endl;
    std::cout << "H.323 Test Utility" << std::endl;
    std::cout << "Based on H323Plus + PTLib" << std::endl;
}

int main(int argc, char* argv[]) {
    using namespace h323p;
    
    // Парсинг командной строки
    CliParser parser;
    CommandConfig config = parser.parse(argc, argv);
    
    // Обработка --help
    if (!parser.isValid()) {
        std::cout << parser.getHelp() << std::endl;
        return 0;
    }
    
    // Инициализация логирования
    Logger::instance().init(
        stringToLogLevel(config.logLevel),
        config.logFile,
        config.quiet
    );
    
    // Обработка команды info
    if (config.type == CommandType::INFO) {
        printVersion();
        return 0;
    }
    
    // Логирование запуска
    LOG_INFO_FMT("Запуск h323p v%s", H323P_VERSION);
    LOG_INFO_FMT("Команда: %d", static_cast<int>(config.type));
    
    // Заглушка для будущих команд
    switch (config.type) {
        case CommandType::CALL:
            LOG_INFO("Команда: call (в разработке)");
            std::cout << "call command: в разработке..." << std::endl;
            break;
            
        case CommandType::LISTEN:
            LOG_INFO("Команда: listen (в разработке)");
            std::cout << "listen command: в разработке..." << std::endl;
            break;
            
        case CommandType::LOAD:
            LOG_INFO("Команда: load (в разработке)");
            std::cout << "load command: в разработке..." << std::endl;
            break;
            
        case CommandType::FUZZ:
            LOG_INFO("Команда: fuzz (в разработке)");
            std::cout << "fuzz command: в разработке..." << std::endl;
            break;
            
        case CommandType::REGISTER:
            LOG_INFO("Команда: register (в разработке)");
            std::cout << "register command: в разработке..." << std::endl;
            break;
            
        default:
            LOG_ERROR("Неизвестная команда");
            std::cerr << "Ошибка: неизвестная команда" << std::endl;
            return 1;
    }
    
    return 0;
}
```

---

### 1.7 Unit тесты

**Файл: `tests/CMakeLists.txt`**

```cmake
add_executable(h323p_tests
    test_cli_parser.cpp
    test_logging.cpp
    test_timer.cpp
    test_utils.cpp
)

target_link_libraries(h323p_tests PRIVATE
    h323p
    CppUTest::CppUTest
    CppUTest::CppUTestExt
)

include(CTest)
enable_testing()
```

**Файл: `tests/test_utils.cpp`**

```cpp
#include "CppUTest/TestHarness.h"
#include "utils/utils.hpp"

using namespace h323p;

TEST_GROUP(UtilsTest) {
    void setup() override { }
    void teardown() override { }
};

TEST(UtilsTest, Trim) {
    STRCMP_EQUAL("hello", utils::trim("  hello  ").c_str());
    STRCMP_EQUAL("test", utils::trim("test").c_str());
    STRCMP_EQUAL("", utils::trim("   ").c_str());
    STRCMP_EQUAL("a b c", utils::trim("  a b c  ").c_str());
}

TEST(UtilsTest, Split) {
    std::vector<std::string> parts = utils::split("a,b,c", ',');
    LONGS_EQUAL(3, parts.size());
    STRCMP_EQUAL("a", parts[0].c_str());
    STRCMP_EQUAL("b", parts[1].c_str());
    STRCMP_EQUAL("c", parts[2].c_str());
}

TEST(UtilsTest, SplitNoDelimiter) {
    std::vector<std::string> parts = utils::split("abc", ',');
    LONGS_EQUAL(1, parts.size());
    STRCMP_EQUAL("abc", parts[0].c_str());
}

TEST(UtilsTest, Join) {
    std::vector<std::string> parts;
    parts.push_back("a");
    parts.push_back("b");
    parts.push_back("c");
    STRCMP_EQUAL("a,b,c", utils::join(parts, ",").c_str());
    STRCMP_EQUAL("a - b - c", utils::join(parts, " - ").c_str());
}

TEST(UtilsTest, ToLower) {
    STRCMP_EQUAL("hello", utils::toLower("HELLO").c_str());
    STRCMP_EQUAL("hello world", utils::toLower("Hello World").c_str());
}

TEST(UtilsTest, ToUpper) {
    STRCMP_EQUAL("HELLO", utils::toUpper("hello").c_str());
    STRCMP_EQUAL("HELLO WORLD", utils::toUpper("Hello World").c_str());
}

TEST(UtilsTest, StartsWith) {
    CHECK(utils::startsWith("hello world", "hello"));
    CHECK_FALSE(utils::startsWith("hello", "world"));
}

TEST(UtilsTest, EndsWith) {
    CHECK(utils::endsWith("hello world", "world"));
    CHECK_FALSE(utils::endsWith("hello", "world"));
}

TEST(UtilsTest, ParseAddress) {
    std::optional<utils::Address> addr = utils::parseAddress("192.168.1.1:1720");
    CHECK_TRUE(addr.has_value());
    STRCMP_EQUAL("192.168.1.1", addr->host.c_str());
    LONGS_EQUAL(1720, addr->port);
    STRCMP_EQUAL("tcp", addr->protocol.c_str());
}

TEST(UtilsTest, ParseAddressWithProtocol) {
    std::optional<utils::Address> addr = utils::parseAddress("192.168.1.1:1720/tcp");
    CHECK_TRUE(addr.has_value());
    STRCMP_EQUAL("192.168.1.1", addr->host.c_str());
    LONGS_EQUAL(1720, addr->port);
    STRCMP_EQUAL("tcp", addr->protocol.c_str());
}

TEST(UtilsTest, ParseAddressInvalid) {
    CHECK_FALSE(utils::parseAddress("invalid").has_value());
    CHECK_FALSE(utils::parseAddress("192.168.1.1").has_value());
    CHECK_FALSE(utils::parseAddress(":1720").has_value());
}

TEST(UtilsTest, AddressToString) {
    utils::Address addr;
    addr.host = "192.168.1.1";
    addr.port = 1720;
    addr.protocol = "tcp";
    STRCMP_EQUAL("192.168.1.1:1720", addr.toString().c_str());
}

TEST(UtilsTest, FileOperations) {
    // Тест fileExists
    CHECK_FALSE(utils::fileExists("/nonexistent/file.txt"));
    
    // Тест writeFile/readFile
    std::string testPath = "/tmp/h323p_test.txt";
    std::string content = "Hello, World!";
    CHECK_TRUE(utils::writeFile(testPath, content));
    CHECK_TRUE(utils::fileExists(testPath));
    STRCMP_EQUAL(content.c_str(), utils::readFile(testPath).c_str());
    
    // Очистка
    std::remove(testPath.c_str());
}
```

**Файл: `tests/test_timer.cpp`**

```cpp
#include "CppUTest/TestHarness.h"
#include "utils/timer.hpp"
#include <thread>

using namespace h323p;

TEST_GROUP(TimerTest) {
    void setup() override { }
    void teardown() override { }
};

TEST(TimerTest, ElapsedTimer) {
    ElapsedTimer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto elapsed = timer.elapsed();
    CHECK(elapsed.count() >= 90);  // Небольшой допуск
    CHECK(elapsed.count() <= 150);
}

TEST(TimerTest, ElapsedTimerReset) {
    ElapsedTimer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.reset();
    auto elapsed = timer.elapsed();
    CHECK(elapsed.count() < 10);
}

TEST(TimerTest, ElapsedTimerSeconds) {
    ElapsedTimer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    auto seconds = timer.elapsedSeconds();
    CHECK(seconds >= 1.4);
    CHECK(seconds <= 2.0);
}

TEST(TimerTest, CountdownTimer) {
    CountdownTimer timer;
    bool callbackCalled = false;

    timer.start(std::chrono::milliseconds(100), [&callbackCalled]() {
        callbackCalled = true;
    });

    CHECK_FALSE(timer.isExpired());
    CHECK_TRUE(timer.isRunning());

    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    CHECK_TRUE(timer.isExpired());
    CHECK_TRUE(callbackCalled);
}

TEST(TimerTest, CountdownTimerStop) {
    CountdownTimer timer;
    bool callbackCalled = false;

    timer.start(std::chrono::milliseconds(100), [&callbackCalled]() {
        callbackCalled = true;
    });

    timer.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    CHECK_FALSE(callbackCalled);
    CHECK_FALSE(timer.isRunning());
}

TEST(TimerTest, CountdownTimerRemaining) {
    CountdownTimer timer;
    timer.start(std::chrono::milliseconds(500));

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto remaining = timer.remaining();
    CHECK(remaining.count() <= 350);
    CHECK(remaining.count() >= 200);
}
```

**Файл: `tests/test_logging.cpp`**

```cpp
#include "CppUTest/TestHarness.h"
#include "utils/logging.hpp"
#include <fstream>

using namespace h323p;

TEST_GROUP(LoggingTest) {
    void setup() override { }
    void teardown() override { }
};

TEST(LoggingTest, LogLevelConversion) {
    LONGS_EQUAL((int)LogLevel::DEBUG, (int)stringToLogLevel("debug"));
    LONGS_EQUAL((int)LogLevel::INFO, (int)stringToLogLevel("info"));
    LONGS_EQUAL((int)LogLevel::WARN, (int)stringToLogLevel("warn"));
    LONGS_EQUAL((int)LogLevel::ERROR, (int)stringToLogLevel("error"));
    LONGS_EQUAL((int)LogLevel::INFO, (int)stringToLogLevel("unknown"));

    STRCMP_EQUAL("DEBUG", logLevelToString(LogLevel::DEBUG).c_str());
    STRCMP_EQUAL("INFO", logLevelToString(LogLevel::INFO).c_str());
    STRCMP_EQUAL("WARN", logLevelToString(LogLevel::WARN).c_str());
    STRCMP_EQUAL("ERROR", logLevelToString(LogLevel::ERROR).c_str());
}

TEST(LoggingTest, LoggerInstance) {
    auto& logger1 = Logger::instance();
    auto& logger2 = Logger::instance();
    CHECK(&logger1 == &logger2);
}

TEST(LoggingTest, LoggerSetLevel) {
    auto& logger = Logger::instance();
    logger.setLevel(LogLevel::DEBUG);
    LONGS_EQUAL((int)LogLevel::DEBUG, (int)logger.getLevel());

    logger.setLevel(LogLevel::ERROR);
    LONGS_EQUAL((int)LogLevel::ERROR, (int)logger.getLevel());
}

TEST(LoggingTest, LoggerFileOutput) {
    std::string testLogPath = "/tmp/h323p_test.log";
    auto& logger = Logger::instance();

    logger.setLevel(LogLevel::DEBUG);
    logger.init(LogLevel::DEBUG, testLogPath, true);

    logger.info("Test message");

    // Проверка файла
    std::ifstream file(testLogPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    CHECK(content.find("Test message") != std::string::npos);

    // Очистка
    std::remove(testLogPath.c_str());
}
```

---

### 1.8 CI/CD (GitHub Actions)

**Файл: `.github/workflows/build.yml`**

```yaml
name: Build and Test

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

env:
  BUILD_TYPE: Release

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y \
          build-essential \
          cmake \
          libssl-dev \
          libcpputest-dev \
          libcli11-dev \
          libspdlog-dev \
          libpugixml-dev
    
    - name: Configure CMake
      run: cmake -B ${{github.workspace}}/build -DCMAKE_BUILD_TYPE=${{env.BUILD_TYPE}}
    
    - name: Build
      run: cmake --build ${{github.workspace}}/build --config ${{env.BUILD_TYPE}}
    
    - name: Test
      working-directory: ${{github.workspace}}/build
      run: ctest --build-config ${{env.BUILD_TYPE}} --output-on-failure
    
    - name: Upload binary
      uses: actions/upload-artifact@v3
      with:
        name: h323p
        path: ${{github.workspace}}/build/h323p
```

---

## ✅ Критерии завершения

### Чек-лист завершения Этапа 1

```
[ ] 1. CMakeLists.txt создан и настроен
[ ] 2. Структура директорий создана
[ ] 3. main.cpp компилируется
[ ] 4. CLI парсер работает (--help, --version)
[ ] 5. Логирование работает (консоль + файл)
[ ] 6. Таймеры работают (ElapsedTimer, CountdownTimer)
[ ] 7. Утилиты работают (trim, split, parseAddress)
[ ] 8. Unit тесты написаны
[ ] 9. Все тесты проходят
[ ] 10. CI/CD настроен
```

---

## 🔧 Рекомендации по улучшению (из RECOMMENDATIONS.md)

### Рекомендация #6: Асинхронное логирование

**Приоритет:** 🟡 Важный  
**Сложность:** Средняя

**Проблема:** Синхронное логирование блокирует выполнение на I/O операциях.

**Решение:**
```cpp
// src/utils/async_logger.hpp
#pragma once

#include "logging.hpp"
#include <queue>
#include <thread>
#include <atomic>

namespace h323p {

class AsyncLogger {
public:
    static AsyncLogger& instance();
    
    void init(LogLevel level = LogLevel::INFO,
              const std::string& file = "",
              size_t queueSize = 1000);
    
    void log(LogLevel level, const std::string& msg);
    void setLevel(LogLevel level);
    void flush();  // Принудительная запись перед выходом
    
private:
    AsyncLogger();
    ~AsyncLogger();
    void writerLoop();
    
    struct LogEntry {
        LogLevel level;
        std::string msg;
        std::chrono::steady_clock::time_point time;
    };
    
    std::queue<LogEntry> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread writerThread_;
    std::atomic<bool> running_{false};
    size_t maxQueueSize_ = 1000;
    size_t droppedMessages_ = 0;
    FILE* file_ = nullptr;
    LogLevel level_ = LogLevel::INFO;
};

#define LOG_ASYNC_DEBUG(msg) AsyncLogger::instance().log(LogLevel::DEBUG, msg)
#define LOG_ASYNC_INFO(msg) AsyncLogger::instance().log(LogLevel::INFO, msg)
#define LOG_ASYNC_WARN(msg) AsyncLogger::instance().log(LogLevel::WARN, msg)
#define LOG_ASYNC_ERROR(msg) AsyncLogger::instance().log(LogLevel::ERROR, msg)

} // namespace h323p
```

**Преимущества:**
- Не блокирует основной поток выполнения
- Пакетная запись повышает производительность
- Поддержка высоких нагрузок (1000+ событий/сек)

---

### Рекомендация #7: Интеграционные тесты (базовые)

**Приоритет:** 🟡 Важный  
**Сложность:** Средняя

**Решение:**
```cpp
// tests/integration/test_basic.cpp
#include "CppUTest/TestHarness.h"
#include "cli/cli_parser.hpp"
#include "utils/utils.hpp"

TEST_GROUP(BasicIntegrationTest) {
    void setup() override { }
    void teardown() override { }
};

TEST_F(BasicIntegrationTest, CliParserHelp) {
    h323p::CliParser parser;
    std::string help = parser.getHelp();

    CHECK_FALSE(help.empty());
    CHECK(help.find("call") != std::string::npos);
    CHECK(help.find("listen") != std::string::npos);
}

TEST_F(BasicIntegrationTest, UtilsParseAddress) {
    auto addr = h323p::utils::parseAddress("192.168.1.1:1720");

    CHECK_TRUE(addr.has_value());
    STRCMP_EQUAL("192.168.1.1", addr->host.c_str());
    LONGS_EQUAL(1720, addr->port);
}
```

**Преимущества:**
- Раннее обнаружение регрессий
- Документирование ожидаемого поведения
- Уверенность при рефакторинге

---

### Рекомендация #12: Graceful Shutdown

**Приоритет:** 🟡 Важный  
**Сложность:** Низкая

**Проблема:** При Ctrl+C процесс убивается без завершения вызовов.

**Решение:**
```cpp
// src/main.cpp
#include <csignal>
#include <atomic>

namespace {
    std::atomic<bool> g_shutdown_requested{false};
    h323p::CallManager* g_callManager = nullptr;
}

void signalHandler(int signum) {
    g_shutdown_requested = true;
    
    if (g_callManager && g_callManager->isRunning()) {
        LOG_INFO("Запрошено завершение работы...");
        g_callManager->stop();
    }
}

int main(int argc, char* argv[]) {
    // Регистрация обработчиков сигналов
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    // ... основной код ...
    
    // Ожидание завершения
    while (!g_shutdown_requested && callManager.isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Корректное завершение вызовов
    callManager.stop();
    
    return 0;
}
```

**Преимущества:**
- Корректное завершение активных вызовов
- Отправка Release сообщений
- Сохранение статистики и CDR

---

## 🧪 Проверка результатов

### Сборка проекта

```bash
# Создание директории сборки
mkdir -p build && cd build

# Конфигурация CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DUSE_H323PLUS=OFF

# Сборка
make -j$(nproc)

# Проверка исполняемого файла
./h323p --help
./h323p info --version
```

### Запуск тестов

```bash
# Запуск всех тестов
ctest --output-on-failure

# Запуск с подробным выводом
ctest -V

# Запуск конкретных тестов
ctest -R "UtilsTest"
ctest -R "TimerTest"
ctest -R "LoggingTest"
```

### Проверка логирования

```bash
# Логирование в консоль
./h323p call 192.168.1.100 --log-level debug

# Логирование в файл
./h323p call 192.168.1.100 --log-file h323p.log --log-level debug

# Тихий режим
./h323p call 192.168.1.100 --quiet
```

### Проверка утилит

```bash
# Тестирование утилит через тесты
./h323p_tests -g UtilsTest

# Ожидаемый результат:
# Tests running in group UtilsTest:
# ....
# OK (12 tests, 0 failures)
```
# [----------] Global test environment set-up.
# [----------] 12 tests from UtilsTest
# [ RUN      ] UtilsTest.Trim
# [       OK ] UtilsTest.Trim (0 ms)
# ...
# [==========] 12 tests from 1 test suite ran. (1 ms total)
# [  PASSED  ] 12 tests.
```

---

## 📚 Ссылки

- **CMake документация:** https://cmake.org/documentation/
- **CLI11:** https://github.com/CLIUtils/CLI11
- **spdlog:** https://github.com/gabime/spdlog
- **CppUTest:** https://cpputest.github.io/
- **H323Plus:** https://www.h323plus.org/

---

*Версия документа: 0.1.0*
*Последнее обновление: 2026-03-16*
