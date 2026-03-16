# H.323 Test Utility (h323p) — Архитектурный проект

## 📋 Содержание

1. [Резюме](#-резюме)
2. [Анализ существующих решений](#-анализ-существующих-решений)
3. [Архитектура утилиты](#-архитектура-утилиты)
4. [Формат XML сценариев](#-формат-xml-сценариев)
5. [CLI интерфейс](#-cli-интерфейс)
6. [План реализации](#-план-реализации)

---

## 📌 Резюме

**h323p** — утилита для тестирования H.323 вызовов по аналогии с SIPP для SIP.

### Цели проекта

- Нагрузочное тестирование H.323 endpoints, gateways, gatekeepers
- Функциональное тестирование протокола H.323
- Fuzzing тестирование для проверки устойчивости
- Поддержка аудио, видео, H.239 (презентации)

### Технические решения

| Решение | Выбор | Обоснование |
|---------|-------|-------------|
| **Язык** | C++ | Совместимость с H323Plus |
| **H.323 стек** | H323Plus + PTLib | Единственная активная open-source реализация |
| **Сценарии** | XML | SIPP-подобный синтаксис, знакомый пользователям |
| **Сборка** | CMake | Кроссплатформенность |
| **Тесты** | Google Test / Catch2 | Популярность, возможности |

---

## 📊 Анализ существующих решений

### 1. H323Plus + PTLib (C++)

| Параметр | Описание |
|----------|----------|
| **Язык** | C++ |
| **Статус** | Активно поддерживается (v1.28.0, 2023) |
| **Лицензия** | OpenH323 License / MPL |
| **Платформы** | Linux, Windows, macOS, BSD, встраиваемые системы |

**Преимущества:**
- Полный стек H.323 (H.225, H.245, H.235, H.460.x)
- Поддержка видео (H.261, H.263, H.264), аудиокодеков
- H.239 (презентации), H.235 (шифрование AES)
- NAT traversal (H.460.17/18/19)
- Уже существует **callgen323** на этой базе — референсная реализация
- PTLib обеспечивает кроссплатформенность

**Недостатки:**
- Сложная кодовая база, старая архитектура
- Требует сборки зависимостей (PTLib → H323Plus → приложение)

**Рекомендация:** ✅ **Основной выбор** для реализации

---

### 2. ooh323c (C)

| Параметр | Описание |
|----------|----------|
| **Язык** | C (99.4%) |
| **Статус** | Не поддерживается (~2010) |
| **Лицензия** | GPL-2.0 |

**Преимущества:**
- Чистый C, проще для интеграции
- ASN.1 код сгенерирован компилятором Objective Systems
- Есть тестовые программы (receiver, player, simple, chansetup)

**Недостатки:**
- ❌ Мёртвый проект (15+ лет без обновлений)
- Ограниченная поддержка кодеков
- Нет современных расширений H.460

**Рекомендация:** ⚠️ Только для изучения или простых сценариев

---

### 3. OPAL (C++)

| Параметр | Описание |
|----------|----------|
| **Язык** | C++ |
| **Статус** | Поддерживается, но порт устарел (v3.10.10, ~2013) |
| **Лицензия** | MPL-1.1 |

**Преимущества:**
- Абстракция над H.323 + SIP + IAX2
- Модульная система кодеков
- Low-latency RTP стек

**Недостатки:**
- Сложнее из-за многопротокольной абстракции
- Меньше контроля над спецификой H.323

**Рекомендация:** ⚠️ Если нужна поддержка нескольких протоколов

---

### 4. callgen323 (референс)

**callgen323** — существующий генератор вызовов на H323Plus.

**Ключевые возможности:**
- Аудио, видео, H.239
- H.235 AES шифрование
- RTP fuzzing
- Режимы: Dialing / Listening

**Командные опции (основные):**
```
-l, --max num           Макс. одновременных вызовов
-r, --repeat num        Повторить N раз
-g, --gatekeeper host   Gatekeeper адрес
-n, --no-gatekeeper     Отключить GK
-v, --video             Включить видео
-P, --prefer codec      Предпочтение кодека
-b, --bandwidth kbps    Полоса пропускания
--mediaenc              Шифрование медиа (128/192/256)
--fuzzing               Включить RTP fuzzing
--h239enable            Включить H.239
--tls                   Включить TLS
```

---

## 🏗️ Архитектура утилиты

### Общая архитектура

```
┌─────────────────────────────────────────────────────────────────┐
│                         H323p (H.323 Tester)                    │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    CLI Interface                        │   │
│  │         (argparse / CLI11 / boost::program_options)     │   │
│  └─────────────────────────────────────────────────────────┘   │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │                    Core Engine                            │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │ Call Manager │  │Scenario Engine│  │ Statistics      │ │ │
│  │  │              │  │  (XML Parser) │  │ & Logging       │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │                  H.323 Protocol Layer                     │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │   H.225 RAS  │  │  H.225 Q.931 │  │     H.245       │ │ │
│  │  │  (Gatekeeper)│  │   (Call Sig) │  │  (Control)      │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │                    Media Layer                            │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │  RTP/RTCP    │  │   Codecs     │  │  Media Generator│ │ │
│  │  │   Stack      │  │  (Plugins)   │  │  (patterns)     │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┴───────────────────────────────┐ │
│  │              H323Plus + PTLib (Backend)                   │ │
│  └───────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

---

### Структура проекта

```
h323p/
├── CMakeLists.txt              # Основная сборка
├── README.md
├── LICENSE
├── docs/
│   ├── architecture.md
│   ├── xml_scenarios.md
│   └── cli_reference.md
├── src/
│   ├── main.cpp                # Точка входа
│   ├── cli/
│   │   ├── cli_parser.hpp
│   │   ├── cli_parser.cpp
│   │   └── commands.hpp        # CLI команды
│   ├── core/
│   │   ├── call_manager.hpp    # Управление вызовами
│   │   ├── call_manager.cpp
│   │   ├── call.hpp            # Класс вызова
│   │   ├── call.cpp
│   │   ├── endpoint.hpp        # H.323 endpoint
│   │   └── endpoint.cpp
│   ├── scenario/
│   │   ├── scenario_parser.hpp # XML парсер
│   │   ├── scenario_parser.cpp
│   │   ├── scenario.hpp        # Модель сценария
│   │   ├── scenario.cpp
│   │   ├── actions.hpp         # Действия сценария
│   │   └── actions.cpp
│   ├── h323/
│   │   ├── h323_wrapper.hpp    # Обёртка над H323Plus
│   │   ├── h323_wrapper.cpp
│   │   ├── ras.hpp             # H.225 RAS
│   │   ├── ras.cpp
│   │   ├── q931.hpp            # H.225 Q.931
│   │   ├── q931.cpp
│   │   └── h245.hpp            # H.245
│   │   └── h245.cpp
│   ├── media/
│   │   ├── rtp_session.hpp     # RTP сессия
│   │   ├── rtp_session.cpp
│   │   ├── codec_manager.hpp   # Кодеки
│   │   ├── codec_manager.cpp
│   │   └── media_generator.hpp # Генератор медиа
│   │   └── media_generator.cpp
│   ├── stats/
│   │   ├── statistics.hpp      # Статистика
│   │   ├── statistics.cpp
│   │   ├── cdr.hpp             # CDR записи
│   │   └── cdr.cpp
│   └── utils/
│       ├── logging.hpp         # Логирование
│       ├── logging.cpp
│       ├── timer.hpp           # Таймеры
│       └── utils.hpp           # Утилиты
├── scenarios/
│   ├── basic_call.xml          # Базовый вызов
│   ├── gatekeeper_register.xml # Регистрация на GK
│   ├── load_test.xml           # Нагрузочный тест
│   └── fuzzing.xml             # Fuzzing тест
├── tests/
│   ├── unit/
│   └── integration/
└── cmake/
    └── FindH323Plus.cmake
```

---

### Компоненты (ключевые классы)

#### Call Manager
```cpp
class CallManager {
public:
    // Конфигурация
    void setMaxCalls(size_t max);
    void setRepeatCount(size_t count);
    void setCycleMode(bool enable);
    
    // Управление вызовами
    void startOutgoingCall(const std::string& destination);
    void startListening();
    void stopAllCalls();
    
    // Статистика
    Statistics& getStatistics();
    
private:
    std::vector<std::shared_ptr<Call>> activeCalls_;
    std::queue<std::string> destinationQueue_;
    Statistics stats_;
};
```

#### Scenario Engine
```cpp
class ScenarioEngine {
public:
    // Загрузка сценария
    bool loadScenario(const std::string& filename);
    
    // Выполнение
    void execute(Call& call);
    void pause(std::chrono::milliseconds duration);
    
    // Переменные
    void setVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name);
    
private:
    Scenario scenario_;
    std::map<std::string, std::string> variables_;
};
```

#### H.323 Wrapper
```cpp
class H323Wrapper {
public:
    // Инициализация
    bool initialize(const Config& config);
    
    // Вызовы
    bool makeCall(const std::string& destination, Call& call);
    bool acceptCall(Call& call);
    bool releaseCall(Call& call);
    
    // Gatekeeper
    bool registerWithGatekeeper(const std::string& gkAddress);
    bool unregisterFromGatekeeper();
    
    // Медиа
    bool openLogicalChannel(MediaType type);
    bool closeLogicalChannel(MediaType type);
    
private:
    H323EndPoint* endpoint_;  // H323Plus
};
```

---

### Диаграмма состояний вызова

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
              │               │ openChannels()
              │               ▼
              │     ┌─────────────────┐
              │     │  ESTABLISHED    │◄────┐
              │     │  (media active) │     │
              │     └────────┬────────┘     │
              │              │              │
              │         duration         timeout
              │              │              │
              │              ▼              │
              │     ┌─────────────────┐     │
              └────►│   RELEASING     │─────┘
                    └────────┬────────┘
                             │ releaseComplete()
                             ▼
                    ┌─────────────────┐
                    │    RELEASED     │
                    └─────────────────┘
```

---

## 📄 Формат XML сценариев

### Общая структура

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="Basic H.323 Call" version="1.0">
  <!-- Глобальные настройки -->
  <globals>
    <variable name="max_calls" value="10"/>
    <variable name="call_duration" value="30"/>
  </globals>
  
  <!-- Сценарий для инициатора (UAC-подобный) -->
  <callflow name="caller">
    <actions>
      <!-- Действия вызова -->
    </actions>
  </callflow>
  
  <!-- Сценарий для отвечающего (UAS-подобный) -->
  <callflow name="callee">
    <actions>
      <!-- Действия ответа -->
    </actions>
  </callflow>
</scenario>
```

---

### Элементы сценария

| Элемент | Описание | Атрибуты |
|---------|----------|----------|
| `<setup>` | Инициирование вызова | `destination`, `gatekeeper`, `faststart` |
| `<wait>` | Ожидание события | `event`, `timeout`, `optional` |
| `<connect>` | Ответ на вызов | `alerting`, `h245address` |
| `<open-channel>` | Открытие логического канала | `type`, `codec`, `direction` |
| `<close-channel>` | Закрытие логического канала | `channel_id` |
| `<release>` | Завершение вызова | `cause`, `location` |
| `<pause>` | Пауза | `milliseconds`, `distribution` |
| `<register>` | Регистрация на GK | `gatekeeper`, `ttl` |
| `<unregister>` | Отписка от GK | — |
| `<send-fuzz>` | Fuzzing пакетов | `target`, `percentage` |
| `<loop>` | Цикл | `variable`, `from`, `to` |
| `<label>` | Метка | `id` |
| `<log>` | Логирование | `message` |

---

### Примеры сценариев

#### Базовый вызов (без Gatekeeper)

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="Basic Call">
  <callflow name="caller">
    <actions>
      <!-- Инициирование вызова -->
      <setup destination="192.168.1.100:1720" faststart="true"/>
      
      <!-- Ожидание прогресса -->
      <wait event="call_proceeding" timeout="5000"/>
      
      <!-- Ожидание соединения -->
      <wait event="connect" timeout="30000">
        <on-success next="established"/>
        <on-timeout next="release"/>
      </wait>
      
      <label id="established"/>
      
      <!-- Открытие медиа каналов -->
      <open-channel type="audio" codec="G.711" direction="sendrecv"/>
      <open-channel type="video" codec="H.264" direction="sendrecv" optional="true"/>
      
      <!-- Пауза на время разговора -->
      <pause distribution="uniform" min="10000" max="30000"/>
      
      <!-- Закрытие каналов -->
      <close-channel type="audio"/>
      <close-channel type="video"/>
      
      <!-- Завершение вызова -->
      <release cause="normal_call_clearing"/>
      
      <!-- Ожидание освобождения -->
      <wait event="release_complete" timeout="5000"/>
    </actions>
  </callflow>
</scenario>
```

---

#### Вызов через Gatekeeper с регистрацией

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="Gatekeeper Call">
  <globals>
    <variable name="gatekeeper" value="192.168.1.1"/>
    <variable name="username" value="test_user"/>
    <variable name="password" value="secret"/>
  </globals>
  
  <callflow name="caller">
    <actions>
      <!-- Регистрация на Gatekeeper -->
      <register gatekeeper="${gatekeeper}" username="${username}" password="${password}"/>
      <wait event="registration_confirm" timeout="5000"/>
      
      <!-- Разрешение вызова (Admission) -->
      <setup destination="alias:1001" gatekeeper="${gatekeeper}"/>
      <wait event="admission_confirm" timeout="5000"/>
      
      <!-- Ожидание прогресса -->
      <wait event="call_proceeding" timeout="5000"/>
      
      <!-- Ожидание соединения -->
      <wait event="connect" timeout="30000"/>
      
      <!-- Открытие аудио канала -->
      <open-channel type="audio" codec="G.711" direction="sendrecv"/>
      
      <!-- Длительность вызова -->
      <pause variable="call_duration"/>
      
      <!-- Закрытие канала -->
      <close-channel type="audio"/>
      
      <!-- Завершение -->
      <release cause="normal_call_clearing"/>
      <wait event="release_complete" timeout="5000"/>
      
      <!-- Отписка от GK -->
      <unregister/>
      <wait event="unregister_confirm" timeout="3000"/>
    </actions>
  </callflow>
</scenario>
```

---

#### Нагрузочный тест с циклическим перебором

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="Load Test">
  <globals>
    <variable name="max_calls" value="100"/>
    <variable name="call_rate" value="10"/>
    <variable name="call_duration" value="60"/>
  </globals>
  
  <callflow name="load_generator">
    <actions>
      <!-- Циклическая генерация вызовов -->
      <loop variable="i" from="1" to="${max_calls}">
        <setup destination="destinations[${i}]"/>
        
        <wait event="connect" timeout="30000">
          <on-success>
            <open-channel type="audio" codec="G.711"/>
            <pause variable="call_duration"/>
            <close-channel type="audio"/>
            <release cause="normal_call_clearing"/>
          </on-success>
          <on-failure>
            <log message="Call ${i} failed"/>
          </on-failure>
        </wait>
        
        <!-- Интервал между вызовами -->
        <pause distribution="exponential" mean="1000"/>
      </loop>
      
      <!-- Ожидание завершения всех вызовов -->
      <wait event="all_calls_released" timeout="300000"/>
      
      <!-- Статистика -->
      <print-stats/>
    </actions>
  </callflow>
  
  <!-- Список назначений (можно загружать из CSV) -->
  <destinations file="destinations.csv"/>
</scenario>
```

---

#### Fuzzing тест (RTP)

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="RTP Fuzzing Test">
  <globals>
    <variable name="fuzz_percentage_header" value="50"/>
    <variable name="fuzz_percentage_media" value="10"/>
  </globals>
  
  <callflow name="fuzzer">
    <actions>
      <setup destination="192.168.1.100:1720"/>
      <wait event="connect" timeout="30000"/>
      
      <!-- Открытие канала с fuzzing -->
      <open-channel type="audio" codec="G.711">
        <fuzz enable="true" 
              header_percentage="${fuzz_percentage_header}"
              media_percentage="${fuzz_percentage_media}"
              rtcp_percentage="5"/>
      </open-channel>
      
      <!-- Длительность fuzzing сессии -->
      <pause milliseconds="60000"/>
      
      <close-channel type="audio"/>
      <release cause="normal_call_clearing"/>
    </actions>
  </callflow>
</scenario>
```

---

### Переменные и подстановки

```xml
<!-- Глобальные переменные -->
<globals>
  <variable name="gatekeeper" value="192.168.1.1"/>
  <variable name="base_port" value="1720"/>
</globals>

<!-- Использование -->
<setup destination="${gatekeeper}:${base_port}"/>

<!-- Переменные из CSV -->
<callflow>
  <actions>
    <setup destination="${csv.destination}"/>
    <log message="Calling ${csv.username} at ${csv.destination}"/>
  </actions>
</callflow>

<!-- Встроенные переменные -->
<log message="Call ID: ${call_id}"/>
<log message="Local IP: ${local_ip}"/>
<log message="Remote IP: ${remote_ip}"/>
<log message="Call duration: ${call_duration}ms"/>
```

---

### Распределения времени

```xml
<!-- Фиксированная пауза -->
<pause distribution="fixed" value="5000"/>

<!-- Равномерное распределение -->
<pause distribution="uniform" min="10000" max="30000"/>

<!-- Нормальное распределение -->
<pause distribution="normal" mean="60000" stdev="15000"/>

<!-- Экспоненциальное (для нагрузки) -->
<pause distribution="exponential" mean="1000"/>

<!-- Вейбулла -->
<pause distribution="weibull" lambda="3" k="4"/>
```

---

## 🖥️ CLI интерфейс

### Структура команд

```
h323p <command> [options] [arguments]
```

---

### Команды

| Команда | Описание |
|---------|----------|
| `call` | Запуск вызова по сценарию или напрямую |
| `listen` | Режим прослушивания входящих вызовов |
| `load` | Нагрузочное тестирование |
| `fuzz` | Fuzzing тестирование |
| `register` | Регистрация на Gatekeeper |
| `info` | Информация о системе и версии |

---

### Команда `call` — Инициирование вызова

```bash
# Базовый вызов
h323p call 192.168.1.100

# Через Gatekeeper
h323p call alias:1001 --gatekeeper 192.168.1.1

# По сценарию
h323p call --scenario scenarios/basic_call.xml

# С видео и H.239
h323p call 192.168.1.100 --video --h239

# С шифрованием
h323p call 192.168.1.100 --media-enc 256

# С указанием кодеков
h323p call 192.168.1.100 --audio-codec G.711 --video-codec H.264

# С файлом назначений (CSV)
h323p call --destinations destinations.csv --cycle
```

**Опции:**
```
Позиционные:
  destination              Адрес назначения (IP, alias, URI)

Опции:
  -s, --scenario FILE      XML сценарий
  -g, --gatekeeper HOST    Gatekeeper адрес
  -n, --no-gatekeeper      Не использовать GK
  -u, --username NAME      Имя пользователя
  -p, --password PASS      Пароль для GK
  -v, --video              Включить видео
  -P, --audio-codec CODEC  Аудиокодек (G.711, G.729, Speex)
  --video-codec CODEC      Видеокодек (H.264, H.263)
  --h239                   Включить H.239 презентации
  --media-enc BITS         Шифрование медиа (128/192/256)
  --tls                    Использовать TLS
  -m, --max-calls N        Макс. одновременных вызовов
  -r, --repeat N           Повторить N раз
  -C, --cycle              Циклический перебор
  --tmin-call SECS         Мин. длительность вызова
  --tmax-call SECS         Макс. длительность вызова
  --cdr FILE               Файл для CDR записей
  -t, --trace LEVEL        Уровень трассировки (0-3)
  -o, --output FILE        Файл вывода трассировки
  -h, --help               Помощь
```

---

### Команда `listen` — Режим прослушивания

```bash
# Базовый режим
h323p listen

# На порту
h323p listen --port 1720

# С TLS
h323p listen --tls --tls-port 1300

# С ограничением вызовов
h323p listen --max-calls 50

# Со сценарием ответа
h323p listen --scenario scenarios/callee.xml
```

**Опции:**
```
Опции:
  -p, --port PORT          Порт для прослушивания (1720)
  --tls-port PORT          TLS порт (1300)
  --tls                    Включить TLS
  --tls-cert FILE          TLS сертификат
  --tls-key FILE           Приватный ключ
  -m, --max-calls N        Макс. одновременных вызовов
  -s, --scenario FILE      Сценарий ответа
  --gatekeeper HOST        Gatekeeper для регистрации
  --auto-answer            Автоматический ответ
  --alerting-time SECS     Время "звонка" до ответа
  --tmin-call SECS         Мин. длительность вызова
  --tmax-call SECS         Макс. длительность вызова
  -t, --trace LEVEL        Уровень трассировки
  -h, --help               Помощь
```

---

### Команда `load` — Нагрузочное тестирование

```bash
# Базовая нагрузка
h323p load 192.168.1.100 --calls 100 --rate 10

# По сценарию
h323p load --scenario scenarios/load_test.xml

# С файлом назначений
h323p load --destinations destinations.csv --rate 20

# С длительностью теста
h323p load 192.168.1.100 --calls 500 --rate 50 --duration 300

# С статистикой
h323p load 192.168.1.100 --calls 100 --rate 10 --stats-interval 5
```

**Опции:**
```
Позиционные:
  destination              Адрес назначения

Опции:
  -c, --calls N            Общее количество вызовов
  -r, --rate N             Вызовов в секунду
  -d, --duration SECS      Длительность теста
  -s, --scenario FILE      XML сценарий
  --destinations FILE      CSV файл с назначениями
  --cycle                  Циклический перебор
  --ramp-up SECS           Время нарастания нагрузки
  --ramp-down SECS         Время спада нагрузки
  --stats-interval SECS    Интервал статистики
  --cdr FILE               Файл CDR
  --report FILE            Файл отчёта (JSON/CSV)
  --stop-on-error          Остановить при ошибке
  -t, --trace LEVEL        Уровень трассировки
  -h, --help               Помощь
```

---

### Команда `fuzz` — Fuzzing тестирование

```bash
# RTP fuzzing
h323p fuzz 192.168.1.100 --type rtp --percentage 50

# H.225 fuzzing
h323p fuzz 192.168.1.100 --type h225 --percentage 30

# H.245 fuzzing
h323p fuzz 192.168.1.100 --type h245 --percentage 20

# По сценарию
h323p fuzz --scenario scenarios/fuzzing.xml
```

**Опции:**
```
Позиционные:
  destination              Адрес назначения

Опции:
  -T, --type TYPE          Тип fuzzing (rtp/h225/h245/rtcp)
  -p, --percentage N       Процент случайных данных (0-100)
  --header-percentage N    Процент заголовка
  --media-percentage N     Процент медиа
  --seed N                 Seed для генератора
  -c, --calls N            Количество вызовов
  -s, --scenario FILE      XML сценарий
  --crash-detect           Детектирование крашей
  --timeout MS             Таймаут ответа
  -t, --trace LEVEL        Уровень трассировки
  -h, --help               Помощь
```

---

### Команда `register` — Регистрация на Gatekeeper

```bash
# Базовая регистрация
h323p register --gatekeeper 192.168.1.1 --username test --password secret

# С TTL
h323p register -g 192.168.1.1 -u test --ttl 300

# Проверка статуса
h323p register --gatekeeper 192.168.1.1 --status

# Отписка
h323p register --gatekeeper 192.168.1.1 --unregister
```

**Опции:**
```
Опции:
  -g, --gatekeeper HOST    Gatekeeper адрес
  -u, --username NAME      Имя пользователя
  -p, --password PASS      Пароль
  --ttl SECS               Время регистрации
  --status                 Проверить статус
  --unregister             Отписаться
  --alias ALIAS            Дополнительный алиас
  --transport TYPE         Транспорт (UDP/TCP/TLS)
  -t, --trace LEVEL        Уровень трассировки
  -h, --help               Помощь
```

---

### Команда `info` — Информация

```bash
# Версия
h323p info --version

# Поддерживаемые кодеки
h323p info --codecs

# Возможности
h323p info --features

# Полная информация
h323p info --all
```

**Опции:**
```
Опции:
  -v, --version            Версия
  --codecs                 Список кодеков
  --features               Возможности
  --protocols              Протоколы
  --all                    Вся информация
  -h, --help               Помощь
```

---

### Глобальные опции

```bash
# Для всех команд
h323p --config /etc/h323p.conf call 192.168.1.100
```

| Опция | Описание |
|-------|----------|
| `--config FILE` | Файл конфигурации |
| `--log-file FILE` | Файл логов |
| `--log-level LEVEL` | Уровень логирования (debug/info/warn/error) |
| `--interface ADDR` | Локальный интерфейс |
| `--tcp-base PORT` | Базовый TCP порт |
| `--udp-base PORT` | Базовый UDP порт |
| `--rtp-base PORT` | Базовый RTP порт |
| `--plugin-dir DIR` | Директория плагинов |
| `-q, --quiet` | Тихий режим |
| `--verbose` | Подробный режим |
| `--help` | Общая помощь |

---

### Примеры использования

```bash
# 1. Простой вызов
h323p call 192.168.1.100

# 2. Вызов через GK с видео
h323p call alias:1001 -g 192.168.1.1 -u test -v

# 3. Нагрузочный тест на 100 вызовов
h323p load 192.168.1.100 -c 100 -r 10 --stats-interval 5

# 4. Fuzzing тест
h323p fuzz 192.168.1.100 -T rtp -p 50 -c 10

# 5. Режим прослушивания
h323p listen -m 50 --auto-answer

# 6. Регистрация на GK
h323p register -g 192.168.1.1 -u terminal1 -p secret --ttl 600

# 7. По сценарию с CDR
h323p call -s scenarios/custom.xml --cdr calls.csv -t 2
```

---

## 📅 План реализации

### Дорожная карта

```
Этап 1 (2-3 недели)  ──► Базовая инфраструктура
         │
         ▼
Этап 2 (3-4 недели)  ──► H.323 стек (H.225)
         │
         ▼
Этап 3 (2-3 недели)  ──► H.245 и медиа
         │
         ▼
Этап 4 (2 недели)    ──► XML сценарии
         │
         ▼
Этап 5 (2 недели)    ──► Нагрузочное тестирование
         │
         ▼
Этап 6 (1-2 недели)  ──► Fuzzing и расширения
         │
         ▼
Этап 7 (1 неделя)    ──► Документация и релиз
```

---

### Этап 1: Базовая инфраструктура (2-3 недели)

**Цель:** Создать каркас проекта и базовые утилиты

📄 **Подробная документация:** См. [STAGE1.md](STAGE1.md)

| Задача | Описание | Оценка |
|--------|----------|--------|
| **1.1** Настройка сборки | CMakeLists.txt, зависимости (H323Plus, PTLib) | 2 дня |
| **1.2** CLI парсер | Интеграция CLI11 или boost::program_options | 3 дня |
| **1.3** Логирование | spdlog или собственная реализация | 2 дня |
| **1.4** Система таймеров | Таймеры для пауз и таймаутов | 2 дня |
| **1.5** Базовые утилиты | Строковые функции, парсинг адресов | 2 дня |
| **1.6** Unit тесты | Настройка Catch2 или Google Test | 2 дня |

**Результат этапа:**
```
✅ Проект собирается
✅ Работает --version, --help
✅ Базовое логирование
✅ Проходят unit тесты
```

**Структура этапа:**
```
CMakeLists.txt              # Сборка
src/
  ├── main.cpp              # Точка входа
  ├── cli/                  # CLI парсер
  ├── utils/                # Утилиты (логи, таймеры)
  └── ...                   # Остальные модули (заглушки)
tests/                      # Unit тесты
```

---

### Этап 2: H.323 стек H.225 (3-4 недели)

**Цель:** Реализовать базовые вызовы через H.225

📄 **Подробная документация:** См. [STAGE2.md](STAGE2.md)

| Задача | Описание | Оценка |
|--------|----------|--------|
| **2.1** Обёртка H323Plus | Базовый endpoint, инициализация | 4 дня |
| **2.2** H.225 RAS | Регистрация, Admission, Bandwidth | 5 дней |
| **2.3** H.225 Q.931 | Setup, Connect, Release | 5 дней |
| **2.4** Call Manager | Управление состояниями вызовов | 4 дня |
| **2.5** Команда `call` | Базовый вызов без медиа | 3 дня |
| **2.6** Команда `listen` | Приём входящих вызовов | 3 дня |
| **2.7** Интеграционные тесты | Тесты с реальным endpoint | 4 дня |

**Результат этапа:**
```
✅ Регистрация на Gatekeeper
✅ Исходящие вызовы (Setup → Connect → Release)
✅ Входящие вызовы
✅ Статистика вызовов
```

**Структура этапа:**
```
src/h323/
  ├── h323_endpoint.hpp     # Обёртка над H323Plus
  ├── h323_endpoint.cpp
  ├── ras.hpp               # H.225 RAS
  ├── ras.cpp
  ├── q931.hpp              # H.225 Q.931
  ├── q931.cpp
  └── h245.hpp              # Заглушка (Этап 3)
src/core/
  ├── call_manager.hpp      # Управление вызовами
  └── call_manager.cpp
src/cli/commands/
  ├── cmd_call.hpp          # Команда call
  ├── cmd_call.cpp
  ├── cmd_listen.hpp        # Команда listen
  └── cmd_listen.cpp
```

---

### Этап 3: H.245 и медиа (2-3 недели)

**Цель:** Добавить управление каналами и RTP

📄 **Подробная документация:** См. [STAGE3.md](STAGE3.md)

| Задача | Описание | Оценка |
|--------|----------|--------|
| **3.1** H.245 обёртка | MasterSlave, CapabilitySet | 3 дня |
| **3.2** Логические каналы | OpenLogicalChannel, CloseLogicalChannel | 4 дня |
| **3.3** RTP сессия | Отправка/получение RTP пакетов | 4 дня |
| **3.4** Кодеки | G.711, загрузка из файла | 3 дня |
| **3.5** Генератор медиа | Паттерны (Fake, BouncingBoxes) | 3 дня |
| **3.6** RTCP | Базовая статистика | 2 дня |
| **3.7** Тесты медиа | Проверка RTP потоков | 3 дня |

**Результат этапа:**
```
✅ Полноценные вызовы с аудио
✅ Генерация тестовых паттернов
✅ Воспроизведение из WAV файлов
✅ RTCP статистика
```

**Структура этапа:**
```
src/h323/
  ├── h245.hpp              # H.245 протокол
  ├── h245.cpp
  └── h245_messages.hpp     # Типы сообщений
src/media/
  ├── rtp_session.hpp       # RTP сессия
  ├── rtp_session.cpp
  ├── rtp_packet.hpp        # RTP пакет
  ├── rtcp_session.hpp      # RTCP статистика
  ├── codec_manager.hpp     # Менеджер кодеков
  ├── codec.hpp             # Интерфейс кодека
  ├── codecs/
  │   ├── g711_codec.hpp    # G.711 кодек
  │   └── g729_codec.hpp    # G.729 кодек
  ├── media_generator.hpp   # Генератор медиа
  └── media_patterns.hpp    # Тестовые паттерны
```

---

### Этап 4: XML сценарии (2 недели)

**Цель:** Реализовать движок сценариев

📄 **Подробная документация:** См. [STAGE4.md](STAGE4.md)

| Задача | Описание | Оценка |
|--------|----------|--------|
| **4.1** XML парсер | pugixml или tinyxml2 | 3 дня |
| **4.2** Модель сценария | Представление в памяти | 3 дня |
| **4.3** Интерпретатор | Выполнение действий | 5 дней |
| **4.4** Переменные | Подстановки, CSV | 2 дня |
| **4.5** Распределения | Uniform, normal, exponential | 2 дня |
| **4.6** Примеры сценариев | Базовые тесты | 3 дня |

**Результат этапа:**
```
✅ Парсинг XML сценариев
✅ Выполнение по шагам
✅ Переменные и CSV
✅ Готовые примеры
```

**Структура этапа:**
```
src/scenario/
  ├── scenario_parser.hpp     # XML парсер
  ├── scenario_parser.cpp
  ├── scenario_types.hpp      # Типы сценария
  ├── scenario.hpp            # Модель сценария
  ├── interpreter.hpp         # Интерпретатор
  ├── interpreter.cpp
  ├── actions.hpp             # Действия
  ├── variables.hpp           # Переменные
  ├── distributions.hpp       # Распределения
  └── csv_reader.hpp          # CSV файлы
```

---

### Этап 5: Нагрузочное тестирование (2 недели)

**Цель:** Реализовать нагрузочные тесты

📄 **Подробная документация:** См. [STAGE5.md](STAGE5.md)

| Задача | Описание | Оценка |
|--------|----------|--------|
| **5.1** Call Manager v2 | Множественные вызовы | 3 дня |
| **5.2** Команда `load` | Генерация нагрузки | 3 дня |
| **5.3** Статистика | Счётчики, метрики | 3 дня |
| **5.4** CDR записи | Логирование вызовов | 2 дня |
| **5.5** Отчёты | JSON/CSV экспорт | 2 дня |
| **5.6** Тесты нагрузки | Проверка на 100+ вызовов | 3 дня |

**Результат этапа:**
```
✅ Нагрузка до 1000 вызовов
✅ Статистика в реальном времени
✅ CDR записи
✅ JSON/CSV отчёты
```

**Структура этапа:**
```
src/load/
  ├── load_test_controller.hpp  # Контроллер теста
  ├── load_test_controller.cpp
  ├── load_config.hpp           # Конфигурация
  ├── call_pool.hpp             # Пул вызовов
  ├── rate_limiter.hpp          # Ограничитель скорости
  └── ramp_profile.hpp          # Профили ramp up/down
src/stats/
  ├── statistics.hpp            # Статистика
  ├── metrics.hpp               # Метрики
  ├── cdr.hpp                   # CDR записи
  ├── cdr_writer.hpp            # Запись CDR
  └── report_generator.hpp      # Генератор отчётов
```

---

### Этап 6: Fuzzing и расширения (1-2 недели)

**Цель:** Добавить fuzzing и дополнительные функции

📄 **Подробная документация:** См. [STAGE6.md](STAGE6.md)

| Задача | Описание | Оценка |
|--------|----------|--------|
| **6.1** RTP fuzzing | Случайная модификация пакетов | 3 дня |
| **6.2** H.225 fuzzing | Fuzzing сигнальных сообщений | 3 дня |
| **6.3** Команда `fuzz` | CLI для fuzzing тестов | 2 дня |
| **6.4** Видео | Базовая поддержка видео | 4 дня |
| **6.5** H.239 | Поддержка презентаций | 4 дня |
| **6.6** TLS | Защищённое соединение | 3 дня |

**Результат этапа:**
```
✅ RTP/H.225 fuzzing
✅ Базовое видео
✅ H.239 презентации
✅ TLS поддержка
```

**Структура этапа:**
```
src/fuzz/
  ├── fuzz_engine.hpp         # Движок fuzzing
  ├── fuzz_types.hpp          # Типы fuzzing
  ├── strategies/
  │   ├── random_flip.hpp     # Случайная инверсия
  │   ├── byte_flip.hpp       # Инверсия байтов
  │   └── boundary_values.hpp # Граничные значения
  ├── rtp_fuzzer.hpp          # RTP fuzzing
  ├── h225_fuzzer.hpp         # H.225 fuzzing
  └── h245_fuzzer.hpp         # H.245 fuzzing
src/extensions/
  ├── video/
  │   ├── video_channel.hpp   # Видео канал
  │   ├── h261_codec.hpp      # H.261 кодек
  │   └── h263_codec.hpp      # H.263 кодек
  ├── h239/
  │   ├── h239_controller.hpp # H.239 контроллер
  │   └── presentation_channel.hpp
  └── tls/
      ├── tls_config.hpp      # TLS конфигурация
      └── tls_endpoint.hpp    # TLS endpoint
```

---

### Этап 7: Документация и релиз (1 неделя)

**Цель:** Подготовить к публикации

📄 **Подробная документация:** См. [STAGE7.md](STAGE7.md)

| Задача | Описание | Оценка |
|--------|----------|--------|
| **7.1** README | Установка, быстрое начало | 2 дня |
| **7.2** CLI справочник | Полное описание команд | 2 дня |
| **7.3** Примеры | Демо сценарии (10+) | 1 день |
| **7.4** CI/CD | GitHub Actions сборка | 2 дня |
| **7.5** Релиз | Версия 0.1.0 | 1 день |

**Результат этапа:**
```
✅ Полная документация
✅ Примеры использования
✅ CI сборка
✅ Релиз на GitHub
```

**Структура этапа:**
```
README.md                   # Главный README
INSTALL.md                  # Инструкция по установке
CHANGELOG.md                # История изменений
docs/
  ├── getting_started.md    # Быстрый старт
  ├── cli_reference.md      # CLI справочник
  ├── xml_scenarios.md      # XML сценарии
  └── architecture.md       # Архитектура
examples/
  ├── scenarios/            # Примеры сценариев
  ├── csv/                  # CSV файлы
  └── scripts/              # Скрипты
.github/workflows/
  ├── build.yml             # Сборка
  ├── test.yml              # Тесты
  └── release.yml           # Релиз
```

---

### Итоговая таблица

| Этап | Длительность | Ключевые функции |
|------|--------------|------------------|
| 1 | 2-3 недели | Инфраструктура, CLI, логи |
| 2 | 3-4 недели | H.225, вызовы без медиа |
| 3 | 2-3 недели | H.245, RTP, кодеки |
| 4 | 2 недели | XML сценарии |
| 5 | 2 недели | Нагрузочное тестирование |
| 6 | 1-2 недели | Fuzzing, видео, H.239, TLS |
| 7 | 1 неделя | Документация, релиз |
| **Итого** | **13-17 недель** | **Полнофункциональная утилита** |

---

### MVP (Минимально жизнеспособный продукт)

**Минимально жизнеспособный продукт (8-10 недель):**

```
✅ Этап 1: Инфраструктура
✅ Этап 2: H.225 вызовы
✅ Этап 3: RTP медиа (базовый)
✅ Этап 5: Нагрузочное тестирование (базовое)
```

**MVP команды:**
```bash
h323p call 192.168.1.100          # Базовый вызов
h323p load 192.168.1.100 -c 100   # Нагрузка
h323p listen                      # Прослушивание
```

---

## 📚 Приложения

### A. Документация по этапам

| Этап | Файл | Статус |
|------|------|--------|
| 1 | [STAGE1.md](STAGE1.md) | ✅ Готов |
| 2 | [STAGE2.md](STAGE2.md) | ✅ Готов |
| 3 | [STAGE3.md](STAGE3.md) | ✅ Готов |
| 4 | [STAGE4.md](STAGE4.md) | ✅ Готов |
| 5 | [STAGE5.md](STAGE5.md) | ✅ Готов |
| 6 | [STAGE6.md](STAGE6.md) | ✅ Готов |
| 7 | [STAGE7.md](STAGE7.md) | ✅ Готов |

### B. Зависимости

```cmake
# Основные зависимости
find_package(H323Plus REQUIRED)
find_package(PTLib REQUIRED)

# Опциональные зависимости
find_package(OpenSSL REQUIRED)      # TLS, шифрование
find_package(pugixml REQUIRED)      # XML парсинг
find_package(CLI11 REQUIRED)        # CLI парсер
find_package(spdlog REQUIRED)       # Логирование

# Тесты
find_package(GTest REQUIRED)
```

---

### C. Пример CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(h323p VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Зависимости
find_package(H323Plus REQUIRED)
find_package(PTLib REQUIRED)
find_package(CLI11 REQUIRED)
find_package(spdlog REQUIRED)
find_package(pugixml REQUIRED)

# Исходные файлы
add_executable(h323p
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
)

# Include directories
target_include_directories(h323p PRIVATE
    ${H323PLUS_INCLUDE_DIRS}
    ${PTLIB_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(h323p PRIVATE
    ${H323PLUS_LIBRARIES}
    ${PTLIB_LIBRARIES}
    CLI11::CLI11
    spdlog::spdlog
    pugixml::pugixml
)

# Тесты
enable_testing()
add_subdirectory(tests)
```

---

### D. Полезные ссылки

- **H323Plus:** https://www.h323plus.org/
- **callgen323:** https://github.com/willamowius/callgen323
- **SIPP:** https://github.com/SIPp/sipp
- **SIPP документация:** https://sipp.readthedocs.io/
- **H.323 стандарты:** ITU-T H.225, H.245, H.239

---

*Документ создан: 2026*
*Версия проекта: 0.1.0 (draft)*
