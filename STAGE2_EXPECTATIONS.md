# Ожидаемые результаты Этапа 2 — H.323 стек H.225

**Этап:** 2 из 7
**Название:** H.323 стек H.225
**Длительность:** 3-4 недели
**Статус:** 📋 Готов к реализации

---

## 📋 Обзор

Этот документ описывает ожидаемые результаты от реализации Этапа 2 (H.323 стек H.225). После завершения проекта должна быть реализована базовая функциональность для совершения и приёма H.323 вызовов через Gatekeeper и напрямую.

---

## ✅ Ожидаемые результаты

### 1. Структура модулей H.323 создана

**Ожидаемое:**
```
src/
├── h323/
│   ├── h323_wrapper.hpp          ✅ Обёртка над H323Plus
│   ├── h323_wrapper.cpp
│   ├── h323_endpoint.hpp         ✅ H.323 endpoint (синглтон)
│   ├── h323_endpoint.cpp
│   ├── h323_event_handler.hpp    ✅ Обработчик событий H323Plus
│   ├── h323_event_handler.cpp
│   ├── ras.hpp                   ✅ H.225 RAS логика
│   ├── ras.cpp
│   ├── q931.hpp                  ✅ H.225 Q.931 логика
│   ├── q931.cpp
│   ├── h245.hpp                  ⏳ Заглушка для Этапа 3
│   └── h245.cpp
├── core/
│   ├── call_manager.hpp          ✅ Управление вызовами
│   ├── call_manager.cpp
│   ├── call.hpp                  ✅ Класс вызова
│   ├── call.cpp
│   ├── endpoint.hpp              ✅ Внутренний endpoint
│   └── endpoint.cpp
└── cli/
    ├── commands.hpp              ✅ Обновлённые команды
    └── commands/
        ├── cmd_call.hpp          ✅ Команда call
        ├── cmd_call.cpp
        ├── cmd_listen.hpp        ✅ Команда listen
        └── cmd_listen.cpp
```

**Критерии приемки:**
- [ ] Все файлы созданы
- [ ] Заголовочные файлы имеют include guards
- [ ] Используется английская терминология в коде
- [ ] Код следует стилю проекта (C++17)

---

### 2. Обёртка H323Plus реализована

**Ожидаемое:**
```cpp
// Инициализация endpoint
auto& endpoint = H323Endpoint::instance();

H323Config config;
config.localAddress = "0.0.0.0";
config.tcpPort = 1720;
config.fastStart = true;
config.h245Tunneling = true;

bool ok = endpoint.initialize(config);
assert(ok == true);
assert(endpoint.isInitialized() == true);

// Завершение работы
endpoint.shutdown();
assert(endpoint.isInitialized() == false);
```

**Критерии приемки:**
- [ ] `H323Endpoint` реализован как синглтон
- [ ] `initialize()` настраивает H323EndPoint из H323Plus
- [ ] `shutdown()` корректно освобождает ресурсы
- [ ] Fast Start и H.245 Tunneling настраиваются
- [ ] Обработчик событий зарегистрирован
- [ ] Логирование всех операций

---

### 3. Обработчик событий H323Plus работает

**Ожидаемое:**
```cpp
// События обрабатываются асинхронно
H323EventType receivedEvent;
endpoint.setEventCallback([&receivedEvent](const H323Event& event) {
    receivedEvent = event.type;
});

// При входящем вызове
// Ожидается: H323EventType::INCOMING_CALL

// При установке вызова
// Ожидается: H323EventType::CALL_ESTABLISHED

// При завершении вызова
// Ожидается: H323EventType::CALL_CLEARED
```

**Критерии приемки:**
- [ ] `H323EventHandler` наследуется от `H323Listener`
- [ ] `OnNewIncomingConnection()` вызывается при входящем вызове
- [ ] `OnConnectionState()` отслеживает изменения состояния
- [ ] `OnConnectionClearing()` обрабатывает завершение
- [ ] Callback механизм работает корректно
- [ ] Потокобезопасная обработка событий

---

### 4. H.225 RAS (Gatekeeper) реализован

