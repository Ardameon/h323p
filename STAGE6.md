# Этап 6: Fuzzing и расширения

**Длительность:** 1-2 недели  
**Статус:** 📋 Готов к реализации  
**Зависимости:** ✅ Этап 1-5 (инфраструктура, H.225, H.245, медиа, сценарии, нагрузка)

---

## 📋 Содержание

1. [Обзор этапа](#-обзор-этапа)
2. [Задачи этапа](#-задачи-этапа)
3. [Архитектура подсистемы fuzzing](#-архитектура-подсистемы-fuzzing)
4. [Детальная реализация](#-детальная-реализация)
5. [Примеры использования](#-примеры-использования)
6. [Критерии завершения](#-критерии-завершения)
7. [Проверка результатов](#-проверка-результатов)

---

## 📌 Обзор этапа

Цель Этапа 6 — реализовать fuzzing тестирование для проверки устойчивости H.323 реализаций, а также добавить поддержку видео, H.239 презентаций и TLS шифрования.

### Результаты этапа

```
✅ RTP fuzzing (модификация заголовков и payload)
✅ H.225 fuzzing (сигнальные сообщения)
✅ H.245 fuzzing (управляющие сообщения)
✅ Команда fuzz для CLI
✅ Базовая поддержка видео (H.261, H.263)
✅ H.239 презентации
✅ TLS шифрование для сигнализации
✅ Интеграция с XML сценариями
```

### Что НЕ входит в Этап 6

```
❌ Документация и релиз — Этап 7
❌ Полноценная поддержка H.264 (требует лицензирования)
❌ Сложные видео тесты
```

---

## 📊 Задачи этапа

| № | Задача | Длительность | Статус |
|---|--------|--------------|--------|
| 6.1 | RTP fuzzing | 3 дня | ⏳ |
| 6.2 | H.225 fuzzing | 3 дня | ⏳ |
| 6.3 | Команда fuzz | 2 дня | ⏳ |
| 6.4 | Видео поддержка | 4 дня | ⏳ |
| 6.5 | H.239 презентации | 4 дня | ⏳ |
| 6.6 | TLS поддержка | 3 дня | ⏳ |
| **Итого** | | **19 дней** | |

---

## 🏗️ Архитектура подсистемы fuzzing

### Общая схема

```
┌─────────────────────────────────────────────────────────────────┐
│                    Application Layer                            │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  CLI: h323p fuzz 192.168.1.100 -T rtp -p 50             │   │
│  └─────────────────────────────────────────────────────────┘   │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │                  Fuzzing Engine                           │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │    RTP       │  │   H.225      │  │     H.245       │ │ │
│  │  │   Fuzzer     │  │   Fuzzer     │  │    Fuzzer       │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │              Mutation Strategies                          │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │    Random     │  │    Bit       │  │     Byte        │ │ │
│  │  │    Flip       │  │  Swapping    │ │     Insertion   │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │  Boundary    │  │   Pattern    │  │     Custom      │ │ │
│  │  │   Values     │  │  Injection   │  │    Rules        │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │              Extensions                                     │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │    Video     │  │    H.239     │  │      TLS        │ │ │
│  │  │   Support    │  │  Presentations│ │  Encryption     │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### Структура модулей

```
src/
└── fuzz/
    ├── fuzz_engine.hpp           # Движок fuzzing
    ├── fuzz_engine.cpp
    ├── fuzz_types.hpp            # Типы fuzzing
    ├── strategies/
    │   ├── random_flip.hpp       # Случайная инверсия битов
    │   ├── random_flip.cpp
    │   ├── byte_flip.hpp         # Инверсия байтов
    │   ├── byte_flip.cpp
    │   ├── boundary_values.hpp   # Граничные значения
    │   ├── boundary_values.cpp
    │   ├── bit_flip.hpp          # Битовая инверсия
    │   └── bit_flip.cpp
    ├── rtp_fuzzer.hpp            # RTP fuzzing
    ├── rtp_fuzzer.cpp
    ├── h225_fuzzer.hpp           # H.225 fuzzing
    ├── h225_fuzzer.cpp
    └── h245_fuzzer.hpp           # H.245 fuzzing
    └── h245_fuzzer.cpp
src/
└── extensions/
    ├── video/
    │   ├── video_channel.hpp     # Видео канал
    │   ├── video_channel.cpp
    │   ├── h261_codec.hpp        # H.261 кодек
    │   ├── h261_codec.cpp
    │   ├── h263_codec.hpp        # H.263 кодек
    │   └── h263_codec.cpp
    ├── h239/
    │   ├── h239_controller.hpp   # H.239 контроллер
    │   ├── h239_controller.cpp
    │   └── presentation_channel.hpp  # Канал презентаций
    │   └── presentation_channel.cpp
    └── tls/
        ├── tls_config.hpp        # TLS конфигурация
        ├── tls_config.cpp
        ├── tls_endpoint.hpp      # TLS endpoint
        └── tls_endpoint.cpp
```

---

## 🔧 Детальная реализация

### 6.1 RTP Fuzzing

**Файл: `src/fuzz/fuzz_types.hpp`**

```cpp
#pragma once

#include <string>
#include <cstdint>
#include <vector>

namespace h323p {

// Типы fuzzing
enum class FuzzType {
    RTP,
    RTCP,
    H225_RAS,
    H225_Q931,
    H245
};

// Стратегии fuzzing
enum class FuzzStrategy {
    RANDOM_FLIP,      // Случайная инверсия битов
    BYTE_FLIP,        // Инверсия байтов
    BIT_FLIP,         // Битовая инверсия
    BOUNDARY_VALUES,  // Граничные значения (0, 255, 0x8000, etc)
    PATTERN_INJECTION, // Внедрение паттернов
    CUSTOM            // Пользовательские правила
};

// Конфигурация fuzzing
struct FuzzConfig {
    FuzzType type = FuzzType::RTP;
    FuzzStrategy strategy = FuzzStrategy::RANDOM_FLIP;
    
    // Проценты модификации
    int headerPercentage = 50;  // % заголовка для модификации
    int payloadPercentage = 0;  // % payload для модификации
    int rtcpPercentage = 5;     // % RTCP пакетов для модификации
    
    // Seed для генератора
    uint32_t seed = 0;  // 0 = случайный
    
    // Фильтры
    std::vector<uint8_t> preserveBytes;  // Байты для сохранения
    std::vector<size_t> preserveRanges;  // Диапазоны для сохранения (start, end)
    
    // Ограничения
    size_t maxFuzzPackets = 0;  // 0 = без ограничений
    bool stopOnCrash = false;   // Остановить при обнаружении краша
};

// Статистика fuzzing
struct FuzzStatistics {
    size_t packetsSent = 0;
    size_t packetsFuzzed = 0;
    size_t crashesDetected = 0;
    size_t timeoutsDetected = 0;
    
    std::time_t startTime = 0;
    std::time_t endTime = 0;
    
    // История
    std::vector<std::string> crashSignatures;
};

} // namespace h323p
```

**Файл: `src/fuzz/strategies/random_flip.hpp`**

```cpp
#pragma once

#include "fuzz_types.hpp"
#include <random>
#include <vector>

namespace h323p {

// Стратегия случайной инверсии
class RandomFlipStrategy {
public:
    RandomFlipStrategy(uint32_t seed = 0);
    
    // Модификация данных
    std::vector<uint8_t> mutate(const std::vector<uint8_t>& data, 
                                 int percentage);
    
    // Модификация конкретного байта
    uint8_t mutateByte(uint8_t value);
    
    // Настройка
    void setPercentage(int percentage) { percentage_ = percentage; }
    int getPercentage() const { return percentage_; }
    
private:
    std::mt19937 gen_;
    int percentage_ = 50;
};

} // namespace h323p
```

**Файл: `src/fuzz/rtp_fuzzer.hpp`**

```cpp
#pragma once

#include "fuzz_types.hpp"
#include "strategies/random_flip.hpp"
#include "media/rtp_packet.hpp"
#include <memory>

namespace h323p {

// RTP Fuzzer
class RtpFuzzer {
public:
    RtpFuzzer(const FuzzConfig& config);
    ~RtpFuzzer();
    
    // Fuzzing пакета
    RtpPacket fuzzPacket(const RtpPacket& original);
    
    // Fuzzing заголовка
    void fuzzHeader(RtpHeader& header);
    
    // Fuzzing payload
    void fuzzPayload(uint8_t* payload, size_t size);
    
    // Статистика
    FuzzStatistics getStatistics() const { return stats_; }
    void resetStatistics();
    
    // Настройка
    void setConfig(const FuzzConfig& config);
    
private:
    void fuzzVersion(RtpHeader& header);
    void fuzzPayloadType(RtpHeader& header);
    void fuzzSequenceNumber(RtpHeader& header);
    void fuzzTimestamp(RtpHeader& header);
    void fuzzSsrc(RtpHeader& header);
    
    FuzzConfig config_;
    FuzzStatistics stats_;
    std::unique_ptr<RandomFlipStrategy> strategy_;
};

} // namespace h323p
```

**Файл: `src/fuzz/rtp_fuzzer.cpp`**

```cpp
#include "rtp_fuzzer.hpp"
#include "utils/logging.hpp"
#include <cstring>

namespace h323p {

RtpFuzzer::RtpFuzzer(const FuzzConfig& config) 
    : config_(config) 
{
    strategy_ = std::make_unique<RandomFlipStrategy>(config.seed);
    strategy_->setPercentage(config.headerPercentage);
}

RtpFuzzer::~RtpFuzzer() = default;

RtpPacket RtpFuzzer::fuzzPacket(const RtpPacket& original) {
    RtpPacket fuzzed = original;
    
    stats_.packetsSent++;
    
    // Fuzzing заголовка
    fuzzHeader(fuzzed.header());
    
    // Fuzzing payload
    if (config_.payloadPercentage > 0) {
        fuzzPayload(fuzzed.payload(), fuzzed.payloadSize());
    }
    
    stats_.packetsFuzzed++;
    
    return fuzzed;
}

void RtpFuzzer::fuzzHeader(RtpHeader& header) {
    // Версия (2 бита)
    if (std::rand() % 100 < config_.headerPercentage) {
        fuzzVersion(header);
    }
    
    // Payload Type (7 бит)
    if (std::rand() % 100 < config_.headerPercentage) {
        fuzzPayloadType(header);
    }
    
    // Sequence Number (16 бит)
    if (std::rand() % 100 < config_.headerPercentage) {
        fuzzSequenceNumber(header);
    }
    
    // Timestamp (32 бита)
    if (std::rand() % 100 < config_.headerPercentage) {
        fuzzTimestamp(header);
    }
    
    // SSRC (32 бита)
    if (std::rand() % 100 < config_.headerPercentage) {
        fuzzSsrc(header);
    }
}

void RtpFuzzer::fuzzVersion(RtpHeader& header) {
    // Инверсия битов версии (должна быть 2)
    uint8_t mutated = strategy_->mutateByte(header.version);
    header.version = mutated & 0x03;  // Маска для 2 бит
    
    LOG_DEBUG_FMT("Fuzz version: %d -> %d", 2, header.version);
}

void RtpFuzzer::fuzzPayloadType(RtpHeader& header) {
    // Случайный payload type (0-127)
    if (std::rand() % 100 < config_.headerPercentage) {
        header.payloadType = std::rand() % 128;
        LOG_DEBUG_FMT("Fuzz payload type: %d", header.payloadType);
    }
}

void RtpFuzzer::fuzzSequenceNumber(RtpHeader& header) {
    // Граничные значения
    uint16_t boundaries[] = {0, 1, 0x7FFF, 0x8000, 0xFFFE, 0xFFFF};
    
    if (std::rand() % 3 == 0) {
        // Граничное значение
        header.sequenceNumber = boundaries[std::rand() % 6];
    } else {
        // Случайное значение
        header.sequenceNumber = std::rand() % 65536;
    }
    
    LOG_DEBUG_FMT("Fuzz sequence number: %u", header.sequenceNumber);
}

void RtpFuzzer::fuzzTimestamp(RtpHeader& header) {
    // Граничные значения для 32 бит
    uint32_t boundaries[] = {
        0, 1, 
        0x7FFFFFFF, 0x80000000,
        0xFFFFFFFE, 0xFFFFFFFF
    };
    
    if (std::rand() % 3 == 0) {
        header.timestamp = boundaries[std::rand() % 6];
    } else {
        header.timestamp = std::rand() | (static_cast<uint32_t>(std::rand()) << 16);
    }
    
    LOG_DEBUG_FMT("Fuzz timestamp: %u", header.timestamp);
}

void RtpFuzzer::fuzzSsrc(RtpHeader& header) {
    // Случайный SSRC
    header.ssrc = std::rand() | (static_cast<uint32_t>(std::rand()) << 16);
    LOG_DEBUG_FMT("Fuzz SSRC: %u", header.ssrc);
}

void RtpFuzzer::fuzzPayload(uint8_t* payload, size_t size) {
    if (size == 0) return;
    
    size_t bytesToFuzz = (size * config_.payloadPercentage) / 100;
    if (bytesToFuzz == 0) bytesToFuzz = 1;
    
    for (size_t i = 0; i < bytesToFuzz; i++) {
        size_t index = std::rand() % size;
        
        // Проверка на сохранение
        bool preserve = false;
        for (uint8_t preserveByte : config_.preserveBytes) {
            if (index == preserveByte) {
                preserve = true;
                break;
            }
        }
        
        if (!preserve) {
            payload[index] = strategy_->mutateByte(payload[index]);
        }
    }
    
    LOG_DEBUG_FMT("Fuzzed %zu bytes in payload", bytesToFuzz);
}

void RtpFuzzer::resetStatistics() {
    stats_ = FuzzStatistics();
}

void RtpFuzzer::setConfig(const FuzzConfig& config) {
    config_ = config;
    strategy_->setPercentage(config.headerPercentage);
}

} // namespace h323p
```

---

### 6.2 H.225 Fuzzing

**Файл: `src/fuzz/h225_fuzzer.hpp`**

```cpp
#pragma once

#include "fuzz_types.hpp"
#include <vector>
#include <string>

namespace h323p {

// H.225 Fuzzer
class H225Fuzzer {
public:
    H225Fuzzer(const FuzzConfig& config);
    ~H225Fuzzer();
    
    // Fuzzing RAS сообщений
    std::vector<uint8_t> fuzzRasMessage(const std::vector<uint8_t>& original);
    
    // Fuzzing Q.931 сообщений
    std::vector<uint8_t> fuzzQ931Message(const std::vector<uint8_t>& original);
    
    // Fuzzing конкретных полей
    std::vector<uint8_t> fuzzCallReferenceValue(const std::vector<uint8_t>& data);
    std::vector<uint8_t> fuzzMessageTypes(const std::vector<uint8_t>& data);
    std::vector<uint8_t> fuzzInformationElements(const std::vector<uint8_t>& data);
    
    // Статистика
    FuzzStatistics getStatistics() const { return stats_; }
    
private:
    FuzzConfig config_;
    FuzzStatistics stats_;
    std::unique_ptr<RandomFlipStrategy> strategy_;
};

} // namespace h323p
```

---

### 6.3 Команда fuzz

**Файл: `src/cli/commands/cmd_fuzz.hpp`**

```cpp
#pragma once

#include "cli/commands.hpp"
#include "fuzz/fuzz_engine.hpp"

namespace h323p {

// Команда fuzz
class FuzzCommand {
public:
    FuzzCommand();
    
    // Выполнение команды
    int execute(const CommandConfig& config);
    
private:
    FuzzEngine fuzzEngine_;
};

} // namespace h323p
```

**Файл: `src/cli/commands/cmd_fuzz.cpp`**

```cpp
#include "cmd_fuzz.hpp"
#include "utils/logging.hpp"
#include <iostream>
#include <csignal>

namespace h323p {

FuzzCommand::FuzzCommand() = default;

int FuzzCommand::execute(const CommandConfig& config) {
    LOG_INFO("Выполнение команды: fuzz");
    
    // Проверка назначения
    if (config.destination.empty()) {
        std::cerr << "Ошибка: требуется указать адрес назначения" << std::endl;
        return 1;
    }
    
    // Настройка fuzzing
    FuzzConfig fuzzConfig;
    fuzzConfig.type = FuzzType::RTP;  // По умолчанию
    
    if (config.fuzzType == "rtp") {
        fuzzConfig.type = FuzzType::RTP;
    } else if (config.fuzzType == "rtcp") {
        fuzzConfig.type = FuzzType::RTCP;
    } else if (config.fuzzType == "h225") {
        fuzzConfig.type = FuzzType::H225_RAS;
    } else if (config.fuzzType == "h245") {
        fuzzConfig.type = FuzzType::H245;
    }
    
    fuzzConfig.headerPercentage = config.fuzzPercentage;
    fuzzConfig.payloadPercentage = 0;  // По умолчанию не fuzzing payload
    
    fuzzEngine_.setConfig(fuzzConfig);
    
    // Обработка сигналов
    std::atomic<bool> interrupted(false);
    std::signal(SIGINT, [&interrupted](int) {
        interrupted = true;
    });
    
    // Запуск fuzzing
    if (!fuzzEngine_.start(config.destination)) {
        std::cerr << "Ошибка запуска fuzzing" << std::endl;
        return 1;
    }
    
    std::cout << "Fuzzing запущен" << std::endl;
    std::cout << "  Тип: " << config.fuzzType << std::endl;
    std::cout << "  Процент: " << config.fuzzPercentage << "%" << std::endl;
    std::cout << "Нажмите Ctrl+C для остановки" << std::endl;
    
    // Ожидание прерывания
    while (!interrupted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Вывод статистики
        auto stats = fuzzEngine_.getStatistics();
        std::cout << "Packets: " << stats.packetsSent 
                  << " | Fuzzed: " << stats.packetsFuzzed << std::endl;
    }
    
    // Остановка
    fuzzEngine_.stop();
    
    // Финальная статистика
    auto stats = fuzzEngine_.getStatistics();
    std::cout << "\n=== Статистика Fuzzing ===" << std::endl;
    std::cout << "  Отправлено пакетов: " << stats.packetsSent << std::endl;
    std::cout << "  Fuzzed пакетов: " << stats.packetsFuzzed << std::endl;
    std::cout << "  Обнаружено крашей: " << stats.crashesDetected << std::endl;
    
    return 0;
}

} // namespace h323p
```

---

### 6.4 Видео поддержка

**Файл: `src/extensions/video/video_channel.hpp`**

```cpp
#pragma once

#include <string>
#include <cstdint>

namespace h323p {

// Видео кодек
enum class VideoCodec {
    H261,
    H263,
    H263_1998,
    H264  // Требует лицензирования
};

// Видео разрешение
enum class VideoResolution {
    QCIF,   // 176x144
    CIF,    // 352x288
    CIF4,   // 704x576
    CIF16,  // 1408x1152
    VGA,    // 640x480
    VGA_480P, // 854x480
    HD_720P,  // 1280x720
    HD_1080I  // 1920x1080 interlaced
};

// Конфигурация видео канала
struct VideoChannelConfig {
    VideoCodec codec = VideoCodec::H261;
    VideoResolution resolution = VideoResolution::QCIF;
    int framerate = 15;  // fps
    int bitrate = 64000; // бит/сек
    bool bidirectional = true;
};

// Видео канал
class VideoChannel {
public:
    VideoChannel(const VideoChannelConfig& config);
    ~VideoChannel();
    
    // Открытие/закрытие
    bool open(const std::string& remoteAddress, int remotePort);
    void close();
    bool isOpen() const { return open_; }
    
    // Отправка/получение
    bool sendFrame(const uint8_t* data, size_t size);
    bool receiveFrame(uint8_t* buffer, size_t bufferSize, size_t& receivedSize);
    
    // Статистика
    uint64_t getFramesSent() const { return framesSent_; }
    uint64_t getFramesReceived() const { return framesReceived_; }
    
private:
    VideoChannelConfig config_;
    bool open_ = false;
    uint64_t framesSent_ = 0;
    uint64_t framesReceived_ = 0;
};

} // namespace h323p
```

---

### 6.5 H.239 Презентации

**Файл: `src/extensions/h239/h239_controller.hpp`**

```cpp
#pragma once

#include <string>
#include <cstdint>

namespace h323p {

// Роль H.239
enum class H239Role {
    PRESENTER,      // Отправляет презентацию
    VIEWER,         // Получает презентацию
    BOTH            // Может и отправлять, и получать
};

// Конфигурация H.239
struct H239Config {
    H239Role role = H239Role::PRESENTER;
    VideoCodec codec = VideoCodec::H263;
    VideoResolution resolution = VideoResolution::XGA;
    int framerate = 10;
    int bitrate = 128000;
};

// H.239 Контроллер
class H239Controller {
public:
    H239Controller(const H239Config& config);
    ~H239Controller();
    
    // Запуск презентации
    bool startPresentation();
    bool stopPresentation();
    bool isPresenting() const { return presenting_; }
    
    // Переключение роли
    bool requestPresenterRole();
    bool releasePresenterRole();
    
    // Статистика
    uint64_t getPresentationDuration() const;  // секунды
    
private:
    H239Config config_;
    bool presenting_ = false;
    std::time_t startTime_ = 0;
};

} // namespace h323p
```

---

### 6.6 TLS Поддержка

**Файл: `src/extensions/tls/tls_config.hpp`**

```cpp
#pragma once

#include <string>
#include <vector>

namespace h323p {

// TLS Конфигурация
struct TlsConfig {
    bool enabled = false;
    
    // Сертификаты
    std::string certFile;       // Файл сертификата
    std::string keyFile;        // Файл приватного ключа
    std::string caFile;         // Файл CA сертификата
    std::string passphrase;     // Пароль для ключа
    
    // Настройки
    int minVersion = 12;  // TLS 1.2
    int maxVersion = 13;  // TLS 1.3
    std::vector<std::string> cipherSuites;
    
    // Проверка
    bool verifyPeer = true;
    bool verifyDepth = 5;
};

} // namespace h323p
```

---

## 📄 Примеры использования

### RTP Fuzzing

```bash
# Базовый RTP fuzzing (50% заголовка)
./h323p fuzz 192.168.1.100 -T rtp -p 50

# Fuzzing с модификацией payload (10%)
./h323p fuzz 192.168.1.100 -T rtp --header-pct 50 --payload-pct 10

# С seed для воспроизводимости
./h323p fuzz 192.168.1.100 -T rtp -p 50 --seed 12345
```

### H.225 Fuzzing

```bash
# H.225 RAS fuzzing
./h323p fuzz 192.168.1.1 -T h225 -p 30

# H.245 fuzzing
./h323p fuzz 192.168.1.100 -T h245 -p 20
```

### Видео вызов

```bash
# Вызов с видео (H.261, QCIF)
./h323p call 192.168.1.100 --video --video-codec H.261 --resolution QCIF

# Видео с H.263
./h323p call 192.168.1.100 -V --video-codec H.263 --framerate 15
```

### H.239 Презентация

```bash
# Запуск презентации
./h323p call 192.168.1.100 --video --h239 --h239-role presenter

# Получение презентации
./h323p call 192.168.1.100 --video --h239 --h239-role viewer
```

### TLS Вызов

```bash
# Вызов через TLS
./h323p call 192.168.1.100 --tls --tls-cert client.pem --tls-key client.key

# С проверкой сервера
./h323p call 192.168.1.100 --tls --tls-ca ca.pem --verify-peer
```

### XML сценарий с fuzzing

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="RTP Fuzzing Test">
  <globals>
    <variable name="fuzz_pct" value="50"/>
  </globals>
  
  <callflow name="fuzzer">
    <actions>
      <setup destination="192.168.1.100:1720"/>
      <wait event="connect" timeout="30000"/>
      
      <open-channel type="audio" codec="G.711">
        <fuzz enable="true" 
              type="rtp"
              header_percentage="${fuzz_pct}"
              payload_percentage="10"/>
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

### Чек-лист завершения Этапа 6

```
[ ] 1. RTP fuzzing реализован
[ ] 2. H.225 fuzzing реализован
[ ] 3. H.245 fuzzing реализован
[ ] 4. Команда fuzz работает
[ ] 5. Видео канал открывается/закрывается
[ ] 6. H.261/H.263 кодеки работают
[ ] 7. H.239 презентации работают
[ ] 8. TLS соединение устанавливается
[ ] 9. Интеграция с XML сценариями
[ ] 10. Тесты fuzzing проходят
```

---

## 🧪 Проверка результатов

### Сборка

```bash
cd build
cmake .. -DBUILD_TESTS=ON -DENABLE_VIDEO=ON -DENABLE_TLS=ON
make -j$(nproc)
```

### Fuzzing тесты

```bash
# RTP fuzzing
./h323p fuzz 192.168.1.100 -T rtp -p 50 -c 1000

# H.225 fuzzing
./h323p fuzz 192.168.1.1 -T h225 -p 30 -c 500

# С отчётом
./h323p fuzz 192.168.1.100 -T rtp -p 50 --report fuzz_report.json
```

### Видео тесты

```bash
# Базовый видео вызов
./h323p call 192.168.1.100 --video --video-codec H.261

# С презентацией
./h323p call 192.168.1.100 --video --h239
```

### TLS тесты

```bash
# TLS вызов
./h323p call 192.168.1.100 --tls --tls-cert test.pem --tls-key test.key

# Без проверки сертификата
./h323p call 192.168.1.100 --tls --no-verify
```

---

## 📚 Ссылки

- **H.239 стандарт:** ITU-T Recommendation H.239
- **H.261 стандарт:** ITU-T Recommendation H.261
- **H.263 стандарт:** ITU-T Recommendation H.263
- **TLS для H.323:** ITU-T H.235.0

---

*Версия документа: 0.1.0*  
*Последнее обновление: 2026-03-16*
