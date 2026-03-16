# Этап 2: H.323 стек H.225

**Длительность:** 3-4 недели  
**Статус:** 📋 Готов к реализации  
**Зависимости:** ✅ Этап 1 (инфраструктура)

---

## 📋 Содержание

1. [Обзор этапа](#-обзор-этапа)
2. [Задачи этапа](#-задачи-этапа)
3. [Архитектура H.323 слоя](#-архитектура-h323-слоя)
4. [Детальная реализация](#-детальная-реализация)
5. [Диаграмма состояний вызова](#-диаграмма-состояний-вызова)
6. [Критерии завершения](#-критерии-завершения)
7. [Проверка результатов](#-проверка-результатов)

---

## 📌 Обзор этапа

Цель Этапа 2 — реализовать базовый H.323 стек на основе H323Plus для возможности совершения и приёма вызовов.

### Результаты этапа

```
✅ Обёртка над H323Plus реализована
✅ H.225 RAS (регистрация на Gatekeeper)
✅ H.225 Q.931 (сигнализация вызовов)
✅ Call Manager управляет вызовами
✅ Команда call работает
✅ Команда listen работает
✅ Интеграционные тесты проходят
```

### Что НЕ входит в Этап 2

```
❌ H.245 (управление медиа каналами) — Этап 3
❌ RTP/RTCP (передача медиа) — Этап 3
❌ Кодеки — Этап 3
❌ XML сценарии — Этап 4
```

---

## 📊 Задачи этапа

| № | Задача | Длительность | Статус |
|---|--------|--------------|--------|
| 2.1 | Обёртка H323Plus | 4 дня | ⏳ |
| 2.2 | H.225 RAS | 5 дней | ⏳ |
| 2.3 | H.225 Q.931 | 5 дней | ⏳ |
| 2.4 | Call Manager | 4 дня | ⏳ |
| 2.5 | Команда call | 3 дня | ⏳ |
| 2.6 | Команда listen | 3 дня | ⏳ |
| 2.7 | Интеграционные тесты | 4 дня | ⏳ |
| **Итого** | | **28 дней** | |

---

## 🏗️ Архитектура H.323 слоя

### Общая схема

```
┌─────────────────────────────────────────────────────────────────┐
│                    Application Layer                            │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  CLI (call, listen, load, ...)                          │   │
│  └─────────────────────────────────────────────────────────┘   │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │                    Call Manager                           │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │    Call      │  │    Call      │  │     Call        │ │ │
│  │  │   State      │  │   State      │  │    State        │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │                  H323Wrapper                              │ │
│  │              (Обёртка над H323Plus)                       │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │                   H323Plus Library                        │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │   H.225 RAS  │  │  H.225 Q.931 │  │     H.245       │ │ │
│  │  │              │  │              │  │   (позже)       │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### Структура модулей

```
src/
├── h323/
│   ├── h323_wrapper.hpp          # Обёртка над H323Plus
│   ├── h323_wrapper.cpp
│   ├── h323_endpoint.hpp         # H.323 endpoint (синглтон)
│   ├── h323_endpoint.cpp
│   ├── ras.hpp                   # H.225 RAS логика
│   ├── ras.cpp
│   ├── q931.hpp                  # H.225 Q.931 логика
│   ├── q931.cpp
│   └── h245.hpp                  # Заглушка для Этапа 3
│   └── h245.cpp
├── core/
│   ├── call_manager.hpp          # Управление вызовами
│   ├── call_manager.cpp
│   ├── call.hpp                  # Класс вызова
│   ├── call.cpp
│   ├── endpoint.hpp              # Внутренний endpoint
│   └── endpoint.cpp
└── cli/
    └── commands/
        ├── cmd_call.hpp          # Команда call
        ├── cmd_call.cpp
        ├── cmd_listen.hpp        # Команда listen
        └── cmd_listen.cpp
```

---

## 🔧 Детальная реализация

### 2.1 Обёртка H323Plus

**Файл: `src/h323/h323_endpoint.hpp`**

```cpp
#pragma once

#include <string>
#include <memory>
#include <functional>
#include <map>

// Forward declaration H323Plus классов
class H323EndPoint;
class H323Connection;
class H323Listener;

namespace h323p {

// Конфигурация endpoint
struct H323Config {
    std::string localAddress = "0.0.0.0";
    int tcpPort = 1720;
    int udpPort = 1720;
    bool gatekeeperRequired = false;
    std::string gatekeeperAddress;
    std::string username;
    std::string password;
    bool fastStart = true;
    bool h245Tunneling = true;
    int maxBandwidth = 0;  // 0 = без ограничений
};

// Типы событий
enum class H323EventType {
    INCOMING_CALL,
    CALL_ESTABLISHED,
    CALL_CLEARED,
    CALL_FAILED,
    GATEKEEPER_REGISTERED,
    GATEKEEPER_REGISTRATION_FAILED,
    GATEKEEPER_DISCOVERED,
    GATEKEEPER_DISCOVERY_FAILED
};

// Данные события
struct H323Event {
    H323EventType type;
    std::string callId;
    std::string remoteAddress;
    std::string remoteNumber;
    std::string gatekeeperAddress;
    int errorCode = 0;
    std::string errorText;
};

// Callback типы
using H323EventCallback = std::function<void(const H323Event&)>;

// Класс endpoint (синглтон)
class H323Endpoint {
public:
    static H323Endpoint& instance();
    
    // Инициализация
    bool initialize(const H323Config& config);
    void shutdown();
    bool isInitialized() const { return initialized_; }
    
    // Настройки
    const H323Config& getConfig() const { return config_; }
    
    // Gatekeeper
    bool registerWithGatekeeper(const std::string& gkAddress);
    bool unregisterFromGatekeeper();
    bool isRegisteredWithGatekeeper() const;
    
    // Вызовы
    bool makeCall(const std::string& destination, std::string& callId);
    bool answerCall(const std::string& callId);
    bool releaseCall(const std::string& callId, int cause = 0);
    
    // Callback
    void setEventCallback(H323EventCallback callback);
    
    // Статистика
    size_t getActiveCalls() const;
    size_t getTotalCalls() const { return totalCalls_; }
    
private:
    H323Endpoint() = default;
    ~H323Endpoint();
    
    H323Endpoint(const H323Endpoint&) = delete;
    H323Endpoint& operator=(const H323Endpoint&) = delete;
    
    // Внутренние методы
    void onIncomingCall(H323Connection* connection);
    void onCallEstablished(H323Connection* connection);
    void onCallCleared(H323Connection* connection);
    void onGatekeeperRegistered(bool success);
    
    H323Config config_;
    H323EndPoint* endpoint_ = nullptr;
    H323EventCallback eventCallback_;
    bool initialized_ = false;
    size_t totalCalls_ = 0;
    
    // Карта активных вызовов
    std::map<std::string, H323Connection*> activeCalls_;
};

} // namespace h323p
```

**Файл: `src/h323/h323_endpoint.cpp`**

```cpp
#include "h323_endpoint.hpp"
#include "utils/logging.hpp"

// Включаем заголовки H323Plus
#include <h323.h>
#include <h323ep.h>
#include <h323con.h>
#include <h323gk.h>
#include <ptlib/socket.h>

namespace h323p {

H323Endpoint& H323Endpoint::instance() {
    static H323Endpoint endpoint;
    return endpoint;
}

H323Endpoint::~H323Endpoint() {
    shutdown();
}

bool H323Endpoint::initialize(const H323Config& config) {
    if (initialized_) {
        LOG_WARN("H323Endpoint уже инициализирован");
        return true;
    }
    
    config_ = config;
    
    LOG_INFO_FMT("Инициализация H323Endpoint: %s:%d", 
                 config.localAddress.c_str(), config.tcpPort);
    
    // Создание H323EndPoint
    endpoint_ = new H323EndPoint();
    
    // Настройка listening address
    PIPSocket::Address bindAddress;
    if (config.localAddress == "0.0.0.0") {
        bindAddress = PIPSocket::GetDefaultIpAny();
    } else {
        bindAddress = PIPSocket::Address(config.localAddress.c_str());
    }
    
    // Настройка портов
    endpoint_->SetPort(config.tcpPort);
    
    // Fast Start
    endpoint_->SetFastStart(config.fastStart);
    
    // H.245 Tunneling
    endpoint_->SetTunnelH245(config.h245Tunneling);
    
    // Bandwidth
    if (config.maxBandwidth > 0) {
        endpoint_->SetInitialBandwidth(config.maxBandwidth);
    }
    
    // Gatekeeper настройки
    if (config.gatekeeperRequired && !config.gatekeeperAddress.empty()) {
        LOG_INFO_FMT("Настройка Gatekeeper: %s", config.gatekeeperAddress.c_str());
        // Настройка будет выполнена в registerWithGatekeeper
    }
    
    initialized_ = true;
    LOG_INFO("H323Endpoint инициализирован");
    
    return true;
}

void H323Endpoint::shutdown() {
    if (!initialized_) return;
    
    LOG_INFO("Завершение работы H323Endpoint");
    
    // Освобождение всех вызовов
    for (auto& [callId, connection] : activeCalls_) {
        if (connection) {
            connection->Release();
        }
    }
    activeCalls_.clear();
    
    // Удаление endpoint
    if (endpoint_) {
        delete endpoint_;
        endpoint_ = nullptr;
    }
    
    initialized_ = false;
}

bool H323Endpoint::registerWithGatekeeper(const std::string& gkAddress) {
    if (!initialized_) {
        LOG_ERROR("H323Endpoint не инициализирован");
        return false;
    }
    
    LOG_INFO_FMT("Регистрация на Gatekeeper: %s", gkAddress.c_str());
    
    // Поиск Gatekeeper
    H323RegistrationInfo info;
    info.gatekeeperAddress = gkAddress.c_str();
    info.username = config_.username.c_str();
    info.password = config_.password.c_str();
    
    // Попытка регистрации
    // Примечание: реальный API H323Plus может отличаться
    bool result = endpoint_->RegisterGatekeeper(info);
    
    if (result) {
        LOG_INFO("Регистрация на Gatekeeper успешна");
        H323Event event;
        event.type = H323EventType::GATEKEEPER_REGISTERED;
        event.gatekeeperAddress = gkAddress;
        if (eventCallback_) eventCallback_(event);
    } else {
        LOG_ERROR("Ошибка регистрации на Gatekeeper");
        H323Event event;
        event.type = H323EventType::GATEKEEPER_REGISTRATION_FAILED;
        event.errorText = "Registration failed";
        if (eventCallback_) eventCallback_(event);
    }
    
    return result;
}

bool H323Endpoint::unregisterFromGatekeeper() {
    if (!initialized_) return false;
    
    LOG_INFO("Отписка от Gatekeeper");
    return endpoint_->UnregisterGatekeeper();
}

bool H323Endpoint::isRegisteredWithGatekeeper() const {
    if (!initialized_) return false;
    return endpoint_->IsRegisteredWithGatekeeper();
}

bool H323Endpoint::makeCall(const std::string& destination, std::string& callId) {
    if (!initialized_) {
        LOG_ERROR("H323Endpoint не инициализирован");
        return false;
    }
    
    LOG_INFO_FMT("Исходящий вызов: %s", destination.c_str());
    
    // Создание исходящего вызова
    PString token = destination.c_str();
    PString alias = config_.username.c_str();
    
    H323Connection* connection = endpoint_->MakeCall(token, alias);
    
    if (connection) {
        callId = (const char*)connection->GetCallReference();
        activeCalls_[callId] = connection;
        totalCalls_++;
        
        LOG_INFO_FMT("Вызов создан: %s", callId.c_str());
        return true;
    }
    
    LOG_ERROR("Не удалось создать вызов");
    return false;
}

bool H323Endpoint::answerCall(const std::string& callId) {
    auto it = activeCalls_.find(callId);
    if (it == activeCalls_.end()) {
        LOG_ERROR_FMT("Вызов %s не найден", callId.c_str());
        return false;
    }
    
    H323Connection* connection = it->second;
    
    LOG_INFO_FMT("Ответ на вызов: %s", callId.c_str());
    
    // Ответ на вызов
    return connection->AnswerCall();
}

bool H323Endpoint::releaseCall(const std::string& callId, int cause) {
    auto it = activeCalls_.find(callId);
    if (it == activeCalls_.end()) {
        LOG_ERROR_FMT("Вызов %s не найден", callId.c_str());
        return false;
    }
    
    H323Connection* connection = it->second;
    
    LOG_INFO_FMT("Завершение вызова: %s (cause=%d)", callId.c_str(), cause);
    
    // Завершение вызова
    bool result = connection->Release(cause);
    
    // Удаление из активных
    activeCalls_.erase(it);
    
    return result;
}

void H323Endpoint::setEventCallback(H323EventCallback callback) {
    eventCallback_ = callback;
}

size_t H323Endpoint::getActiveCalls() const {
    return activeCalls_.size();
}

// Обработчики событий H323Plus

void H323Endpoint::onIncomingCall(H323Connection* connection) {
    std::string callId = (const char*)connection->GetCallReference();
    std::string remote = (const char*)connection->GetRemoteAddress();
    
    LOG_INFO_FMT("Входящий вызов: %s от %s", callId.c_str(), remote.c_str());
    
    activeCalls_[callId] = connection;
    totalCalls_++;
    
    H323Event event;
    event.type = H323EventType::INCOMING_CALL;
    event.callId = callId;
    event.remoteAddress = remote;
    
    if (eventCallback_) eventCallback_(event);
}

void H323Endpoint::onCallEstablished(H323Connection* connection) {
    std::string callId = (const char*)connection->GetCallReference();
    
    LOG_INFO_FMT("Вызов установлен: %s", callId.c_str());
    
    H323Event event;
    event.type = H323EventType::CALL_ESTABLISHED;
    event.callId = callId;
    
    if (eventCallback_) eventCallback_(event);
}

void H323Endpoint::onCallCleared(H323Connection* connection) {
    std::string callId = (const char*)connection->GetCallReference();
    
    LOG_INFO_FMT("Вызов завершён: %s", callId.c_str());
    
    H323Event event;
    event.type = H323EventType::CALL_CLEARED;
    event.callId = callId;
    
    if (eventCallback_) eventCallback_(event);
    
    activeCalls_.erase(callId);
}

void H323Endpoint::onGatekeeperRegistered(bool success) {
    H323Event event;
    if (success) {
        event.type = H323EventType::GATEKEEPER_REGISTERED;
        LOG_INFO("Регистрация на Gatekeeper успешна");
    } else {
        event.type = H323EventType::GATEKEEPER_REGISTRATION_FAILED;
        LOG_ERROR("Ошибка регистрации на Gatekeeper");
    }
    
    if (eventCallback_) eventCallback_(event);
}

} // namespace h323p
```

---

### 2.2 H.225 RAS

**Файл: `src/h323/ras.hpp`**

```cpp
#pragma once

#include "h323_endpoint.hpp"
#include <string>
#include <functional>

namespace h323p {

// Статус регистрации
enum class RegistrationStatus {
    NOT_REGISTERED,
    REGISTERING,
    REGISTERED,
    REJECTED,
    ERROR
};

// Конфигурация Gatekeeper
struct GatekeeperConfig {
    std::string address;
    int port = 1719;
    std::string username;
    std::string password;
    std::string alias;  // E.164 или H.323 ID
    int ttl = 300;      // Время жизни регистрации (сек)
    bool discover = true;  // Авто-обнаружение GK
};

// Менеджер Gatekeeper
class GatekeeperManager {
public:
    GatekeeperManager();
    ~GatekeeperManager();
    
    // Настройка
    void setConfig(const GatekeeperConfig& config);
    const GatekeeperConfig& getConfig() const { return config_; }
    
    // Регистрация
    bool registerWithGatekeeper();
    bool unregisterFromGatekeeper();
    
    // Статус
    RegistrationStatus getStatus() const { return status_; }
    bool isRegistered() const { return status_ == RegistrationStatus::REGISTERED; }
    
    // Обнаружение Gatekeeper
    bool discoverGatekeeper();
    std::string getDiscoveredGatekeeper() const { return discoveredGk_; }
    
    // Callback
    using StatusCallback = std::function<void(RegistrationStatus)>;
    void setStatusCallback(StatusCallback callback);
    
    // Статистика
    time_t getRegistrationTime() const { return registrationTime_; }
    int getRegistrationAttempts() const { return registrationAttempts_; }
    
private:
    void setStatus(RegistrationStatus status);
    
    GatekeeperConfig config_;
    RegistrationStatus status_ = RegistrationStatus::NOT_REGISTERED;
    std::string discoveredGk_;
    time_t registrationTime_ = 0;
    int registrationAttempts_ = 0;
    StatusCallback statusCallback_;
};

} // namespace h323p
```

**Файл: `src/h323/ras.cpp`**

```cpp
#include "ras.hpp"
#include "utils/logging.hpp"
#include <chrono>

namespace h323p {

GatekeeperManager::GatekeeperManager() = default;
GatekeeperManager::~GatekeeperManager() {
    unregisterFromGatekeeper();
}

void GatekeeperManager::setConfig(const GatekeeperConfig& config) {
    config_ = config;
}

void GatekeeperManager::setStatus(RegistrationStatus status) {
    status_ = status;
    if (statusCallback_) {
        statusCallback_(status);
    }
}

void GatekeeperManager::setStatusCallback(StatusCallback callback) {
    statusCallback_ = callback;
}

bool GatekeeperManager::discoverGatekeeper() {
    if (!config_.discover) {
        LOG_INFO("Авто-обнаружение Gatekeeper отключено");
        return false;
    }
    
    LOG_INFO("Обнаружение Gatekeeper (multicast)...");
    
    // H.225 RAS Gatekeeper Request (GRQ) multicast
    // Реализация зависит от API H323Plus
    
    // Симуляция для примера
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Предполагаем, что обнаружили
    discoveredGk_ = "192.168.1.1";
    LOG_INFO_FMT("Gatekeeper обнаружен: %s", discoveredGk_.c_str());
    
    return true;
}

bool GatekeeperManager::registerWithGatekeeper() {
    if (status_ == RegistrationStatus::REGISTERED) {
        LOG_WARN("Уже зарегистрирован на Gatekeeper");
        return true;
    }
    
    setStatus(RegistrationStatus::REGISTERING);
    registrationAttempts_++;
    
    std::string gkAddress = config_.address;
    
    // Если адрес не указан, пытаемся обнаружить
    if (gkAddress.empty() && config_.discover) {
        if (!discoverGatekeeper()) {
            LOG_ERROR("Не удалось обнаружить Gatekeeper");
            setStatus(RegistrationStatus::ERROR);
            return false;
        }
        gkAddress = discoveredGk_;
    }
    
    if (gkAddress.empty()) {
        LOG_ERROR("Адрес Gatekeeper не указан");
        setStatus(RegistrationStatus::ERROR);
        return false;
    }
    
    LOG_INFO_FMT("Регистрация на Gatekeeper: %s:%d", 
                 gkAddress.c_str(), config_.port);
    
    // Использование H323Endpoint для регистрации
    auto& endpoint = H323Endpoint::instance();
    
    bool result = endpoint.registerWithGatekeeper(gkAddress);
    
    if (result) {
        setStatus(RegistrationStatus::REGISTERED);
        registrationTime_ = std::time(nullptr);
        LOG_INFO("Регистрация успешна");
    } else {
        setStatus(RegistrationStatus::REJECTED);
        LOG_ERROR("Регистрация отклонена");
    }
    
    return result;
}

bool GatekeeperManager::unregisterFromGatekeeper() {
    if (status_ != RegistrationStatus::REGISTERED) {
        return false;
    }
    
    LOG_INFO("Отписка от Gatekeeper");
    
    auto& endpoint = H323Endpoint::instance();
    bool result = endpoint.unregisterFromGatekeeper();
    
    if (result) {
        setStatus(RegistrationStatus::NOT_REGISTERED);
        registrationTime_ = 0;
    }
    
    return result;
}

} // namespace h323p
```

---

### 2.3 H.225 Q.931

**Файл: `src/h323/q931.hpp`**

```cpp
#pragma once

#include "h323_endpoint.hpp"
#include <string>
#include <map>

namespace h323p {

// Состояния вызова Q.931
enum class CallState {
    NULL_STATE,           // Начальное состояние
    CALL_INITIATED,       // Вызов инициирован (Setup sent)
    CALL_RECEIVED,        // Вызов получен (Setup received)
    CALL_PRESENTED,       // Вызов представлен пользователю
    CALL_PROCEEDING,      // Call Proceeding received
    CALL_DELIVERED,       // Call Delivered
    CALL_ALERTING,        // Alerting (звонок)
    CALL_ACTIVE,          // Call Active (Connect received)
    RELEASE_REQUEST,      // Запрос освобождения
    RELEASE_INDICATION,   // Индикация освобождения
    RELEASE_COMPLETE,     // Освобождение завершено
    ERROR_STATE           // Ошибка
};

// Причины завершения вызова (Q.850)
enum class ReleaseCause {
    NORMAL_CALL_CLEARING = 16,
    NO_USER_RESPONDING = 18,
    NO_ANSWER = 19,
    CALL_REJECTED = 21,
    NUMBER_CHANGED = 22,
    USER_BUSY = 17,
    NETWORK_OUT_OF_ORDER = 38,
    TEMPORARY_FAILURE = 41,
    SERVICE_UNAVAILABLE = 47,
    INVALID_NUMBER = 28,
    UNKNOWN = -1
};

// Информация о вызове
struct CallInfo {
    std::string callId;
    std::string localAddress;
    std::string remoteAddress;
    std::string localNumber;
    std::string remoteNumber;
    CallState state = CallState::NULL_STATE;
    ReleaseCause releaseCause = ReleaseCause::UNKNOWN;
    time_t startTime = 0;
    time_t connectTime = 0;
    time_t releaseTime = 0;
    bool isIncoming = false;
    bool fastStart = false;
    bool h245Tunneling = false;
};

// Менеджер вызовов Q.931
class Q931Manager {
public:
    Q931Manager();
    ~Q931Manager();
    
    // Создание вызова
    std::string createOutgoingCall(const std::string& destination);
    
    // Обработка входящего вызова
    void handleIncomingCall(const std::string& callId, const std::string& caller);
    
    // Ответ на вызов
    bool answerCall(const std::string& callId);
    
    // Отклонение вызова
    bool rejectCall(const std::string& callId, ReleaseCause cause = ReleaseCause::CALL_REJECTED);
    
    // Завершение вызова
    bool releaseCall(const std::string& callId, ReleaseCause cause = ReleaseCause::NORMAL_CALL_CLEARING);
    
    // Получение информации о вызове
    CallInfo* getCallInfo(const std::string& callId);
    
    // Все активные вызовы
    std::vector<std::string> getActiveCallIds() const;
    size_t getActiveCallCount() const;
    
    // Callback
    using CallStateCallback = std::function<void(const std::string& callId, CallState newState)>;
    void setCallStateCallback(CallStateCallback callback);
    
private:
    void setCallState(const std::string& callId, CallState newState);
    std::string generateCallId();
    
    std::map<std::string, std::unique_ptr<CallInfo>> calls_;
    CallStateCallback stateCallback_;
    uint32_t callCounter_ = 0;
};

// Конвертация состояний
std::string callStateToString(CallState state);
std::string releaseCauseToString(ReleaseCause cause);

} // namespace h323p
```

**Файл: `src/h323/q931.cpp`**

```cpp
#include "q931.hpp"
#include "utils/logging.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>

namespace h323p {

Q931Manager::Q931Manager() = default;
Q931Manager::~Q931Manager() {
    // Освобождение всех вызовов
    for (auto& [callId, info] : calls_) {
        if (info->state != CallState::NULL_STATE && 
            info->state != CallState::RELEASE_COMPLETE) {
            releaseCall(callId, ReleaseCause::NORMAL_CALL_CLEARING);
        }
    }
}

std::string Q931Manager::generateCallId() {
    std::ostringstream oss;
    oss << "h323p-" << std::hex << std::this_thread::get_id() 
        << "-" << ++callCounter_;
    return oss.str();
}

void Q931Manager::setCallStateCallback(CallStateCallback callback) {
    stateCallback_ = callback;
}

void Q931Manager::setCallState(const std::string& callId, CallState newState) {
    auto it = calls_.find(callId);
    if (it == calls_.end()) return;
    
    CallInfo* info = it->second.get();
    CallState oldState = info->state;
    info->state = newState;
    
    LOG_INFO_FMT("Call %s: %s -> %s", 
                 callId.c_str(),
                 callStateToString(oldState).c_str(),
                 callStateToString(newState).c_str());
    
    if (stateCallback_) {
        stateCallback_(callId, newState);
    }
    
    // Обновление времён
    if (newState == CallState::CALL_ACTIVE && info->connectTime == 0) {
        info->connectTime = std::time(nullptr);
    } else if (newState == CallState::RELEASE_COMPLETE) {
        info->releaseTime = std::time(nullptr);
    }
}

std::string Q931Manager::createOutgoingCall(const std::string& destination) {
    std::string callId = generateCallId();
    
    LOG_INFO_FMT("Создание исходящего вызова: %s (ID: %s)", 
                 destination.c_str(), callId.c_str());
    
    auto info = std::make_unique<CallInfo>();
    info->callId = callId;
    info->remoteNumber = destination;
    info->isIncoming = false;
    info->startTime = std::time(nullptr);
    info->state = CallState::CALL_INITIATED;
    
    calls_[callId] = std::move(info);
    
    // Использование H323Endpoint для создания вызова
    auto& endpoint = H323Endpoint::instance();
    std::string actualCallId;
    
    if (!endpoint.makeCall(destination, actualCallId)) {
        LOG_ERROR("Не удалось создать вызов");
        setCallState(callId, CallState::ERROR_STATE);
        return "";
    }
    
    LOG_INFO_FMT("Вызов создан: %s", callId.c_str());
    return callId;
}

void Q931Manager::handleIncomingCall(const std::string& callId, const std::string& caller) {
    LOG_INFO_FMT("Входящий вызов: %s от %s", callId.c_str(), caller.c_str());
    
    auto info = std::make_unique<CallInfo>();
    info->callId = callId;
    info->remoteNumber = caller;
    info->remoteAddress = caller;
    info->isIncoming = true;
    info->startTime = std::time(nullptr);
    info->state = CallState::CALL_RECEIVED;
    
    calls_[callId] = std::move(info);
    
    // Callback будет уведомлён через H323Endpoint
}

bool Q931Manager::answerCall(const std::string& callId) {
    auto it = calls_.find(callId);
    if (it == calls_.end()) {
        LOG_ERROR_FMT("Вызов %s не найден", callId.c_str());
        return false;
    }
    
    CallInfo* info = it->second.get();
    
    if (info->state != CallState::CALL_RECEIVED && 
        info->state != CallState::CALL_ALERTING) {
        LOG_ERROR_FMT("Невозможно ответить на вызов в состоянии %s",
                      callStateToString(info->state).c_str());
        return false;
    }
    
    LOG_INFO_FMT("Ответ на вызов: %s", callId.c_str());
    
    auto& endpoint = H323Endpoint::instance();
    bool result = endpoint.answerCall(callId);
    
    if (result) {
        setCallState(callId, CallState::CALL_ACTIVE);
    }
    
    return result;
}

bool Q931Manager::rejectCall(const std::string& callId, ReleaseCause cause) {
    auto it = calls_.find(callId);
    if (it == calls_.end()) {
        return false;
    }
    
    LOG_INFO_FMT("Отклонение вызова %s: %s", 
                 callId.c_str(), releaseCauseToString(cause).c_str());
    
    return releaseCall(callId, cause);
}

bool Q931Manager::releaseCall(const std::string& callId, ReleaseCause cause) {
    auto it = calls_.find(callId);
    if (it == calls_.end()) {
        LOG_ERROR_FMT("Вызов %s не найден", callId.c_str());
        return false;
    }
    
    CallInfo* info = it->second.get();
    
    LOG_INFO_FMT("Завершение вызова %s: %s", 
                 callId.c_str(), releaseCauseToString(cause).c_str());
    
    setCallState(callId, CallState::RELEASE_REQUEST);
    
    auto& endpoint = H323Endpoint::instance();
    bool result = endpoint.releaseCall(callId, static_cast<int>(cause));
    
    if (result) {
        info->releaseCause = cause;
        setCallState(callId, CallState::RELEASE_COMPLETE);
    }
    
    return result;
}

CallInfo* Q931Manager::getCallInfo(const std::string& callId) {
    auto it = calls_.find(callId);
    if (it != calls_.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<std::string> Q931Manager::getActiveCallIds() const {
    std::vector<std::string> result;
    for (const auto& [callId, info] : calls_) {
        if (info->state != CallState::NULL_STATE &&
            info->state != CallState::RELEASE_COMPLETE) {
            result.push_back(callId);
        }
    }
    return result;
}

size_t Q931Manager::getActiveCallCount() const {
    size_t count = 0;
    for (const auto& [callId, info] : calls_) {
        if (info->state != CallState::NULL_STATE &&
            info->state != CallState::RELEASE_COMPLETE) {
            count++;
        }
    }
    return count;
}

// Конвертация состояний в строку

std::string callStateToString(CallState state) {
    switch (state) {
        case CallState::NULL_STATE: return "NULL";
        case CallState::CALL_INITIATED: return "INITIATED";
        case CallState::CALL_RECEIVED: return "RECEIVED";
        case CallState::CALL_PRESENTED: return "PRESENTED";
        case CallState::CALL_PROCEEDING: return "PROCEEDING";
        case CallState::CALL_DELIVERED: return "DELIVERED";
        case CallState::CALL_ALERTING: return "ALERTING";
        case CallState::CALL_ACTIVE: return "ACTIVE";
        case CallState::RELEASE_REQUEST: return "RELEASE_REQ";
        case CallState::RELEASE_INDICATION: return "RELEASE_IND";
        case CallState::RELEASE_COMPLETE: return "RELEASED";
        case CallState::ERROR_STATE: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string releaseCauseToString(ReleaseCause cause) {
    switch (cause) {
        case ReleaseCause::NORMAL_CALL_CLEARING: return "Normal Call Clearing (16)";
        case ReleaseCause::NO_USER_RESPONDING: return "No User Responding (18)";
        case ReleaseCause::NO_ANSWER: return "No Answer (19)";
        case ReleaseCause::CALL_REJECTED: return "Call Rejected (21)";
        case ReleaseCause::NUMBER_CHANGED: return "Number Changed (22)";
        case ReleaseCause::USER_BUSY: return "User Busy (17)";
        case ReleaseCause::NETWORK_OUT_OF_ORDER: return "Network Out of Order (38)";
        case ReleaseCause::TEMPORARY_FAILURE: return "Temporary Failure (41)";
        case ReleaseCause::SERVICE_UNAVAILABLE: return "Service Unavailable (47)";
        case ReleaseCause::INVALID_NUMBER: return "Invalid Number (28)";
        default: return "Unknown";
    }
}

} // namespace h323p
```

---

### 2.4 Call Manager

**Файл: `src/core/call_manager.hpp`**

```cpp
#pragma once

#include "h323/h323_endpoint.hpp"
#include "h323/q931.hpp"
#include "h323/ras.hpp"
#include <string>
#include <vector>
#include <functional>

namespace h323p {

// Конфигурация Call Manager
struct CallManagerConfig {
    size_t maxCalls = 10;
    size_t repeatCount = 1;
    bool cycleMode = false;
    int tminCall = 10;  // Мин. длительность вызова (сек)
    int tmaxCall = 30;  // Макс. длительность вызова (сек)
    bool requireGatekeeper = false;
    std::vector<std::string> destinations;
};

// Статистика вызовов
struct CallStatistics {
    size_t totalCalls = 0;
    size_t successfulCalls = 0;
    size_t failedCalls = 0;
    size_t activeCalls = 0;
    double averageCallDuration = 0.0;
    double totalCallDuration = 0.0;
};

// Call Manager
class CallManager {
public:
    CallManager();
    ~CallManager();
    
    // Конфигурация
    void setConfig(const CallManagerConfig& config);
    const CallManagerConfig& getConfig() const { return config_; }
    
    // Запуск режимов
    bool startOutgoingCall(const std::string& destination);
    bool startListening();
    void stop();
    
    // Статус
    bool isRunning() const { return running_; }
    bool isListening() const { return listening_; }
    
    // Статистика
    CallStatistics getStatistics() const;
    void resetStatistics();
    
    // Callback для событий
    using EventCallback = std::function<void(const std::string& event, const std::string& data)>;
    void setEventCallback(EventCallback callback);
    
private:
    // Обработчики событий
    void onH323Event(const H323Event& event);
    void onCallStateChanged(const std::string& callId, CallState newState);
    
    // Внутренние методы
    void processCall(const std::string& destination);
    void answerIncomingCall(const std::string& callId);
    
    CallManagerConfig config_;
    CallStatistics stats_;
    EventCallback eventCallback_;
    
    Q931Manager q931Manager_;
    GatekeeperManager gkManager_;
    
    bool running_ = false;
    bool listening_ = false;
    bool stopRequested_ = false;
};

} // namespace h323p
```

**Файл: `src/core/call_manager.cpp`**

```cpp
#include "call_manager.hpp"
#include "utils/logging.hpp"
#include "utils/timer.hpp"
#include <random>
#include <thread>

namespace h323p {

CallManager::CallManager() {
    // Настройка callback
    q931Manager_.setCallStateCallback(
        [this](const std::string& callId, CallState newState) {
            onCallStateChanged(callId, newState);
        }
    );
}

CallManager::~CallManager() {
    stop();
}

void CallManager::setConfig(const CallManagerConfig& config) {
    config_ = config;
}

void CallManager::setEventCallback(EventCallback callback) {
    eventCallback_ = callback;
}

bool CallManager::startOutgoingCall(const std::string& destination) {
    if (running_) {
        LOG_WARN("CallManager уже запущен");
        return false;
    }
    
    LOG_INFO_FMT("Запуск исходящих вызовов: %s", destination.c_str());
    running_ = true;
    stopRequested_ = false;
    
    // Инициализация H323Endpoint
    H323Config h323Config;
    h323Config.gatekeeperRequired = config_.requireGatekeeper;
    
    auto& endpoint = H323Endpoint::instance();
    if (!endpoint.initialize(h323Config)) {
        LOG_ERROR("Ошибка инициализации H323Endpoint");
        return false;
    }
    
    // Регистрация на Gatekeeper если требуется
    if (config_.requireGatekeeper) {
        GatekeeperConfig gkConfig;
        gkConfig.address = config_.requireGatekeeper ? "" : "";  // Auto-discover
        gkManager_.setConfig(gkConfig);
        
        if (!gkManager_.registerWithGatekeeper()) {
            LOG_ERROR("Ошибка регистрации на Gatekeeper");
            return false;
        }
    }
    
    // Запуск вызова
    processCall(destination);
    
    return true;
}

bool CallManager::startListening() {
    if (running_) {
        LOG_WARN("CallManager уже запущен");
        return false;
    }
    
    LOG_INFO("Запуск режима прослушивания");
    running_ = true;
    listening_ = true;
    stopRequested_ = false;
    
    // Инициализация H323Endpoint
    H323Config h323Config;
    h323Config.tcpPort = 1720;
    
    auto& endpoint = H323Endpoint::instance();
    if (!endpoint.initialize(h323Config)) {
        LOG_ERROR("Ошибка инициализации H323Endpoint");
        return false;
    }
    
    // Endpoint автоматически начинает слушать после инициализации
    
    return true;
}

void CallManager::stop() {
    if (!running_) return;
    
    LOG_INFO("Остановка CallManager");
    
    stopRequested_ = true;
    running_ = false;
    listening_ = false;
    
    // Завершение всех вызовов
    auto activeCalls = q931Manager_.getActiveCallIds();
    for (const auto& callId : activeCalls) {
        q931Manager_.releaseCall(callId, ReleaseCause::NORMAL_CALL_CLEARING);
    }
    
    // Отписка от Gatekeeper
    gkManager_.unregisterFromGatekeeper();
    
    // Завершение H323Endpoint
    H323Endpoint::instance().shutdown();
}

void CallManager::processCall(const std::string& destination) {
    LOG_INFO_FMT("Обработка вызова: %s", destination.c_str());
    
    stats_.totalCalls++;
    stats_.activeCalls++;
    
    // Создание вызова
    std::string callId = q931Manager_.createOutgoingCall(destination);
    
    if (callId.empty()) {
        LOG_ERROR("Не удалось создать вызов");
        stats_.failedCalls++;
        stats_.activeCalls--;
        
        if (eventCallback_) {
            eventCallback_("call_failed", destination);
        }
        return;
    }
    
    // Ожидание установки вызова (упрощённо)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Генерация длительности вызова
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(config_.tminCall, config_.tmaxCall);
    int callDuration = dist(gen);
    
    LOG_INFO_FMT("Длительность вызова: %d сек", callDuration);
    
    // Ожидание завершения
    std::this_thread::sleep_for(std::chrono::seconds(callDuration));
    
    // Завершение вызова
    q931Manager_.releaseCall(callId, ReleaseCause::NORMAL_CALL_CLEARING);
    
    stats_.activeCalls--;
    stats_.successfulCalls++;
    stats_.totalCallDuration += callDuration;
    stats_.averageCallDuration = stats_.totalCallDuration / stats_.successfulCalls;
    
    if (eventCallback_) {
        eventCallback_("call_completed", callId);
    }
    
    // Повтор если требуется
    if (config_.repeatCount > 1 && !stopRequested_) {
        config_.repeatCount--;
        LOG_INFO_FMT("Повтор вызова (осталось: %d)", config_.repeatCount);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        processCall(destination);
    }
}

void CallManager::answerIncomingCall(const std::string& callId) {
    LOG_INFO_FMT("Ответ на входящий вызов: %s", callId.c_str());
    
    bool result = q931Manager_.answerCall(callId);
    
    if (result) {
        if (eventCallback_) {
            eventCallback_("call_answered", callId);
        }
    }
}

void CallManager::onH323Event(const H323Event& event) {
    switch (event.type) {
        case H323EventType::INCOMING_CALL:
            LOG_INFO_FMT("Входящий вызов: %s от %s", 
                         event.callId.c_str(), event.remoteAddress.c_str());
            answerIncomingCall(event.callId);
            break;
            
        case H323EventType::CALL_ESTABLISHED:
            LOG_INFO_FMT("Вызов установлен: %s", event.callId.c_str());
            break;
            
        case H323EventType::CALL_CLEARED:
            LOG_INFO_FMT("Вызов завершён: %s", event.callId.c_str());
            break;
            
        case H323EventType::GATEKEEPER_REGISTERED:
            LOG_INFO_FMT("Зарегистрирован на Gatekeeper: %s", 
                         event.gatekeeperAddress.c_str());
            break;
            
        case H323EventType::GATEKEEPER_REGISTRATION_FAILED:
            LOG_ERROR("Ошибка регистрации на Gatekeeper");
            break;
            
        default:
            break;
    }
}

void CallManager::onCallStateChanged(const std::string& callId, CallState newState) {
    LOG_INFO_FMT("Состояние вызова %s: %s", 
                 callId.c_str(), callStateToString(newState).c_str());
    
    switch (newState) {
        case CallState::CALL_ACTIVE:
            stats_.successfulCalls++;
            break;
            
        case CallState::ERROR_STATE:
        case CallState::RELEASE_COMPLETE:
            stats_.activeCalls--;
            break;
            
        default:
            break;
    }
}

CallStatistics CallManager::getStatistics() const {
    CallStatistics result = stats_;
    result.activeCalls = q931Manager_.getActiveCallCount();
    return result;
}

void CallManager::resetStatistics() {
    stats_ = CallStatistics();
}

} // namespace h323p
```

---

### 2.5 Команда call

**Файл: `src/cli/commands/cmd_call.hpp`**

```cpp
#pragma once

#include "cli/commands.hpp"
#include "core/call_manager.hpp"

namespace h323p {

// Команда call
class CallCommand {
public:
    CallCommand();
    
    // Выполнение команды
    int execute(const CommandConfig& config);
    
private:
    CallManager callManager_;
};

} // namespace h323p
```

**Файл: `src/cli/commands/cmd_call.cpp`**

```cpp
#include "cmd_call.hpp"
#include "utils/logging.hpp"
#include <iostream>
#include <csignal>

namespace h323p {

CallCommand::CallCommand() = default;

int CallCommand::execute(const CommandConfig& config) {
    LOG_INFO("Выполнение команды: call");
    
    // Проверка назначения
    if (config.destination.empty()) {
        std::cerr << "Ошибка: требуется указать адрес назначения" << std::endl;
        return 1;
    }
    
    // Настройка CallManager
    CallManagerConfig cmConfig;
    cmConfig.maxCalls = config.maxCalls;
    cmConfig.repeatCount = config.repeatCount;
    cmConfig.cycleMode = config.cycleMode;
    cmConfig.tminCall = config.tminCall;
    cmConfig.tmaxCall = config.tmaxCall;
    cmConfig.requireGatekeeper = !config.gatekeeper.empty();
    
    callManager_.setConfig(cmConfig);
    
    // Обработка сигналов
    std::atomic<bool> interrupted(false);
    std::signal(SIGINT, [&interrupted](int) {
        interrupted = true;
    });
    
    // Запуск вызова
    if (!callManager_.startOutgoingCall(config.destination)) {
        std::cerr << "Ошибка запуска вызова" << std::endl;
        return 1;
    }
    
    // Ожидание завершения
    while (!interrupted && callManager_.isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Остановка
    callManager_.stop();
    
    // Вывод статистики
    auto stats = callManager_.getStatistics();
    std::cout << "\nСтатистика:" << std::endl;
    std::cout << "  Всего вызовов: " << stats.totalCalls << std::endl;
    std::cout << "  Успешных: " << stats.successfulCalls << std::endl;
    std::cout << "  Неудачных: " << stats.failedCalls << std::endl;
    std::cout << "  Средняя длительность: " << stats.averageCallDuration << " сек" << std::endl;
    
    return 0;
}

} // namespace h323p
```

---

### 2.6 Команда listen

**Файл: `src/cli/commands/cmd_listen.hpp`**

```cpp
#pragma once

#include "cli/commands.hpp"
#include "core/call_manager.hpp"

namespace h323p {

// Команда listen
class ListenCommand {
public:
    ListenCommand();
    
    // Выполнение команды
    int execute(const CommandConfig& config);
    
private:
    CallManager callManager_;
};

} // namespace h323p
```

**Файл: `src/cli/commands/cmd_listen.cpp`**

```cpp
#include "cmd_listen.hpp"
#include "utils/logging.hpp"
#include <iostream>
#include <csignal>

namespace h323p {

ListenCommand::ListenCommand() = default;

int ListenCommand::execute(const CommandConfig& config) {
    LOG_INFO("Выполнение команды: listen");
    
    // Настройка CallManager
    CallManagerConfig cmConfig;
    cmConfig.maxCalls = config.maxCalls;
    
    callManager_.setConfig(cmConfig);
    
    // Обработка сигналов
    std::atomic<bool> interrupted(false);
    std::signal(SIGINT, [&interrupted](int) {
        interrupted = true;
    });
    
    // Запуск прослушивания
    if (!callManager_.startListening()) {
        std::cerr << "Ошибка запуска прослушивания" << std::endl;
        return 1;
    }
    
    std::cout << "Режим прослушивания запущен (порт 1720)" << std::endl;
    std::cout << "Нажмите Ctrl+C для остановки" << std::endl;
    
    // Ожидание прерывания
    while (!interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Вывод статистики
        auto stats = callManager_.getStatistics();
        if (stats.activeCalls > 0) {
            std::cout << "Активных вызовов: " << stats.activeCalls << std::endl;
        }
    }
    
    // Остановка
    callManager_.stop();
    
    // Вывод статистики
    auto stats = callManager_.getStatistics();
    std::cout << "\nСтатистика:" << std::endl;
    std::cout << "  Всего вызовов: " << stats.totalCalls << std::endl;
    std::cout << "  Успешных: " << stats.successfulCalls << std::endl;
    
    return 0;
}

} // namespace h323p
```

---

### 2.7 Интеграционные тесты

**Файл: `tests/test_h323.cpp`**

```cpp
#include "CppUTest/TestHarness.h"
#include "h323/h323_endpoint.hpp"
#include "h323/q931.hpp"
#include "h323/ras.hpp"

using namespace h323p;

// Тесты Q931Manager

TEST_GROUP(Q931Test) {
    void setup() override { }
    void teardown() override { }
};

TEST(Q931Test, CreateCallId) {
    Q931Manager manager;

    std::string callId = manager.createOutgoingCall("192.168.1.100");

    CHECK_FALSE(callId.empty());
    LONGS_EQUAL(1, manager.getActiveCallCount());
}

TEST(Q931Test, GetCallInfo) {
    Q931Manager manager;

    std::string callId = manager.createOutgoingCall("192.168.1.100");

    CallInfo* info = manager.getCallInfo(callId);
    CHECK(info != nullptr);
    STRCMP_EQUAL("192.168.1.100", info->remoteNumber.c_str());
    CHECK_FALSE(info->isIncoming);
}

TEST(Q931Test, ReleaseCall) {
    Q931Manager manager;

    std::string callId = manager.createOutgoingCall("192.168.1.100");
    LONGS_EQUAL(1, manager.getActiveCallCount());

    bool result = manager.releaseCall(callId, ReleaseCause::NORMAL_CALL_CLEARING);
    CHECK_TRUE(result);
    LONGS_EQUAL(0, manager.getActiveCallCount());
}

TEST(Q931Test, MultipleCalls) {
    Q931Manager manager;

    std::string callId1 = manager.createOutgoingCall("192.168.1.100");
    std::string callId2 = manager.createOutgoingCall("192.168.1.101");
    std::string callId3 = manager.createOutgoingCall("192.168.1.102");

    LONGS_EQUAL(3, manager.getActiveCallCount());

    manager.releaseCall(callId2, ReleaseCause::NORMAL_CALL_CLEARING);
    LONGS_EQUAL(2, manager.getActiveCallCount());
}

// Тесты GatekeeperManager

TEST(GatekeeperTest, Config) {
    GatekeeperManager manager;

    GatekeeperConfig config;
    config.address = "192.168.1.1";
    config.port = 1719;
    config.username = "test";
    config.ttl = 600;

    manager.setConfig(config);

    const auto& retrievedConfig = manager.getConfig();
    STRCMP_EQUAL("192.168.1.1", retrievedConfig.address.c_str());
    LONGS_EQUAL(1719, retrievedConfig.port);
    STRCMP_EQUAL("test", retrievedConfig.username.c_str());
}

TEST(GatekeeperTest, Status) {
    GatekeeperManager manager;

    LONGS_EQUAL((int)RegistrationStatus::NOT_REGISTERED, (int)manager.getStatus());
    CHECK_FALSE(manager.isRegistered());
}

// Тесты CallState conversion

TEST(CallStateTest, StateToString) {
    STRCMP_EQUAL("NULL", callStateToString(CallState::NULL_STATE).c_str());
    STRCMP_EQUAL("INITIATED", callStateToString(CallState::CALL_INITIATED).c_str());
    STRCMP_EQUAL("ACTIVE", callStateToString(CallState::CALL_ACTIVE).c_str());
    STRCMP_EQUAL("RELEASED", callStateToString(CallState::RELEASE_COMPLETE).c_str());
}

TEST(CallStateTest, CauseToString) {
    STRCMP_EQUAL("Normal Call Clearing (16)", 
                 releaseCauseToString(ReleaseCause::NORMAL_CALL_CLEARING).c_str());
    STRCMP_EQUAL("User Busy (17)", 
                 releaseCauseToString(ReleaseCause::USER_BUSY).c_str());
    STRCMP_EQUAL("Call Rejected (21)", 
                 releaseCauseToString(ReleaseCause::CALL_REJECTED).c_str());
}
```

---

## 📊 Диаграмма состояний вызова

```
                                    ┌─────────────────┐
                                    │                 │
                                    ▼                 │
┌──────────────┐    Setup    ┌─────────────┐         │
│            │ ───────────► │             │         │
│   NULL     │              │  INITIATED  │         │
│            │ ◄─────────── │             │         │
└──────────────┘   Release   └─────────────┘         │
       ▲                                             │
       │                                             │
       │  Setup                                      │
       │                                             │
       │  ┌──────────────┐    Call Proceeding   ┌─────────────┐
       │  │              │ ───────────────────► │             │
       └─►│  RECEIVED    │                      │  PROCEEDING │
          │              │ ◄─────────────────── │             │
          └──────────────┘      Release          └─────────────┘
                 │                                      │
                 │  Alerting                            │
                 │                                      │
                 ▼                                      │
          ┌─────────────┐                               │
          │             │                               │
          │  ALERTING   │                               │
          │             │                               │
          └─────────────┘                               │
                 │                                      │
                 │  Connect                             │
                 │                                      │
                 ▼                                      │
          ┌─────────────┐                               │
          │             │                               │
          │   ACTIVE    │ ◄─────────────────────────────┘
          │             │
          └─────────────┘
                 │
                 │  Release
                 │
                 ▼
          ┌─────────────┐
          │             │
          │  RELEASED   │
          │             │
          └─────────────┘
```

---

## ✅ Критерии завершения

### Чек-лист завершения Этапа 2

```
[ ] 1. H323Endpoint реализован и инициализируется
[ ] 2. H.225 RAS работает (регистрация на GK)
[ ] 3. H.225 Q.931 работает (создание/приём вызовов)
[ ] 4. Call Manager управляет вызовами
[ ] 5. Команда call выполняется
[ ] 6. Команда listen выполняется
[ ] 7. Интеграционные тесты написаны
[ ] 8. Все тесты проходят
[ ] 9. Документация обновлена
```

---

## 🔧 Рекомендации по улучшению (из RECOMMENDATIONS.md)

### Рекомендация #1: Обработка ошибок H323Plus

**Приоритет:** 🔴 Критический  
**Сложность:** Средняя

**Проблема:** API H323Plus возвращает коды ошибок, которые нужно обрабатывать.

**Решение:**
```cpp
// src/h323/h323_errors.hpp
#pragma once

#include <string>

namespace h323p {

enum class H323Error {
    // Gatekeeper ошибки
    GATEKEEPER_DISCOVERY_FAILED = 1,
    GATEKEEPER_REJECTED = 2,
    GATEKEEPER_TIMEOUT = 3,
    
    // Вызовы
    CALL_SETUP_FAILED = 10,
    CALL_RELEASED_ABNORMALLY = 11,
    CALL_CAPABILITY_MISMATCH = 12,
    
    // H.245
    CAPABILITY_NEGOTIATION_FAILED = 20,
    CHANNEL_OPEN_FAILED = 21,
    MASTER_SLAVE_DETERMINATION_FAILED = 22,
    
    // Медиа
    RTP_PORT_BIND_FAILED = 30,
    CODEC_NOT_SUPPORTED = 31,
    
    // Общее
    SUCCESS = 0,
    UNKNOWN_ERROR = 999
};

// Маппинг ошибок H323Plus
H323Error mapH323PlusError(int h323plusCode);
std::string errorToString(H323Error error);

} // namespace h323p
```

**Реализация:**
```cpp
// src/h323/h323_errors.cpp
#include "h323_errors.hpp"
#include <h323.h>

namespace h323p {

H323Error mapH323PlusError(int h323plusCode) {
    switch (h323plusCode) {
        case 0: return H323Error::SUCCESS;
        
        // Gatekeeper ошибки (H.225 RAS)
        case 16: return H323Error::GATEKEEPER_DISCOVERY_FAILED;
        case 17: return H323Error::GATEKEEPER_REJECTED;
        case 18: return H323Error::GATEKEEPER_TIMEOUT;
        
        // Вызовы
        case 31: return H323Error::CALL_SETUP_FAILED;
        case 32: return H323Error::CALL_RELEASED_ABNORMALLY;
        case 33: return H323Error::CALL_CAPABILITY_MISMATCH;
        
        // H.245
        case 64: return H323Error::CAPABILITY_NEGOTIATION_FAILED;
        case 65: return H323Error::CHANNEL_OPEN_FAILED;
        case 66: return H323Error::MASTER_SLAVE_DETERMINATION_FAILED;
        
        // Медиа
        case 96: return H323Error::RTP_PORT_BIND_FAILED;
        case 97: return H323Error::CODEC_NOT_SUPPORTED;
        
        default: return H323Error::UNKNOWN_ERROR;
    }
}

std::string errorToString(H323Error error) {
    switch (error) {
        case H323Error::SUCCESS: return "Success";
        case H323Error::GATEKEEPER_DISCOVERY_FAILED: return "Gatekeeper discovery failed";
        case H323Error::GATEKEEPER_REJECTED: return "Gatekeeper rejected";
        case H323Error::GATEKEEPER_TIMEOUT: return "Gatekeeper timeout";
        case H323Error::CALL_SETUP_FAILED: return "Call setup failed";
        case H323Error::CALL_RELEASED_ABNORMALLY: return "Call released abnormally";
        case H323Error::CALL_CAPABILITY_MISMATCH: return "Call capability mismatch";
        case H323Error::CAPABILITY_NEGOTIATION_FAILED: return "Capability negotiation failed";
        case H323Error::CHANNEL_OPEN_FAILED: return "Channel open failed";
        case H323Error::MASTER_SLAVE_DETERMINATION_FAILED: return "Master/Slave determination failed";
        case H323Error::RTP_PORT_BIND_FAILED: return "RTP port bind failed";
        case H323Error::CODEC_NOT_SUPPORTED: return "Codec not supported";
        case H323Error::UNKNOWN_ERROR: return "Unknown error";
        default: return "Invalid error";
    }
}

} // namespace h323p
```

**Использование:**
```cpp
// В h323_endpoint.cpp
bool H323Endpoint::registerWithGatekeeper(const std::string& gkAddress) {
    int resultCode = endpoint_->RegisterGatekeeper(gkAddress.c_str());
    
    if (resultCode != 0) {
        H323Error error = mapH323PlusError(resultCode);
        LOG_ERROR_FMT("Gatekeeper registration failed: %s", errorToString(error).c_str());
        return false;
    }
    
    LOG_INFO("Gatekeeper registration successful");
    return true;
}
```

---

### Рекомендация #2: Потокобезопасность Call Manager

**Приоритет:** 🔴 Критический  
**Сложность:** Низкая

**Проблема:** В `CallManager` используется `std::map` без синхронизации.

**Решение:**
```cpp
// src/core/call_manager.hpp (обновлённая версия)
#pragma once

#include <map>
#include <mutex>
#include <memory>

namespace h323p {

class CallManager {
public:
    // Управление вызовами
    void startOutgoingCall(const std::string& destination);
    void acceptIncomingCall(const std::string& callId);
    void releaseCall(const std::string& callId, int cause = 0);
    
    // Статистика
    size_t getActiveCalls() const;
    Call* getCall(const std::string& callId);
    
private:
    // recursive_mutex для вложенных вызовов
    mutable std::recursive_mutex mutex_;
    
    std::map<std::string, std::shared_ptr<Call>> activeCalls_;
    std::map<std::string, std::shared_ptr<Call>> completedCalls_;
    
    void cleanupCompletedCalls();
};

} // namespace h323p
```

**Реализация:**
```cpp
// src/core/call_manager.cpp (обновлённая версия)
#include "call_manager.hpp"

void CallManager::startOutgoingCall(const std::string& destination) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    auto call = std::make_shared<Call>(generateCallId(), false);
    call->setDestination(destination);
    
    activeCalls_[call->getId()] = call;
    call->start();
}

void CallManager::releaseCall(const std::string& callId, int cause) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    auto it = activeCalls_.find(callId);
    if (it != activeCalls_.end()) {
        it->second->release(cause);
        completedCalls_[callId] = it->second;
        activeCalls_.erase(it);
        cleanupCompletedCalls();
    }
}

size_t CallManager::getActiveCalls() const {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return activeCalls_.size();
}

void CallManager::cleanupCompletedCalls() {
    auto now = std::time(nullptr);
    std::vector<std::string> toRemove;
    
    for (const auto& [id, call] : completedCalls_) {
        if (std::difftime(now, call->getEndTime()) > 300) {
            toRemove.push_back(id);
        }
    }
    
    for (const auto& id : toRemove) {
        completedCalls_.erase(id);
    }
}
```

---

### Рекомендация #3: PSmartPointer для H323Plus

**Приоритет:** 🔴 Критический  
**Сложность:** Средняя

**Проблема:** Raw pointers приводят к утечкам памяти с H323Plus.

**Решение:**
```cpp
// src/h323/h323_endpoint.hpp (обновлённая версия)
#pragma once

#include <ptlib.h>
#include <h323.h>
#include <ptlib/objects.h>

namespace h323p {

class H323Endpoint {
public:
    static H323Endpoint& instance();
    
    bool initialize(const H323Config& config);
    void shutdown();
    bool isInitialized() const { return initialized_; }
    
private:
    H323Endpoint() = default;
    ~H323Endpoint();
    
    // PSmartPointer для авто-управления памятью
    PSmartPointer<H323EndPoint> endpoint_;
    
    // PDictionary для активных соединений
    PDictionary<H323Connection, PString> activeConnections_;
    
    bool initialized_ = false;
};

} // namespace h323p
```

**Реализация:**
```cpp
// src/h323/h323_endpoint.cpp (обновлённая версия)
#include "h323_endpoint.hpp"

bool H323Endpoint::initialize(const H323Config& config) {
    if (initialized_) {
        return true;
    }
    
    config_ = config;
    
    // PSmartPointer автоматически освободит память
    endpoint_ = new H323EndPoint();
    
    // Настройка endpoint
    endpoint_->SetPort(config.tcpPort);
    endpoint_->SetFastStart(config.fastStart);
    endpoint_->SetTunnelH245(config.h245Tunneling);
    
    initialized_ = true;
    LOG_INFO("H323Endpoint инициализирован");
    return true;
}

void H323Endpoint::shutdown() {
    if (!initialized_) return;
    
    // PSmartPointer автоматически вызовет деструктор
    endpoint_ = nullptr;
    activeConnections_.RemoveAll();
    
    initialized_ = false;
    LOG_INFO("H323Endpoint завершён");
}
```

---

### Рекомендация #5: Rate limiting для listen режима

**Приоритет:** 🔴 Критический  
**Сложность:** Средняя

**Проблема:** Злоумышленник может создать DDoS через множество вызовов.

**Решение:**
```cpp
// src/core/rate_limiter.hpp
#pragma once

#include <map>
#include <chrono>
#include <mutex>
#include <string>

namespace h323p {

class CallRateLimiter {
public:
    struct Config {
        size_t maxCallsPerSecond = 10;
        size_t maxCallsPerIp = 5;
        size_t maxConcurrentTotal = 50;
        std::chrono::seconds banDuration = std::chrono::minutes(5);
    };
    
    explicit CallRateLimiter(const Config& config);
    
    // Проверка возможности приёма вызова
    bool allowCall(const std::string& remoteIp);
    
    // Регистрация начала вызова
    void startCall(const std::string& remoteIp);
    
    // Завершение вызова
    void endCall(const std::string& remoteIp);
    
    // Статистика
    struct Stats {
        size_t callsAllowed = 0;
        size_t callsRejected = 0;
        size_t bannedIps = 0;
    };
    
    Stats getStats() const;
    
private:
    bool isBanned(const std::string& ip) const;
    void banIp(const std::string& ip);
    void cleanupOldEntries();
    
    Config config_;
    
    struct IpStats {
        std::chrono::steady_clock::time_point lastCallTime;
        size_t callsLastSecond = 0;
        size_t callsTotal = 0;
        std::chrono::steady_clock::time_point banUntil{std::chrono::steady_clock::now()};
    };
    
    std::map<std::string, IpStats> ipStats_;
    size_t activeCalls_ = 0;
    mutable std::mutex mutex_;
};

} // namespace h323p
```

**Использование в CallManager:**
```cpp
// src/core/call_manager.hpp
#include "rate_limiter.hpp"

class CallManager {
public:
    void setRateLimitConfig(const CallRateLimiter::Config& config);
    
private:
    CallRateLimiter rateLimiter_;
};

// src/core/call_manager.cpp
void CallManager::acceptIncomingCall(const std::string& callId) {
    std::string remoteIp = getRemoteIp(callId);
    
    if (!rateLimiter_.allowCall(remoteIp)) {
        LOG_WARN_FMT("Call from %s rejected by rate limiter", remoteIp.c_str());
        releaseCall(callId, 17);  // User busy
        return;
    }
    
    rateLimiter_.startCall(remoteIp);
    // ... продолжение обработки вызова
}
```

---

### Рекомендация #9: Защита паролей в CLI

**Приоритет:** 🔴 Критический  
**Сложность:** Низкая

**Проблема:** Пароль виден в `ps aux` при передаче через `-p`.

**Решение:**
```cpp
// src/cli/secure_input.hpp
#pragma once

#include <string>

namespace h323p {

// Безопасный ввод пароля (без эхо)
std::string readPassword(const std::string& prompt = "Password: ");

// Чтение пароля из файла
std::string readPasswordFromFile(const std::string& filename);

// Проверка прав доступа к файлу
bool checkFilePermissions(const std::string& filename);

} // namespace h323p
```

**Реализация:**
```cpp
// src/cli/secure_input.cpp
#include "secure_input.hpp"
#include <termios.h>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>

std::string h323p::readPassword(const std::string& prompt) {
    std::cout << prompt << std::flush;
    
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);  // Отключение эхо
    
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    std::string password;
    std::getline(std::cin, password);
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << std::endl;
    
    return password;
}

std::string h323p::readPasswordFromFile(const std::string& filename) {
    if (!checkFilePermissions(filename)) {
        throw std::runtime_error("Небезопасные права доступа к файлу пароля");
    }
    
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Не удалось прочитать файл пароля");
    }
    
    std::string password;
    std::getline(file, password);
    return password;
}

bool h323p::checkFilePermissions(const std::string& filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) != 0) {
        return false;
    }
    
    // Файл должен быть доступен только владельцу
    return (st.st_mode & (S_IRWXG | S_IRWXO)) == 0;
}
```

**CLI опции:**
```cpp
// src/cli/cli_parser.cpp (обновлённая версия)
app.add_flag("--password-prompt", config.passwordPrompt, "Интерактивный запрос пароля");
app.add_option("--password-file", config.passwordFile, "Файл с паролем");

// В main.cpp
if (config.passwordPrompt) {
    config.password = h323p::readPassword();
} else if (!config.passwordFile.empty()) {
    config.password = h323p::readPasswordFromFile(config.passwordFile);
}
```

**Примеры использования:**
```bash
# Интерактивный запрос пароля
./h323p register -g 192.168.1.1 -u test --password-prompt

# Из файла (права 600)
./h323p register -g 192.168.1.1 -u test --password-file ~/.h323p_creds

# Из переменной окружения
export H323P_PASSWORD=secret
./h323p register -g 192.168.1.1 -u test
```

---

## 🧪 Проверка результатов

### Сборка

```bash
cd build
cmake .. -DBUILD_TESTS=ON
make -j$(nproc)
```

### Запуск команды call

```bash
# Базовый вызов
./h323p call 192.168.1.100

# Через Gatekeeper
./h323p call alias:1001 --gatekeeper 192.168.1.1

# С повторами
./h323p call 192.168.1.100 --repeat 5

# С трассировкой
./h323p call 192.168.1.100 --trace -t 2
```

### Запуск команды listen

```bash
# Базовое прослушивание
./h323p listen

# С ограничением вызовов
./h323p listen --max-calls 50
```

### Тесты

```bash
# Запуск тестов H.323
ctest -R "Q931Test" --output-on-failure
ctest -R "GatekeeperTest" --output-on-failure
ctest -R "CallStateTest" --output-on-failure
```

### Пример вывода

```
$ ./h323p call 192.168.1.100

2026-03-16 10:00:00.123 [INFO] Starting h323p v0.1.0
2026-03-16 10:00:00.124 [INFO] Command: call
2026-03-16 10:00:00.125 [INFO] Initializing H323Endpoint: 0.0.0.0:1720
2026-03-16 10:00:00.126 [INFO] H323Endpoint initialized
2026-03-16 10:00:00.127 [INFO] Outgoing call: 192.168.1.100
2026-03-16 10:00:00.128 [INFO] Call created: h323p-abc-1
2026-03-16 10:00:00.129 [INFO] Call h323p-abc-1: NULL -> INITIATED
2026-03-16 10:00:00.230 [INFO] Call h323p-abc-1: INITIATED -> PROCEEDING
2026-03-16 10:00:00.331 [INFO] Call h323p-abc-1: PROCEEDING -> ALERTING
2026-03-16 10:00:00.432 [INFO] Call h323p-abc-1: ALERTING -> ACTIVE
2026-03-16 10:00:00.433 [INFO] Call duration: 15 sec
2026-03-16 10:00:15.434 [INFO] Releasing call: h323p-abc-1 (cause=16)
2026-03-16 10:00:15.435 [INFO] Call h323p-abc-1: ACTIVE -> RELEASED

Statistics:
  Total calls: 1
  Successful: 1
  Failed: 0
  Average duration: 15 sec
```

---

## 📚 Ссылки

- **H.225.0 стандарт:** ITU-T Recommendation H.225.0
- **H323Plus API:** https://www.h323plus.org/api/
- **Q.850 причины:** ITU-T Recommendation Q.850
- **callgen323:** https://github.com/willamowius/callgen323

---

*Версия документа: 0.1.0*  
*Последнее обновление: 2026-03-16*