**Ожидаемое:**
```bash
# Регистрация на Gatekeeper
$ ./h323p register --gatekeeper 192.168.1.1 --username test --password secret
2026-03-16 10:00:00.123 [INFO] Регистрация на Gatekeeper: 192.168.1.1:1719
2026-03-16 10:00:00.456 [INFO] Gatekeeper обнаружен: 192.168.1.1
2026-03-16 10:00:01.234 [INFO] Регистрация успешна

# Статус регистрации
$ ./h323p register --gatekeeper 192.168.1.1 --status
Gatekeeper: 192.168.1.1:1719
Status: REGISTERED
Alias: test
TTL: 300s

# Отписка
$ ./h323p register --gatekeeper 192.168.1.1 --unregister
2026-03-16 10:05:00.123 [INFO] Отписка от Gatekeeper
2026-03-16 10:05:00.456 [INFO] Отписка успешна
```

**Критерии приемки:**
- [ ] `GatekeeperManager` реализован
- [ ] Авто-обнаружение Gatekeeper (multicast GRQ) работает
- [ ] Регистрация (RRQ) работает
- [ ] Отписка (URQ) работает
- [ ] Статус регистрации отслеживается
- [ ] Поддержка TTL и refresh регистрации
- [ ] Обработка ошибок регистрации

---

### 5. H.225 Q.931 (сигнализация вызовов) реализован

**Ожидаемое:**
```cpp
// Создание исходящего вызова
std::string callId = q931.createOutgoingCall("192.168.1.100");
assert(!callId.empty());

// Проверка состояния
CallInfo* info = q931.getCallInfo(callId);
assert(info->state == CallState::CALL_INITIATED);

// Обработка входящего вызова
q931.handleIncomingCall(callId, "192.168.1.50");

// Ответ на вызов
bool ok = q931.answerCall(callId);
assert(ok == true);
assert(info->state == CallState::CALL_ACTIVE);

// Завершение вызова
q931.releaseCall(callId, ReleaseCause::NORMAL_CALL_CLEARING);
assert(info->state == CallState::RELEASE_COMPLETE);
```

**Критерии приемки:**
- [ ] `Q931Manager` управляет состояниями вызовов
- [ ] Все состояния Q.931 реализованы (Null → Setup → Proceeding → Active → Released)
- [ ] Причины завершения (Q.850) поддерживаются
- [ ] `CallInfo` хранит полную информацию о вызове
- [ ] Callback при смене состояния работает
- [ ] Генерация уникальных Call ID

---

### 6. Call Manager управляет вызовами

**Ожидаемое:**
```cpp
// Настройка
CallManager& cm = CallManager::instance();
cm.setMaxCalls(10);
cm.setRepeatCount(5);
cm.setCycleMode(false);

// Исходящий вызов
std::string callId = cm.startOutgoingCall("192.168.1.100");
assert(!callId.empty());

// Статистика
assert(cm.getActiveCalls() == 1);
assert(cm.getTotalCalls() >= 1);

// Остановка всех вызовов
cm.stopAllCalls();
assert(cm.getActiveCalls() == 0);
```

**Критерии приемки:**
- [ ] `CallManager` реализован как синглтон
- [ ] Потокобезопасные операции (mutex)
- [ ] Ограничение максимального числа вызовов
- [ ] Поддержка повторных вызовов (repeat count)
- [ ] Циклический режим (cycle mode)
- [ ] Управление жизненным циклом вызовов

---

### 7. Команда `call` работает

**Ожидаемое:**
```bash
# Базовый вызов
$ ./h323p call 192.168.1.100
2026-03-16 10:00:00.123 [INFO] Запуск h323p v0.1.0
2026-03-16 10:00:00.124 [INFO] Команда: call
2026-03-16 10:00:00.125 [INFO] Назначение: 192.168.1.100
2026-03-16 10:00:00.456 [INFO] Вызов создан: h323p-call-1
2026-03-16 10:00:01.234 [INFO] Вызов установлен
2026-03-16 10:00:31.234 [INFO] Вызов завершён (длительность: 30s)

# Через Gatekeeper
$ ./h323p call alias:1001 --gatekeeper 192.168.1.1 --username test
2026-03-16 10:00:00.123 [INFO] Регистрация на Gatekeeper: 192.168.1.1
2026-03-16 10:00:01.234 [INFO] Регистрация успешна
2026-03-16 10:00:01.456 [INFO] Вызов: alias:1001
2026-03-16 10:00:02.789 [INFO] Вызов установлен

# С опциями
$ ./h323p call 192.168.1.100 --max-calls 5 --repeat 3
2026-03-16 10:00:00.123 [INFO] Max calls: 5, Repeat: 3
```

