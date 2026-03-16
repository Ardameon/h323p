# Этап 5: Нагрузочное тестирование

**Длительность:** 2 недели  
**Статус:** 📋 Готов к реализации  
**Зависимости:** ✅ Этап 1 (инфраструктура), ✅ Этап 2 (H.225), ✅ Этап 3 (H.245 + медиа), ✅ Этап 4 (XML сценарии)

---

## 📋 Содержание

1. [Обзор этапа](#-обзор-этапа)
2. [Задачи этапа](#-задачи-этапа)
3. [Архитектура подсистемы нагрузки](#-архитектура-подсистемы-нагрузки)
4. [Детальная реализация](#-детальная-реализация)
5. [Примеры использования](#-примеры-использования)
6. [Критерии завершения](#-критерии-завершения)
7. [Проверка результатов](#-проверка-результатов)

---

## 📌 Обзор этапа

Цель Этапа 5 — реализовать подсистему нагрузочного тестирования для одновременной генерации множества H.323 вызовов со статистикой и отчётами.

### Результаты этапа

```
✅ Call Manager v2 для множественных вызовов
✅ Команда load для генерации нагрузки
✅ Статистика в реальном времени
✅ CDR записи для каждого вызова
✅ Отчёты JSON/CSV
✅ Ramp-up/ramp-down профили
✅ Ограничители (rate limit, max calls)
```

### Что НЕ входит в Этап 5

```
❌ Fuzzing тестирование — Этап 6
❌ Видео вызовы — Этап 6
❌ H.239 презентации — Этап 6
❌ TLS шифрование — Этап 6
```

---

## 📊 Задачи этапа

| № | Задача | Длительность | Статус |
|---|--------|--------------|--------|
| 5.1 | Call Manager v2 | 3 дня | ⏳ |
| 5.2 | Команда load | 3 дня | ⏳ |
| 5.3 | Статистика и метрики | 3 дня | ⏳ |
| 5.4 | CDR записи | 2 дня | ⏳ |
| 5.5 | Отчёты (JSON/CSV) | 2 дня | ⏳ |
| 5.6 | Тесты нагрузки | 3 дня | ⏳ |
| **Итого** | | **16 дней** | |

---

## 🏗️ Архитектура подсистемы нагрузки

### Общая схема

```
┌─────────────────────────────────────────────────────────────────┐
│                    Application Layer                            │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  CLI: h323p load 192.168.1.100 -c 1000 -r 50            │   │
│  └─────────────────────────────────────────────────────────┘   │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │              Load Test Engine                             │ │
│  │  ┌─────────────────────────────────────────────────────┐ │ │
│  │  │           Load Test Controller                      │ │ │
│  │  │  - Ramp up/down profiles                            │ │ │
│  │  │  - Rate limiting                                    │ │ │
│  │  │  - Call scheduling                                  │ │ │
│  │  └─────────────────────────────────────────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │              Call Manager v2                              │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │    Call      │  │    Call      │  │     Call        │ │ │
│  │  │   Pool       │  │   Pool       │  │    Pool         │ │ │
│  │  │  (active)    │  │  (waiting)   │  │  (completed)    │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │              Statistics Engine                            │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │   Real-time  │  │    CDR       │  │     Report      │ │ │
│  │  │   Metrics    │  │   Recorder   │  │   Generator     │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │                  H.323 Stack                              │ │
│  │         (Множественные одновременных вызовов)             │ │
│  └───────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### Структура модулей

```
src/
└── load/
    ├── load_test_controller.hpp    # Контроллер нагрузочного теста
    ├── load_test_controller.cpp
    ├── load_config.hpp             # Конфигурация нагрузки
    ├── load_config.cpp
    ├── call_pool.hpp               # Пул вызовов
    ├── call_pool.cpp
    ├── rate_limiter.hpp            # Ограничитель скорости
    ├── rate_limiter.cpp
    ├── ramp_profile.hpp            # Профили ramp up/down
    └── ramp_profile.cpp
src/
└── stats/
    ├── statistics.hpp              # Статистика
    ├── statistics.cpp
    ├── metrics.hpp                 # Метрики
    ├── metrics.cpp
    ├── cdr.hpp                     # CDR записи
    ├── cdr.cpp
    ├── cdr_writer.hpp              # Запись CDR
    ├── cdr_writer.cpp
    ├── report_generator.hpp        # Генератор отчётов
    ├── report_generator.cpp
    └── report_types.hpp            # Типы отчётов
```

---

## 🔧 Детальная реализация

### 5.1 Call Manager v2

**Файл: `src/load/call_pool.hpp`**

```cpp
#pragma once

#include "core/call.hpp"
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <functional>

namespace h323p {

// Состояние вызова в пуле
enum class CallPoolState {
    WAITING,      // Ожидает запуска
    ACTIVE,       // Активный вызов
    COMPLETED,    // Завершён успешно
    FAILED,       // Завершён с ошибкой
    RELEASED      // Освобождён (удалён из пула)
};

// Элемент пула
struct CallPoolItem {
    std::shared_ptr<Call> call;
    CallPoolState state = CallPoolState::WAITING;
    std::time_t startTime = 0;
    std::time_t connectTime = 0;
    std::time_t endTime = 0;
    std::string errorMessage;
};

// Пул вызовов
class CallPool {
public:
    CallPool(size_t maxCalls = 1000);
    ~CallPool();
    
    // Создание вызова
    std::string createCall(const std::string& destination);
    
    // Получение вызова
    Call* getCall(const std::string& callId);
    CallPoolItem* getCallItem(const std::string& callId);
    
    // Управление состоянием
    bool setState(const std::string& callId, CallPoolState state);
    CallPoolState getState(const std::string& callId) const;
    
    // Статистика
    size_t getTotalCalls() const { return totalCalls_; }
    size_t getActiveCalls() const { return activeCalls_.load(); }
    size_t getWaitingCalls() const;
    size_t getCompletedCalls() const { return completedCalls_.load(); }
    size_t getFailedCalls() const { return failedCalls_.load(); }
    
    // Очистка завершённых
    void cleanupReleased(size_t maxAge = 60);  // секунд
    
    // Итерация по активным вызовам
    void forEachActive(std::function<void(Call*)> callback);
    
private:
    std::map<std::string, std::unique_ptr<CallPoolItem>> calls_;
    std::queue<std::string> waitingQueue_;
    mutable std::mutex mutex_;
    
    size_t maxCalls_;
    std::atomic<size_t> totalCalls_{0};
    std::atomic<size_t> activeCalls_{0};
    std::atomic<size_t> completedCalls_{0};
    std::atomic<size_t> failedCalls_{0};
};

} // namespace h323p
```

**Файл: `src/load/call_pool.cpp`**

```cpp
#include "call_pool.hpp"
#include "utils/logging.hpp"
#include <chrono>

namespace h323p {

CallPool::CallPool(size_t maxCalls) : maxCalls_(maxCalls) {}

CallPool::~CallPool() {
    // Завершение всех вызовов
    for (auto& [id, item] : calls_) {
        if (item->call && item->state != CallPoolState::RELEASED) {
            item->call->release();
        }
    }
}

std::string CallPool::createCall(const std::string& destination) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (calls_.size() >= maxCalls_) {
        LOG_ERROR("Достигнут лимит вызовов");
        return "";
    }
    
    std::string callId = fmt::format("load-{}", totalCalls_ + 1);
    
    auto item = std::make_unique<CallPoolItem>();
    item->call = std::make_shared<Call>(callId, false);
    item->state = CallPoolState::WAITING;
    item->startTime = std::time(nullptr);
    
    calls_[callId] = std::move(item);
    waitingQueue_.push(callId);
    totalCalls_++;
    
    LOG_DEBUG_FMT("Создан вызов %s -> %s", callId.c_str(), destination.c_str());
    
    return callId;
}

Call* CallPool::getCall(const std::string& callId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = calls_.find(callId);
    if (it != calls_.end()) {
        return it->second->call.get();
    }
    return nullptr;
}

CallPoolItem* CallPool::getCallItem(const std::string& callId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = calls_.find(callId);
    if (it != calls_.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool CallPool::setState(const std::string& callId, CallPoolState state) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = calls_.find(callId);
    if (it == calls_.end()) {
        return false;
    }
    
    CallPoolItem* item = it->second.get();
    CallPoolState oldState = item->state;
    item->state = state;
    
    // Обновление счётчиков
    if (oldState == CallPoolState::ACTIVE) {
        activeCalls_--;
    }
    
    if (state == CallPoolState::ACTIVE) {
        activeCalls_++;
        item->connectTime = std::time(nullptr);
    } else if (state == CallPoolState::COMPLETED) {
        completedCalls_++;
        item->endTime = std::time(nullptr);
    } else if (state == CallPoolState::FAILED) {
        failedCalls_++;
        item->endTime = std::time(nullptr);
    }
    
    LOG_DEBUG_FMT("Call %s: %d -> %d", callId.c_str(), 
                  static_cast<int>(oldState), static_cast<int>(state));
    
    return true;
}

CallPoolState CallPool::getState(const std::string& callId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = calls_.find(callId);
    if (it != calls_.end()) {
        return it->second->state;
    }
    return CallPoolState::RELEASED;
}

size_t CallPool::getWaitingCalls() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return waitingQueue_.size();
}

void CallPool::cleanupReleased(size_t maxAge) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::time(nullptr);
    std::vector<std::string> toRemove;
    
    for (const auto& [id, item] : calls_) {
        if (item->state == CallPoolState::RELEASED ||
            item->state == CallPoolState::COMPLETED ||
            item->state == CallPoolState::FAILED) {
            
            if (std::difftime(now, item->endTime) > maxAge) {
                toRemove.push_back(id);
            }
        }
    }
    
    for (const auto& id : toRemove) {
        calls_.erase(id);
    }
    
    if (!toRemove.empty()) {
        LOG_DEBUG_FMT("Очищено %zu завершённых вызовов", toRemove.size());
    }
}

void CallPool::forEachActive(std::function<void(Call*)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& [id, item] : calls_) {
        if (item->state == CallPoolState::ACTIVE && item->call) {
            callback(item->call.get());
        }
    }
}

} // namespace h323p
```

---

### 5.2 Контроллер нагрузочного теста

**Файл: `src/load/load_config.hpp`**

```cpp
#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace h323p {

// Конфигурация нагрузочного теста
struct LoadTestConfig {
    // Целевые параметры
    std::string destination;
    std::vector<std::string> destinations;  // Для циклического перебора
    
    // Параметры нагрузки
    size_t totalCalls = 100;      // Общее количество вызовов
    size_t maxConcurrent = 50;    // Макс. одновременных вызовов
    double callRate = 10.0;       // Вызовов в секунду
    
    // Длительность
    size_t callDuration = 30;     // Длительность вызова (сек)
    size_t testDuration = 0;      // Длительность теста (0 = без ограничений)
    
    // Ramp up/down
    size_t rampUpTime = 0;        // Время нарастания (сек)
    size_t rampDownTime = 0;      // Время спада (сек)
    
    // Повторы
    size_t repeatCount = 1;
    bool cycleMode = false;       // Циклический перебор назначений
    
    // Ограничения
    size_t maxFailures = 0;       // Макс. ошибок перед остановкой (0 = без ограничений)
    double failureThreshold = 0.0; // Процент ошибок для остановки (0.0-1.0)
    
    // Отчёты
    std::string cdrFile;
    std::string reportFile;
    size_t statsInterval = 5;     // Интервал статистики (сек)
    
    // Сценарий
    std::string scenarioFile;
};

} // namespace h323p
```

**Файл: `src/load/ramp_profile.hpp`**

```cpp
#pragma once

#include <cstdint>
#include <chrono>

namespace h323p {

// Профиль нарастания/спада нагрузки
class RampProfile {
public:
    RampProfile();
    
    // Настройка
    void setRampUp(uint64_t rampUpSeconds);
    void setRampDown(uint64_t rampDownSeconds);
    void setTotalCalls(size_t total);
    void setMaxConcurrent(size_t maxConcurrent);
    
    // Расчёт текущей скорости
    double getCurrentRate(uint64_t elapsedSeconds) const;
    
    // Расчёт количества активных вызовов
    size_t getTargetActive(uint64_t elapsedSeconds) const;
    
    // Проверка завершения
    bool isRampUpComplete(uint64_t elapsedSeconds) const;
    bool isRampDownStarted(uint64_t elapsedSeconds) const;
    bool isTestComplete(uint64_t elapsedSeconds, size_t completedCalls) const;
    
    // Фазы теста
    enum class Phase {
        RAMP_UP,
        STEADY_STATE,
        RAMP_DOWN,
        COMPLETE
    };
    
    Phase getCurrentPhase(uint64_t elapsedSeconds, size_t completedCalls) const;
    
private:
    uint64_t rampUpSeconds_ = 0;
    uint64_t rampDownSeconds_ = 0;
    size_t totalCalls_ = 0;
    size_t maxConcurrent_ = 0;
    double peakRate_ = 0.0;
};

} // namespace h323p
```

**Файл: `src/load/load_test_controller.hpp`**

```cpp
#pragma once

#include "load_config.hpp"
#include "call_pool.hpp"
#include "ramp_profile.hpp"
#include <atomic>
#include <thread>
#include <functional>

namespace h323p {

// Статистика нагрузочного теста
struct LoadTestStatistics {
    std::time_t startTime = 0;
    std::time_t endTime = 0;
    
    size_t totalCalls = 0;
    size_t completedCalls = 0;
    size_t failedCalls = 0;
    size_t activeCalls = 0;
    
    double callsPerSecond = 0.0;
    double successRate = 0.0;
    double failureRate = 0.0;
    
    double avgCallDuration = 0.0;
    double minCallDuration = 0.0;
    double maxCallDuration = 0.0;
    
    // Постепенная статистика
    std::vector<double> callsPerSecondHistory;
};

// Контроллер нагрузочного теста
class LoadTestController {
public:
    LoadTestController();
    ~LoadTestController();
    
    // Конфигурация
    void setConfig(const LoadTestConfig& config);
    const LoadTestConfig& getConfig() const { return config_; }
    
    // Запуск/остановка
    bool start();
    void stop();
    void pause();
    void resume();
    
    // Состояние
    bool isRunning() const { return running_; }
    bool isPaused() const { return paused_; }
    LoadTestStatistics getStatistics() const;
    
    // Callback для событий
    using EventCallback = std::function<void(const std::string& event, const LoadTestStatistics& stats)>;
    void setEventCallback(EventCallback callback);
    
private:
    void testLoop();
    void scheduleCall();
    void updateStatistics();
    void checkStopConditions();
    
    LoadTestConfig config_;
    CallPool callPool_;
    RampProfile rampProfile_;
    
    std::atomic<bool> running_{false};
    std::atomic<bool> paused_{false};
    std::atomic<bool> stopRequested_{false};
    
    std::thread testThread_;
    std::thread schedulerThread_;
    
    LoadTestStatistics stats_;
    EventCallback eventCallback_;
    
    std::time_t startTime_ = 0;
    size_t scheduledCalls_ = 0;
    size_t lastCompletedCalls_ = 0;
};

} // namespace h323p
```

**Файл: `src/load/load_test_controller.cpp`**

```cpp
#include "load_test_controller.hpp"
#include "utils/logging.hpp"
#include <chrono>

namespace h323p {

LoadTestController::LoadTestController() = default;

LoadTestController::~LoadTestController() {
    stop();
}

void LoadTestController::setConfig(const LoadTestConfig& config) {
    config_ = config;
    
    // Настройка профиля
    rampProfile_.setTotalCalls(config.totalCalls);
    rampProfile_.setMaxConcurrent(config.maxConcurrent);
    rampProfile_.setRampUp(config.rampUpTime);
    rampProfile_.setRampDown(config.rampDownTime);
    
    // Настройка пула
    callPool_ = CallPool(config.maxConcurrent);
}

bool LoadTestController::start() {
    if (running_) {
        LOG_WARN("Тест уже запущен");
        return false;
    }
    
    LOG_INFO("Запуск нагрузочного теста");
    LOG_INFO_FMT("  Всего вызовов: %zu", config_.totalCalls);
    LOG_INFO_FMT("  Макс. одновременных: %zu", config_.maxConcurrent);
    LOG_INFO_FMT("  Скорость: %.1f вызовов/сек", config_.callRate);
    
    running_ = true;
    stopRequested_ = false;
    paused_ = false;
    startTime_ = std::time(nullptr);
    
    stats_ = LoadTestStatistics();
    stats_.startTime = startTime_;
    
    // Запуск основного потока
    testThread_ = std::thread(&LoadTestController::testLoop, this);
    
    // Запуск планировщика
    schedulerThread_ = std::thread(&LoadTestController::scheduleCall, this);
    
    return true;
}

void LoadTestController::stop() {
    if (!running_) return;
    
    LOG_INFO("Остановка нагрузочного теста");
    
    stopRequested_ = true;
    running_ = false;
    
    if (schedulerThread_.joinable()) {
        schedulerThread_.join();
    }
    
    if (testThread_.joinable()) {
        testThread_.join();
    }
    
    stats_.endTime = std::time(nullptr);
    
    // Финальная статистика
    updateStatistics();
    
    if (eventCallback_) {
        eventCallback_("test_stopped", stats_);
    }
}

void LoadTestController::testLoop() {
    auto lastStatsUpdate = std::chrono::steady_clock::now();
    
    while (running_ && !stopRequested_) {
        if (paused_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        // Проверка условий остановки
        checkStopConditions();
        
        // Обновление статистики
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastStatsUpdate).count() 
            >= config_.statsInterval) {
            updateStatistics();
            lastStatsUpdate = now;
            
            if (eventCallback_) {
                eventCallback_("stats_update", stats_);
            }
        }
        
        // Очистка завершённых вызовов
        callPool_.cleanupReleased(30);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void LoadTestController::scheduleCall() {
    auto testStart = std::chrono::steady_clock::now();
    double intervalMs = 1000.0 / config_.callRate;
    
    while (running_ && !stopRequested_) {
        if (paused_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        // Расчёт текущей скорости по профилю
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - testStart).count();
        
        double currentRate = rampProfile_.getCurrentRate(elapsed);
        double currentIntervalMs = currentRate > 0 ? 1000.0 / currentRate : intervalMs;
        
        // Планирование вызова
        if (callPool_.getActiveCalls() < config_.maxConcurrent) {
            std::string destination;
            
            if (config_.cycleMode && !config_.destinations.empty()) {
                destination = config_.destinations[scheduledCalls_ % config_.destinations.size()];
            } else {
                destination = config_.destination;
            }
            
            std::string callId = callPool_.createCall(destination);
            
            if (!callId.empty()) {
                scheduledCalls_++;
                
                // Запуск вызова в отдельном потоке
                std::thread([this, callId]() {
                    Call* call = callPool_.getCall(callId);
                    if (call) {
                        callPool_.setState(callId, CallPoolState::ACTIVE);
                        
                        // Выполнение вызова
                        bool success = call->execute();
                        
                        if (success) {
                            callPool_.setState(callId, CallPoolState::COMPLETED);
                        } else {
                            callPool_.setState(callId, CallPoolState::FAILED);
                        }
                        
                        call->release();
                        callPool_.setState(callId, CallPoolState::RELEASED);
                    }
                }).detach();
            }
        }
        
        // Ожидание следующего интервала
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(currentIntervalMs)));
    }
}

void LoadTestController::updateStatistics() {
    auto now = std::time(nullptr);
    auto elapsed = std::difftime(now, startTime_);
    
    stats_.activeCalls = callPool_.getActiveCalls();
    stats_.completedCalls = callPool_.getCompletedCalls();
    stats_.failedCalls = callPool_.getFailedCalls();
    stats_.totalCalls = callPool_.getTotalCalls();
    
    if (stats_.totalCalls > 0) {
        stats_.successRate = 100.0 * stats_.completedCalls / stats_.totalCalls;
        stats_.failureRate = 100.0 * stats_.failedCalls / stats_.totalCalls;
    }
    
    if (elapsed > 0) {
        stats_.callsPerSecond = static_cast<double>(stats_.completedCalls) / elapsed;
    }
    
    // История
    stats_.callsPerSecondHistory.push_back(stats_.callsPerSecond);
    if (stats_.callsPerSecondHistory.size() > 60) {
        stats_.callsPerSecondHistory.erase(stats_.callsPerSecondHistory.begin());
    }
}

void LoadTestController::checkStopConditions() {
    // Проверка по количеству завершённых вызовов
    if (config_.totalCalls > 0 && 
        callPool_.getCompletedCalls() + callPool_.getFailedCalls() >= config_.totalCalls) {
        LOG_INFO("Достигнуто целевое количество вызовов");
        stopRequested_ = true;
        return;
    }
    
    // Проверка по длительности теста
    if (config_.testDuration > 0) {
        auto elapsed = std::difftime(std::time(nullptr), startTime_);
        if (elapsed >= config_.testDuration) {
            LOG_INFO("Истекло время теста");
            stopRequested_ = true;
            return;
        }
    }
    
    // Проверка по проценту ошибок
    if (config_.failureThreshold > 0 && stats_.totalCalls > 0) {
        if (stats_.failureRate >= config_.failureThreshold * 100) {
            LOG_ERROR_FMT("Превышен порог ошибок (%.1f%% >= %.1f%%)", 
                          stats_.failureRate, config_.failureThreshold * 100);
            stopRequested_ = true;
            return;
        }
    }
    
    // Проверка по абсолютному количеству ошибок
    if (config_.maxFailures > 0 && 
        callPool_.getFailedCalls() >= config_.maxFailures) {
        LOG_ERROR_FMT("Достигнуто макс. количество ошибок (%zu)", 
                      callPool_.getFailedCalls());
        stopRequested_ = true;
    }
}

LoadTestStatistics LoadTestController::getStatistics() const {
    return stats_;
}

void LoadTestController::setEventCallback(EventCallback callback) {
    eventCallback_ = callback;
}

void LoadTestController::pause() {
    paused_ = true;
    LOG_INFO("Тест на паузе");
}

void LoadTestController::resume() {
    paused_ = false;
    LOG_INFO("Тест возобновлён");
}

} // namespace h323p
```

---

### 5.3 Статистика и метрики

**Файл: `src/stats/metrics.hpp`**

```cpp
#pragma once

#include <string>
#include <map>
#include <vector>
#include <chrono>

namespace h323p {

// Типы метрик
enum class MetricType {
    COUNTER,      // Счётчик (только возрастает)
    GAUGE,        // Измеритель (может расти/падать)
    HISTOGRAM,    // Гистограмма (распределение)
    RATE          // Скорость (события в секунду)
};

// Метрика
class Metric {
public:
    Metric(const std::string& name, MetricType type, const std::string& unit = "");
    
    // Доступ
    const std::string& getName() const { return name_; }
    MetricType getType() const { return type_; }
    const std::string& getUnit() const { return unit_; }
    
    // Значения
    void increment(int64_t delta = 1);
    void set(int64_t value);
    void record(double value);  // Для гистограмм
    
    // Получение значения
    int64_t getValue() const { return value_; }
    double getRate() const;  // События в секунду
    
    // Сброс
    void reset();
    
private:
    std::string name_;
    MetricType type_;
    std::string unit_;
    
    int64_t value_ = 0;
    std::chrono::steady_clock::time_point startTime_;
    
    // Для гистограмм
    std::vector<double> values_;
    double min_ = 0;
    double max_ = 0;
    double sum_ = 0;
    size_t count_ = 0;
};

// Менеджер метрик
class MetricsManager {
public:
    static MetricsManager& instance();
    
    // Регистрация метрики
    Metric* registerMetric(const std::string& name, MetricType type, 
                           const std::string& unit = "");
    
    // Получение метрики
    Metric* getMetric(const std::string& name);
    
    // Все метрики
    std::map<std::string, Metric*> getAllMetrics() const;
    
    // Экспорт в Prometheus формат
    std::string exportPrometheus() const;
    
private:
    MetricsManager() = default;
    std::map<std::string, std::unique_ptr<Metric>> metrics_;
};

// Макросы для удобства
#define METRIC_COUNTER(name) \
    h323p::MetricsManager::instance().registerMetric(name, h323p::MetricType::COUNTER)

#define METRIC_GAUGE(name) \
    h323p::MetricsManager::instance().registerMetric(name, h323p::MetricType::GAUGE)

} // namespace h323p
```

---

### 5.4 CDR записи

**Файл: `src/stats/cdr.hpp`**

```cpp
#pragma once

#include <string>
#include <cstdint>
#include <ctime>

namespace h323p {

// Причина завершения вызова
enum class CallTerminationCause {
    NORMAL = 0,
    BUSY = 17,
    NO_ANSWER = 18,
    REJECTED = 21,
    UNAVAILABLE = 27,
    INVALID_NUMBER = 28,
    NETWORK_ERROR = 38,
    TIMEOUT = 39,
    SERVICE_UNAVAILABLE = 47,
    OTHER = 127
};

// CDR запись вызова
struct CdrRecord {
    // Идентификаторы
    std::string callId;
    std::string globalCallId;
    
    // Стороны
    std::string caller;
    std::string callee;
    std::string sourceIp;
    std::string destIp;
    
    // Временные метки
    std::time_t startTime = 0;
    std::time_t connectTime = 0;
    std::time_t endTime = 0;
    
    // Длительности
    uint32_t setupTime = 0;      // мс (setup -> connect)
    uint32_t duration = 0;       // сек (connect -> release)
    uint32_t totalDuration = 0;  // сек (setup -> release)
    
    // Статус
    bool successful = false;
    CallTerminationCause cause = CallTerminationCause::OTHER;
    std::string errorMessage;
    
    // Медиа
    std::string audioCodec;
    std::string videoCodec;
    bool hasAudio = false;
    bool hasVideo = false;
    
    // RTP статистика
    uint64_t rtpPacketsSent = 0;
    uint64_t rtpPacketsReceived = 0;
    uint32_t jitter = 0;
    uint32_t packetLoss = 0;
    
    // Gatekeeper
    std::string gatekeeper;
    std::string rasAddress;
    
    // Сериализация
    std::string toCsv() const;
    std::string toJson() const;
    
    static std::string csvHeader();
};

// Менеджер CDR
class CdrManager {
public:
    static CdrManager& instance();
    
    // Запись
    void record(const CdrRecord& record);
    
    // Статистика
    size_t getTotalRecords() const { return totalRecords_; }
    size_t getSuccessfulRecords() const { return successfulRecords_; }
    size_t getFailedRecords() const { return failedRecords_; }
    
    // Очистка
    void clear();
    
private:
    CdrManager() = default;
    
    size_t totalRecords_ = 0;
    size_t successfulRecords_ = 0;
    size_t failedRecords_ = 0;
};

} // namespace h323p
```

**Файл: `src/stats/cdr_writer.hpp`**

```cpp
#pragma once

#include "cdr.hpp"
#include <string>
#include <fstream>
#include <mutex>

namespace h323p {

// Типы вывода CDR
enum class CdrOutputType {
    CSV,
    JSON,
    DATABASE  // Будущая реализация
};

// Конфигурация записи CDR
struct CdrWriterConfig {
    CdrOutputType type = CdrOutputType::CSV;
    std::string filename;
    bool append = true;
    bool flushOnWrite = false;
};

// Запись CDR в файл
class CdrWriter {
public:
    CdrWriter();
    ~CdrWriter();
    
    // Инициализация
    bool open(const CdrWriterConfig& config);
    void close();
    bool isOpen() const { return file_.is_open(); }
    
    // Запись
    bool write(const CdrRecord& record);
    void flush();
    
    // Статистика
    size_t getRecordsWritten() const { return recordsWritten_; }
    
private:
    CdrWriterConfig config_;
    std::ofstream file_;
    std::mutex mutex_;
    size_t recordsWritten_ = 0;
    bool headerWritten_ = false;
};

} // namespace h323p
```

---

### 5.5 Генератор отчётов

**Файл: `src/stats/report_generator.hpp`**

```cpp
#pragma once

#include "statistics.hpp"
#include "load/load_test_controller.hpp"
#include <string>
#include <vector>

namespace h323p {

// Типы отчётов
enum class ReportType {
    SUMMARY,      // Краткая сводка
    DETAILED,     // Подробный отчёт
    JSON,         // JSON формат
    CSV,          // CSV формат
    HTML          // HTML формат
};

// Конфигурация отчёта
struct ReportConfig {
    ReportType type = ReportType::SUMMARY;
    std::string outputFile;
    bool includePerSecondStats = true;
    bool includePercentiles = true;
    std::vector<int> percentiles = {50, 90, 95, 99};
};

// Генератор отчётов
class ReportGenerator {
public:
    ReportGenerator();
    
    // Генерация
    bool generate(const LoadTestStatistics& stats, const ReportConfig& config);
    
    // Отдельные форматы
    std::string generateSummary(const LoadTestStatistics& stats);
    std::string generateJson(const LoadTestStatistics& stats);
    std::string generateCsv(const LoadTestStatistics& stats);
    std::string generateHtml(const LoadTestStatistics& stats);
    
private:
    bool writeToFile(const std::string& content, const std::string& filename);
    std::string formatDuration(double seconds);
    std::string formatTimestamp(std::time_t time);
};

} // namespace h323p
```

---

## 📄 Примеры использования

### Базовая нагрузка

```bash
# 100 вызовов, 10 вызовов/сек
./h323p load 192.168.1.100 -c 100 -r 10

# 500 вызовов, 50 одновременных
./h323p load 192.168.1.100 -c 500 -m 50

# С длительностью вызова 60 сек
./h323p load 192.168.1.100 -c 100 -r 10 --call-duration 60
```

### Ramp up/down

```bash
# 5 сек нарастание, 5 сек спад
./h323p load 192.168.1.100 -c 200 -r 20 --ramp-up 5 --ramp-down 5

# Плавное нарастание за 30 сек
./h323p load 192.168.1.100 -c 1000 -r 50 --ramp-up 30
```

### С отчётами

```bash
# С CDR файлом
./h323p load 192.168.1.100 -c 100 -r 10 --cdr calls.csv

# С JSON отчётом
./h323p load 192.168.1.100 -c 100 -r 10 --report report.json

# С HTML отчётом
./h323p load 192.168.1.100 -c 100 -r 10 --report report.html
```

### С ограничениями

```bash
# Остановить после 10 ошибок
./h323p load 192.168.1.100 -c 1000 -r 50 --max-failures 10

# Остановить при 5% ошибок
./h323p load 192.168.1.100 -c 1000 -r 50 --failure-threshold 0.05

# Тест на 5 минут
./h323p load 192.168.1.100 -c 0 -r 50 --duration 300
```

### Циклический перебор

```bash
# CSV с назначениями
./h323p load --destinations destinations.csv -c 1000 -r 50 --cycle

# Список адресов
./h323p load 192.168.1.100,192.168.1.101,192.168.1.102 -c 300 -r 30 --cycle
```

---

## ✅ Критерии завершения

### Чек-лист завершения Этапа 5

```
[ ] 1. Call Pool реализован
[ ] 2. Load Test Controller работает
[ ] 3. Ramp up/down профили работают
[ ] 4. Статистика в реальном времени
[ ] 5. CDR записи пишутся
[ ] 6. JSON отчёты генерируются
[ ] 7. CSV отчёты генерируются
[ ] 8. Команда load работает
[ ] 9. Тесты на 100+ вызовов проходят
[ ] 10. Ограничители работают
```

---

## 🧪 Проверка результатов

### Сборка

```bash
cd build
cmake .. -DBUILD_TESTS=ON
make -j$(nproc)
```

### Запуск тестов

```bash
# Базовый тест
./h323p load 192.168.1.100 -c 50 -r 10

# Тест с отчётом
./h323p load 192.168.1.100 -c 100 -r 20 --report report.json

# Длительный тест
./h323p load 192.168.1.100 -c 500 -r 50 --stats-interval 10
```

### Пример вывода

```
$ ./h323p load 192.168.1.100 -c 100 -r 10

2026-03-16 10:00:00.123 [INFO] Запуск нагрузочного теста
2026-03-16 10:00:00.124 [INFO]   Всего вызовов: 100
2026-03-16 10:00:00.125 [INFO]   Макс. одновременных: 50
2026-03-16 10:00:00.126 [INFO]   Скорость: 10.0 вызовов/сек

[00:05] Calls: 50/100 | Active: 45 | Success: 48 (96.0%) | Fail: 2 (4.0%) | Rate: 9.6/s
[00:10] Calls: 100/100 | Active: 30 | Success: 68 (97.1%) | Fail: 2 (2.9%) | Rate: 9.8/s
[00:15] Calls: 100/100 | Active: 0 | Success: 98 (98.0%) | Fail: 2 (2.0%) | Rate: 9.9/s

2026-03-16 10:00:15.456 [INFO] Тест завершён

=== Статистика ===
Всего вызовов:     100
Успешных:         98 (98.0%)
Неудачных:        2 (2.0%)
Длительность:     15.3 сек
Средняя скорость: 9.9 вызовов/сек

Отчёт сохранён: report.json
CDR сохранён: calls.csv
```

### Пример JSON отчёта

```json
{
  "test_info": {
    "start_time": "2026-03-16T10:00:00Z",
    "end_time": "2026-03-16T10:00:15Z",
    "duration_seconds": 15.3
  },
  "summary": {
    "total_calls": 100,
    "successful_calls": 98,
    "failed_calls": 2,
    "success_rate": 98.0,
    "calls_per_second": 9.9
  },
  "call_durations": {
    "avg_seconds": 30.5,
    "min_seconds": 28.1,
    "max_seconds": 35.2,
    "p50_seconds": 30.2,
    "p90_seconds": 32.1,
    "p95_seconds": 33.5,
    "p99_seconds": 34.8
  }
}
```

---

## 📚 Ссылки

- **SIPP нагрузочное тестирование:** https://sipp.readthedocs.io/
- **Prometheus метрики:** https://prometheus.io/docs/concepts/metric_types/

---

*Версия документа: 0.1.0*  
*Последнее обновление: 2026-03-16*
