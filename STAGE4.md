# Этап 4: XML сценарии

**Длительность:** 2 недели  
**Статус:** 📋 Готов к реализации  
**Зависимости:** ✅ Этап 1 (инфраструктура), ✅ Этап 2 (H.225), ✅ Этап 3 (H.245 + медиа)

---

## 📋 Содержание

1. [Обзор этапа](#-обзор-этапа)
2. [Задачи этапа](#-задачи-этапа)
3. [Архитектура движка сценариев](#-архитектура-движка-сценариев)
4. [Синтаксис XML сценариев](#-синтаксис-xml-сценариев)
5. [Детальная реализация](#-детальная-реализация)
6. [Примеры сценариев](#-примеры-сценариев)
7. [Критерии завершения](#-критерии-завершения)
8. [Проверка результатов](#-проверка-результатов)

---

## 📌 Обзор этапа

Цель Этапа 4 — реализовать движок XML сценариев по аналогии с SIPP для гибкого описания тестовых сценариев H.323 вызовов.

### Результаты этапа

```
✅ XML парсер сценариев реализован
✅ Модель сценария в памяти
✅ Интерпретатор выполняет сценарии
✅ Переменные и подстановки работают
✅ Распределения времени (fixed, uniform, normal, exponential)
✅ CSV файлы для массовых тестов
✅ Примеры сценариев готовы
```

### Что НЕ входит в Этап 4

```
❌ Нагрузочное тестирование (масштабирование) — Этап 5
❌ Fuzzing сценарии — Этап 6
❌ Видео сценарии — Этап 6
```

---

## 📊 Задачи этапа

| № | Задача | Длительность | Статус |
|---|--------|--------------|--------|
| 4.1 | XML парсер | 3 дня | ⏳ |
| 4.2 | Модель сценария | 3 дня | ⏳ |
| 4.3 | Интерпретатор | 5 дней | ⏳ |
| 4.4 | Переменные | 2 дня | ⏳ |
| 4.5 | Распределения | 2 дня | ⏳ |
| 4.6 | Примеры сценариев | 3 дня | ⏳ |
| **Итого** | | **18 дней** | |

---

## 🏗️ Архитектура движка сценариев

### Общая схема

```
┌─────────────────────────────────────────────────────────────────┐
│                    Application Layer                            │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  CLI: h323p call --scenario scenario.xml                │   │
│  └─────────────────────────────────────────────────────────┘   │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │                Scenario Engine                            │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │  XML Parser  │  │   Scenario   │  │  Interpreter    │ │ │
│  │  │  (pugixml)   │  │    Model     │  │  (Executor)     │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │              Variable System                              │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │   Global     │  │    Local     │  │      CSV        │ │ │
│  │  │  Variables   │  │  Variables   │  │   Variables     │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │              Distribution Engine                          │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │    Fixed     │  │   Uniform    │  │     Normal      │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │ Exponential  │  │   Weibull    │  │     Pareto      │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │                  Call Manager                             │ │
│  │         (Выполнение действий сценария)                    │ │
│  └───────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### Структура модулей

```
src/
└── scenario/
    ├── scenario_parser.hpp       # XML парсер
    ├── scenario_parser.cpp
    ├── scenario.hpp              # Модель сценария
    ├── scenario.cpp
    ├── scenario_types.hpp        # Типы сценария
    ├── interpreter.hpp           # Интерпретатор
    ├── interpreter.cpp
    ├── actions.hpp               # Действия
    ├── actions.cpp
    ├── variables.hpp             # Переменные
    ├── variables.cpp
    ├── distributions.hpp         # Распределения
    ├── distributions.cpp
    └── csv_reader.hpp            # CSV файлы
    └── csv_reader.cpp
```

---

## 📄 Синтаксис XML сценариев

### Общая структура

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="Scenario Name" version="1.0">
  <!-- Глобальные настройки -->
  <globals>
    <variable name="var_name" value="value"/>
  </globals>
  
  <!-- Сценарий для вызова -->
  <callflow name="caller">
    <actions>
      <!-- Действия -->
    </actions>
  </callflow>
</scenario>
```

---

## 🔧 Детальная реализация

### 4.1 XML парсер

**Файл: `src/scenario/scenario_types.hpp`**

```cpp
#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace h323p {

// Типы действий
enum class ActionType {
    // H.225 действия
    SETUP,
    WAIT,
    CONNECT,
    RELEASE,
    
    // H.245 действия
    OPEN_CHANNEL,
    CLOSE_CHANNEL,
    
    // Медиа действия
    START_MEDIA,
    STOP_MEDIA,
    
    // Управление потоком
    PAUSE,
    LABEL,
    JUMP,
    LOOP,
    
    // Gatekeeper
    REGISTER,
    UNREGISTER,
    
    // Логирование
    LOG,
    
    // Переменные
    SET_VARIABLE,
    ASSIGN,
    
    // Fuzzing
    SEND_FUZZ,
    
    // Статистика
    PRINT_STATS
};

// Распределения времени
enum class DistributionType {
    FIXED,
    UNIFORM,
    NORMAL,
    LOGNORMAL,
    EXPONENTIAL,
    WEIBULL,
    PARETO,
    GAMMA,
    NEGBIN
};

// Параметры распределения
struct DistributionParams {
    DistributionType type = DistributionType::FIXED;
    double value = 0.0;       // Для fixed
    double min = 0.0;         // Для uniform
    double max = 0.0;         // Для uniform
    double mean = 0.0;        // Для normal, exponential, lognormal
    double stddev = 0.0;      // Для normal
    double lambda = 0.0;      // Для weibull, gamma
    double k = 0.0;           // Для weibull
    double x_m = 0.0;         // Для pareto
    double p = 0.0;           // Для negbin
    double n = 0.0;           // Для negbin
};

// Базовое действие
struct Action {
    ActionType type;
    std::string id;
    
    // Переходы
    std::string next;
    std::string onTimeout;
    std::string onSuccess;
    std::string onFailure;
    
    // Условия
    std::string testVariable;
    double chance = 1.0;  // Вероятность выполнения (0.0-1.0)
    
    // Таймауты
    int timeout = 0;  // мс
    bool optional = false;
    
    // Параметры действия
    std::map<std::string, std::string> params;
    
    // Вложенные действия
    std::vector<Action> subActions;
    
    // Распределение для pause
    DistributionParams distribution;
};

// Поток вызова (callflow)
struct CallFlow {
    std::string name;
    std::vector<Action> actions;
};

// Сценарий
struct Scenario {
    std::string name;
    std::string version;
    std::string description;
    
    // Глобальные переменные
    std::map<std::string, std::string> globalVariables;
    
    // Пользовательские переменные
    std::vector<std::string> userVariables;
    
    // Потоки
    std::vector<CallFlow> callflows;
    
    // CSV настройки
    std::string csvFile;
    std::vector<std::string> csvColumns;
    
    // Метки
    std::map<std::string, size_t> labelIndex;
};

} // namespace h323p
```

**Файл: `src/scenario/scenario_parser.hpp`**

```cpp
#pragma once

#include "scenario_types.hpp"
#include <string>
#include <memory>

namespace h323p {

// Результат парсинга
struct ParseResult {
    bool success = false;
    std::string error;
    std::string warning;
    Scenario scenario;
};

// XML парсер сценариев
class ScenarioParser {
public:
    ScenarioParser();
    ~ScenarioParser();
    
    // Парсинг из файла
    ParseResult parseFile(const std::string& filename);
    
    // Парсинг из строки
    ParseResult parseString(const std::string& xmlContent);
    
    // Валидация сценария
    bool validateScenario(Scenario& scenario, std::string& error);
    
private:
    // Парсинг элементов
    bool parseScenario(void* node, Scenario& scenario);
    bool parseGlobals(void* node, Scenario& scenario);
    bool parseCallFlow(void* node, CallFlow& callflow);
    bool parseActions(void* node, std::vector<Action>& actions);
    bool parseAction(void* node, Action& action);
    bool parseDistribution(void* node, DistributionParams& dist);
    
    // Утилиты
    std::string getNodeAttribute(void* node, const std::string& name, 
                                  const std::string& defaultValue = "");
    int getNodeAttributeInt(void* node, const std::string& name, int defaultValue = 0);
    double getNodeAttributeDouble(void* node, const std::string& name, double defaultValue = 0.0);
    bool getNodeAttributeBool(void* node, const std::string& name, bool defaultValue = false);
    
    std::string lastError_;
};

} // namespace h323p
```

**Файл: `src/scenario/scenario_parser.cpp`**

```cpp
#include "scenario_parser.hpp"
#include "utils/logging.hpp"
#include <pugixml.hpp>

namespace h323p {

ScenarioParser::ScenarioParser() = default;
ScenarioParser::~ScenarioParser() = default;

ParseResult ScenarioParser::parseFile(const std::string& filename) {
    ParseResult result;
    
    LOG_INFO_FMT("Парсинг сценария: %s", filename.c_str());
    
    pugi::xml_document doc;
    pugi::xml_parse_result parseResult = doc.load_file(filename.c_str());
    
    if (!parseResult) {
        result.success = false;
        result.error = fmt::format("Ошибка парсинга XML: {}", parseResult.description());
        return result;
    }
    
    auto scenarioNode = doc.child("scenario");
    if (!scenarioNode) {
        result.success = false;
        result.error = "Отсутствует корневой элемент <scenario>";
        return result;
    }
    
    // Парсинг сценария
    if (!parseScenario(scenarioNode, result.scenario)) {
        result.success = false;
        result.error = lastError_;
        return result;
    }
    
    // Валидация
    if (!validateScenario(result.scenario, result.error)) {
        result.success = false;
        return result;
    }
    
    result.success = true;
    LOG_INFO_FMT("Сценарий загружен: %s (версия %s)", 
                 result.scenario.name.c_str(), 
                 result.scenario.version.c_str());
    
    return result;
}

ParseResult ScenarioParser::parseString(const std::string& xmlContent) {
    ParseResult result;
    
    pugi::xml_document doc;
    pugi::xml_parse_result parseResult = doc.load_string(xmlContent.c_str());
    
    if (!parseResult) {
        result.success = false;
        result.error = fmt::format("Ошибка парсинга XML: {}", parseResult.description());
        return result;
    }
    
    auto scenarioNode = doc.child("scenario");
    if (!scenarioNode) {
        result.success = false;
        result.error = "Отсутствует корневой элемент <scenario>";
        return result;
    }
    
    if (!parseScenario(scenarioNode, result.scenario)) {
        result.success = false;
        result.error = lastError_;
        return result;
    }
    
    if (!validateScenario(result.scenario, result.error)) {
        result.success = false;
        return result;
    }
    
    result.success = true;
    return result;
}

bool ScenarioParser::parseScenario(void* nodePtr, Scenario& scenario) {
    auto node = static_cast<pugi::xml_node*>(nodePtr);
    
    scenario.name = getNodeAttribute(*node, "name", "Unnamed");
    scenario.version = getNodeAttribute(*node, "version", "1.0");
    scenario.description = getNodeAttribute(*node, "description", "");
    
    // Парсинг globals
    auto globalsNode = node->child("globals");
    if (globalsNode) {
        for (auto varNode = globalsNode.child("variable"); varNode; 
             varNode = varNode.next_sibling("variable")) {
            std::string name = varNode.attribute("name").as_string();
            std::string value = varNode.attribute("value").as_string();
            scenario.globalVariables[name] = value;
        }
    }
    
    // Парсинг user variables
    auto userVarsNode = node->child("user_variables");
    if (userVarsNode) {
        std::string vars = userVarsNode.attribute("names").as_string();
        // Разделение по запятой
        // ...
    }
    
    // Парсинг callflows
    for (auto callflowNode = node->child("callflow"); callflowNode; 
         callflowNode = callflowNode.next_sibling("callflow")) {
        CallFlow callflow;
        if (!parseCallFlow(&callflowNode, callflow)) {
            return false;
        }
        scenario.callflows.push_back(callflow);
    }
    
    // Парсинг CSV
    auto csvNode = node->child("csv");
    if (csvNode) {
        scenario.csvFile = csvNode.attribute("file").as_string();
        // Парсинг колонок
        // ...
    }
    
    // Индексация меток
    for (size_t i = 0; i < scenario.callflows.size(); i++) {
        for (size_t j = 0; j < scenario.callflows[i].actions.size(); j++) {
            auto& action = scenario.callflows[i].actions[j];
            if (action.type == ActionType::LABEL) {
                std::string labelId = action.params["id"];
                scenario.labelIndex[labelId] = j;
            }
        }
    }
    
    return true;
}

bool ScenarioParser::parseCallFlow(void* nodePtr, CallFlow& callflow) {
    auto node = static_cast<pugi::xml_node*>(nodePtr);
    
    callflow.name = getNodeAttribute(*node, "name", "default");
    
    auto actionsNode = node->child("actions");
    if (actionsNode) {
        return parseActions(&actionsNode, callflow.actions);
    }
    
    return true;
}

bool ScenarioParser::parseActions(void* nodePtr, std::vector<Action>& actions) {
    auto node = static_cast<pugi::xml_node*>(nodePtr);
    
    for (auto child = node->first_child(); child; child = child.next_sibling()) {
        Action action;
        if (!parseAction(&child, action)) {
            return false;
        }
        actions.push_back(action);
    }
    
    return true;
}

bool ScenarioParser::parseAction(void* nodePtr, Action& action) {
    auto node = static_cast<pugi::xml_node*>(nodePtr);
    std::string nodeName = node->name();
    
    // Определение типа действия
    if (nodeName == "setup") action.type = ActionType::SETUP;
    else if (nodeName == "wait") action.type = ActionType::WAIT;
    else if (nodeName == "connect") action.type = ActionType::CONNECT;
    else if (nodeName == "release") action.type = ActionType::RELEASE;
    else if (nodeName == "open-channel") action.type = ActionType::OPEN_CHANNEL;
    else if (nodeName == "close-channel") action.type = ActionType::CLOSE_CHANNEL;
    else if (nodeName == "pause") action.type = ActionType::PAUSE;
    else if (nodeName == "label") action.type = ActionType::LABEL;
    else if (nodeName == "loop") action.type = ActionType::LOOP;
    else if (nodeName == "register") action.type = ActionType::REGISTER;
    else if (nodeName == "log") action.type = ActionType::LOG;
    else if (nodeName == "set_variable") action.type = ActionType::SET_VARIABLE;
    else if (nodeName == "nop") action.type = ActionType::PAUSE;  // nop как pause без действия
    else {
        LOG_WARN_FMT("Неизвестный элемент: %s", nodeName.c_str());
        return true;  // Пропускаем неизвестные элементы
    }
    
    // Парсинг атрибутов
    action.id = getNodeAttribute(*node, "id", "");
    action.next = getNodeAttribute(*node, "next", "");
    action.onTimeout = getNodeAttribute(*node, "ontimeout", "");
    action.onSuccess = getNodeAttribute(*node, "onsuccess", "");
    action.onFailure = getNodeAttribute(*node, "onfailure", "");
    action.testVariable = getNodeAttribute(*node, "test", "");
    action.chance = getNodeAttributeDouble(*node, "chance", 1.0);
    action.optional = getNodeAttributeBool(*node, "optional", false);
    action.timeout = getNodeAttributeInt(*node, "timeout", 0);
    
    // Парсинг параметров действия
    for (auto attr = node->first_attribute(); attr; attr = attr.next_attribute()) {
        std::string name = attr.name();
        if (name != "id" && name != "next" && name != "ontimeout" && 
            name != "onsuccess" && name != "onfailure" && name != "test" &&
            name != "chance" && name != "optional" && name != "timeout") {
            action.params[name] = attr.value();
        }
    }
    
    // Парсинг распределения для pause
    if (action.type == ActionType::PAUSE) {
        if (!parseDistribution(node, action.distribution)) {
            return false;
        }
    }
    
    // Парсинг вложенных элементов
    auto onSuccessNode = node->child("on-success");
    if (onSuccessNode) {
        parseActions(&onSuccessNode, action.subActions);
    }
    
    auto onFailureNode = node->child("on-failure");
    if (onFailureNode) {
        parseActions(&onFailureNode, action.subActions);
    }
    
    auto onTimeoutNode = node->child("on-timeout");
    if (onTimeoutNode) {
        parseActions(&onTimeoutNode, action.subActions);
    }
    
    return true;
}

bool ScenarioParser::parseDistribution(void* nodePtr, DistributionParams& dist) {
    auto node = static_cast<pugi::xml_node*>(nodePtr);
    
    std::string distType = getNodeAttribute(*node, "distribution", "fixed");
    
    if (distType == "fixed") {
        dist.type = DistributionType::FIXED;
        dist.value = getNodeAttributeDouble(*node, "value", 0.0);
    } else if (distType == "uniform") {
        dist.type = DistributionType::UNIFORM;
        dist.min = getNodeAttributeDouble(*node, "min", 0.0);
        dist.max = getNodeAttributeDouble(*node, "max", 0.0);
    } else if (distType == "normal") {
        dist.type = DistributionType::NORMAL;
        dist.mean = getNodeAttributeDouble(*node, "mean", 0.0);
        dist.stddev = getNodeAttributeDouble(*node, "stdev", 0.0);
    } else if (distType == "exponential") {
        dist.type = DistributionType::EXPONENTIAL;
        dist.mean = getNodeAttributeDouble(*node, "mean", 0.0);
    } else if (distType == "weibull") {
        dist.type = DistributionType::WEIBULL;
        dist.lambda = getNodeAttributeDouble(*node, "lambda", 0.0);
        dist.k = getNodeAttributeDouble(*node, "k", 0.0);
    }
    // Другие распределения...
    
    return true;
}

bool ScenarioParser::validateScenario(Scenario& scenario, std::string& error) {
    if (scenario.callflows.empty()) {
        error = "Сценарий должен содержать хотя бы один callflow";
        return false;
    }
    
    // Проверка ссылок на метки
    for (auto& callflow : scenario.callflows) {
        for (auto& action : callflow.actions) {
            if (!action.next.empty() && 
                scenario.labelIndex.find(action.next) == scenario.labelIndex.end()) {
                // Проверяем, что next - это числовой индекс
                try {
                    std::stoul(action.next);
                } catch (...) {
                    error = fmt::format("Неизвестная метка: {}", action.next);
                    return false;
                }
            }
        }
    }
    
    return true;
}

std::string ScenarioParser::getNodeAttribute(void* nodePtr, const std::string& name, 
                                              const std::string& defaultValue) {
    auto node = static_cast<pugi::xml_node*>(nodePtr);
    auto attr = node->attribute(name.c_str());
    if (attr) {
        return attr.value();
    }
    return defaultValue;
}

int ScenarioParser::getNodeAttributeInt(void* nodePtr, const std::string& name, int defaultValue) {
    auto node = static_cast<pugi::xml_node*>(nodePtr);
    return node->attribute(name.c_str()).as_int(defaultValue);
}

double ScenarioParser::getNodeAttributeDouble(void* nodePtr, const std::string& name, 
                                               double defaultValue) {
    auto node = static_cast<pugi::xml_node*>(nodePtr);
    return node->attribute(name.c_str()).as_double(defaultValue);
}

bool ScenarioParser::getNodeAttributeBool(void* nodePtr, const std::string& name, 
                                           bool defaultValue) {
    auto node = static_cast<pugi::xml_node*>(nodePtr);
    return node->attribute(name.c_str()).as_bool(defaultValue);
}

} // namespace h323p
```

---

### 4.2 Модель сценария

**Файл: `src/scenario/scenario.hpp`**

```cpp
#pragma once

#include "scenario_types.hpp"
#include <string>
#include <memory>

namespace h323p {

// Контекст выполнения сценария
struct ScenarioContext {
    // Переменные
    std::map<std::string, std::string> globalVars;
    std::map<std::string, std::string> localVars;
    std::map<std::string, std::string> csvVars;
    
    // Состояние
    size_t currentActionIndex = 0;
    bool isRunning = false;
    bool isPaused = false;
    
    // Статистика
    uint64_t callsTotal = 0;
    uint64_t callsSuccessful = 0;
    uint64_t callsFailed = 0;
    
    // Таймеры
    uint64_t startTime = 0;
    uint64_t endTime = 0;
};

// Менеджер сценариев
class ScenarioManager {
public:
    static ScenarioManager& instance();
    
    // Загрузка
    bool loadScenario(const std::string& name, const std::string& filename);
    bool unloadScenario(const std::string& name);
    
    // Получение сценария
    Scenario* getScenario(const std::string& name);
    const Scenario* getScenario(const std::string& name) const;
    
    // Выполнение
    bool executeScenario(const std::string& name, ScenarioContext& context);
    
private:
    ScenarioManager() = default;
    ~ScenarioManager();
    
    std::map<std::string, std::unique_ptr<Scenario>> scenarios_;
};

} // namespace h323p
```

---

### 4.3 Интерпретатор

**Файл: `src/scenario/interpreter.hpp`**

```cpp
#pragma once

#include "scenario_types.hpp"
#include "variables.hpp"
#include <functional>
#include <memory>

namespace h323p {

// Callback для выполнения действий
using ActionCallback = std::function<bool(const Action& action, ScenarioContext& context)>;

// Интерпретатор сценариев
class ScenarioInterpreter {
public:
    ScenarioInterpreter();
    ~ScenarioInterpreter();
    
    // Установка callback
    void setActionCallback(ActionType type, ActionCallback callback);
    
    // Выполнение
    bool execute(const Scenario& scenario, ScenarioContext& context);
    void stop();
    
    // Состояние
    bool isRunning() const { return running_; }
    bool isPaused() const { return paused_; }
    
    void pause();
    void resume();
    
private:
    void executeLoop();
    bool executeAction(const Action& action, ScenarioContext& context);
    bool executePause(const Action& action, ScenarioContext& context);
    bool executeJump(const Action& action, ScenarioContext& context);
    bool executeLoop(const Action& action, ScenarioContext& context);
    
    double generateDelay(const DistributionParams& dist);
    
    std::map<ActionType, ActionCallback> callbacks_;
    bool running_ = false;
    bool paused_ = false;
    bool stopRequested_ = false;
    
    std::thread executionThread_;
};

} // namespace h323p
```

**Файл: `src/scenario/interpreter.cpp`**

```cpp
#include "interpreter.hpp"
#include "utils/logging.hpp"
#include "utils/timer.hpp"
#include <random>
#include <chrono>

namespace h323p {

ScenarioInterpreter::ScenarioInterpreter() = default;
ScenarioInterpreter::~ScenarioInterpreter() {
    stop();
}

void ScenarioInterpreter::setActionCallback(ActionType type, ActionCallback callback) {
    callbacks_[type] = callback;
}

bool ScenarioInterpreter::execute(const Scenario& scenario, ScenarioContext& context) {
    if (running_) {
        LOG_WARN("Интерпретатор уже запущен");
        return false;
    }
    
    LOG_INFO_FMT("Запуск сценария: %s", scenario.name.c_str());
    
    running_ = true;
    stopRequested_ = false;
    context.isRunning = true;
    context.startTime = std::time(nullptr);
    
    executionThread_ = std::thread(&ScenarioInterpreter::executeLoop, this);
    
    return true;
}

void ScenarioInterpreter::stop() {
    if (!running_) return;
    
    LOG_INFO("Остановка интерпретатора");
    
    stopRequested_ = true;
    running_ = false;
    
    if (executionThread_.joinable()) {
        executionThread_.join();
    }
}

void ScenarioInterpreter::executeLoop() {
    // Основная логика выполнения
    while (running_ && !stopRequested_) {
        if (paused_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        // Выполнение следующего действия
        // ...
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

bool ScenarioInterpreter::executeAction(const Action& action, ScenarioContext& context) {
    LOG_DEBUG_FMT("Выполнение действия: %d", static_cast<int>(action.type));
    
    // Проверка условия test
    if (!action.testVariable.empty()) {
        auto it = context.localVars.find(action.testVariable);
        if (it == context.localVars.end() || it->second.empty()) {
            return true;  // Условие не выполнено, пропускаем
        }
    }
    
    // Проверка chance
    if (action.chance < 1.0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        
        if (dis(gen) > action.chance) {
            return true;  // Не выполнилось по вероятности
        }
    }
    
    // Поиск callback
    auto it = callbacks_.find(action.type);
    if (it != callbacks_.end()) {
        return it->second(action, context);
    }
    
    LOG_WARN_FMT("Нет callback для действия типа %d", static_cast<int>(action.type));
    return true;
}

bool ScenarioInterpreter::executePause(const Action& action, ScenarioContext& context) {
    double delayMs = generateDelay(action.distribution);
    
    LOG_DEBUG_FMT("Пауза: %.0f мс", delayMs);
    
    auto start = std::chrono::steady_clock::now();
    auto end = start + std::chrono::milliseconds(static_cast<int>(delayMs));
    
    while (std::chrono::steady_clock::now() < end) {
        if (stopRequested_) {
            return false;
        }
        
        if (paused_) {
            // Ждем возобновления
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            end = std::chrono::steady_clock::now() + (end - start);
            start = std::chrono::steady_clock::now();
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    return true;
}

double ScenarioInterpreter::generateDelay(const DistributionParams& dist) {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    switch (dist.type) {
        case DistributionType::FIXED:
            return dist.value;
            
        case DistributionType::UNIFORM: {
            std::uniform_real_distribution<> dis(dist.min, dist.max);
            return dis(gen);
        }
            
        case DistributionType::NORMAL: {
            std::normal_distribution<> dis(dist.mean, dist.stddev);
            return std::max(0.0, dis(gen));
        }
            
        case DistributionType::EXPONENTIAL: {
            std::exponential_distribution<> dis(1.0 / dist.mean);
            return dis(gen);
        }
            
        case DistributionType::WEIBULL: {
            std::weibull_distribution<> dis(dist.lambda, dist.k);
            return dis(gen);
        }
            
        default:
            return dist.value;
    }
}

void ScenarioInterpreter::pause() {
    paused_ = true;
    LOG_INFO("Интерпретатор на паузе");
}

void ScenarioInterpreter::resume() {
    paused_ = false;
    LOG_INFO("Интерпретатор возобновлён");
}

} // namespace h323p
```

---

### 4.4 Переменные

**Файл: `src/scenario/variables.hpp`**

```cpp
#pragma once

#include <string>
#include <map>
#include <regex>

namespace h323p {

// Менеджер переменных
class VariableManager {
public:
    static VariableManager& instance();
    
    // Глобальные переменные
    void setGlobal(const std::string& name, const std::string& value);
    std::string getGlobal(const std::string& name) const;
    
    // Локальные переменные
    void setLocal(const std::string& name, const std::string& value);
    std::string getLocal(const std::string& name) const;
    
    // CSV переменные
    void setCSV(const std::string& name, const std::string& value);
    std::string getCSV(const std::string& name) const;
    
    // Подстановка переменных в строку
    std::string substitute(const std::string& str) const;
    
    // Проверка существования
    bool exists(const std::string& name) const;
    
    // Очистка
    void clearLocal();
    void clearCSV();
    
private:
    VariableManager() = default;
    
    std::map<std::string, std::string> globalVars_;
    std::map<std::string, std::string> localVars_;
    std::map<std::string, std::string> csvVars_;
};

} // namespace h323p
```

---

### 4.5 Распределения

**Файл: `src/scenario/distributions.hpp`**

```cpp
#pragma once

#include "scenario_types.hpp"
#include <random>

namespace h323p {

// Генератор распределений
class DistributionGenerator {
public:
    DistributionGenerator();
    
    // Генерация значения
    double generate(const DistributionParams& params);
    
    // Отдельные методы для каждого распределения
    double fixed(double value);
    double uniform(double min, double max);
    double normal(double mean, double stddev);
    double lognormal(double mean, double stddev);
    double exponential(double mean);
    double weibull(double lambda, double k);
    double pareto(double x_m, double k);
    double gamma(double k, double theta);
    double negbin(double p, double n);
    
private:
    std::mt19937 gen_;
};

} // namespace h323p
```

---

## 📄 Примеры сценариев

### Базовый вызов

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="Basic Call" version="1.0">
  <globals>
    <variable name="call_duration" value="30"/>
  </globals>
  
  <callflow name="caller">
    <actions>
      <setup destination="192.168.1.100:1720" faststart="true"/>
      
      <wait event="call_proceeding" timeout="5000" optional="true"/>
      
      <wait event="connect" timeout="30000">
        <on-success next="established"/>
        <on-timeout next="release"/>
      </wait>
      
      <label id="established"/>
      
      <open-channel type="audio" codec="G.711" direction="sendrecv"/>
      
      <pause distribution="fixed" value="${call_duration}000"/>
      
      <close-channel type="audio"/>
      
      <release cause="normal_call_clearing"/>
      
      <wait event="release_complete" timeout="5000"/>
    </actions>
  </callflow>
</scenario>
```

---

### Нагрузочный тест с циклом

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="Load Test" version="1.0">
  <globals>
    <variable name="max_calls" value="100"/>
    <variable name="call_duration" value="60"/>
  </globals>
  
  <callflow name="load_generator">
    <actions>
      <loop variable="i" from="1" to="${max_calls}">
        <setup destination="192.168.1.10${i % 10}:1720"/>
        
        <wait event="connect" timeout="30000">
          <on-success>
            <open-channel type="audio" codec="G.711"/>
            <pause distribution="normal" mean="${call_duration}000" stdev="10000"/>
            <close-channel type="audio"/>
            <release cause="normal_call_clearing"/>
          </on-success>
          <on-failure>
            <log message="Call ${i} failed"/>
          </on-failure>
        </wait>
        
        <pause distribution="exponential" mean="1000"/>
      </loop>
      
      <wait event="all_calls_released" timeout="300000"/>
      
      <print-stats/>
    </actions>
  </callflow>
</scenario>
```

---

### Сценарий с CSV

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="CSV Test" version="1.0">
  <csv file="destinations.csv">
    <columns>
      <column name="destination"/>
      <column name="username"/>
      <column name="expected_result"/>
    </columns>
  </csv>
  
  <callflow name="csv_caller">
    <actions>
      <loop variable="row" in="csv">
        <log message="Calling ${csv.destination} as ${csv.username}"/>
        
        <setup destination="${csv.destination}" username="${csv.username}"/>
        
        <wait event="connect" timeout="30000">
          <on-success>
            <set_variable name="result" value="success"/>
            <release cause="normal_call_clearing"/>
          </on-success>
          <on-failure>
            <set_variable name="result" value="failure"/>
          </on-failure>
        </wait>
        
        <log message="Result: ${result}, Expected: ${csv.expected_result}"/>
      </loop>
    </actions>
  </callflow>
</scenario>
```

**Пример CSV файла (destinations.csv):**
```csv
destination,username,expected_result
192.168.1.100,user1,success
192.168.1.101,user2,success
192.168.1.102,user3,failure
alias:1001,test_user,success
```

---

### Fuzzing сценарий

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="RTP Fuzzing" version="1.0">
  <globals>
    <variable name="fuzz_header_pct" value="50"/>
    <variable name="fuzz_media_pct" value="10"/>
  </globals>
  
  <callflow name="fuzzer">
    <actions>
      <setup destination="192.168.1.100:1720"/>
      
      <wait event="connect" timeout="30000"/>
      
      <open-channel type="audio" codec="G.711">
        <fuzz enable="true" 
              header_percentage="${fuzz_header_pct}"
              media_percentage="${fuzz_media_pct}"/>
      </open-channel>
      
      <pause distribution="fixed" value="60000"/>
      
      <close-channel type="audio"/>
      <release cause="normal_call_clearing"/>
    </actions>
  </callflow>
</scenario>
```

---

## ✅ Критерии завершения

### Чек-лист завершения Этапа 4

```
[ ] 1. XML парсер реализован
[ ] 2. Модель сценария работает
[ ] 3. Интерпретатор выполняет действия
[ ] 4. Переменные подставляются
[ ] 5. Распределения работают (fixed, uniform, normal, exponential)
[ ] 6. CSV файлы читаются
[ ] 7. Примеры сценариев готовы
[ ] 8. Тесты парсера проходят
[ ] 9. Тесты интерпретатора проходят
```

---

## 🧪 Проверка результатов

### Сборка

```bash
cd build
cmake .. -DBUILD_TESTS=ON
make -j$(nproc)
```

### Запуск сценария

```bash
# Базовый сценарий
./h323p call --scenario scenarios/basic_call.xml

# Сценарий с CSV
./h323p call --scenario scenarios/csv_test.xml

# Нагрузочный тест
./h323p load --scenario scenarios/load_test.xml
```

### Тесты

```bash
# Тесты парсера
ctest -R "ScenarioParserTest" --output-on-failure

# Тесты интерпретатора
ctest -R "InterpreterTest" --output-on-failure

# Тесты переменных
ctest -R "VariableTest" --output-on-failure

# Тесты распределений
ctest -R "DistributionTest" --output-on-failure
```

### Пример вывода

```
$ ./h323p call --scenario scenarios/basic_call.xml

2026-03-16 10:00:00.123 [INFO] Загрузка сценария: Basic Call
2026-03-16 10:00:00.124 [INFO] Парсинг XML: scenarios/basic_call.xml
2026-03-16 10:00:00.125 [INFO] Сценарий загружен: Basic Call (версия 1.0)
2026-03-16 10:00:00.126 [INFO] Запуск сценария...
2026-03-16 10:00:00.127 [INFO] Действие: SETUP -> 192.168.1.100:1720
2026-03-16 10:00:00.228 [INFO] Действие: WAIT -> call_proceeding
2026-03-16 10:00:00.329 [INFO] Действие: WAIT -> connect
2026-03-16 10:00:00.430 [INFO] Действие: LABEL -> established
2026-03-16 10:00:00.431 [INFO] Действие: OPEN_CHANNEL -> audio G.711
2026-03-16 10:00:00.432 [INFO] Действие: PAUSE -> 30000 мс
...
2026-03-16 10:00:30.433 [INFO] Действие: CLOSE_CHANNEL -> audio
2026-03-16 10:00:30.434 [INFO] Действие: RELEASE -> normal_call_clearing
2026-03-16 10:00:30.535 [INFO] Сценарий завершён успешно
```

---

## 📚 Ссылки

- **SIPP документация:** https://sipp.readthedocs.io/
- **pugixml:** https://pugixml.org/
- **C++ <random>:** https://en.cppreference.com/w/cpp/header/random

---

*Версия документа: 0.1.0*  
*Последнее обновление: 2026-03-16*