**Критерии приемки:**
- [ ] Команда `call` принимает адрес назначения
- [ ] Поддержка формата IP:port
- [ ] Поддержка формата alias (через GK)
- [ ] Опция `--gatekeeper` для регистрации
- [ ] Опция `--username` для аутентификации
- [ ] Опция `--max-calls` для ограничения
- [ ] Опция `--repeat` для повторных вызовов
- [ ] Graceful shutdown при Ctrl+C
- [ ] Логирование всех этапов вызова

---

### 8. Команда `listen` работает

**Ожидаемое:**
```bash
# Базовый режим прослушивания
$ ./h323p listen
2026-03-16 10:00:00.123 [INFO] Запуск h323p v0.1.0
2026-03-16 10:00:00.124 [INFO] Команда: listen
2026-03-16 10:00:00.456 [INFO] Прослушивание на порту 1720
2026-03-16 10:05:00.123 [INFO] Входящий вызов: h323p-call-1 от 192.168.1.50
2026-03-16 10:05:01.456 [INFO] Вызов установлен
2026-03-16 10:05:31.456 [INFO] Вызов завершён

# На указанном порту
$ ./h323p listen --port 1721
2026-03-16 10:00:00.456 [INFO] Прослушивание на порту 1721

# С ограничением вызовов
$ ./h323p listen --max-calls 5
2026-03-16 10:00:00.456 [INFO] Максимум одновременных вызовов: 5
```

**Критерии приемки:**
- [ ] Команда `listen` запускает сервер
- [ ] Опция `--port` для указания порта
- [ ] Опция `--max-calls` для ограничения
- [ ] Автоматический ответ на вызовы
- [ ] Обработка множественных вызовов
- [ ] Graceful shutdown при Ctrl+C
- [ ] Логирование всех входящих вызовов

---

### 9. Диаграмма состояний вызова реализована

**Ожидаемое:**
```
                    ┌─────────────┐
                    │   IDLE      │
                    └──────┬──────┘
                           │ makeCall()
                           ▼
                    ┌─────────────┐
              ┌─────│  SETUP      │─────┐
              │     └──────┬──────┘     │ timeout
              │            │ proceed    ▼
              │            │      ┌─────────────┐
              │            ▼      │  RELEASED   │
              │     ┌─────────────┴──────┬──────┘
              │     │     CONNECT        │
              │     └─────────┬──────────┘
              │               │
              │               ▼
              │     ┌─────────────────┐
              │     │  ESTABLISHED    │
              │     └────────┬────────┘
              │              │
              │         duration
              │              │
              │              ▼
              │     ┌─────────────────┐
              └────►│   RELEASING     │
                    └────────┬────────┘
                             │ releaseComplete()
                             ▼
                    ┌─────────────────┐
                    │    RELEASED     │
                    └─────────────────┘
```

**Критерии приемки:**
- [ ] Все состояния реализованы в `CallState`
- [ ] Переходы между состояниями корректны
- [ ] Таймауты обрабатываются
- [ ] Причины завершения (Q.850) поддерживаются
- [ ] Логирование переходов состояний

---

### 10. Интеграционные тесты проходят

**Ожидаемое:**
```bash
# Unit тесты
$ ctest --output-on-failure
Test project /path/to/build
    Start 1: H323EndpointTest.Initialize
1/8 Test #1: H323EndpointTest.Initialize .........   Passed    0.05 sec
    Start 2: H323EndpointTest.MakeCall
2/8 Test #2: H323EndpointTest.MakeCall ...........   Passed    0.10 sec
    Start 3: GatekeeperManagerTest.Discovery
3/8 Test #3: GatekeeperManagerTest.Discovery .....   Passed    0.15 sec
    Start 4: GatekeeperManagerTest.Registration
4/8 Test #4: GatekeeperManagerTest.Registration ..   Passed    0.20 sec
    Start 5: Q931ManagerTest.CallState
5/8 Test #5: Q931ManagerTest.CallState ...........   Passed    0.10 sec
    Start 6: CallManagerTest.SingleCall
6/8 Test #6: CallManagerTest.SingleCall ..........   Passed    0.15 sec
    Start 7: CallManagerTest.MultipleCalls
7/8 Test #7: CallManagerTest.MultipleCalls .......   Passed    0.30 sec
    Start 8: IntegrationTest.CallFlow
8/8 Test #8: IntegrationTest.CallFlow ............   Passed    0.50 sec

100% tests passed!

# Интеграционный тест сценария вызова
$ ./h323p_tests -g IntegrationTest
Tests running in group IntegrationTest:
....
OK (8 tests, 0 failures)
```

**Критерии приемки:**
- [ ] Unit тесты для `H323Endpoint`
- [ ] Unit тесты для `GatekeeperManager`
- [ ] Unit тесты для `Q931Manager`
- [ ] Unit тесты для `CallManager`
- [ ] Интеграционные тесты call flow
- [ ] Все тесты проходят (100%)
- [ ] Тесты запускаются через `ctest`

---

### 11. Сборка с H323Plus работает

**Ожидаемое:**
```bash
# Настройка с H323Plus
$ mkdir build && cd build
$ cmake .. -DH323PLUS_DIR=/path/to/h323plus -DUSE_H323PLUS=ON
-- Found H323Plus: /path/to/h323plus/lib/libH323Plus.a
-- Found PTLib: /path/to/h323plus/lib/libPTLib.a
-- Configuring done
-- Generating done

# Сборка
$ make -j4
[ 10%] Building CXX object CMakeFiles/h323p.dir/src/h323/h323_endpoint.cpp.o
[ 20%] Building CXX object CMakeFiles/h323p.dir/src/h323/ras.cpp.o
[ 30%] Building CXX object CMakeFiles/h323p.dir/src/h323/q931.cpp.o
...
[100%] Linking CXX executable h323p
[100%] Built target h323p

# Запуск
$ ./h323p call --help
Call command options:
  destination              Адрес назначения
  -g, --gatekeeper HOST    Gatekeeper адрес
  -u, --username NAME      Имя пользователя
  ...
```

**Критерии приемки:**
- [ ] CMake находит H323Plus и PTLib
- [ ] Сборка проходит без ошибок компиляции
- [ ] Сборка проходит без предупреждений
- [ ] Исполняемый файл запускается
- [ ] Команды выводят помощь

---

## 📊 Итоговый чек-лист

### Результаты по коду

- [ ] `src/h323/h323_wrapper.*` — Обёртка H323Plus
- [ ] `src/h323/h323_endpoint.*` — H.323 endpoint (синглтон)
- [ ] `src/h323/h323_event_handler.*` — Обработчик событий
- [ ] `src/h323/ras.*` — H.225 RAS (Gatekeeper)
- [ ] `src/h323/q931.*` — H.225 Q.931 (сигнализация)
- [ ] `src/h323/h245.*` — Заглушка для Этапа 3
- [ ] `src/core/call_manager.*` — Управление вызовами
- [ ] `src/core/call.*` — Класс вызова
- [ ] `src/core/endpoint.*` — Внутренний endpoint
- [ ] `src/cli/commands.*` — Обновлённые определения команд
- [ ] `src/cli/commands/cmd_call.*` — Команда call
- [ ] `src/cli/commands/cmd_listen.*` — Команда listen

### Результаты по документации

- [ ] Комментарии к коду — Все публичные API задокументированы
- [ ] `CHANGELOG.md` — Запись о Этапе 2 добавлена
- [ ] `README.md` — Обновлён с примерами call/listen
- [ ] `STAGE2.md` — Детальное описание реализации

### Метрики качества

- [ ] Сборка проходит без предупреждений (`-Wall -Wextra -Wpedantic`)
- [ ] Все тесты проходят (100%)
- [ ] Покрытие тестами > 70% для кода Этапа 2
- [ ] Нет утечек памяти (valgrind)
- [ ] CI/CD пайплайн зелёный

---

## ❌ Что НЕ входит в Этап 2

Следующие функции откладываются до последующих этапов:

- ❌ **H.245** (управление медиа каналами) — Этап 3
- ❌ **RTP/RTCP** (передача медиа) — Этап 3
- ❌ **Кодеки** (G.711, G.729, H.264) — Этап 3
- ❌ **Генератор медиа** — Этап 3
- ❌ **XML сценарии** — Этап 4
- ❌ **Нагрузочное тестирование** — Этап 5
- ❌ **Fuzzing** — Этап 6
- ❌ **Видео и H.239** — Этап 6
- ❌ **TLS** — Этап 6

---

## 🚀 Следующие шаги (Этап 3)

После успешного завершения Этапа 2, перейти к Этапу 3:

**Этап 3: H.245 и медиа**
- Реализовать H.245 (управление логическими каналами)
- Реализовать RTP/RTCP сессии
- Реализовать поддержку кодеков (G.711, G.729)
- Реализовать генератор медиа (синус, шум)
- Реализовать команду `call` с медиа

---

*Версия документа: 1.0*
*Создан: 2026-03-19*
*Репозиторий: https://github.com/Ardameon/h323p*
